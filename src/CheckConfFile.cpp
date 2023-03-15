#include "../inc/CheckConfFile.hpp"



CheckConfFile::~CheckConfFile()
{
}

CheckConfFile::CheckConfFile(const std::string path): serverBrackets(false),locationBrackets(false)
{
    std::ifstream ifs(path.c_str());
	if (!ifs.is_open()) {
        std::cerr << "File does not exist: " << path << std::endl;
        exit(1);
    }
    std::string token;
    std::vector<std::string> tokens;
    while (ifs >> token) {
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

}

void CheckConfFile::checkBrackets(std::vector<std::string> tokens)
{
    for (int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].compare("{") == 0)
        {
            if (i < 0){
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
                std::cout << " burda " << tokens[i] << tokens[i -1]  << getServerBrackets() << getLocationBrackets() << std::endl; 
                throw std::invalid_argument("brackets error 2");
            }
        }

        if (tokens[i].compare("}") == 0){
            if (getLocationBrackets() == true){
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
                    if ((tokens[i + 1].compare("server") != 0) && (i + 1) < tokens.size()){
                        std::cout << "size is"<< tokens.size() << "i " << i << "["<<  tokens[i + 1] << "]"<< std::endl;
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





//getter setter 
//---------------------------Brackets------------------------------------
void CheckConfFile::setServerBrackets(bool isOpen){serverBrackets = isOpen;}
void CheckConfFile::setLocationBrackets(bool isOpen){locationBrackets = isOpen;}
bool CheckConfFile::getLocationBrackets(){return locationBrackets;}
bool CheckConfFile::getServerBrackets(){return serverBrackets;}