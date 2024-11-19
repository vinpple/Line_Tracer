#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;

int main() {
    string videoPath = "/home/jetson/workspace/test/simulation/5_lt_cw_100rpm_out.mp4"; 

    VideoCapture source(videoPath, CAP_FFMPEG);
    if (!source.isOpened()) {
        cout << "Video file could not be opened!" << endl;
        return -1;
    }

    string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! rtph264pay pt=96 ! udpsink host=203.234.58.154 port=8001 sync=false";
    VideoWriter writer1(dst1, VideoWriter::fourcc('H', '2', '6', '4'), source.get(CAP_PROP_FPS), Size((int)source.get(CAP_PROP_FRAME_WIDTH), (int)source.get(CAP_PROP_FRAME_HEIGHT)), true);
    if (!writer1.isOpened()) {
        cerr << "Writer1 open failed!" << endl;
        return -1;
    }

    string dst2 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! rtph264pay pt=96 ! udpsink host=203.234.58.154 port=8002 sync=false";
    VideoWriter writer2(dst2, VideoWriter::fourcc('H', '2', '6', '4'), source.get(CAP_PROP_FPS), Size((int)source.get(CAP_PROP_FRAME_WIDTH), (int)source.get(CAP_PROP_FRAME_HEIGHT)), false);
    if (!writer2.isOpened()) {
        cerr << "Writer2 open failed!" << endl;
        return -1;
    }

    string dst3 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! \
        nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! \
        h264parse ! rtph264pay pt=96 ! \
        udpsink host=203.234.58.154 port=8003 sync=false";
    VideoWriter writer3(dst3, VideoWriter::fourcc('H', '2', '6', '4'), source.get(CAP_PROP_FPS), Size((int)source.get(CAP_PROP_FRAME_WIDTH), (int)source.get(CAP_PROP_FRAME_HEIGHT)), false);
    if (!writer3.isOpened()) {
        cerr << "Writer3 open failed!" << endl;
        return -1;
    }

    Mat frame;
    while (true) {
        source >> frame; 
        if (frame.empty()) {
            cout << "End of video." << endl;
            break;
        }

        writer1 << frame; 
        writer2 << frame; 
        writer3 << frame; 

        imshow("Video", frame); 
        if (waitKey(30) == 27) { 
            break;
        }
    }

    return 0;
}
