function(MAKE_SINGLE_EXAMPLE EXAMPLE_FILE PROJECT_FOLDER ADDITIONAL_LIBS)					# Begin of function declaration
    set(EXAMPLE_SOURCE_FILE ${EXAMPLE_FILE})												# Create variable "EXAMPLE_SOURCE_FILE" and assign the function parameter "EXAMPLE_FILE" to it (i.e. a single .cpp file)
    file(RELATIVE_PATH PATH_TO_EXAMPLE_FILE ${CMAKE_CURRENT_SOURCE_DIR} ${EXAMPLE_FILE})	# Compute the relative path from directory "CMAKE_CURRENT_SOURCE_DIR" to the file "EXAMPLE_FILE" and store it in the variable "PATH_TO_EXAMPLE_FILE"
    get_filename_component(EXAMPLE_NAME ${PATH_TO_EXAMPLE_FILE} NAME_WE)					# Set the variable "NAME" Get a specific component of a full filename. NAME_WE = File name without directory or longest extension
	set(EXECUTABLE "${EXAMPLE_NAME}")														# Set the variable "EXECUTABLE" to the value of "EXAMPLE_NAME"
    add_executable(${EXECUTABLE} ${EXAMPLE_SOURCE_FILE})									# Create executable using the source file specified in the "EXAMPLE_SOURCE_FILE" variable
    target_link_libraries(${EXECUTABLE} "${ADDITIONAL_LIBS}" ${3rdPartyLibs})				# Add the 3rdPartyLibs and the additional libraries passed as an argument to the function
    set_target_properties(${EXECUTABLE} PROPERTIES FOLDER ${PROJECT_FOLDER})				# Put the executable in the folder specified by the "PROJECT_FOLDER" function argument
endfunction()																				# End of function declaration

function(MAKE_ALL_EXAMPLES PATH_TO_EXAMPLES PROJECT_FOLDER ADDITIONAL_LIBS)					# Begin of function declaration
    file(GLOB ALL_EXAMPLES ${PATH_TO_EXAMPLES})												# Collect all files in the directory specified by the function parameter "PATH_TO_EXAMPLES" and store them in the list "ALL_EXAMPLES"
    list(SORT ALL_EXAMPLES)																	# Sort the list "ALL_EXAMPLES"
    foreach (EXAMPLE_FILE ${ALL_EXAMPLES})													# Loop over all files in the list "ALL_EXAMPLES"
        MAKE_SINGLE_EXAMPLE(${EXAMPLE_FILE} ${PROJECT_FOLDER} "${ADDITIONAL_LIBS}")			# Call helper function MAKE_SINGLE_EXAMPLE for every file and pass the function parameters "PROJECT_FOLDER" and "ADDITIONAL_LIBS" along
    endforeach()																			# End of loop over all fines in the list "ALL_EXAMPLES"
endfunction() 																				# End of function declaration

###########################################################
# CommonLib
#
# Add the CommonLib library and related include directories

add_subdirectory(CommonLib)                                              					# Add subfolder "CommonLib"
set_target_properties(CommonLib PROPERTIES FOLDER "1stPartyLibs")     						# Put the library "CommonLib" into the folder "1stPartyLibs"
include_directories(CommonLib)																# Add the "CommonLib" subdirectory to the include path
include_directories(${CMAKE_CURRENT_BINARY_DIR}/CommonLib)									# ?????

##############################################################
# KinectLib
#
# If the Kinect v2 SDK was found during configuration, add the 
# CommonLib library and related include directories

if(HAVE_KINECT)																				# Check whether the Kinect v2 SDK was found.
	add_subdirectory(KinectLib)                                              				# Add subfolder "KinectLib"
	set_target_properties(KinectLib PROPERTIES FOLDER "1stPartyLibs")     					# Add the library "KinectLib" as project to VS under the folder "1stPartyLibs"
	include_directories(KinectLib)															# Add the "KinectLib" subdirectory to the include path
	include_directories(${CMAKE_CURRENT_BINARY_DIR}/KinectLib)								# ?????
endif()																						# End of Kinect v2 SDK check

##################################################################################
# SmartGesture
#
# Library containing function to build the SmartGesture example

add_subdirectory(SmartGestureLib)                                                                 # Add subfolder "CommonLib"
set_target_properties(SmartGestureLib PROPERTIES FOLDER "1stPartyLibs")                           # Put the library "CommonLib" into the folder "1stPartyLibs"
include_directories(SmartGestureLib)                                                              # Add the "CommonLib" subdirectory to the include path
include_directories(${CMAKE_CURRENT_BINARY_DIR}/SmartGestureLib)                                                                                        # End of Kinect v2 SDK check

##############################################################
# ExamplesCommon
#
# Common examples demonstrating the usage of the GLFWFramework

set(ExamplesCommonLibs CommonLib)															# Store names of additional link libraries in variable "ExamplesCommonLibs"
MAKE_ALL_EXAMPLES(ExamplesCommon/*.cpp "ExamplesCommon" "${ExamplesCommonLibs}")			# Create all examples specified by the wildcard "ExamplesCommon/*.cpp", put them in the project folder "ExamplesCommon" and link against additional libraries "ExamplesCommonLibs"

##################################################################################
# ExamplesKinect
#
# Examples demonstrating the use of the Kinect SDK together with the GLFWFramework

if(HAVE_KINECT)                                                                             # Check whether the Kinect v2 SDK was found.
    set(ExamplesKinectLibs CommonLib KinectLib SmartGestureLib)                                             # Store names of additional link libraries in variable "ExamplesKinectLibs"
    MAKE_ALL_EXAMPLES(ExamplesKinect/*.cpp "ExamplesKinect" "${ExamplesKinectLibs}")        # Create all examples specified by the wildcard "ExamplesKinect/*.cpp", put them in the project folder "ExamplesKinect" and link against additional libraries "ExamplesKinectLibs"
endif()                                                                                     # End of Kinect v2 SDK check
