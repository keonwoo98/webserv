#include "request_message.hpp"
#include <sstream>
#include <fstream>
#include <locale>

RequestMessage::RequestMessage() {
	std::string filePath = "ex/request-headers";

	std::ifstream openFile(filePath.data());
	if (openFile.is_open()) {
		std::string line;
		while (getline(openFile, line)) {
			valid_header_name_.insert(line);
		}
		openFile.close();
	}
}

RequestMessage::~RequestMessage() {}

const std::string &RequestMessage::GetMethod() const {
	return method_;
}

const std::string &RequestMessage::GetUri() const {
	return uri_;
}

const RequestMessage::header_map_type &RequestMessage::GetHeaderMap() const {
	return header_map_;
}

void RequestMessage::ParsingMessage(const std::string &message) {
	std::string buf;
	size_t index;

	index = message.find("R\n");
	buf = message.substr(0, index + 1);
	ParsingStartLine(buf);

	index = message.find("R\nR\n");
	buf = message.substr(buf.length(), index- buf.length() + 2);
	ParsingHeaders(buf);

	body_ = message.substr(index + 4, message.length() - (index + 4));
}

void RequestMessage::ParsingStartLine(const std::string start_line) {
	std::stringstream ss(start_line);
	std::string buf;

	std::getline(ss, method_, ' ');
	if (method_ != "GET" && method_ != "POST" && method_ != "DELETE")
		std::cerr << "error: " << method_ << std::endl;

	std::getline(ss, uri_, ' ');

	std::getline(ss, buf, 'R');
	if (buf != "HTTP/1.1")
		std::cerr << "error: " << buf << std::endl;
}

void IsValidHeaderName(std::string name, std::string value) {
	
}

void RequestMessage::SplitHeader(const std::string &header) {
	std::string name;
	std::string value;
	size_t index;

	index = header.find(": ");
	name = header.substr(0, index);
	value = header.substr(index + 2, header.length());
	header_map_[name] = value;
}

void RequestMessage::ParsingHeaders(const std::string headers) {
	size_t start = 0;
	size_t end;

	end = headers.find("R\n");
	size_t i = 0;
	while (end != std::string::npos) {
		SplitHeader(headers.substr(start, end - start));
		start = end + 2;
		end = headers.find("R\n", start);
	}
}

/*여기서 body에 넘어오는 문자열은 헤더와 바디를 구분하는 CRLF다음부터 메시지의 끝까지이다.*/
void RequestMessage::ParsingBody(const std::string &message_body)
{
	if (is_chunked_ == false)
	{
		this->body_.reserve(content_size_);
		this->body_.assign(message_body, 0, content_size_);
	}
	else
	{
		enum chunk_state
		{ 
			CHUNK_SIZE = 0, 
			CHUNK_EXTENSION,
			CHUNK_SIZE_CRLF,
			CHUNK_BODY,
			CHUNK_LASTBODY,
			CHUNK_CRLF,
			CHUNK_CRLF2,
			CHUNK_TRAILER,
			CHUNK_END
		};

		chunk_state state = CHUNK_SIZE;
		size_t chunk_size = 0;
		std::string chunk_size_str = "";
		for (int i = 0 ; state != CHUNK_END ; i++)
		{
			switch (state)
			{
				case CHUNK_SIZE:
					chunk_size = 0;
					while (std::isdigit(message_body[i]))
						chunk_size_str.push_back(message_body[i++]);
					chunk_size = std::atoi(chunk_size_str.c_str());
					state = (message_body[i] == 'R') ? CHUNK_SIZE_CRLF : CHUNK_EXTENSION;
					break;

				case CHUNK_EXTENSION:
					while (message_body[i] != 'R')
						i++;
					state = CHUNK_SIZE_CRLF;
					break;
				
				case CHUNK_SIZE_CRLF:
					if (message_body[i] != '\n')
						return ; // ERROR
					state = (chunk_size == 0) ? CHUNK_LASTBODY : CHUNK_BODY;
					break;
				
				case CHUNK_BODY:
					while (std::isdigit(message_body[i]))
						chunk_size_str.push_back(message_body[i++]);
					state = (message_body[i] == 'R') ? CHUNK_CRLF : CHUNK_TRAILER;
					break;
				
				case CHUNK_LASTBODY:
					break;
				
				case CHUNK_TRAILER:
					while (message_body[i] != 'R')
						i++;
					state = CHUNK_CRLF2;
					break;
				
				case CHUNK_CRLF:
				if (message_body[i] != '\n')
					return ; // ERROR

					break;

				case CHUNK_CRLF2:
					
					break;
				
				default :
					return ; //ERROR
			}
		}
		std::cout << "body parse done" << std::endl;
	}

}

bool RequestMessage::isTchar(char c)
{
	return (std::isalnum(c) || c == '!' || c == '#' || c == '$' \
	|| c == '%' || c == '&' || c == '\'' || c == '|' || c == '~');
}