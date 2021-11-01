# Note:
# In this CMakeLists.txt, GLOBbing is used for both header
# and source files. According to the CMake specification, 
# GLOBing should not be used for source files. However, in
# case CMake is re-run everytime a new source file is added,
# GLOBbing .cpp files does not pose a problem.

file(GLOB HEADER_FILES *.h)                                       	# Get all header files in current dir and store them in HEADER_FILES
file(GLOB SOURCE_FILES *.cpp)										# Get all source files in current dir and store them in SOURCE_FILES
source_group(inc FILES ${HEADER_FILES})                           	# Create VS source group "inc" with all header files
source_group(src FILES ${SOURCE_FILES})                           	# Create VS source group "src" with all source files    		 

include_directories(${CMAKE_CURRENT_BINARY_DIR})                  	# Add current binary dir to include dirs (location in build folder - WHY???)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})                  	# Add current source dir to include dirs (where this CMakeLists.txt is located)

add_library(KinectLib STATIC ${HEADER_FILES} ${SOURCE_FILES})     	# Create static library including all source and header files
target_link_libraries(KinectLib ${3rdPartyLibs})                	# Link against 3rdParty libraries

add_custom_command(TARGET KinectLib POST_BUILD 						# Add post build custom command for KinectLib, which is executed everytime the library is build
                   COMMAND ${CMAKE_COMMAND} -E copy 				# The command copies a file...
				   ${KinectSDK2_DIR}/bin/Kinect20.Face.dll 			# which is located here...
				   $<TARGET_FILE_DIR:KinectLib>)					# to the output directory of the KinectLib target
				   
add_custom_command(TARGET KinectLib POST_BUILD 						# Add post build custom command for KinectLib, which is executed everytime the library is build
                   COMMAND ${CMAKE_COMMAND} -E copy_directory		# The command copies the contents of a directory
				   ${KinectSDK2_DIR}/bin/NuiDatabase 				# which is located here...
				   $<TARGET_FILE_DIR:KinectLib>/NuiDatabase)		# to the output directory of the KinectLib target
				  