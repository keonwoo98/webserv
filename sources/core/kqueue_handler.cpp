/**
 * #include <sys/event.h>
 * int kqueue(void);
 *
 * int kevent(int kq, const struct kevent *changelist, int nchanges,
 * 		struct kevent *eventlist, int nevents, const struct timespec *timeout);
 *
 * EV_SET(kev, ident, filter, flags, fflags, data, udata);
 */
#include "kqueue_handler.hpp"
#include "core_exception.hpp"

#include <unistd.h>

KqueueHandler::KqueueHandler() {
	kq_ = kqueue();
	if (kq_ < 0) {
		throw CoreException::KQueueException();
	}
}

KqueueHandler::~KqueueHandler() {
	close(kq_);
}

void KqueueHandler::AddReadEvent(uintptr_t ident, void *udata) {
	CollectEvents(ident, EVFILT_READ, EV_ADD, 0, 0, udata);
}

void KqueueHandler::AddWriteEvent(uintptr_t ident, void *udata) {
	CollectEvents(ident, EVFILT_WRITE, EV_ADD, 0, 0, udata);
}

void KqueueHandler::AddWriteLogEvent(uintptr_t ident, void *udata) {
	CollectEvents(ident, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, udata);
}

void KqueueHandler::AddProcExitEvent(uintptr_t ident) {
	CollectEvents(ident, EVFILT_PROC, EV_ADD | EV_ONESHOT, NOTE_EXIT, 0, 0);
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

	struct kevent event = {};
	while (true) { // kevent error handling
		int number_of_events = kevent(kq_, change_list, (int)list_size, &event, 1, NULL);
		if (number_of_events == 1) {
			break;
		}
	}
	return event;
}
