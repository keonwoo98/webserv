include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.0
)

FetchContent_MakeAvailable(googletest)

add_executable(webserv_test
        main.cpp
        http_method/post_method_test.cpp

        config/config_utils_method_test.cpp

        core/kqueue_handler_test.cpp
        core/kqueue_file_read.cpp
        core/server_socket_test.cpp
        core/socket_addr.cpp

        http_method/post_method_test.cpp
        http_method/delete_method_test.cpp
        http_method/get_method_test.cpp

        response/status_line_test.cpp
        response/header_test.cpp
        response/build_response_test.cpp
        response/send_response_test.cpp

        uri/resolve_uri_test.cpp

        core/kqueue_handler_test.cpp
        core/server_socket_test.cpp)

target_compile_options(webserv_test PRIVATE -std=c++11)

target_link_libraries(webserv_test PRIVATE webserv_lib gtest gmock gtest_main)
