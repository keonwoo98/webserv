#include <map>
#include <string>
#include <vector>

class RequestHeader {
   private:
	std::map<std::string, std::vector<std::string>> header_;

   public:
	RequestHeader(std::string &request_header);
	void parse(std::string &requestHeader);
	~RequestHeader();
};

RequestHeader::RequestHeader(std::string &request_header) {
	parse(request_header);
}

void RequestHeader::parse(std::string &request_header) {
	// TODO : parse header
	while () {
		// TODO
		switch (expression)
		{
		case /* constant-expression */:
			/* code */
			break;
		
		default:
			break;
		}
	}
}

RequestHeader::~RequestHeader() {}
