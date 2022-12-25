add_executable(webserv main.cpp ../includes/request/request_validation.h)

target_link_libraries(webserv PUBLIC webserv_lib)
