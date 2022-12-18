add_library(webserv_lib STATIC
        cgi_handler.cpp
        client_socket.cpp
        config_parser.cpp
        date.cpp
        delete_method.cpp
        get_method.cpp
        http_exception.cpp
        kqueue_handler.cpp
        location.cpp
        post_method.cpp
        request_chunked_message.cpp
        request_message.cpp
        request_parser.cpp
        response_message.cpp
        server.cpp
        server_socket.cpp
        socket.cpp
        uri.cpp
        webserv.cpp)

target_include_directories(webserv_lib PUBLIC ${CMAKE_SOURCE_DIR}/includes/)

target_compile_options(webserv_lib PRIVATE -Wall -Wextra -Werror -std=c++98)
