#ifndef KQUEUE_HANDLER_HPP
#define KQUEUE_HANDLER_HPP

#include <sys/event.h>

#include <vector>

class KqueueHandler {
public:
    KqueueHandler();
    ~KqueueHandler();

    void AddReadEvent(uintptr_t ident, void *udata);
    void AddWriteEvent(uintptr_t ident, void *udata);
    void DeleteReadEvent(uintptr_t ident, void *udata);
    void DeleteWriteEvent(uintptr_t ident, void *udata);

    const std::vector<struct kevent> &MonitorEvents();

private:
    int kq_;
    std::vector<struct kevent> change_list_;
    std::vector<struct kevent> event_list_;

    void CollectEvents(uintptr_t ident, int16_t filter, uint16_t flags,
                       uint32_t fflags, intptr_t data, void *udata);
};

#endif
