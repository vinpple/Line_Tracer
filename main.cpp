#include "opencv2/opencv.hpp"
#include <iostream>
#include <deque>
#include <cmath>
#include <csignal>  // SIGINT 처리
#include <unistd.h> // usleep 함수
#include "dxl.hpp"  // Dynamixel 제어 라이브러리

using namespace cv;
using namespace std;

// 글로벌 변수
bool ctrl_c_pressed = false;
Dxl mx; // Dynamixel 객체

// SIGINT 핸들러
void ctrlc_handler(int) {
    ctrl_c_pressed = true;
    cerr << "\nCtrl+C pressed. Stopping motors...\n";
    if (!mx.setVelocity(0, 0)) {
        cerr << "Failed to stop motors during Ctrl+C handling!\n";
    }
    mx.close(); // Dynamixel 닫기
    exit(0);    // 프로그램 종료
}

int main() {
    // SIGINT 핸들러 설정
    signal(SIGINT, ctrlc_handler);

    // Dynamixel 초기화
    if (!mx.open()) {
        cerr << "Dynamixel open error" << endl;
        return -1;
    }

    string src = "/home/jetson/workspace/LineTracer/simulation/5_lt_cw_100rpm_out.mp4";

    VideoCapture source(src, CAP_FFMPEG);
    if (!source.isOpened()) {
        cerr << "Camera error" << endl;
        return -1;
    }

    // dst1: 원본 영상
    string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! rtph264pay pt=96 ! udpsink host=203.234.58.154 port=9011 sync=false";
    VideoWriter writer1(dst1, 0, (double)30, Size(640, 360), true);
    if (!writer1.isOpened()) {
        cerr << "Writer1 open failed!" << endl;
        return -1;
    }

    // dst3: threshold 처리 후 하단 90픽셀 자르기
    string dst3 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! rtph264pay pt=96 ! udpsink host=203.234.58.154 port=9013 sync=false";
    VideoWriter writer3(dst3, 0, (double)30, Size(640, 90), true);
    if (!writer3.isOpened()) {
        cerr << "Writer3 open failed!" << endl;
        return -1;
    }

    cv::TickMeter tm;
    Mat frame, gray, thresh;
    const int roiHeight = 90;
    Rect roi(0, 270, 640, roiHeight);
    Point prevCenter(roi.width / 2, roi.height / 2);
    deque<Point> velocityHistory;
    const int historySize = 5;
    const int maxShift = 50;
    const int minArea = 50;
    const int maxArea = 4000;
    double areaWeight = 0.5;

    int straightSpeed = 100;
    double k = 0.13;

    while (true) {
        if (ctrl_c_pressed) break;

        tm.start();
        source >> frame;
        if (frame.empty()) {
            cerr << "Frame empty!" << endl;
            break;
        }

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        threshold(gray, thresh, 150, 255, THRESH_BINARY);
        Mat croppedThresh = thresh(roi);

        Mat labels, stats, centroids;
        int nComponents = connectedComponentsWithStats(croppedThresh, labels, stats, centroids, 8, CV_32S);

        Point roiCenter(roi.width / 2, roi.height / 2);
        double bestScore = DBL_MAX;
        Point bestCenter = prevCenter;
        bool foundValidCenter = false;

        Mat binaryColored;
        cvtColor(croppedThresh, binaryColored, COLOR_GRAY2BGR);

        for (int i = 1; i < nComponents; i++) {
            int area = stats.at<int>(i, CC_STAT_AREA);
            if (area < minArea || area > maxArea) continue;

            int x_center = static_cast<int>(centroids.at<double>(i, 0));
            int y_center = static_cast<int>(centroids.at<double>(i, 1));
            Point candidateCenter(x_center, y_center);

            double distance = norm(candidateCenter - roiCenter);
            double score = distance * (1.0 - areaWeight) + (1.0 / area) * areaWeight;

            // y 좌표가 ROI 높이의 절반 이하인 경우 제외
            if (y_center < roiHeight / 2) continue;

            if (score < bestScore) {
                bestScore = score;
                bestCenter = candidateCenter;
                foundValidCenter = true;
            }

            // 최적의 중심과 다른 객체를 구분해서 색상 설정
            Scalar boxColor = (candidateCenter == bestCenter) ? Scalar(0, 0, 255) : Scalar(255, 0, 0);
            rectangle(binaryColored, Rect(stats.at<int>(i, CC_STAT_LEFT), stats.at<int>(i, CC_STAT_TOP),
                                          stats.at<int>(i, CC_STAT_WIDTH), stats.at<int>(i, CC_STAT_HEIGHT)),
                      boxColor, 2);
            circle(binaryColored, candidateCenter, 5, boxColor, -1);
        }

        // 중심 이동 제한
        if (foundValidCenter) {
            if (norm(bestCenter - prevCenter) > maxShift) {
                bestCenter = prevCenter;
            }
            prevCenter = bestCenter;
        }

        // 이동 벡터 기록
        if (foundValidCenter) {
            velocityHistory.push_back(bestCenter);
            if (velocityHistory.size() > historySize) {
                velocityHistory.pop_front();
            }
        }

        int error = bestCenter.x - frame.cols / 2;
        int vel1 = straightSpeed - k * error;
        int vel2 = -(straightSpeed + k * error);

        vel1 = max(-200, min(200, vel1));
        vel2 = max(-200, min(200, vel2));

        if (!mx.setVelocity(vel1, vel2)) {
            cerr << "Dynamixel setVelocity error!" << endl;
            break;
        }

        cout << "Error: " << error 
             << ", Vel1: " << vel1 
             << ", Vel2: " << vel2 
             << ", Response Time(ms): " << tm.getTimeMilli() << endl;

        writer3 << binaryColored;
        writer1 << frame;

        tm.stop();
        usleep(30000);
    }

    if (!mx.setVelocity(0, 0)) {
        cerr << "Failed to stop motors!" << endl;
    }
    mx.close();
    return 0;
}
