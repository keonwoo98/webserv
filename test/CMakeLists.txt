include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.0
)

FetchContent_MakeAvailable(googletest)

add_executable(webserv_test
        main.cpp
        config_utils_method_test.cpp
        post_method_test.cpp
        status_line_test.cpp
        header_test.cpp
        response_message_test.cpp
        response_event_test.cpp
        kqueue_handler_test.cpp
#        send_response_test.cpp)
        )
target_compile_options(webserv_test PRIVATE -std=c++11)

target_link_libraries(webserv_test PRIVATE webserv_lib gtest gmock gtest_main)
