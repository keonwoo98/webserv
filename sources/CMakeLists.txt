# 현재 디렉토리에 있는 모든 파일을 SRC_FILES 변수에 추가한다.
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(webserv_lib STATIC ${SRC_FILES})

target_include_directories(webserv_lib PUBLIC ${CMAKE_SOURCE_DIR}/includes)

target_compile_options(webserv_lib PRIVATE -Wall -Wextra -Werror -std=c++98)

