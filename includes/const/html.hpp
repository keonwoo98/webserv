#include <string.h>

const std::string auto_index_prefix = "<html>\n"
									  "	<head>\n"
									  "		<title> Directory Index</title>\n"
									  "	</head>\n"
									  "	<body>\n"
									  "		<h1>Directory Index</h1>"
									  "	<ul>\n";

const std::string auto_index_suffix = "	</ul>\n"
									  "	</body>\n"
									  "</html>\n";

const std::string delete_ok_html = "<html>\n"
								   "  <body>\n"
								   "    <h1>File deleted.</h1>\n"
								   "  </body>\n"
								   "</html>";

const std::string default_error_html = "<!DOCTYPE html>"
									   "<html>"
									   "  <head>"
									   "    <title> Default Error Page </title>"
									   "  </head>"
									   "  <body>"
									   "    <h1> Page Not Found </h1>"
									   "    <p>The page you are looking for could not be found on our server.</p>"
									   "  </body>"
									   "</html>";
