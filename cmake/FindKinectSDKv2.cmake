# Adapted from https://gist.github.com/Furkanzmc/3f52ed09d6f24710750b

set(KinectSDK2_FOUND OFF CACHE BOOL "Kinect 2.x SDK found")
set(KinectSDK2_DIR "NOT FOUND" CACHE PATH "Kinect 2.x SDK path")

if(WIN32)
    if(EXISTS $ENV{KINECTSDK20_DIR})
        set(KinectSDK2_FOUND ON CACHE BOOL "Kinect 2.x SDK found" FORCE)
        set(KinectSDK2_DIR $ENV{KINECTSDK20_DIR} CACHE PATH "Kinect 2.x SDK path" FORCE)
		message(STATUS "Found Kinect SDK v2 in ${KinectSDK2_DIR}")
		set(KinectSDK2_LIBRARIES ${KinectSDK2_DIR}/Lib/x64/Kinect20.lib $ENV{KINECTSDK20_DIR}/Lib/x64/Kinect20.Face.lib)
	else()
	    message(KINECT NOT FOUND !!!)
		message(The Kinect library as well as the Kinect examples will not be build!)
		message(Please specify the system environment variable KINECTSDK20_DIR as the directory containing the Kinect bin, inc, and Lib folders!)
    endif()
else()
    message(Microsoft Kinect SDK requires x64 Windows > 8.1)
endif()