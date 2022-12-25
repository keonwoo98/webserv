add_executable(webserv main.cpp ../includes/request/request_validation.h ../includes/core/fd_handler.h ../includes/exception/handle_event_exception.h)

target_link_libraries(webserv PUBLIC webserv_lib)
