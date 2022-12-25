#include "server_socket.hpp"
#include "client_socket.hpp"
#include "kqueue_handler.hpp"
#include "response_message.hpp"
#include "request_message.hpp"

class EventHandler {
public:
    static const int RESPONSE_END = 0;
    static const int CLOSE = 1;
    static const int KEEP_ALIVE = 2;
    static const int ERROR = 3;
    static const int HAS_MORE = 4;

    enum RequestState {
        REQUEST_DONE = 0,
        REQUEST_ERROR
    };

    static int HandleListenEvent(ServerSocket server_socket);

    static int HandleRequestEvent(const ClientSocket &client_socket,
                                  Udata *user_data);

    static int HandleResponseEvent(const ClientSocket &client_socket,
                                   Udata *user_data);
};
