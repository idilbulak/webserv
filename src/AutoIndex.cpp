#include "../inc/AutoIndex.hpp"

AutoIndex::AutoIndex(std::string root)  : _root(root) {
    // return generate_html_page(list_directory(root));
}
AutoIndex::~AutoIndex(){}

std::vector<std::string> AutoIndex::list_directory(const std::string& dir_path) {
    std::vector<std::string> output;
    // Open the directory at dir_path
    DIR* dir = opendir(dir_path.c_str());
    if (dir == NULL) {
        // If it fails to open, add an error message to the output vector and return it
        output.push_back("Error: Could not open directory " + dir_path);
        return output;
    }
    // Create vectors to hold files and directories
    std::vector<std::string> files;
    std::vector<std::string> directories;
    // Loop through each entry in the directory
    dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Get the name of the entry
        std::string name = entry->d_name;
        if (name == "." || name == "..") {
            // Ignore . and .. entries
            continue;
        }
        // Get the full path of the entry
        std::string path = dir_path + "/" + name;
        // Get information about the entry
        struct stat statbuf;
        if (lstat(path.c_str(), &statbuf) == -1) {
            // If stat fails, add an error message to the output vector and continue to the next entry
            output.push_back("Error: Could not stat " + path);
            continue;
        }
        if (S_ISDIR(statbuf.st_mode)) {
            // If the entry is a directory, add it to the directories vector and add a directory link to the output vector
            directories.push_back(path);
            output.push_back("[DIR] <a href=\"" + name + "\">" + name + "</a>");
        } else {
            // If the entry is a file, add it to the files vector and add a file link to the output vector
            files.push_back(path);
            std::ostringstream oss;
            if (_dirname.empty())
                oss << "[FILE] " << std::setw(8) << statbuf.st_size << "  " << std::asctime(std::localtime(&statbuf.st_mtime)) << "  " << std::oct << (statbuf.st_mode & 0777) << std::dec << "  <a href=\"" + name + "\">" + name + "</a>";
            else
                oss << "[FILE] " << std::setw(8) << statbuf.st_size << "  " << std::asctime(std::localtime(&statbuf.st_mtime)) << "  " << std::oct << (statbuf.st_mode & 0777) << std::dec << "  <a href=\"" + _dirname + "/" + name + "\">" + _dirname + "/" + name + "</a>";
            output.push_back(oss.str());
        }
    }
    // Close the directory
    closedir(dir);
    // Sort the files and directories vectors
    std::sort(files.begin(), files.end());
    std::sort(directories.begin(), directories.end());
    // Recursively list the contents of each directory and add the output to the output vector
    for (std::vector<std::string>::const_iterator it = directories.begin(); it != directories.end(); ++it) {
        // output.push_back("[DIR] <a href=\"" + *it + "\">" + *it + "</a>");
        _dirname = *it;
        size_t start = _dirname.find("//");
        _dirname = _dirname.substr(start+2);
        std::cout << _dirname << std::endl;
        std::vector<std::string> sub_dir_output = list_directory(*it);
        output.insert(output.end(), sub_dir_output.begin(), sub_dir_output.end());
    }
    // Return the output vector
    return output;
}

std::string AutoIndex::generate_html_page() {
    const std::vector<std::string> listing =list_directory(_root);
    std::stringstream ss;
    // Output the HTML header
    ss << "<html><head><title>Listing</title></head><body>" << std::endl;
    // Output the listing as an unordered list
    ss << "<ul>" << std::endl;
    for (std::vector<std::string>::const_iterator it = listing.begin(); it != listing.end(); ++it) {
        ss << "<li>" << *it << "</li>" << std::endl;
    }
    ss << "</ul>" << std::endl;
    // Output the HTML footer
    ss << "</body></html>" << std::endl;

    return ss.str();
}

