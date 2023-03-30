// #include "Response.hpp"

// std::string str = _req.getUri().path;

// bool Response::findLocation(Location* loc, std::string str) {
//     if(str.empty())
//         return false;
//     // check if the full path is existing in one of the location blocks.
//     for (int i = 0; i < _server.locations.size(); i++) {
//         if (_server.locations[i].path.compare(str) == 0) {
//             *loc = _server.locations[i];
//             // _path = str;
//             return true;
//         }
// 	}
//     // if we cant find the lcoation block, extract the string that is next to the last slash
//     std::string::size_type last_slash_pos = str.find_last_of('/');
//     std::string::size_type last_dot_pos = str.find_last_of('.');
//     if (last_slash_pos != std::string::npos) {
//         _path.clear();
//         _file.clear();
//         _ext.clear();
//         _path = str.substr(0, last_slash_pos);
//         _file = _req.getUri().path.substr(last_slash_pos + 1);
//         if (last_dot_pos != std::string::npos)
//             _ext = str.substr(last_dot_pos + 1);
//     }
//     if (!_ext.empty()) {
// 		std::string newPath = "~\\." + _ext + "$";
// 		for (int i = 0; i < _server.locations.size(); i++) {
// 			if (_server.locations[i].path.compare(newPath) == 0) {
// 				*loc = _server.locations[i];
// 				return true;
//             }
// 		}
//     }
//     for (int i = 0; i < _server.locations.size(); i++) {
//         if (_server.locations[i].path.compare(_path) == 0) {
//             std::string filePath;
//             if(!_server.locations[i].root.empty())
//                 filePath = _server.locations[i].root + "/" + _file;
//             else
//                 filePath = _file;
//             if fileExists(filePath) {
//                 *loc = _server.locations[i];
//                 _indxFile = filePath;
//                 return true;
//             }
//         }
	
//     }
//     return findLocation(loc, _path);
// }
