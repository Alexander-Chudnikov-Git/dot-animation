#OBJS specifies which files to compile as part of the project
OBJS = main.cpp loaderwindow.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I C:\MinGW\x86_64-w64-mingw32\include

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L C:\MinGW\x86_64-w64-mingw32\lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -std=c++17 -pthread -municode

#LINKER_FLAGS specifies the libraries we're linking againsts
LINKER_FLAGS = -lSDL2 -lSDL2_ttf -lSDL2_image -Wl,-subsystem,windows -lopengl32 -lfreeglut

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = loader

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)