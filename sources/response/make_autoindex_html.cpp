#include <iostream>
#include <sstream>
#include <dirent.h>

// uri가 디렉토리로 들어온다 가정 , 당연히 autoindex option on 이고 /
std::string MakeAutoindexHtml(const std::string &dir_uri)
{
    // Open the directory
    DIR* dir = opendir(dir_uri.c_str());
    if (!dir)
    {
        return "Error: Could not open directory";
    }
    std::stringstream ss;
    // Write the HTML header
    ss << "<html>\n<head>\n<title>Directory Index</title>\n</head>\n<body>\n<h1>Directory Index</h1>\n<ul>\n";
    // Iterate through the directory entries
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        // Skip hidden files
        if (entry->d_name[0] == '.')
            continue;
        // Write a list item for the file
        ss << "<li><a href=\"" << entry->d_name << "\">" << entry->d_name << "</a></li>\n";
    }
    // Write the HTML footer
    ss << "</ul>\n</body>\n</html>\n";
    closedir(dir);
    return ss.str();
}
