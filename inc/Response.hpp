#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <map>
#include "Config.hpp"

#define CRFL "\r\n"

class Response {

	public:
		Response(Config cf);
		~Response(void);

		std::string read_html_file(const std::string& filename);
		std::string errRes(int err);
		std::string statuscode(int cd);

	private:
		Config	_cf;
		int		_statusCode;
		
};

#endif