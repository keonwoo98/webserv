#ifndef KQUEUE_HANDLER_HPP
#define KQUEUE_HANDLER_HPP

#include <sys/event.h>

#include <vector>

class KqueueHandler {
   public:
	KqueueHandler();
	~KqueueHandler();

	struct kevent CreateEvent(uintptr_t ident, int16_t filter, uint16_t flags,
					   uint32_t fflags, intptr_t data, void *udata);
	int MonitorEvents(struct kevent *event_list);
	void addEventToChangeList(struct kevent event);

   private:
	int kq_;
	std::vector<struct kevent> change_list_;
};

#endif
