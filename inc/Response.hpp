#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "HttpRequest.hpp"
#include "AutoIndex.hpp"

class Response {
	public:
		Response(std::string buff, Config cf);
		~Response(void);
		
		void			makeCodeMap();
		
		VirtualServer	findServer(HttpRequest req);
		bool			findLocation(std::string reqUri, Location* loc, VirtualServer server);
		bool			validMethod(Location loc, std::string method);
		bool			fileExists(const char* filename);
		void			setIndxFile();
		bool			checkIndx();
		std::string		generate();
		std::string		getRes();
		std::string		postRes();
		std::string		delRes();
		std::string		res();
		std::string		read_html_file(const std::string& fileName);
		std::string		statuscode(int cd);
		std::string		codeRes(int err);
		void			moveFile(const std::string& source_path, const std::string& destination_path);

	private:
		std::map<int, std::string>	_codeMap;
		HttpRequest				    _req;
		Config						_cf;
		Location        			_loc;
		std::string 			    _indxFile;
		int						    _cgiOn;
		std::string					_version;
    	std::string 				_type;
    	std::string					_body;
    	std::string					_filename;
    	int							_code;
		// Response        _res;
		// std::map<std::string, std::string> _env;
};

#endif