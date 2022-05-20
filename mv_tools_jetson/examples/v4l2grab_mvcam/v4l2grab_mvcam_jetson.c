
// compile with all three access methods
#if !defined(IO_MMAP)
#define IO_MMAP
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <libv4l2.h>
#include <signal.h>
#include <stdint.h>
#include <inttypes.h>


#define TEGRA_CAMERA_CID_BASE	(V4L2_CTRL_CLASS_CAMERA | 0x2000)
#define TEGRA_CAMERA_CID_LOW_LATENCY            (TEGRA_CAMERA_CID_BASE+109)
#define TEGRA_CAMERA_CID_VI_TIME_OUT_DISABLE    (TEGRA_CAMERA_CID_BASE+120)
#define TEGRA_CAMERA_CID_VI_PREFERRED_STRIDE    (TEGRA_CAMERA_CID_BASE+110)
/* user define v4l2 controls*/
#define V4L2_CID_VEYE_MV_BASE					(V4L2_CID_USER_BASE + 0x1000)
#define V4L2_CID_VEYE_MV_TRIGGER_MODE			(V4L2_CID_VEYE_MV_BASE + 1)
#define V4L2_CID_VEYE_MV_TRIGGER_SRC			(V4L2_CID_VEYE_MV_BASE + 2)
#define V4L2_CID_VEYE_MV_SOFT_TRGONE			(V4L2_CID_VEYE_MV_BASE + 3)
#define V4L2_CID_VEYE_MV_FRAME_RATE				(V4L2_CID_VEYE_MV_BASE + 4)
#define V4L2_CID_VEYE_MV_ROI_X		        	(V4L2_CID_VEYE_MV_BASE + 5)
#define V4L2_CID_VEYE_MV_ROI_Y  				(V4L2_CID_VEYE_MV_BASE + 6)

/* TX2 */
#define V4L2_PIX_FMT_TX2_Y10     v4l2_fourcc('T', 'Y', '1', '0') /* 10  Greyscale     */
#define V4L2_PIX_FMT_TX2_Y12     v4l2_fourcc('T', 'Y', '1', '2') /* 12  Greyscale     */

/* Xavier */
#define V4L2_PIX_FMT_XAVIER_Y10     v4l2_fourcc('X', 'Y', '1', '0') /* 10  Greyscale     */
#define V4L2_PIX_FMT_XAVIER_Y12     v4l2_fourcc('X', 'Y', '1', '2') /* 12  Greyscale     */
//end

#define CLEAR(x) memset (&(x), 0, sizeof (x))

#define ALIGN_DOWN(p,n) (((int)(p)) & ~((n)-1))
#define ALIGN_UP(p,n) ALIGN_DOWN((int)(p)+(n)-1,(n))

#ifndef VERSION
#define VERSION "v1.0"
#endif

#if defined(IO_MMAP)
// minimum number of buffers to request in VIDIOC_REQBUFS call
#define VIDIOC_REQBUFS_COUNT 4
#endif

#define LINE_BUFF_ALIGN 64
#define ROI_ALIGN 8

typedef enum {

#ifdef IO_MMAP
        IO_METHOD_MMAP,
#endif

} io_method;

struct buffer {
        void *                  start;
        size_t                  length;
};

static io_method        io              = IO_METHOD_MMAP;
static int              fd              = -1;
struct buffer *         buffers         = NULL;
static unsigned int     n_buffers       = 0;

// global settings
static unsigned int pixelformat = 0; //0:raw8,1:raw10,2:raw12
static unsigned int trgmode = 0;     //0:Video streaming mode,1:Normal trigger mode,2:High-speed continuous trigger mode.
static unsigned int trgsrc = 0;      //0:Software trigger,1:Hardware trigger.
static char* JetsonBoard = NULL;

static unsigned int fourCC = 0;
static unsigned int preferred_stride = 0;
static unsigned int frame_buff_len = 0;

