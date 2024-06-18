# Sets the minimum version of CMake required to build the native
# library.
cmake_minimum_required(VERSION  3.10.2)

project(ofxOpenCVAndroid LANGUAGES CXX)
set(TARGET_ANDROID TRUE)

set(CMAKE_VERBOSE_MAKEFILE on)

set(LOCAL_PATH ${CMAKE_SOURCE_DIR})
set(OFXOPENCV_ROOT ${LOCAL_PATH}/../../)
set(OFXOPENCV_SOURCE_PATH ${OFXOPENCV_ROOT}/src)
set(OPENCV_LIBS_ROOT ${OFXOPENCV_ROOT}/libs/opencv)

set(OFX_OPENCV_ANDROID_OUTPUT ${OFXOPENCV_ROOT}/libs/android)

# Paths to the OF Android lib

set(PURE_OF_ROOT ${LOCAL_PATH}/../../../../)
set(CORE_OF_ROOT ${PURE_OF_ROOT}/libs/openFrameworks)
set(OF_LIBS_ROOT ${PURE_OF_ROOT}/libs)

set(OF_ANDROID ${PURE_OF_ROOT}/libs/openFrameworksCompiled/project/android)
set(OF_ANDROID_OUTPUT ${PURE_OF_ROOT}/libs/openFrameworksCompiled/lib/android)


macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

# Custom function to check if the library is built
function(check_library)
    if (NOT TARGET openFrameworksAndroid)
        message(STATUS "openFrameworksAndroid Library not found. Building library...")

        # Invoke the build process for the library
        execute_process(
                COMMAND ${CMAKE_COMMAND} --build ${OF_ANDROID}/
                RESULT_VARIABLE result
        )
        if (result)
            message(FATAL_ERROR "Failed to build the library.")
        endif ()
    endif ()
endfunction()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -error-limit=0 -std=c17 -Oz -frtti -Wall -fno-short-enums -fPIE -fPIC -fuse-ld=gold -fexceptions -ffunction-sections -fdata-sections ")
set(CMAKE_CPP_FLAGS "${CMAKE_C_FLAGS} -error-limit=0 -std=c++17 -Oz -stdlib=libc++ -frtti -Wall -fno-short-enums -fPIE -fPIC -fuse-ld=gold -fexceptions -ffunction-sections -fdata-sections")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-export-dynamic")

set(distribution_DIR ${OFX_OPENCV_ANDROID_OUTPUT})

# Creates the project's shared lib: libnative-lib.so.
# The lib is loaded by this project's Java code in MainActivity.java:
#     System.loadLibrary("native-lib");
# The lib name in both places must match.
# Specify your native library name, type and the cpp source files.
add_library( ofxOpenCVAndroid
		SHARED
		${OFXOPENCV_SOURCE_PATH}/ofxCvColorImage.cpp
		${OFXOPENCV_SOURCE_PATH}/ofxCvContourFinder.cpp
		${OFXOPENCV_SOURCE_PATH}/ofxCvFloatImage.cpp
		${OFXOPENCV_SOURCE_PATH}/ofxCvGrayscaleImage.cpp
		${OFXOPENCV_SOURCE_PATH}/ofxCvHaarFinder.cpp
		${OFXOPENCV_SOURCE_PATH}/ofxCvImage.cpp
		${OFXOPENCV_SOURCE_PATH}/ofxCvShortImage.cpp
)

set_target_properties(ofxOpenCVAndroid PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY "${distribution_DIR}/${ANDROID_ABI}"
)

# Specifies a path to native header files.
include_directories(
        # openFrameworks headers
        ${CORE_OF_ROOT}/3d
        ${CORE_OF_ROOT}/app
        ${CORE_OF_ROOT}/communication
        ${CORE_OF_ROOT}/events
        ${CORE_OF_ROOT}/gl
        ${CORE_OF_ROOT}/graphics
        ${CORE_OF_ROOT}/math
        ${CORE_OF_ROOT}/sound
        ${CORE_OF_ROOT}/types
        ${CORE_OF_ROOT}/utils
        ${CORE_OF_ROOT}/video
		${CORE_OF_ROOT}

		# OpenCV Headers
		${OPENCV_LIBS_ROOT}/include

        # openFrameworks Libs includes
        ${OF_LIBS_ROOT}/FreeImage/include
        ${OF_LIBS_ROOT}/freetype/include
        ${OF_LIBS_ROOT}/freetype/include/freetype2
        ${OF_LIBS_ROOT}/freetype/include/freetype2/freetype/config
        ${OF_LIBS_ROOT}/freetype/include/freetype2/freetype/internal
        ${OF_LIBS_ROOT}/freetype/include/freetype2/freetype/internal/services
        ${OF_LIBS_ROOT}/glm/include
        ${OF_LIBS_ROOT}/pugixml/include
        ${OF_LIBS_ROOT}/json/include
        ${OF_LIBS_ROOT}/tess2/include
        ${OF_LIBS_ROOT}/utf8/include
        ${OF_LIBS_ROOT}/uriparser/include
)

find_library(android-lib android)
find_library(log-lib log)
find_library(GLES1-lib GLESv1_CM)
find_library(GLES2-lib GLESv2)
find_library(GLES3-lib GLESv3)

#   [18, 24)         ES2 & ES3
#   [24, infinite)   ES2 & ES3 & Vulkan

if (${ANDROID_PLATFORM_LEVEL} LESS 24)
	set(OPENGL_LIB GLESv2)
else ()
	set(OPENGL_LIB GLESv2)
endif (${ANDROID_PLATFORM_LEVEL} LESS 24)

target_link_libraries(ofxOpenCVAndroid
		android
		EGL
		GLESv2
#		GLESv3
		log
		m
		z
		dl
		)

target_link_libraries( ofxOpenCVAndroid
		${android-lib} )
target_link_libraries( ofxOpenCVAndroid
		${GLES2-lib} )
#target_link_libraries( ofxOpenCVAndroid
#		${GLES3-lib} )
target_link_libraries( ofxOpenCVAndroid
		${GLES1-lib} )
target_link_libraries( ofxOpenCVAndroid
		${log-lib} )

# Set the third-party library header files directory.
#include_directories(src/main/cpp/Libraries/include/)
# Link the thrid-party library with your native library.
target_link_libraries( ofxOpenCVAndroid 
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libade.a 
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/liblibopenjp2.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_calib3d.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_core.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_dnn.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_features2d.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_flann.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_gapi.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_highgui.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_imgcodecs.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_imgproc.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_ml.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_objdetect.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_photo.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_stitching.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_video.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libopencv_videoio.a
		${OPENCV_LIBS_ROOT}/lib/android/${ANDROID_ABI}/libquirc.a

		# link with the openFrameworks Library for each ABI
		${OF_ANDROID_OUTPUT}/${ANDROID_ABI}/libopenFrameworksAndroid.so

)


