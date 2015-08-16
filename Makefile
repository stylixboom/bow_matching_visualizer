#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I${HOME}/local/include
CFLAGS = -std=c++11 -Wall -fexceptions
RESINC = 
LIBDIR = -L${HOME}/local/lib
LIB = ../lib/alphautils/libalphautils.a ../lib/sifthesaff/libsifthesaff.a ../lib/ins/libins.a -lopencv_stitching -lopencv_videostab -lopencv_gpu -lopencv_legacy -lopencv_ts -lopencv_nonfree -lopencv_contrib -lopencv_calib3d -lopencv_objdetect -lopencv_features2d -lopencv_video -lopencv_photo -lopencv_highgui -lopencv_flann -lopencv_imgproc -lopencv_ml -lopencv_core -lx264 -lfaac -llapacke -llapack -lblas -ltmglib -lgomp
LDFLAGS = `pkg-config opencv --libs` `pkg-config --libs lapacke` -lgomp -lrt -lpthread -ldl

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/bow_matching_visualizer

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -march=corei7 -O3
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/bow_matching_visualizer

OBJ_DEBUG = $(OBJDIR_DEBUG)/bow_matching_visualizer.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/bow_matching_visualizer.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/bow_matching_visualizer.o: bow_matching_visualizer.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c bow_matching_visualizer.cpp -o $(OBJDIR_DEBUG)/bow_matching_visualizer.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/bow_matching_visualizer.o: bow_matching_visualizer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c bow_matching_visualizer.cpp -o $(OBJDIR_RELEASE)/bow_matching_visualizer.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

