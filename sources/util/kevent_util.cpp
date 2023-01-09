#include "webserv.hpp"

bool IsProcessExitEvent(const struct kevent &event) {
	return event.fflags & NOTE_EXIT;
}

bool IsEventHasData(const struct kevent &event) {
	return event.data > 0;
}

/**
* EV_EOF
* - If the read direction of the socket has shutdown (socket)
* - When the last writer disconnects (pipe)
*/
bool IsDisconnected(const struct kevent &event) {
	return (event.flags & EV_EOF) && Webserv::FindClientSocket((int) event.ident);
}

bool IsLogEvent(const struct kevent &event) {
	return (int) event.ident == Webserv::error_log_fd_ || (int) event.ident == Webserv::access_log_fd_;
}

bool IsReadEvent(const struct kevent &event) {
	return event.filter == EVFILT_READ;
}