static unsigned int start_x = 0;
static unsigned int start_y = 0;
static unsigned int width = 3088;
static unsigned int height = 2064;

static int fps = -1;

static int continuous = 0;
static char* Filename = NULL;
static char* FilenamePart = NULL;
static char* deviceName = "/dev/video0";

static const char* const continuousFilenameFmt = "%s_%010"PRIu32"_%"PRId64".raw";

static int xioctl(int fd, int request, void* argp);


/**
	Print error message and terminate programm with EXIT_FAILURE return code.

	\param s error message to print
*/
static void errno_exit(const char* s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

/**
	Do ioctl and retry if error was EINTR ("A signal was caught during the ioctl() operation."). Parameters are the same as on ioctl.

	\param fd file descriptor
	\param request request
	\param argp argument
	\returns result from ioctl
*/
static int xioctl(int fd, int request, void* argp)
{
	int r;

	do r = v4l2_ioctl(fd, request, argp);
	while (-1 == r && EINTR == errno);

	return r;
}


static void raw_write(const unsigned char* img, char* Filename,int size)
{
    FILE *outfile = fopen( Filename, "wb" );
    if (!outfile) {
		errno_exit("raw");
	}
    fwrite(img,size,1,outfile);
    
    fclose(outfile);
}

/**
	process image read
*/
int number_total = 0;

static void imageProcess(const void* p,struct timeval timestamp)
{
    	if(continuous==1) {
		static uint32_t img_ind = 0;
		int64_t timestamp_long;
		timestamp_long = timestamp.tv_sec*1e6 + timestamp.tv_usec;
		sprintf(Filename,continuousFilenameFmt,FilenamePart,img_ind++,timestamp_long);

	}
    printf("capture number %d\r\n",++number_total);
    raw_write(p,Filename,frame_buff_len);
    return;
}

/**
	read single frame
*/
static int frameRead(void)
{
	struct v4l2_buffer buf;

	switch (io) {
#ifdef IO_MMAP
		case IO_METHOD_MMAP:
			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;

			if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
				switch (errno) {
					case EAGAIN:
						return 0;

					case EIO:
						// Could ignore EIO, see spec
						// fall through

					default:
						errno_exit("VIDIOC_DQBUF");
				}
			}

			assert(buf.index < n_buffers);
            //printf("get buff len %d\r\n",buf.length);
			imageProcess(buffers[buf.index].start,buf.timestamp);

			if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
				errno_exit("VIDIOC_QBUF");

			break;
#endif
	}

	return 1;
}

/**
	mainloop: read frames and process them
*/
static void mainLoop(void)
{	
	int count;
	unsigned int numberOfTimeouts;

	numberOfTimeouts = 0;
	count = 1;

	while (count-- > 0) {
		for (;;) {
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO(&fds);
			FD_SET(fd, &fds);

			/* Timeout. */
			tv.tv_sec = 5;
			tv.tv_usec = 0;

			r = select(fd + 1, &fds, NULL, NULL, &tv);

			if (-1 == r) {
				if (EINTR == errno)
					continue;

				errno_exit("select");
			}

			if (0 == r) {
				if (numberOfTimeouts <= 0) {
					count++;
				} else {
					fprintf(stderr, "select timeout\n");
					exit(EXIT_FAILURE);
				}
			}
			if(continuous == 1) {
				count = 1;
			}

			if (frameRead())
				break;

			/* EAGAIN - continue select loop. */
		}
	}
}

/**
	stop capturing
*/
static void captureStop(void)
{
	enum v4l2_buf_type type;

	switch (io) {
#ifdef IO_MMAP
		case IO_METHOD_MMAP:
#endif

#if defined(IO_MMAP)
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
			errno_exit("VIDIOC_STREAMOFF");

			break;
#endif
	}
}

