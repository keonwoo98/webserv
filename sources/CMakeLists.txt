add_library(webserv_lib STATIC
	config/config_parser.cpp
	config/config_utils.cpp
	config/error_pages.cpp
	config/index.cpp
	config/location_info.cpp
	config/server_info.cpp
	config/allow_methods.cpp

	core/client_socket.cpp
	core/event_executor.cpp
	core/fd_handler.cpp
	core/kqueue_handler.cpp
	core/resolve_uri.cpp
	core/server_socket.cpp
	core/socket.cpp
	core/udata.cpp
	core/webserv.cpp
	core/logger.cpp

	exception/http_exception.cpp

	request/request_chunked_parser.cpp
	request/request_message_getter_setter.cpp
	request/request_message.cpp
	request/request_parser.cpp
	request/request_validation.cpp
	request/uri.cpp

	response/cgi_handler.cpp
	response/date.cpp
	response/get_method.cpp
	response/header.cpp
	response/http_version.cpp
	response/post_method.cpp
	response/response_message.cpp
	response/status_line.cpp
	response/cgi_parser.cpp

	util/util.cpp
)

target_include_directories(webserv_lib PUBLIC 
	${CMAKE_SOURCE_DIR}/includes/const
	${CMAKE_SOURCE_DIR}/includes/config
	${CMAKE_SOURCE_DIR}/includes/core
	${CMAKE_SOURCE_DIR}/includes/exception
	${CMAKE_SOURCE_DIR}/includes/request
	${CMAKE_SOURCE_DIR}/includes/response
	${CMAKE_SOURCE_DIR}/includes/util
)

target_compile_options(webserv_lib PRIVATE -Wall -Wextra -Werror -std=c++98 -g3)
