# 收集所有模型文件
file(GLOB MODEL_FILES "${CMAKE_SOURCE_DIR}/model/*")

add_custom_target(model ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/client
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/server

    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/model ${CMAKE_BINARY_DIR}/client
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/model ${CMAKE_BINARY_DIR}/server
    DEPENDS ${MODEL_FILES}
)