/**
  start capturing
*/
static void captureStart(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

	switch (io) {

#ifdef IO_MMAP
		case IO_METHOD_MMAP:
			for (i = 0; i < n_buffers; ++i) {
				struct v4l2_buffer buf;

				CLEAR(buf);

				buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory = V4L2_MEMORY_MMAP;
				buf.index = i;

				if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
					errno_exit("VIDIOC_QBUF");
				}

			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
				errno_exit("VIDIOC_STREAMON");

			break;
#endif

	}
}

static void deviceUninit(void)
{
	unsigned int i;

	switch (io) {
#ifdef IO_MMAP
		case IO_METHOD_MMAP:
			for (i = 0; i < n_buffers; ++i)
				if (-1 == v4l2_munmap(buffers[i].start, buffers[i].length))
					errno_exit("munmap");
			break;
#endif

	}

	free(buffers);
}


#ifdef IO_MMAP
static void mmapInit(void)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = VIDIOC_REQBUFS_COUNT;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support memory mapping\n", deviceName);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n", deviceName);
		exit(EXIT_FAILURE);
	}

	buffers = calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = v4l2_mmap(NULL /* start anywhere */, buf.length, PROT_READ | PROT_WRITE /* required */, MAP_SHARED /* recommended */, fd, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}
#endif



/**
	initialize device
*/
static void deviceInit(void)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	//struct v4l2_streamparm frameint;

    struct v4l2_control v4l2_args;
    
	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n",deviceName);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n",deviceName);
		exit(EXIT_FAILURE);
	}

	switch (io) {

#ifdef IO_MMAP
		case IO_METHOD_MMAP:
#endif
#if defined(IO_MMAP)
      			if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
				fprintf(stderr, "%s does not support streaming i/o\n",deviceName);
				exit(EXIT_FAILURE);
			}
			break;
