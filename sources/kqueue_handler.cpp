#include "kqueue_handler.hpp"

KqueueHandler::KqueueHandler() { kq_ = kqueue(); }

KqueueHandler::~KqueueHandler() {}

void KqueueHandler::CollectEvents(uintptr_t ident, int16_t filter,
                                  uint16_t flags, uint32_t fflags,
                                  intptr_t data, void *udata) {
    struct kevent event;

    EV_SET(&event, ident, filter, flags, fflags, data, udata);
    change_list_.push_back(event);
}

const std::vector<struct kevent> &KqueueHandler::MonitorEvents() {
    struct kevent tmp[8];
    int n_of_events;

    if ((n_of_events = kevent(kq_, (struct kevent *)change_list_.data(),
                              change_list_.size(), tmp, 8, NULL)) < 0) {
        perror("kevent");
        exit(EXIT_FAILURE);
    }
    event_list_.clear();
    change_list_.clear();
    for (int i = 0; i < n_of_events; ++i) {
        event_list_.push_back(tmp[i]);
    }
    return event_list_;
}
