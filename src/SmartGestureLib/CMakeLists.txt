# Note:
# In this CMakeLists.txt, GLOBbing is used for both header
# and source files. According to the CMake specification, 
# GLOBing should not be used for source files. However, in
# case CMake is re-run everytime a new source file is added,
# GLOBbing .cpp files does not pose a problem.
file(GLOB SOURCE_FILES *.h *.cpp)                        # Get all header and cpp files in current dir and store them in SOURCE_FILES
source_group(core FILES ${SOURCE_FILES})                 # Create VS source group "core" for SOURCE_FILES

foreach(dir organs screen socket gui)                          # Loop over all subdirectories specified by name
    file(GLOB SUBDIR_FILES ${dir}/*)                     # Use GLOBbing to colled all header and cpp files from each individual subdirectory and store them in SUBDIR_FILES
    list(APPEND SOURCE_FILES ${SUBDIR_FILES})            # Append additional files from current subdirectory to SUBDIR_FILES
	source_group(${dir} FILES ${SUBDIR_FILES})           # Create VS source group for each subdirectory with associated header and cpp files
endforeach()                                             # End loop over all subdirectories 

include_directories(${CMAKE_CURRENT_BINARY_DIR})         # Add current binary dir to include dirs (location in build folder - WHY???)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})         # Add current source dir to include dirs (where this CMakeLists.txt is located)

add_library(SmartGestureLib STATIC ${SOURCE_FILES})            # Create static library including all source and header files
target_link_libraries(SmartGestureLib ${3rdPartyLibs})         # Link against 3rdParty libraries