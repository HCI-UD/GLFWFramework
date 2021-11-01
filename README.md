===============================
======== GLFWFramework ========
===============================

Simple GLFW framework for simple and easy prototyping of OpenGL applications.

# System Requirements
This framework makes extensive use of C++11 and C++14 features, thus only compilers which support these features
are supported (e.g. Visual Studio 2013 and newer).

# Dependencies
The only dependencies are OpenCV 3.1 (required) and the Microsoft v2 Kinect SDK (only required for Kinect examples).

OpenCV:

1) Create system environment variable OpenCV_DIR = <PATH_TO_OPENCV_DIRECTORY>\opencv\build which contains the file "OpenCVConfig.cmake".

2) Add <PATH_TO_OPENCV_DIRECTORY>\opencv\build\x64\{vc12/vc14}\bin to the system PATH variable, so that the DLL will be found


Kinect:

1) Download the Microsoft Kinect SDK 2.0 (https://www.microsoft.com/en-us/download/details.aspx?id=44561) and install

# Folder Structure
The framework contains a folder 3rdParty with common 3rdParty libraries (GLFW, GLEW, GLM, etc.). Those are build
automatically when CMake is run. Additionally, there are two main 1stParty libraries, one for common OpenGL
operations and functions, and one for Kinect functionality.

# Example Structure
Each .cpp file in the src folder is an individual project, which is build when CMake is run. It contains a 
class with (preferrably) the same name as the .cpp file, as well as a main function, when the Run() method
of the class is called. All examples inherit from the abstract base class GLFWApp, which defines a set of
functions, that are called in a specific order during the Run() method:

1) Setup
2) PreCreate
3) CreateRenderingTarget
4) PostCreate
5) InitGL
	Main loop:
	6) Update
	7) Draw
	8) FinishFrame
9) ShutdownGL
10) Cleanup

One of those, the Draw function, is purely virtual functions and has to be overridden in any
example. The others are optional, and used for setting up OpenGL and the scene. In order to create a new example, 
simply add a new .cpp file in the src folder and name it accordingly.

========================================================================================================================================