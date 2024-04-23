#include "../../includes/webserv.hpp"
#include <arpa/inet.h>

void    current_location::parseErrorPage(std::string *keys, size_t size)
{
    if (size <= 1)
    {
        std::cerr << "Error: couldn't find the error page\n";
        exit(1);
    }
    this->error_page = keys[size - 1];
    for(size_t i = 1; i < size - 1;i++)
        this->error_codes.push_back(stoi(keys[i]));
    delete [] keys;
}

void    current_location::parseIndexPage(std::string *keys, size_t size)
{
    if(size <= 1 || size > 2)
    {
        std::cerr << "ERROR: couldn't find index page";
        exit(1);
    }
    this->index = keys[size - 1];
    delete [] keys;
}
void    current_location::parseAutoIndex(std::string *keys, size_t size)
{
    if(size <= 1 || size > 2)
    {
        std::cerr << "ERROR: autoindex has bad argumennts\n";
        exit(1);
    }
    if(keys[size - 1] == "off")
        this->is_auto_index = false;
    else if(keys[size - 1] == "on")
        this->is_auto_index = true;
    else
    {
        std::cerr << "ERROR: wrong autoindex argument";
        exit(1);
    }
    delete [] keys;
}
void    current_location::requestParseroot(std::string *keys, size_t size)
{
    if(size <= 1 || size > 2)
    {
        std::cerr << "Error: couldn't find the root page\n";
        exit(1);
    }
    this->root = keys[size - 1];
    delete [] keys;
}

void    current_location::requestParseredirection(std::string *keys, size_t size)
{
    if(size != 3)
    {
        std::cerr << "Error: bad redirection arguments !\n";
        exit(1);
    }
    if(size == 3 && keys[1] == "301")
        this->has_301_code = true;
    else if (size == 3 && keys[1] != "301")
    {
        std::cerr << "ERROR: Check Redirection Code Value\n";
        exit(1);
    }
    this->redirection = keys[size - 1];

    delete [] keys;
}

void    current_location::parseLocationCmds(std::string *keys, size_t size)
{
    if(size <= 1 || size > 2)
    {
        std::cerr << "ERROR: client max body size arguments";
        exit(1);
    }
    this->client_max_body_size = stoi(keys[size - 1]);
    delete [] keys;
}
void    current_location::parseMethods(std::string *Keys, size_t size)
{
    if(size <= 1 || size > 4)
    {
        std::cerr << "Error: bad methods assignemt\n";
        exit(1);
    }
    for(size_t i = 1;i < size; i++)
    {
        Keys[i].erase(Keys[i].find_last_not_of(",") + 1);
        this->httpMethods.push_back(Keys[i]);
    }
    delete [] Keys;
}

void    current_location::parseCgiPath(std::string *Keys, size_t size)
{
    if(size <= 1)
    {
        std::cerr << "ERROR: bad cgi path assignemt\n";
        exit(1);
    }
    for(size_t i = 1; i < size; i++)
        this->cgiPaths.push_back(Keys[i]);

    delete [] Keys;
}


void    current_location::parseCgiExec(std::string *Keys, size_t size)
{
    if(size <= 1)
    {
        std::cerr << "ERROR: cgi exec assinment\n";
        exit(1);
    }
    for(size_t i = 1; i < size; i++)
        this->cgiExtension.push_back(Keys[i]);
    delete[] Keys;
}

void    current_location::parseLocationName(std::string *Keys, size_t size)
{
    if(size <= 1 || size > 3)
    {
        std::cerr << "ERROR: location name arguments\n";
        exit(1);
    }
    if(Keys[size - 1] != "{")
        this->location_name = Keys[size - 1];
    else
        this->location_name = Keys[size - 2];
    
    delete [] Keys;
}

void    current_location::parseUpload(std::string *Keys, size_t size)
{
    if(size <= 1 || size > 2)
    {
        std::cerr << "ERROR: upload arguments\n";
        exit(1);
    }
    this->upload = Keys[size - 1];
    delete [] Keys; 
}

