//
// Created by 김민준 on 2022/12/27.
//

#include "logger.hpp"

Logger::Logger(const std::string &message) : message_(message) {
}

std::string &Logger::GetMessage() {
	return message_;
}



