aux_source_directory(src LIB_SRC_FILES)
add_library(camera SHARED ${LIB_SRC_FILES})
target_include_directories(camera PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include PUBLIC /opt/MVS/include PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_directories(camera PRIVATE /opt/MVS/lib/64)
target_link_libraries(camera PRIVATE MvCameraControl PRIVATE ${OpenCV_LIBS})

