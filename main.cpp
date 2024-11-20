#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;
int main()
{
    string src = "/home/jetson/workspace/LineTracer/simulation/5_lt_cw_100rpm_out.mp4";

    VideoCapture source(src, CAP_FFMPEG);
    if (!source.isOpened()){ cout << "Camera error" << endl; return -1; }
    string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! rtph264pay pt=96 ! udpsink host=203.234.58.154 port=9011 sync=false";

    VideoWriter writer1(dst1, 0, (double)30, Size(640, 360), true);
    if (!writer1.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}

    string dst2 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! rtph264pay pt=96 ! udpsink host=203.234.58.154 port=9012 sync=false";
    
    VideoWriter writer2(dst2, 0, (double)30, Size(640, 360), false);
    if (!writer2.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}
    
    string dst3 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=203.234.58.154 port=9013 sync=false";

    VideoWriter writer3(dst3, 0, (double)30, Size(640, 90), false);
    if (!writer3.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}
    

    cv::TickMeter tm;
    Mat frame, gray, thresh;
    
    int x = 0, y = 270, width = 640, height = 90; // y = 270부터 시작 (하단 90픽셀)
    Rect roi(x, y, width, height);

    while (true) {
        tm.start();
        source >> frame;
        if (frame.empty()) { 
            cerr << "frame empty!" << endl; 
            break; 
        }
        cvtColor(frame, gray, COLOR_BGR2GRAY); 
        threshold(gray, thresh, 0, 255, THRESH_OTSU | THRESH_BINARY); 
        Mat croppedThresh = thresh(roi); 

        writer1 << frame;
        writer2 << gray;

        writer3 << croppedThresh;

        tm.stop();
        cout << "Time : " << tm.getTimeMilli() << "ms" << endl;

        waitKey(30);
    }
    return 0;
}
