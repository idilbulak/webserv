#include "../inc/Cgi.hpp"

Cgi::Cgi(std::string file, Response &response) : _response(response), _indxFile(file), _req(response.getReq()), _server(response.getServer()) {
    envCgi();
    std::cout << file << "sdds" << std::endl;

}

Cgi::~Cgi(void) {}

char** maptoarray(std::map<std::string, std::string>& map)
{
    char** arr = new char*[map.size() + 1];
    char** current = arr;
    for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it, ++current)
    {
        std::string key = it->first;
        std::string value = it->second;
        *current = new char[key.length() + value.length() + 2];

        std::string keyValuePair = key + "=" + value;
        strcpy(*current, keyValuePair.c_str());
    }
    *current = NULL;
    return arr;
}

static void freearr(char **arr)
{
	for (int i=0; arr[i]; i++)
		delete[] arr[i];
	delete[] arr;
}

std::string Cgi::itos(int num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
}

void    Cgi::envCgi()
{
    // BURAYI EKLEMEYIIII UNUTMAAAAAAAAAAAAA
    _env.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
    _env.insert(std::make_pair("SERVER_SOFTWARE", _server.name));
    _env.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
    _env.insert(std::make_pair("SERVER_PORT", _server.port));
    _env.insert(std::make_pair("REQUEST_METHOD", _req.getMethod()));
    _env.insert(std::make_pair("PATH_INFO", _req.getUri().path));
    std::cout << _req.getUri().path << std::endl;
    _env.insert(std::make_pair("PATH_TRANSLATED", _req.getUri().path));
    _env.insert(std::make_pair("SCRIPT_NAME", _indxFile));
    _env.insert(std::make_pair("SCRIPT_FILENAME", _indxFile));
    _env.insert(std::make_pair("QUERY_STRING", _req.getQueryStr()));
    _env.insert(std::make_pair("REMOTE_ADDR", _server.host));
    _env.insert(std::make_pair("REMOTEaddr", _server.host));
    _env.insert(std::make_pair("REQUEST_URI", _req.getUri().path + _req.getQueryStr()));
    _env.insert(std::make_pair("REDIRECT_STATUS", "200"));
    // _env.insert(std::make_pair("CONTENT_TYPE", _response.getOtherHeaders()["Content-Type"]));
    _env.insert(std::make_pair("CONTENT_LENGTH", itos(_req.getBody().length())));

    // _env.insert(std::make_pair("SERVER_NAME", ""));
    // _env.insert(std::make_pair("HTTP_COOKIE", ""));
    // _env.insert(std::make_pair("HTTP_USER_AGENT", ""));
    // _env.insert(std::make_pair("REMOTE_HOST", ""));
    // _env.insert(std::make_pair("REQUEST_METHOD", ""));
    // _env.insert(std::make_pair("PATH_INFO", _req.uri));
    // _env.insert(std::make_pair("BODY", _req.body));
}

std::string Cgi::execute()
{
    std::string output;
    char **env = maptoarray(_env);
    // Save standard input and output file descriptors
    int tmpIn = dup(0);
    int tmpOut = dup(1);
    // Create temporary input and output files
    FILE *fIn = tmpfile();
    long fdIn = fileno(fIn);
    FILE *fOut = tmpfile();
    long fdOut = fileno(fOut);
    // Write the body of the HTTP request to the input file
    write(fdIn, _req.getBody().c_str(), _req.getBody().length());
    // Move the file position indicator to the beginning of the file
    lseek(fdIn, 0, 0);

    // Create a new process by forking the current process
    pid_t pid = fork();

    if (pid == -1)
    {
        // Error: failed to create a new process
        freearr(env);
        return ("Status: 501\r\n\r\n");
    }
    else if (pid == 0)
    {
        // Child process

        // Close standard input and output file descriptors
        // and redirect them to the input and output files
        dup2(fdIn, 0);
        dup2(fdOut, 1);

        // Execute the CGI program with environment variables
        char * const * _null = NULL;
        std::cout << _indxFile.c_str() << std::endl;
        execve(_indxFile.c_str(), _null, env);
        // Error: failed to execute the CGI program
        std::cout <<  std::strerror(errno) << std::endl;
        std::cout << "Status: 502\r\n\r\n" << std::endl;
        exit(1);
    }
    else
    {
        // Parent process

        // Read the output of the CGI program from the output file
        char buff[SIZE] = {0};

        waitpid(pid, NULL, 0);
        lseek(fdOut, 0, 0);

        while (read(fdOut, buff, SIZE) > 0)
        {
            output += buff;
            memset(buff, 0, SIZE);
        }
    }

    // Restore standard input and output file descriptors
    dup2(tmpIn, 0);
    dup2(tmpOut, 1);

    // Close and delete the temporary input and output files
    fclose(fIn);
    fclose(fOut);
    close(fdIn);
    close(fdOut);
    close(tmpIn);
    close(tmpOut);

    freearr(env);
    return output;
}