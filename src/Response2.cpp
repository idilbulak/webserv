
// std::string Request::cgiRes() {
// 	std::string buff = executeCgi();
// 	// Find the index of the empty line
// 	size_t pos = buff.find("\r\n\r\n");
// 	// If the empty line was found, extract the substring that follows it
// 	if (pos != std::string::npos)
// 	{
// 		_res.body = buff.substr(pos + 4);
// 		// Find the "Content-type" header and extract the value
// 		size_t typePos = buff.find("Content-type:");
// 		if (typePos != std::string::npos)
// 		{
// 			size_t endPos = buff.find("\r\n", typePos);
// 			_res.type = buff.substr(typePos + 14, endPos - (typePos + 14));
// 		}
// 		// Find the "Status" header and extract the value
// 		size_t codePos = buff.find("Status: ");
// 		if (codePos == 0)
// 			_res.code = 201;
// 		else if (codePos != std::string::npos)
// 		{
// 			size_t endPos = buff.find("\r\n", codePos);
// 			_res.type = buff.substr(codePos + 8, endPos - (codePos + 8));
// 		}
// 	}
// 	std::cout << buff << "body" << std::endl;
// 	_res.version = "HTTP/1.1 ";
// 	return htmlRes();
// }



// so the function returns true. Otherwise, it means that the file does not exist,
// so the function returns false.
// bool Request::fileExists(const char* filename) {
//     struct stat buffer;
//     return (stat(filename, &buffer) == 0);
// }

// std::string Request::findValidFile(Location loc) {
//     for (int i = 0; i < loc.index.size(); i++) {
//         std::string filename = loc.root + "/" + loc.index[i];
//         if (fileExists(filename.c_str()))
//             return filename;
//     }
//     return "";
// }

// static int writeContent(const std::string &content, const std::string &path)
// {
// 	std::ofstream	file;

// 		file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
// 		if (file.is_open() == false)
// 			return (403);
// 		return (201);
// }
