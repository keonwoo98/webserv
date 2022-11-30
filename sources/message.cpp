#include "message.hpp"

Message::Message() {}
Message::~Message() {}

const std::string &Message::GetMessage() const { return header_ + body_; }