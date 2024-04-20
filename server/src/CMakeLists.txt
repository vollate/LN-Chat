set(SOURCE_FILES main.cpp)

add_executable(ln_server ${SOURCE_FILES})
target_include_directories(ln_server PRIVATE ${HEADER_PATH})
