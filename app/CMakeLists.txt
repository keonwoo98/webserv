add_executable(webserv main.cpp ../sources/post_method.cpp)

target_link_libraries(webserv PUBLIC webserv_lib)
