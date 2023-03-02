#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>

class Request {

	public:
		Request(std::string buff);
		~Request(void);

		// std::string getters() {return _;} ??

	private:
        std::string _request; //tum bufferi buraya koysak? acaba isimize yarar mi? gereksiz
        //GET / HTTP/1.1
        std::string _reqMethod; // GET
        std::string _reqUrl; //buna gerek olmayabilir "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
        std::string _reqHost;
        std::string _reqPort;
        std::string _reqUri; //yukardaki abs_path
        std::string _reqVersion; // HTTP/1.1
//bizim bastirdigimiz header da body yok simdilik  bunu yapalim. body eklememiz lazim POST icin gerekli
        std::string _reqBody;
//mesela acaba burda bir sikinti oldugunda direk bir error pageine mi yonlendirmeliyiz ona bakalim.


};

std::ostream& operator<<(std::ostream &os, Request& obj);


#endif