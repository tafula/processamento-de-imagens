all: HandCounter_bgRemoval HandCounter VoteMode DialMode Counter

FLAGS = -g -pedantic #-Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wsign-promo -Wnoexcept -Wno-sign-compare -Wno-unused #-Wsign-conversion -Werror

LIBS = -isystem/usr/include `pkg-config --libs opencv`
X11FLAG = -I/usr/X11R6/include -L/usr/X11R6/lib -lX11
CPPVERSION = --std=c++11


clean:
	rm -f HandCounter
	rm -f HandCounter_bgRemoval
	rm -f VoteMode
	rm -f DialMode
	rm -rf Counter
	rm -f *.o
	rm -f proc/*.o
	rm -rf *.dSYM

InteractiveObject.o: proc/InteractiveObject.cpp
	g++ $< -o proc/$@ -c $(CPPVERSION)

BlobDetector.o: proc/BlobDetector.cpp
	g++ $< -o proc/$@ -c $(CPPVERSION)

Util.o: proc/Util.cpp
	g++ $< -o proc/$@ -c $(X11FLAG)

HandCounter_bgRemoval: HandCounter_bgRemoval.cpp Util.o 
	g++ $< -o $@  $(LIBS) $(X11FLAG) proc/Util.o $(CPPVERSION) $(FLAGS)

HandCounter: HandCounter.cpp Util.o BlobDetector.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) proc/Util.o proc/BlobDetector.o $(CPPVERSION) $(FLAGS)

VoteMode: VoteMode.cpp Util.o InteractiveObject.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) proc/Util.o proc/InteractiveObject.o proc/BlobDetector.o $(CPPVERSION) $(FLAGS)

DialMode: DialMode.cpp Util.o InteractiveObject.o BlobDetector.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) proc/Util.o proc/InteractiveObject.o proc/BlobDetector.o $(CPPVERSION) $(FLAGS)

Counter: ObjectCounter.cpp Util.o BlobDetector.o
	g++ $< -o $@  $(LIBS) $(X11FLAG) proc/Util.o proc/BlobDetector.o $(CPPVERSION) $(FLAGS)
