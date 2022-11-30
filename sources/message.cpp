#include "message.hpp"

Message::Message() {}
Message::~Message() {}

const std::string Message::GetMessage() { return header_ + body_; }
