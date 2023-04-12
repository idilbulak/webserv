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

void CheckConfFile::checkArgument(const std::string line){
    std::vector<std::string> parse = split(line, " ");
    // std::cout << "burda" << parse.size() << std::endl;
    for (int i = 0; parse.size() > i; i++)
        parse[i] = deleteSpace(parse[i]);

    if (parse[0].compare("listen") == 0 || parse[0].compare("server_name") == 0 || parse[0].compare("root") == 0 || parse[0].compare("client_max_body_size") == 0 ||  parse[0].compare("autoindex") == 0 ){
        if (parse.size() != 2){
            throw std::invalid_argument(parse[0] + ": argument error");
            exit (1);
        }
    }
    if (parse[0].compare("return") == 0 || parse[0].compare("error_page") == 0){
        if (parse.size() != 3){
            throw std::invalid_argument(parse[0] + ": argument error");
            exit (1);
        }
    }
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
            throw std::invalid_argument("invalid Key");
            exit (1);
        }
        checkArgument(line);
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
            if (token.size() != token.find("{") + 1){
                throw std::invalid_argument("brackets error 7");
                exit (1);
            }
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


std::string CheckConfFile::deleteSpace(std::string word)
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
        return "";
    word.erase(0, count);
    return word;
}

bool CheckConfFile::checkLocationKey(std::string word)
{
    if (!getLocationBrackets())
        return false;
    word = deleteSpace(word);
    if(word.empty())
        return true;
    if ( word == "max_body_size" || word == "cgi_path" || word == "root" || word == "allow" || word == "autoindex" || word == "index" || word == "upload_dir" || word == "cgi_ext"  || word == "return")
        return true;
    return false;
}



//server key have "root"???????? @idil
bool CheckConfFile::checkServerKey(std::string word)
{
    word = deleteSpace(word);
    if (word.empty())
        return true;
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
    else if (word == "root" || word == "server" || word == "{" || word == "}" || word == "listen" || word == "location" || word == "server_name" || word == "error_page" || word == "cgi_ext" )
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
                exit (1);
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
                // std::cout << " burda " << tokens[i] << tokens[i - 1] << getServerBrackets() << getLocationBrackets() << std::endl;
                throw std::invalid_argument("brackets error 2");
                exit(1);
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
                        throw std::invalid_argument("brackets error 5");
                        exit (1);
                    }
                }
            }
            else{
                throw std::invalid_argument("brackets error 3");
                exit (1);
            }
        }
    }

    if (getServerBrackets() || getLocationBrackets()){
        throw std::invalid_argument("brackets error (dont close)");
        exit (1);
    }
}

// getter setter
//---------------------------Brackets------------------------------------
void CheckConfFile::setServerBrackets(bool isOpen) { serverBrackets = isOpen; }
void CheckConfFile::setLocationBrackets(bool isOpen) { locationBrackets = isOpen; }
bool CheckConfFile::getLocationBrackets() { return locationBrackets; }
bool CheckConfFile::getServerBrackets() { return serverBrackets; }