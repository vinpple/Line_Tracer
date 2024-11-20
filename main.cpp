#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;

int main() {
    string src = "/home/jetson/workspace/LineTracer/simulation/5_lt_cw_100rpm_out.mp4";

    VideoCapture source(src, CAP_FFMPEG);
    if (!source.isOpened()) { cout << "Camera error" << endl; return -1; }

    // dst1: 원본 영상
    string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! rtph264pay pt=96 ! udpsink host=203.234.58.154 port=9011 sync=false";
    VideoWriter writer1(dst1, 0, (double)30, Size(640, 360), true);
    if (!writer1.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}

    // dst2: 흑백 영상
    string dst2 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! rtph264pay pt=96 ! udpsink host=203.234.58.154 port=9012 sync=false";
    VideoWriter writer2(dst2, 0, (double)30, Size(640, 90), false);
    if (!writer2.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}

    // dst3: threshold 처리 후 하단 90픽셀 자르기
    string dst3 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! rtph264pay pt=96 ! udpsink host=203.234.58.154 port=9013 sync=false";
    VideoWriter writer3(dst3, 0, (double)30, Size(640, 90), false);
    if (!writer3.isOpened()) { cerr << "Writer open failed!" << endl; return -1;}

    cv::TickMeter tm;
    Mat frame, gray, thresh, labeledImage;
    int x = 0, y = 270, width = 640, height = 90; // 하단 90픽셀
    Rect roi(x, y, width, height);

    while (true) {
        tm.start();
        source >> frame;
        if (frame.empty()) {
            cerr << "frame empty!" << endl;
            break;
        }

        // threshold 적용 (그레이스케일로 변환 후 임계값 처리)
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        threshold(gray, thresh, 0, 255, THRESH_OTSU | THRESH_BINARY);
        Mat croppedThresh = thresh(roi); // 하단 90픽셀만 잘라내기

        // connectedComponentsWithStats를 사용하여 객체 분석
        // connectedComponentsWithStats 함수는 컴포넌트를 찾고, 각 컴포넌트의 통계를 반환합니다
        Mat labels, stats, centroids;
        int nComponents = connectedComponentsWithStats(croppedThresh, labels, stats, centroids, 8, CV_32S);

        // dst3에만 빨간 점 그리기
        for (int i = 1; i < nComponents; i++) { // 0은 배경이므로 1부터 시작
            int x_center = static_cast<int>(centroids.at<double>(i, 0));
            int y_center = static_cast<int>(centroids.at<double>(i, 1));

            // 빨간 점 그리기
            circle(frame, Point(x + x_center, y + y_center), 5, Scalar(0, 0, 255), -1); // (x, y) 위치에 빨간 점 그리기
        }

        // 각 writer에 출력
        writer1 << frame; // 원본 영상
        writer2 << gray;  // 그레이스케일 영상
        writer3 << croppedThresh; // Threshold 후 잘린 영상 + 빨간 점

        tm.stop();
        cout << "Time : " << tm.getTimeMilli() << "ms" << endl;

        waitKey(30);
    }
    return 0;
}