std::string current_location::parseCgiPathObject(std::string path)
{
    if(strrchr(path.c_str(), '.') == NULL)
        return "";
    for(size_t i = 0; i < this->cgiPaths.size(); i++)
    {
        if((!strcmp(strrchr(this->cgiPaths[i].c_str(), '/') + 1, "php") || !strcmp(strrchr(this->cgiPaths[i].c_str(), '/') + 1, "php-cgi")) && !strcmp(strrchr(path.c_str(), '.') + 1, "php"))
            return (this->cgiPaths[i]);
        else if((!strcmp(strrchr(this->cgiPaths[i].c_str(), '/') + 1, "python") || !strcmp(strrchr(this->cgiPaths[i].c_str(), '/') + 1, "python3")) && !strcmp(strrchr(path.c_str(), '.') + 1, "py"))
            return (this->cgiPaths[i]);
        else if(!strcmp(strrchr(this->cgiPaths[i].c_str(), '/') + 1, "ruby") && !strcmp(strrchr(path.c_str(), '.') + 1, "py"))
            return (this->cgiPaths[i]);
    }
    return "";
}

void    current_server::parsePort(std::string *Port, size_t temp_size)
{
    if(temp_size < 1)
    {
        std::cerr << "ERROR: Check The Port Arguments\n";
        exit(1);
    }
    for(size_t i = 1; i < temp_size; i++)
    {
        if(atoi(Port[i].c_str()) < 1)
        {
            delete [] Port;
            std::cerr << "ERROR: wrong port value\n";
            exit(1);
        }
        this->port.push_back(atoi(Port[i].c_str()));
    }
    delete [] Port;
}
void    current_server::parseServerName(std::string *keys, size_t size)
{
    if (size <= 1)
    {
        std::cerr << "ERROR: missing server name !";
        exit(1);
    }
    if(this->server_names.size() > 0)
    {
        std::cerr << "ERROR: duplicate server_name !";
        exit(1);
    }
    for(size_t i = 1; i < size; i++)
        this->server_names.push_back(keys[i]);

    delete [] keys;
}
void    current_server::parseErrorPage(std::string *keys, size_t size)
{
    if (size <= 1)
    {
        std::cerr << "ERROR: couldn't find error page\n";
        exit(1);
    }
    this->error_page = keys[size - 1];
    for(size_t i = 1; i < size - 1;i++)
    {
        if(!(stoi(keys[i]) >= 400 && stoi(keys[i]) <= 501))
        {
            std::cerr << "ERROR: wrong error code range !\n";
            delete [] keys;
            exit(1);
        }
        this->error_codes.push_back(stoi(keys[i]));
    }
    delete [] keys;
}
void    current_server::parseIndexPage(std::string *keys, size_t size)
{
    if(size <= 1 || size > 2)
    {
        std::cerr << "ERROR: missing index page !";
        exit(1);
    }
    this->index = keys[size - 1];
    delete [] keys;
}

void    current_server::parseAutoIndex(std::string *keys, size_t size)
{
    if(size <= 1 || size > 2)
    {
        std::cerr << "ERROR: autoindex bad argument\n";
        exit(1);
    }
    if(keys[size - 1] == "on")
        this->is_auto_index = true;
    else if(keys[size - 1] == "off")
        this->is_auto_index = false;
    else
    {
        std::cerr << "ERROR: wrong autoindex argument";
        exit(1);
    }
    delete [] keys;
}

void    current_server::parseCmds(std::string *keys, size_t size)
{
    if(size <= 1 || size > 2)
    {
        std::cerr << "ERROR: client max body size arguments";
        exit(1);
    }
    this->client_max_body_size = stoi(keys[size - 1]);
    delete [] keys;
}

void    current_server::parseHost(std::string *keys, size_t size)
{
    // this->host = convertIpToInt(temparray[0]);
    if(size < 1 || size > 2)
    {
        std::cerr << "ERROR: check the host argumets";
        exit(1);
    }
    this->host = convertIpToInt(keys[1]);
    delete [] keys;
}