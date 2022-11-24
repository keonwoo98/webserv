#include "kqueue.hpp"

int main(void) {
	Kqueue kq;

	try {
		kq.MonitorKevent();
	} catch (const std::exception &e) {
		std::cerr << e.what() << '\n';
	}

	return 0;
}
