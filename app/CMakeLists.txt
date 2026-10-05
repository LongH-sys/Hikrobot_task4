aux_source_directory(src APP_SRC_FILES)
add_executable(main ${APP_SRC_FILES})
target_link_libraries(main PRIVATE camera)
