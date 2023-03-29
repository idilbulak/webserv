#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "HttpRequest.hpp"
#include "Cgi.hpp"
#include "AutoIndex.hpp"
#include <dirent.h> // For opendir() and closedir()

class Response {
	public:
		Response(std::string buff, Config cf, std::string port);
		~Response(void);
		
		void			makeCodeMap();
		VirtualServer	findServer();
		std::string		generate();
		// bool			findLocation(Location* loc);
		bool findLocation(Location* loc, std::string str);
		std::string		chooseMethod();
		std::string		errRes(std::string err);
		std::string		res();
		std::string		getMimeType(const std::string &ext);
		std::string		itos(int value);
		std::string		getRes();
		std::string		postRes();
		std::string		delRes();
		std::string		putRes();
		void			parseCgiResponse(void);
		int				pathType(const std::string& path);
		bool			validMethod(std::string method);
		bool			readContent(const std::string &path);
		int				readFile(const std::string &path);
		int writeContent(const std::string &content, const std::string &path);

		bool folderExists(const std::string folder_to_check);

		std::string 	getCgiRes();
		VirtualServer 	getServer();
		HttpRequest 	getReq();

		std::string	cgiOff();

		bool			fileExists(std::string name);
		bool			checkIndx();
		void			setIndxFile();
		Location		getLocation( const std::string &path );
		void			parsePath(const std::string& path);
		std::string		read_html_file(const std::string& fileName);

	private:
		std::map<int, std::string>	_codeMap;
		HttpRequest				    _req;
		Config						_cf;
		VirtualServer   _server;
		Location        			_loc;

		int							_code;
    	int			 				_type;
    	std::string					_body;
    	std::string 				_header;
    	std::map<std::string, std::string> 	_otherHeaders;

		std::string					_cgiRes;
    	int							_cgiCode;
    	std::string					_cgiType;
		int						    _cgiOn;

		std::string 			    _indxFile;
		std::vector<std::string>		_locationdirs;
		int _found;

		std::string _path;
		std::string _file;
		std::string _ext;
    	int							_typePath;
};
		std::string findFirstFileWithExtension(const std::string& directory, const std::string& extension);

#endif