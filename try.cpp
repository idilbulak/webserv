#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>

std::vector<std::string> getAssetPaths() {
    std::vector<std::string> assetPaths;

    DIR *dir = opendir("/Users/bulak/Desktop/webserv/assets");
    if (dir == NULL) {
        std::cerr << "Error: unable to open directory assets/" << std::endl;
        return assetPaths;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        // Construct full path
        std::string fullPath = "assets/" + std::string(ent->d_name);

        // Check if it's a directory
        struct stat st;
        if (stat(fullPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
            // Recursively add all files and directories inside
            std::vector<std::string> subPaths = getAssetPaths();
            assetPaths.insert(assetPaths.end(), subPaths.begin(), subPaths.end());
        }

        // Add to list
        assetPaths.push_back(fullPath);
    }

    closedir(dir);
    return assetPaths;
}

std::vector<std::string> getAssetPaths();

int main() {
    std::vector<std::string> assetPaths = getAssetPaths();
    for (const auto& path : assetPaths) {
        std::cout << path << std::endl;
    }
    return 0;
}

