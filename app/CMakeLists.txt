# 현재 디렉토리에 있는 모든 파일을 SRC_FILES 변수에 추가한다.
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(webserv ${SRC_FILES} ../includes/status_code.hpp)

target_link_libraries(webserv PUBLIC webserv_lib)
