#ifndef WEBSERV_INCLUDES_UTIL_EVENT_UTIL_H_
#define WEBSERV_INCLUDES_UTIL_EVENT_UTIL_H_

bool IsLogEvent(const struct kevent &event);
bool IsDisconnected(const struct kevent &event);
bool IsEventHasData(const struct kevent &event);
bool IsProcessExitEvent(const struct kevent &event);
bool IsReadEvent(const struct kevent &event);
#endif //WEBSERV_INCLUDES_UTIL_EVENT_UTIL_H_
