#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <iostream>
#include <fstream>

#define CRFL "\r\n"
  
class Response {

	public:
		Response();
		~Response(void);

        std::string read_html_file(const std::string& filename);
        void errResponse(int err);
        void getResponse(int err);
        std::string statuscode(int cd);
		std::string getRes() {return _res;};

	private:
        std::string _res;
        int         _statusCode;


        

};

std::ostream& operator<<(std::ostream &os, Response& obj);


#endif