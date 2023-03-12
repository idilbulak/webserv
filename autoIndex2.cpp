#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <dirent.h>
#include <sys/stat.h>

std::vector<std::string> list_directory(const std::string& dir_path) {
    std::vector<std::string> output;

    DIR* dir = opendir(dir_path.c_str());
    if (dir == nullptr) {
        output.push_back("Error: Could not open directory " + dir_path);
        return output;
    }

    std::vector<std::string> files;
    std::vector<std::string> directories;

    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") {
            continue;
        }

        std::string path = dir_path + "/" + name;

        struct stat statbuf;
        if (lstat(path.c_str(), &statbuf) == -1) {
            output.push_back("Error: Could not stat " + path);
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            directories.push_back(path);
            output.push_back("[DIR] " + path);
        } else {
            files.push_back(path);
            std::ostringstream oss;
            oss << "[FILE] " << std::setw(8) << statbuf.st_size << "  " << std::put_time(std::localtime(&statbuf.st_mtime), "%Y-%m-%d %H:%M:%S") << "  " << std::oct << (statbuf.st_mode & 0777) << std::dec << "  " << path;
            output.push_back(oss.str());
        }
    }

    closedir(dir);

    std::sort(files.begin(), files.end());
    std::sort(directories.begin(), directories.end());

    for (const auto& sub_dir : directories) {
        auto sub_dir_output = list_directory(sub_dir);
        output.insert(output.end(), sub_dir_output.begin(), sub_dir_output.end());
    }

    return output;
}

void print_directory_listing(const std::vector<std::string>& listing) {
    for (const auto& line : listing) {
        std::cout << line << std::endl;
    }
}

int main() {
    print_directory_listing(list_directory("./assets"));
    return 0;
}