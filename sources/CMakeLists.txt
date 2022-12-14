add_library(webserv_lib STATIC
        client_socket.cpp
        server_socket.cpp
        socket.cpp
        webserv.cpp
        kqueue_handler.cpp
        date.cpp
        message/body.cpp
        message/header.cpp
        message/response_message.cpp
        message/status_line.cpp
        message/http_version.cpp
        get_method.cpp
        cgi_handler.cpp
        delete_method.cpp http_exception.cpp message.cpp request_chunked_message.cpp request_message.cpp request_parser.cpp response_message.cpp)

target_include_directories(webserv_lib PUBLIC ${CMAKE_SOURCE_DIR}/includes/)

target_compile_options(webserv_lib PRIVATE -Wall -Wextra -Werror -std=c++98)
