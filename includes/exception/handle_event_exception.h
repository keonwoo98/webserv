//
// Created by Alvin Lee on 2022/12/25.
//

#ifndef WEBSERV_HANDLE_EVENT_EXCEPTION_H
#define WEBSERV_HANDLE_EVENT_EXCEPTION_H

#include <exception>

class HandleEventExeption {
public:
    class OpenExeption : std::exception {
        const char *what() const throw() { return "open() Failed"; }
    };

    class RecvExeption : std::exception {
        const char *what() const throw() { return "recv() Failed"; }
    }
};

#endif //WEBSERV_HANDLE_EVENT_EXCEPTION_H
