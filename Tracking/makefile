OPENCV= -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_calib3d -lopencv_legacy
FLAGS= -Wall -g --std=c++11
X11FLAG = -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
OBJECTS = MKalman.o \
	 Tracker.o \
	TrackObj.o \
	baratas.o \
	Util.o \
	BlobDetector.o

exec: $(OBJECTS)
	g++ $(LIBS) $(X11FLAG)  $(FLAGS) -o exec $(OBJECTS)

baratas.o: baratas.cpp Util.o
	g++ $(FLAGS) -I./dlib/ -c baratas.cpp

MKalman.o:MKalman.cpp MKalman.hpp
	g++ $(FLAGS) -c MKalman.cpp

Tracker.o: Tracker.cpp Tracker.hpp TrackObj.hpp
	g++  $(FLAGS) -I./dlib/ ./dlib/dlib/all/source.cpp -c Tracker.cpp

TackObj.o: TrackObj.cpp TrackObj.hpp
	g++ $(FLAGS)  -c TrackObj.cpp

Util.o: ../Util.cpp
	g++ $< -c $(X11FLAG)

BlobDetector.o: ../BlobDetector.cpp
	g++ $(FLAGS) -c ../BlobDetector.cpp

clean:
	rm *~ *.o exec
