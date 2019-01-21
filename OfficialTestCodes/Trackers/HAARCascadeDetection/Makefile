
CPP = main.cpp

OPENCV="/usr/local/include/opencv"

OPENCV2 = "/usr/local/include/opencv2"

LOCALLIB = /usr/local/lib

ARG = -Iinclude -Llib -lrealsense -lm -I${OPENCV} -I${OPENCV2} -L${LOCALLIB} `pkg-config --cflags --libs glfw3 glu gl` -std=c++11 -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy -lopencv_stitching -I${LOCALLIB}


all: compile
	./a.out

compile: ${CPP}
	g++ $^ ${ARG}
