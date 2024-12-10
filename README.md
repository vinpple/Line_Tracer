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

k: 속도 제어를 위한 게인 값. (값이 클수록 작게 돌음 여러 번의 실험 결과 0.13정도가 적합)

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L80-L88

그레이스케일 변환 및 이진화 후 90픽셀 추출

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L93-L96

roiCenter:ROI (Region of Interest)의 중심 좌표를 설정

roi.width / 2, roi.height / 2를 통해 중심점이 ROI의 중앙을 기준으로 하도록 설정됩니다.

bestScore: 객체들 중에서 최적의 중심을 찾기 위한 점수입니다.

DBL_MAX로 초기화하여, 가장 작은 점수를 찾을 수 있게 합니다.

bestCenter: 최적의 중심 좌표를 저장하는 변수입니다.

이전 프레임의 중심 prevCenter로 초기화됩니다.

foundValidCenter: 유효한 중심을 찾았는지 여부를 나타내는 플래그입니다.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L101-L103

반복문:

nComponents는 연결된 객체의 개수를 나타냅니다. i = 1부터 시작해 배경(0번 레이블)을 제외한 객체를 반복합니다.

area:각 객체의 면적을 가져옵니다.

stats.at<int>(i, CC_STAT_AREA)로 객체의 면적을 확인합니다.

면적 필터링: area가 minArea보다 작거나 maxArea보다 크면 continue로 해당 객체를 무시합니다.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L105-L107

centroids는 각 객체의 중심 좌표를 저장한 행렬입니다.

x_center와 y_center는 객체 중심의 x와 y 좌표입니다.

candidateCenter는 객체의 중심을 Point 형식으로 저장합니다.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L109-L110

score: 거리와 면적을 고려해 점수를 계산합니다.

distance: 중심과의 거리 (작을수록 좋음).

(1.0 / area): 면적이 클수록 점수가 낮아집니다.

areaWeight: 거리와 면적의 가중치 비율을 조정합니다.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L113

y 좌표가 ROI 높이의 절반 이하이면 continue로 무시합니다.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L115C12-L119C14

core가 bestScore보다 작으면: bestScore를 현재 score로 업데이트합니다. bestCenter를 candidateCenter로 설정합니다. foundValidCenter를 true로 설정해 유효한 중심을 찾았음을 표시합니다.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L129-L135

중심이 과하게 이동하는 경우를 제한: maxShift보다 많이 이동하면 bestCenter를 이전 중심 prevCenter로 설정합니다.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L137-L142

bestCenter를 velocityHistory에 저장합니다.

기록이 historySize를 초과하면 가장 오래된 기록을 제거합니다.

https://github.com/vinpple/Line_Tracer/blob/4b5e9b3bd4b9849d93af1aa2a2006e142f93477b/main.cpp#L145-L160

모터 속도 조절절










