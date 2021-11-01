project(GLFWFramework)
cmake_minimum_required(VERSION 3.4)

###############################################################################
# Set output path for libraries and executables
#
# Executables are copied into /bin and libraries into /lib folder in build dir

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

###############################################################################
# Add directory "cmake" to CMakeModulePath
#
# The CMakeModulePath will be searched for *.cmake files. Such files enable the
# FIND_PACKAGE command.

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

###############################################################################
# Eigen - Math library for matrices, vectors and more complex functionality 
#
# Header only library, we only need to provide the include directory

include_directories(${CMAKE_SOURCE_DIR}/3rdParty/eigen_3_2_8)

###############################################################################
# GLEW - Cross platform access to OpenGL extensions and > 1.x functions 
#
# For further information about the GLEW library, see: http://glew.sourceforge.net/

add_definitions(-DGLEW_STATIC)
add_subdirectory(3rdParty/glew)
set_target_properties(glew PROPERTIES FOLDER "3rdPartyLibs")
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/glew/include)
list(APPEND 3rdPartyLibs glew)

###############################################################################
# GLFW - Cross platform OpenGL window creation and input handling
#
# We need to set a bunch of values so we build only the library and not the 
# install targets, documentation or examples

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target")
add_subdirectory(3rdParty/glfw_3_2)
set_target_properties(glfw PROPERTIES FOLDER "3rdPartyLibs")
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/glfw_3_2/include)
list(APPEND 3rdPartyLibs glfw ${GLFW_LIBRARIES})

###############################################################################
# freeType

add_subdirectory(3rdParty/freetype_2_6_3)
set_target_properties(freetype PROPERTIES FOLDER "3rdPartyLibs/freetype")
set_target_properties(dist PROPERTIES FOLDER "3rdPartyLibs/freetype")
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/freetype_2_6_3/include)
list(APPEND 3rdPartyLibs freetype)

###############################################################################
# Kinect v2 SDK
#
# CMake searches in CMAKE_MODULE_PATH for a FindKinectSDKv2.cmake

find_package(KinectSDKv2)
if (KinectSDK2_FOUND)
	include_directories(${KinectSDK2_DIR}/inc)
	list(APPEND 3rdPartyLibs ${KinectSDK2_LIBRARIES})
	set(HAVE_KINECT 1)
else()
    message(STATUS "Kinect v2 SDK NOT found")
endif()

###############################################################################
# OpenCV - a Computer vision library with advanced image loading and 
# manipulation functionality, including a simple API for accessing cameras  
#  
# OpenCV will be found if you set an environment variable OpenCV_DIR 
# pointing to the location of the installed OpenCVConfig.cmake file

set(OpenCV_STATIC OFF)
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND) 
    message(STATUS "OpenCV Found")
    include_directories(${OpenCV_INCLUDE_DIRS})
    list(APPEND 3rdPartyLibs ${OpenCV_LIBS})
	set(HAVE_OPENCV 1)
else()
    message(STATUS "OpenCV NOT found")
endif()
	
###############################################################################
# GLM - Vector / matrix header only math library based on the syntax of GLSL

include_directories(${CMAKE_SOURCE_DIR}/3rdParty/glm_0_9_7_2)

###############################################################################
# QRGenerator 

add_definitions(-DHAVE_CONFIG_H)
add_subdirectory(3rdParty/qr)
set_target_properties(qrcoder PROPERTIES FOLDER "3rdPartyLibs")
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/qr/)
list(APPEND 3rdPartyLibs qrcoder)


###############################################################################
# ASSIMP
set(ENV{_NO_DEBUG_HEAP} "1")
option(ASSIMP_BUILD_3DS_IMPORTER OFF)
option(ASSIMP_BUILD_3DS_IMPORTER OFF)
option(ASSIMP_BUILD_3D_IMPORTER OFF)
option(ASSIMP_BUILD_AC_IMPORTER OFF)
option(ASSIMP_BUILD_ASE_IMPORTER OFF)
option(ASSIMP_BUILD_ASSBIN_IMPORTER OFF)
option(ASSIMP_BUILD_ASSXML_IMPORTER OFF)
option(ASSIMP_BUILD_B3D_IMPORTER OFF)
option(ASSIMP_BUILD_BLEND_IMPORTER OFF)
option(ASSIMP_BUILD_BVH_IMPORTER OFF)
option(ASSIMP_BUILD_COB_IMPORTER OFF)
option(ASSIMP_BUILD_COLLADA_IMPORTER OFF)
option(ASSIMP_BUILD_CSM_IMPORTER OFF)
option(ASSIMP_BUILD_DXF_IMPORTER OFF)
option(ASSIMP_BUILD_FBX_IMPORTER OFF)
option(ASSIMP_BUILD_HMP_IMPORTER OFF)
option(ASSIMP_BUILD_IFC_IMPORTER OFF)
#option(ASSIMP_BUILD_IRR_IMPORTER OFF)
option(ASSIMP_BUILD_LWO_IMPORTER OFF)
option(ASSIMP_BUILD_LWS_IMPORTER OFF)
option(ASSIMP_BUILD_MD2_IMPORTER OFF)
option(ASSIMP_BUILD_MD3_IMPORTER OFF)
option(ASSIMP_BUILD_MD5_IMPORTER OFF)
option(ASSIMP_BUILD_MDC_IMPORTER OFF)
option(ASSIMP_BUILD_MDL_IMPORTER OFF)
option(ASSIMP_BUILD_MS3D_IMPORTER OFF)
option(ASSIMP_BUILD_NDO_IMPORTER OFF)
option(ASSIMP_BUILD_NFF_IMPORTER OFF)
#option(ASSIMP_BUILD_OBJ_IMPORTER OFF)
option(ASSIMP_BUILD_OFF_IMPORTER OFF)
option(ASSIMP_BUILD_OGRE_IMPORTER OFF)
option(ASSIMP_BUILD_OPENGEX_IMPORTER OFF)
#option(ASSIMP_BUILD_PLY_IMPORTER OFF)
option(ASSIMP_BUILD_Q3BSP_IMPORTER OFF)
option(ASSIMP_BUILD_Q3D_IMPORTER OFF)
option(ASSIMP_BUILD_RAW_IMPORTER OFF)
option(ASSIMP_BUILD_SMD_IMPORTER OFF)
option(ASSIMP_BUILD_STL_IMPORTER OFF)
option(ASSIMP_BUILD_TERRAGEN_IMPORTER OFF)
option(ASSIMP_BUILD_XGL_IMPORTER OFF)
option(ASSIMP_BUILD_X_IMPORTER OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_INSTALL_PDB OFF)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "ASSIMP Export" FORCE)

add_subdirectory(3rdParty/assimp_3_2)
set_target_properties(assimp PROPERTIES FOLDER "3rdPartyLibs")
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/assimp_3_2/include)
list(APPEND 3rdPartyLibs assimp)


###############################################################################
# asio - Cross-platform C++ header-only library for network and low-level I/O 
#
# ASIO_STANDALONE definition is required to make asio run without boost

add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_HAS_STD_ADDRESSOF)
add_definitions(-DASIO_HAS_STD_ARRAY)
add_definitions(-DASIO_HAS_CSTDINT)
add_definitions(-DASIO_HAS_STD_SHARED_PTR)
add_definitions(-DASIO_HAS_STD_TYPE_TRAITS)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/asio_1_10_6/include)

###############################################################################
# Source code subdirectory

add_subdirectory(src)