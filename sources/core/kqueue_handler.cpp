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
	if (kq_ > 0) {
		close(kq_);
	}
}

void KqueueHandler::AddReadEvent(uintptr_t ident, void *udata) {
	CollectEvents(ident, EVFILT_READ, EV_ADD, 0, 0, udata);
}

void KqueueHandler::AddWriteEvent(uintptr_t ident, void *udata) {
	CollectEvents(ident, EVFILT_WRITE, EV_ADD, 0, 0, udata);
}

void KqueueHandler::AddWriteLogEvent(uintptr_t ident, void *udata) {
	CollectEvents(ident, EVFILT_WRITE, EV_ONESHOT, 0, 0, udata);
}

void KqueueHandler::AddProcExitEvent(uintptr_t ident) {
	CollectEvents(ident, EVFILT_PROC, EV_ONESHOT, NOTE_EXIT, 0, 0);
}

void KqueueHandler::DeleteEvent(const struct kevent &event) {
	struct kevent set_event = {};
	EV_SET(&set_event, event.ident, event.filter, EV_DELETE, 0, 0, NULL);
	kevent(kq_, &set_event, 1, NULL, 0, NULL);
}

void KqueueHandler::CollectEvents(uintptr_t ident, int16_t filter,
								  uint16_t flags, uint32_t fflags,
								  intptr_t data, void *udata) {
	struct kevent event = {};

	EV_SET(&event, ident, filter, flags, fflags, data, udata);
	change_list_.push_back(event);
}

std::vector<struct kevent> KqueueHandler::MonitorEvent() {
	struct kevent event_list[8];
	std::vector<struct kevent> ret;

	int number_of_events = -1;
	while (number_of_events < 0) {	// kevent error handling
		number_of_events = kevent(kq_, &(change_list_[0]), change_list_.size(),
								  event_list, 8, NULL);
	}
	for (int i = 0; i < number_of_events; ++i) {
		ret.push_back(event_list[i]);
	}
	change_list_.clear();
	return ret;
}
