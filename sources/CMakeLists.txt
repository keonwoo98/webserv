add_library(webserv_lib STATIC
        client_socket.cpp
        server_socket.cpp
        socket.cpp
        webserv.cpp
        kqueue_handler.cpp
        message/body.cpp
        message/header.cpp
        message/response_message.cpp
        message/status_line.cpp
        message/http_version.cpp
        get_method.cpp
        delete_method.cpp)

target_include_directories(webserv_lib PUBLIC ${CMAKE_SOURCE_DIR}/includes/)

target_compile_options(webserv_lib PRIVATE -Wall -Wextra -Werror -std=c++98)
