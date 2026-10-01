add_library(camera SHARED src/camera.cpp src/SDKmanager.cpp)
target_include_directories(camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PRIVATE /opt/MVS/include)
target_link_directories(camera PRIVATE /opt/MVS/lib/64)
target_link_libraries(camera PRIVATE MvCameraControl)

