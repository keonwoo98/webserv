#include "kqueue_handler.hpp"

#include "core_exception.hpp"

/**
 * #include <sys/event.h>
 * int kqueue(void);
 *
 * int kevent(int kq, const struct kevent *changelist, int nchanges,
 * 		struct kevent *eventlist, int nevents, const struct timespec *timeout);
 *
 * EV_SET(kev, ident, filter, flags, fflags, data, udata);
 */

KqueueHandler::KqueueHandler() {
	kq_ = kqueue();
	if (kq_ < 0) {
		throw CoreException::KQueueException();
	}
}

KqueueHandler::~KqueueHandler() {}

void KqueueHandler::AddReadEvent(uintptr_t ident, void *udata) {
	CollectEvents(ident, EVFILT_READ, EV_ADD, 0, 0, udata);
}

void KqueueHandler::AddWriteEvent(uintptr_t ident, void *udata) {
	CollectEvents(ident, EVFILT_WRITE, EV_ADD, 0, 0, udata);
}

void KqueueHandler::DeleteReadEvent(uintptr_t ident) {
	CollectEvents(ident, EVFILT_READ, EV_DELETE, 0, 0, 0);
}

void KqueueHandler::DeleteWriteEvent(uintptr_t ident) {
	CollectEvents(ident, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
}

void KqueueHandler::CollectEvents(uintptr_t ident, int16_t filter,
								  uint16_t flags, uint32_t fflags,
								  intptr_t data, void *udata) {
	struct kevent event;

	EV_SET(&event, ident, filter, flags, fflags, data, udata);
	change_list_.push_back(event);
}

struct kevent KqueueHandler::MonitorEvent() {
	size_t list_size = change_list_.size();
	struct kevent *change_list = new struct kevent[list_size];

	for (size_t i = 0; i < list_size; i++) {
		change_list[i] = change_list_[i];
	}
	change_list_.clear();

	struct kevent event;
	int n_of_events = kevent(kq_, change_list, list_size, &event, 1, NULL);
	if (n_of_events != 1) {
		perror("kevent");
		exit(EXIT_FAILURE);
	}
	return event;
}
