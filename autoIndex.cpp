#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>

// Define a function to list the contents of a directory and its subdirectories
void list_directory(const std::string& dir_path) {
    // Open the directory
    DIR* dir = opendir(dir_path.c_str());
    if (dir == nullptr) {
        std::cerr << "Error: Could not open directory " << dir_path << std::endl;
        return;
    }

    // Create vectors to store the paths of files and directories
    std::vector<std::string> files;
    std::vector<std::string> directories;

    // Iterate over each item in the directory
    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip the "." and ".." entries
        std::string name = entry->d_name;
        if (name == "." || name == "..") {
            continue;
        }

        // Build the full path of the item
        std::string path = dir_path + "/" + name;

        // Use the lstat function to determine whether the item is a file or directory
        struct stat statbuf;
        if (lstat(path.c_str(), &statbuf) == -1) {
            std::cerr << "Error: Could not stat " << path << std::endl;
            continue;
        }

        // If the item is a directory, add its path to the directories vector
        // and print the prefix "[DIR]" before the path
        if (S_ISDIR(statbuf.st_mode)) {
            directories.push_back(path);
            std::cout << "[DIR] ";
        }
        // If the item is a file, add its path to the files vector
        // and print the prefix "[FILE]" before the path
        else {
            files.push_back(path);
            std::cout << "[FILE] ";
        }
        // Print the full path of the item to the console
        std::cout << path << std::endl;
    }

    // Close the directory
    closedir(dir);

    // Sort the files and directories vectors alphabetically
    std::sort(files.begin(), files.end());
    std::sort(directories.begin(), directories.end());

    // Recursively call the list_directory function for each subdirectory
    std::for_each(directories.begin(), directories.end(), list_directory);
}


int main() {
    std::string assets_path = "/Users/ib/Desktop/webserv/assets";
    list_directory(assets_path);
    return 0;
}


