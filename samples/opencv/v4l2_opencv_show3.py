import sys
import cv2

def read_cam():
    cap = cv2.VideoCapture("nvv4l2camerasrc ! video/x-raw(memory:NVMM), format=(string)UYVY, width=(int)1920, height=(int)1080 ! nvvidconv ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink")
    if cap.isOpened():
        cv2.namedWindow("demo", cv2.WINDOW_AUTOSIZE)
        while True:
            ret_val, img = cap.read();
            #img2 = cv2.cvtColor(img, cv2.COLOR_YUV2BGR_I420);
            cv2.imshow('demo',img)
            cv2.waitKey(1)
    else:
     print "camera open failed"

    cv2.destroyAllWindows()


if __name__ == '__main__':
    read_cam()