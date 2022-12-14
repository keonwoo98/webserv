add_executable(webserv main.cpp ../sources/http_exception.cpp ../sources/message.cpp ../sources/request_chunked_message.cpp ../sources/request_message.cpp ../sources/request_parser.cpp ../sources/response_message.cpp)

target_link_libraries(webserv PUBLIC webserv_lib)
