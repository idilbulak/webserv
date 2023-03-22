#include "../inc/CheckConfFile.hpp"


CheckConfFile::~CheckConfFile()
{
}


std::vector<std::string> CheckConfFile::split(std::string str, std::string delimiter) {
    std::vector<std::string> words;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        if (!token.empty()) {
            words.push_back(token);
        }
        str.erase(0, pos + delimiter.length());
    }
    if (!str.empty()) {
        words.push_back(str);
    }
    if (words.size() == 0)
        words.push_back("");
    return words;
}


void CheckConfFile::checkKey(const std::string path)
{
    std::ifstream ifs(path.c_str());
    if (!ifs.is_open())
    {
        std::cerr << "File does not exits:" << path << std::endl;
        exit(1);
    }
    std::string line;
    while (getline(ifs, line))
    {
        if (!checkServerKey(split(line, " ")[0])){
            std::cout << "{"<<split(line, " ")[0] << "}"<< std::endl;
            throw std::invalid_argument("invalid Key");
        }
    }

}

CheckConfFile::CheckConfFile(const std::string path) : serverBrackets(false), locationBrackets(false)
{
    std::ifstream ifs(path.c_str());
    if (!ifs.is_open())
    {
        std::cerr << "File does not exist: " << path << std::endl;
        exit(1);
    }
    std::string token;
    std::vector<std::string> tokens;
    while (ifs >> token)
    {
        if (token.find("{") != std::string::npos && token.size() > 1)
        {
            if (token.size() != token.find("{") + 1)
                throw std::invalid_argument("brackets error 7");
            token.resize(token.find("{"));
            tokens.push_back(token);
            tokens.push_back("{");
        }
        else
            tokens.push_back(token);
    }
    checkBrackets(tokens);
    checkKey(path);
}



bool CheckConfFile::checkLocationKey(std::string word)
{
     int count = 0;
    if (!getLocationBrackets())
        return false;
    for (int i = 0; word[i]; i++)
    {
        if (word[i] == ' ' || word[i] == '\t')
            count++;
        else
            break;
    }
    if (word.size() == count)
        return true;
    word.erase(0, count);
    if (word == "allow" || word == "autoindex" || word == "index" || word == "upload_dir" || word == "cgi_ext"  || word == "return")
        return true;
    return false;
}



//server key have "root"???????? @idil
bool CheckConfFile::checkServerKey(std::string word)
{
    int count = 0;
    
    for (int i = 0; word[i]; i++)
    {
        if (word[i] == ' ' || word[i] == '\t')
            count++;
        else
            break;
    }
    if (word.size() == count)
        return true;
    word.erase(0, count);
    if (word == "{")
        return true;
    else if(word == "}")
    {
        setLocationBrackets(false);
        return true;
    }
    if (word == "location")
    {
        if (getLocationBrackets())
            return false;
        else{
            setLocationBrackets(true);
            return true;
            }
    }
    else if (getLocationBrackets())
        return checkLocationKey(word);
    else if (word == "root" || word == "server" || word == "{" || word == "}" || word == "listen" || word == "location" || word == "client_max_body_size" || word == "server_name" || word == "error_page" )
        return true;
    return false;
}

void CheckConfFile::checkBrackets(std::vector<std::string> tokens)
{
    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].compare("{") == 0)
        {

            if (i < 0)
            {
                throw std::invalid_argument("brackets error! 1");
            }
            if (tokens[i - 1].compare("server") == 0 && getServerBrackets() == false)
            {
                setServerBrackets(true);
            }
            else if (tokens[i - 2].compare("location") == 0 && getLocationBrackets() == false)
            {
                setLocationBrackets(true);
            }
            else
            {
                std::cout << " burda " << tokens[i] << tokens[i - 1] << getServerBrackets() << getLocationBrackets() << std::endl;
                throw std::invalid_argument("brackets error 2");
            }
        }

        if (tokens[i].compare("}") == 0)
        {
            if (getLocationBrackets() == true)
            {
                setLocationBrackets(false);
                if (i < tokens.size())
                {
                    if (!(tokens[i + 1].compare("}") == 0 || tokens[i + 1].compare("location") == 0))
                        throw std::invalid_argument("brackets error 4");
                }
            }
            else if (getServerBrackets() == true)
            {
                setServerBrackets(false);
                if (i < tokens.size())
                {
                    if ((tokens[i + 1].compare("server") != 0) && (i + 1) < tokens.size())
                    {
                        std::cout << "size is" << tokens.size() << "i " << i << "[" << tokens[i + 1] << "]" << std::endl;
                        throw std::invalid_argument("brackets error 5");
                    }
                }
            }
            else
                throw std::invalid_argument("brackets error 3");
        }
    }

    if (getServerBrackets() || getLocationBrackets())
        throw std::invalid_argument("brackets error (dont close)");
}

// getter setter
//---------------------------Brackets------------------------------------
void CheckConfFile::setServerBrackets(bool isOpen) { serverBrackets = isOpen; }
void CheckConfFile::setLocationBrackets(bool isOpen) { locationBrackets = isOpen; }
bool CheckConfFile::getLocationBrackets() { return locationBrackets; }
bool CheckConfFile::getServerBrackets() { return serverBrackets; }