#endif
	}

	/* Select video input, video standard and tune here. */
	CLEAR(cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; /* reset to default */

		if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
			switch (errno) {
				case EINVAL:
					/* Cropping not supported. */
					break;
				default:
					/* Errors ignored. */
					break;
			}
		}
	} else {
		/* Errors ignored. */
	}

    //set starx start y first
    v4l2_args.id = V4L2_CID_VEYE_MV_ROI_X;
    v4l2_args.value = start_x;
    if (0 != xioctl(fd, VIDIOC_S_CTRL, &v4l2_args)){
        printf("set startx failed!!\r\n");
    }
    v4l2_args.id = V4L2_CID_VEYE_MV_ROI_Y;
    v4l2_args.value = start_y;
    if (0 != xioctl(fd, VIDIOC_S_CTRL, &v4l2_args)){
        printf("set starty failed!!\r\n");
    }
	CLEAR(fmt);
	//v4l2_format
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = width;
	fmt.fmt.pix.height = height;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    fmt.fmt.pix.pixelformat = fourCC;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
		errno_exit("VIDIOC_S_FMT");

	/*if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUV420) {
		fprintf(stderr,"Libv4l didn't accept YUV420 format. Can't proceed.\n");
		exit(EXIT_FAILURE);
	}*/

	/* Note VIDIOC_S_FMT may change width and height. */
	if (width != fmt.fmt.pix.width) {
		width = fmt.fmt.pix.width;
		fprintf(stderr,"Image width set to %i by device %s.\n", width, deviceName);
	}

	if (height != fmt.fmt.pix.height) {
		height = fmt.fmt.pix.height;
		fprintf(stderr,"Image height set to %i by device %s.\n", height, deviceName);
	}
	
  /* If the user has set the fps to -1, don't try to set the frame interval */
  if (fps != -1)
  {
      
    #if 0
    CLEAR(frameint);
    
    /* Attempt to set the frame interval. */
    frameint.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    frameint.parm.capture.timeperframe.numerator = 1;
    frameint.parm.capture.timeperframe.denominator = fps;
    if (-1 == xioctl(fd, VIDIOC_S_PARM, &frameint))
      fprintf(stderr,"Unable to set frame interval.\n");
    #endif
    v4l2_args.id = V4L2_CID_VEYE_MV_FRAME_RATE;
    v4l2_args.value = fps;
    if (0 != xioctl(fd, VIDIOC_S_CTRL, &v4l2_args)){
        printf("set frame rate failed!!\r\n");
    }
  }

	switch (io) {

#ifdef IO_MMAP
		case IO_METHOD_MMAP:
			mmapInit();
			break;
#endif

	}
    
    v4l2_args.id = TEGRA_CAMERA_CID_VI_PREFERRED_STRIDE;
    v4l2_args.value = preferred_stride;
    if (0 != xioctl(fd, VIDIOC_S_CTRL, &v4l2_args)){
        printf("set preferred_stride failed!!\r\n");
    }
    
    v4l2_args.id = V4L2_CID_VEYE_MV_TRIGGER_MODE;
    v4l2_args.value = trgmode;
    if (0 != xioctl(fd, VIDIOC_S_CTRL, &v4l2_args)){
        printf("set trgmode failed!!\r\n");
    }
    
    if(trgmode != 0)
    {
        v4l2_args.id = V4L2_CID_VEYE_MV_TRIGGER_SRC;
        v4l2_args.value = trgsrc;
        if (0 != xioctl(fd, VIDIOC_S_CTRL, &v4l2_args)){
            printf("set trgsrc failed!!\r\n");
        }
        
        v4l2_args.id = TEGRA_CAMERA_CID_LOW_LATENCY;
        v4l2_args.value = 1;
        if (0 != xioctl(fd, VIDIOC_S_CTRL, &v4l2_args)){
            printf("set low latency failed!!\r\n");
        }

        v4l2_args.id = TEGRA_CAMERA_CID_VI_TIME_OUT_DISABLE;
        v4l2_args.value = 1;
        if (0 != xioctl(fd, VIDIOC_S_CTRL, &v4l2_args)){
            printf("set vi timeout disable failed!!\r\n");
        }
        
    }
}
/**
	close device
*/
static void deviceClose(void)
{
	if (-1 == v4l2_close(fd))
		errno_exit("close");

	fd = -1;
}

