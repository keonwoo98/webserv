#include "kqueue_handler.hpp"

KqueueHandler::KqueueHandler() { kq_ = kqueue(); }

KqueueHandler::~KqueueHandler() {}

struct kevent KqueueHandler::CreateEvent(uintptr_t ident, int16_t filter,
										 uint16_t flags, uint32_t fflags,
										 intptr_t data, void *udata) {
	struct kevent event;

	EV_SET(&event, ident, filter, flags, fflags, data, udata);
	// change_list_.push_back(event);
	return event;
}

void KqueueHandler::addEventToChangeList(struct kevent event) {
	change_list_.push_back(event);
}

int  KqueueHandler::MonitorEvents(struct kevent *event_list) {
	int n_of_events;

	if ((n_of_events = kevent(kq_, (struct kevent *)change_list_.data(),
							  change_list_.size(), event_list, sizeof(event_list),
							  NULL)) < 0) {
		perror("kevent");
		exit(EXIT_FAILURE);
	}
	change_list_.clear();
	return n_of_events;
}
