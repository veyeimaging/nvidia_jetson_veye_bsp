import cv2
import argparse
import subprocess
    
def main():
    # Set up command-line argument parser
    parser = argparse.ArgumentParser(description='Real-time display of GREY image from /dev/video0')
    parser.add_argument('--roix', type=int, default=0, help='roi start x (default: 0)')
    parser.add_argument('--roiy', type=int, default=0, help='roi start y (default: 0)')
    parser.add_argument('--width', type=int, default=640, help='image width (default: 640)')
    parser.add_argument('--height', type=int, default=480, help='image height (default: 480)')
    parser.add_argument('--fps', type=int, default=30, help='frame rate (default: 30)')
    args = parser.parse_args()

    v4l2_cmd = f"v4l2-ctl --set-ctrl roi_x={args.roix}"
    subprocess.run(v4l2_cmd, shell=True)
    
    v4l2_cmd = f"v4l2-ctl --set-ctrl roi_y={args.roiy}"
    subprocess.run(v4l2_cmd, shell=True)
    
    v4l2_cmd = f"v4l2-ctl --set-fmt-video=width={args.width},height={args.height}"
    subprocess.run(v4l2_cmd, shell=True)
    
    v4l2_cmd = f"v4l2-ctl --set-ctrl frame_rate={args.fps}"
    subprocess.run(v4l2_cmd, shell=True)
    
    # Open the /dev/video0 device
    cap = cv2.VideoCapture('/dev/video0')
    if not cap.isOpened():
        print("Failed to open video device")
        return

    # Set the image size
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, args.width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, args.height)

    # Loop over frames and display them
    while True:
        # Read a frame
        ret, frame = cap.read()

        # Check if reading was successful
        if not ret:
            print("Failed to read frame")
            break

        # Display the frame
        cv2.imshow('VEYE MV camera GREY image preview', frame)

        # Exit if 'q' key is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Release resources
    cap.release()
    cv2.destroyAllWindows()

if __name__ == '__main__':
    main()