/**
	open device
*/
static void deviceOpen(void)
{
	struct stat st;

	// stat file
	if (-1 == stat(deviceName, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n", deviceName, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	// check if its device
	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", deviceName);
		exit(EXIT_FAILURE);
	}

	// open device
    //signal 11 , delete nonblock
	//fd = v4l2_open(deviceName, O_RDWR /* required */ | O_NONBLOCK, 0);
    //https://forums.developer.nvidia.com/t/libv4l2-nvargus-so-source-code/168303/11 
    //jetson 32.5 bug
    fd = open(deviceName, O_RDWR /* required */ | O_NONBLOCK, 0);
	// check if opening was successfull
	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n", deviceName, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static int CheckParam(void)
{
    int bytes_per_pixel = 0;
    
    if(pixelformat > 2)
    {
        fprintf(stderr,"pixel format wrong %d\n",pixelformat);
        return -1;
    }
    if(trgmode > 2)
    {
        fprintf(stderr,"trgmode wrong %d\n",pixelformat);
        return -1;
    }
    if(trgsrc > 1)
    {
        fprintf(stderr,"trgmode wrong %d\n",pixelformat);
        return -1;
    }
    if(JetsonBoard == NULL)
    {
        fprintf(stderr,"must specify jetson platform\n");
        return -1;
    }
    if(strcasecmp(JetsonBoard,"tx2") == 0)
    {
        if(pixelformat == 0)
        {
            fourCC = V4L2_PIX_FMT_GREY;
            bytes_per_pixel = 1;
        }
        else if(pixelformat == 1)
        {
            fourCC = V4L2_PIX_FMT_TX2_Y10;
            bytes_per_pixel = 2;
        }
        else if(pixelformat == 2)
        {
            fourCC = V4L2_PIX_FMT_TX2_Y12;
            bytes_per_pixel = 2;
        }
    }
    else if(strcasecmp(JetsonBoard,"xavier") == 0)
    {
        if(pixelformat == 0)
        {
            fourCC = V4L2_PIX_FMT_GREY;
            bytes_per_pixel = 1;
        }
        else if(pixelformat == 1)
        {
            fourCC = V4L2_PIX_FMT_XAVIER_Y10;
            bytes_per_pixel = 2;
        }
        else if(pixelformat == 2)
        {
            fourCC = V4L2_PIX_FMT_XAVIER_Y12;
            bytes_per_pixel = 2;
        }
    }
    else
    {
        fprintf(stderr,"jetson board not supported %s\n",JetsonBoard);
        return -1;
    }
    
    start_x = ALIGN_DOWN(start_x,ROI_ALIGN);
    start_y = ALIGN_DOWN(start_y,ROI_ALIGN);
    width = ALIGN_UP(width,ROI_ALIGN);
    height = ALIGN_UP(height,ROI_ALIGN);
    fprintf(stdout,"Will use ROI(%d,%d,%d,%d)\n",start_x,start_y,width,height);
    
    preferred_stride = ALIGN_UP(width*bytes_per_pixel,LINE_BUFF_ALIGN);
    frame_buff_len = preferred_stride*height;
    
    fprintf(stdout,"preferred_stride %d frame_buff_len %d !\n",preferred_stride,frame_buff_len);

    fprintf(stdout,"please open the raw file with width %d height %d !\n",bytes_per_pixel == 2 ?preferred_stride/2 : preferred_stride,height);
    return 0;
}

/**
	print usage information
*/
static void usage(FILE* fp, int argc, char** argv)
{
	fprintf(fp,
		"Usage: %s [options]\n\n"
		"Options:\n"
		"-d | --device name   Video device name [/dev/video0]\n"
		"-h | --help          Print this message\n"
		"-o | --output        Set output filename\n"
        "-p | --pixelformat   Set pixel format,0:raw8,1:raw10,2:raw12\n"
        "-m | --trgmode       Set camera mode,0:Video streaming mode,1:Normal trigger mode,2:High-speed continuous trigger mode.\n"
        "-s | --trgsrc        Set camera trigger source,0:Software trigger,1:Hardware trigger.\n"
        "-j | --jetson        Set jetson board, tx2 or xavier.\n"
        "-X | --start_x        Set image start x\n"
		"-Y | --start_y        Set image start y\n"
		"-W | --width         Set image width\n"
		"-H | --height        Set image height\n"
		"-I | --interval      Set frame interval (fps) (-1 to skip)\n"
		"-c | --continuous    Do continous capture, stop with SIGINT.\n"
		"-v | --version       Print version\n"
		"",
		argv[0]);
	}

static const char short_options [] = "d:ho:p:m:s:j:X:Y:W:H:I:vc";

static const struct option
long_options [] = {
	{ "device",     required_argument,      NULL,           'd' },
	{ "help",       no_argument,            NULL,           'h' },
	{ "output",     required_argument,      NULL,           'o' },
    { "pixelformat",required_argument,      NULL,           'p' },
    { "trgmode",    required_argument,      NULL,           'm' },
    { "trgsrc",     optional_argument,      NULL,           's' },
    { "jetson",     optional_argument,      NULL,           'j' },
    { "start_x",    required_argument,      NULL,           'X' },
    { "start_y",    required_argument,      NULL,           'Y' },
	{ "width",      required_argument,      NULL,           'W' },
	{ "height",     required_argument,      NULL,           'H' },
	{ "interval",   required_argument,      NULL,           'I' },
	{ "version",	no_argument,		NULL,		'v' },
	{ "continuous",	no_argument,		NULL,		'c' },
	{ 0, 0, 0, 0 }
    
};
/**
SIGINT interput handler
*/
void StopContCapture(int sig_id) {
	printf("stoping continuous capture\n");
    //signal_backtrace(info, p);
	struct v4l2_control v4l2_args;
    if(trgmode != 0)
    {
        v4l2_args.id = TEGRA_CAMERA_CID_VI_TIME_OUT_DISABLE;
        v4l2_args.value = 0;
        if (0 != xioctl(fd, VIDIOC_S_CTRL, &v4l2_args)){
            printf("set vi timeout enable failed!!\r\n");
        }else{
            printf("set vi timeout enable success!!\r\n");
        }
    }
	continuous = 0;
    //wait for xavier(2.5s)
   // sleep(3);
    captureStop();
	// close device
	deviceUninit();
	deviceClose();

	if(FilenamePart != 0){ 
		free(Filename);
	}

	exit(EXIT_SUCCESS);

}

void InstallSIGINTHandler() {
	struct sigaction sa;
	CLEAR(sa);
	
	sa.sa_handler = StopContCapture;
	if(sigaction(SIGINT, &sa, 0) != 0)
	{
		fprintf(stderr,"could not install SIGINT handler, continuous capture disabled");
		continuous = 0;
	}
}
int main(int argc, char **argv)
{

	for (;;) {
		int index, c = 0;

		c = getopt_long(argc, argv, short_options, long_options, &index);

		if (-1 == c)
			break;

		switch (c) {
			case 0: /* getopt_long() flag */
				break;

			case 'd':
				deviceName = optarg;
				break;

			case 'h':
				// print help
				usage(stdout, argc, argv);
				exit(EXIT_SUCCESS);

			case 'o':
				// set jpeg filename
				Filename = optarg;
				break;

			case 'p':
				pixelformat = atoi(optarg);
				break;

			case 'm':
				trgmode = atoi(optarg);
				break;

			case 's':
				trgsrc = atoi(optarg);
				break;

            case 'j':
				JetsonBoard = optarg;
				break;
                
			case 'X':
				// set width
				start_x = atoi(optarg);
				break;

			case 'Y':
				// set width
				start_y = atoi(optarg);
				break;

			case 'W':
				// set width
				width = atoi(optarg);
				break;

			case 'H':
				// set height
				height = atoi(optarg);
				break;
				
			case 'I':
				// set fps
				fps = atoi(optarg);
				break;

			case 'c':
				// set flag for continuous capture, interuptible by sigint
				continuous = 1;
				InstallSIGINTHandler();
				break;
				
			case 'v':
				printf("Version: %s\n", VERSION);
				exit(EXIT_SUCCESS);
				break;

			default:
				usage(stderr, argc, argv);
				exit(EXIT_FAILURE);
		}
	}

	// check for need parameters
	if (!Filename) {
		fprintf(stderr, "You have to specify output filename!\n\n");
		usage(stdout, argc, argv);
		exit(EXIT_FAILURE);
	}
	
	if(continuous == 1) {
		int max_name_len = snprintf(NULL,0,continuousFilenameFmt,Filename,UINT32_MAX,INT64_MAX);
		FilenamePart = Filename;
		Filename = calloc(max_name_len+1,sizeof(char));
		strcpy(Filename,FilenamePart);
	}
	
    if(CheckParam()!=0)
    {
        fprintf(stderr, "CheckParam failed !\n\n");
        exit(EXIT_FAILURE);
    }
	// open and initialize device
	deviceOpen();

	deviceInit();

	// start capturing
	captureStart();
    
	// process frames
	mainLoop();

	// stop capturing
	captureStop();

	// close device
	deviceUninit();
    
	deviceClose();

	if(FilenamePart != 0){ 
		free(Filename);
	}

	exit(EXIT_SUCCESS);

	return 0;
}
