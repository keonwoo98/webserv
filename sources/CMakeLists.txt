add_library(webserv_lib STATIC
	config/config_parser.cpp
	config/server_info.cpp
	config/location_info.cpp

	core/client_socket.cpp
	core/kqueue_handler.cpp
	core/server_socket.cpp
	core/socket.cpp
	core/webserv.cpp
	core/udata.cpp
	core/event_handler.cpp

	exception/http_exception.cpp

	request/request_chunked_message.cpp
	request/request_message.cpp
	request/request_parser.cpp
	request/uri.cpp

	response/cgi_handler.cpp
	response/date.cpp
	response/delete_method.cpp
	response/get_method.cpp
	response/post_method.cpp
	response/response_message.cpp
	response/header.cpp
	response/http_version.cpp
	response/status_line.cpp
)

target_include_directories(webserv_lib PUBLIC 
	${CMAKE_SOURCE_DIR}/includes/character_const
	${CMAKE_SOURCE_DIR}/includes/config
	${CMAKE_SOURCE_DIR}/includes/core
	${CMAKE_SOURCE_DIR}/includes/exception
	${CMAKE_SOURCE_DIR}/includes/request
	${CMAKE_SOURCE_DIR}/includes/response
)

target_compile_options(webserv_lib PRIVATE -Wall -Wextra -Werror -std=c++98)
