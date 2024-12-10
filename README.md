# Line_Tracer

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L13-L14

ctrl_c_pressed: Ctrl+C 신호를 감지하여 프로그램을 종료.

mx: Dynamixel 모터 제어 객체.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L16-L24

ctrlc_handler: Ctrl+C를 감지하면 모터를 정지하고 프로그램을 종료합니다.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L27-L43

Dynamixel 초기화: mx.open()으로 모터 연결.

string src: 주어진 경로의 비디오 파일을 source 객체로 열고 오류를 확인.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L45-L58

비디오 출력

writer3는 하단 90픽셀만 출력

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L61-L74

roi: 영상에서 관심 영역 (ROI, Region of Interest)을 설정.
prevCenter: 이전 중심 위치.
velocityHistory: 이동 벡터 기록.
historySize: 이동 벡터 기록의 최대 크기.
maxShift: 중심 이동 허용 최대 거리.
minArea / maxArea: 객체의 최소 및 최대 면적 필터링.
areaWeight: 객체 탐지 시 점수 계산에 사용되는 가중치.
straightSpeed: 기본 직진 속도.
k: 속도 제어를 위한 게인 값.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L80-L88

그레이스케일 변환 및 이진화 후 90픽셀 추출

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L90-L91

각 객체의 레이블, 통계, 중심 계산




