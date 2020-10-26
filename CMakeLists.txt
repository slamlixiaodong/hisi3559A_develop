project( ffmpeg_test )
cmake_minimum_required(VERSION 3.5)
message("111111111111111")

include_directories(
        /home/lxd/arm/ffmpeg_camera/include/
)
message(${CMAKE_SOURCE_DIR})
set(ffmpeg_Lib_path "/home/lxd/arm/ffmpeg_camera/lib/")
set(link_libs  ${ffmpeg_Lib_path}libavcodec.so
               ${ffmpeg_Lib_path}libavformat.so
               ${ffmpeg_Lib_path}libswscale.so
               ${ffmpeg_Lib_path}libavdevice.so
               ${ffmpeg_Lib_path}libavutil.so
               ${ffmpeg_Lib_path}libopencv_core.so
               ${ffmpeg_Lib_path}libopencv_highgui.so
               ${ffmpeg_Lib_path}libopencv_imgcodecs.so
               ${ffmpeg_Lib_path}libopencv_imgproc.so
               ${ffmpeg_Lib_path}libopencv_videoio.so

               pthread
        )

add_executable(Main main.cpp )
target_link_libraries(Main ${link_libs})


