//
// Created by 김민준 on 2023/01/03.
//

#include "auto_index.hpp"
#include "status_code.hpp"
#include "http_exception.hpp"
#include <dirent.h>
#include <sstream>

std::string AutoIndexHtml(const std::string &path, const std::string &dir_list) {
	std::stringstream ss;
	ss << "<!DOCTYPE html>\n"
	   << "<html>\n"
	   << "<head>\n"
	   << "<title>Index of " << path << "</title>\n"
	   << "</head>\n"
	   << "<body>\n"
	   << "<h1> Index of " << path << "</h1>\n"
	   << "<hr>\n"
	   << "<pre>\n"
	   << "<a href=\"../\">../</a>\n"
	   << dir_list
	   << "</pre>\n"
	   << "<hr>\n"
	   << "</body>\n"
	   << "</html>";
	return ss.str();
}

std::string MakeDirList(const std::string &dir_uri) {
	// Open the directory
	DIR *dir = opendir(dir_uri.c_str());
	if (!dir) {
		throw HttpException(NOT_FOUND, "Error: Could not open directory");
	}
	std::stringstream ss;
	// Write the HTML header
	// Iterate through the directory entries
	struct dirent *entry;
	while ((entry = readdir(dir)) != nullptr) {
		// Skip hidden files
		if (entry->d_name[0] == '.')
			continue;
		// Write a list item for the file
		ss << "<a href=\"" << entry->d_name << "\">" << entry->d_name << "</a>\n";
	}
	// Write the HTML footer
	closedir(dir);
	return ss.str();
}
