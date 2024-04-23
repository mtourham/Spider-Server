#include "../../includes/webserv.hpp"
#include <arpa/inet.h>

current_server::current_server(current_server const & s) : current_location(s)
{
    this->host = s.parseHostObject();
    this->server_index = s.getServerIndexObject();
    this->location_count = s.getLocationCount();
    this->is_auto_index = s.getIsAutoIndexObject();
    this->client_max_body_size = s.getCmbsObject();
    this->port = s.parsePortObject();
    this->error_codes = s.getErrorCodesObject();
    this->location = s.getServerLocation();
    this->redirection = s.requestParseredirectionObject();
    this->error_page = s.getServerErrorPageObject();
    this->root = s.requestParserootObject();
    this->server_names = s.parseServerNamesObject();
}

current_server & current_server::operator=(current_server const & s)
{
    this->location_count = s.getLocationCount();
    this->error_page = s.getServerErrorPageObject();
    this->client_max_body_size = s.getCmbsObject();
    this->server_index = s.getServerIndexObject();
    this->redirection = s.requestParseredirectionObject();
    this->port = s.parsePortObject();
    this->is_auto_index = s.getIsAutoIndexObject();
    this->error_codes = s.getErrorCodesObject();
    this->root = s.requestParserootObject();
    this->server_names = s.parseServerNamesObject();
    this->host = s.parseHostObject();
    this->location = s.getServerLocationsObject();
    return (*this);
}
std::vector<current_location> current_server::getServerLocationsObject(void) const
{
    return (this->location);
}
std::vector<std::string> current_server::parseServerNamesObject(void) const
{
    return (this->server_names);
}
std::vector<int> current_server::parsePortObject(void) const
{
    return (this->port);
}
int current_server::parseHostObject(void) const
{
    return (this->host);
}
int     current_location::getLocationCmbsObject(void)
{
    return (this->client_max_body_size);
}

size_t current_server::getLocationCount(void) const
{
    return (this->location.size());
}
int current_server::getCmbsObject(void) const
{
    return (this->client_max_body_size);
}
std::vector<int> current_server::getErrorCodesObject(void) const
{
    return (this->error_codes);
}
std::string current_server::requestParseredirectionObject(void) const
{
    return (this->redirection);
}
std::vector<current_location> current_server::getServerLocation(void) const
{
    return (this->location);
}
std::string current_server::getIndexObject(void) const
{
    return (this->index);
}
std::string current_server::requestParserootObject(void) const
{
    return (this->root);
}
int current_server::getServerIndexObject(void) const
{
    return (this->server_index);
}
std::string current_server::getServerErrorPageObject(void) const
{
    return (this->error_page);
}
bool current_server::getIsAutoIndexObject(void) const
{
    return (this->is_auto_index);
}


bool current_server::is_digits(const std::string &str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

int current_server::convertIpToInt(std::string host)
{
    size_t size;
    int ret = 0; // we can make it size_t !!
    std::vector<int> Array;
    std::string *keys = split_string(host, '.', &size);
    for(size_t i = 0; i < size; i++)
    {
        Array.push_back(stoi(keys[i]));
        if(!is_digits(keys[i]) || Array[i] > 256 || Array[i] < 0)
        {
            std::cerr << "ERROR: wrong IP value\n";
            exit(1);
        }
    }
    if(size != 4)
    {
        std::cerr << "ERROR: wrong ip address";
        exit(1);
    }
    for (int i = 0; i < 4; i++) {
        ret |= (Array[i] << (8 * (3 - i)));
    }
    delete [] keys;
    return (ret);
}


void    current_server::requestParseroot(std::string *keys, size_t size)
{
    if(size <= 1 || size > 2)
    {
        std::cerr << "ERROR: missing root page\n";
        exit(1);
    }
    this->root = keys[size - 1];
    delete [] keys;
}

void    current_server::requestParseredirection(std::string *keys, size_t size)
{
    if(size <= 1 || size > 2)
    {
        std::cerr << "ERROR: redirection arguments !\n";
        exit(1);
    }
    this->redirection = keys[size - 1];
    delete [] keys;
}

int current_server::getLocationByName(std::string name) const
{
    for(size_t i = 0; i < location.size(); i++)
    {
        if(location[i].parseLocationNameObject() == name)
            return (i);
    }
    return (-1);
}

void    current_server::checkServerData(void)
{
    if(this->port.size() == 0)
    {
        std::cerr << "ERROR: server missing port\n";
        exit(1);
    }
    else if(this->getServerErrorPageObject().empty())
    {
        std::cerr << "ERROR: server missing error page\n";
        exit(1);
    }
    else if(this->getLocationByName("/") == -1)
    {
        std::cerr << "ERROR: server missing \\ location\n";
        exit(1);
    }
    else if(this->host == -1)
    {
        std::cerr << "ERROR: couldn't find the host\n";
        exit(1);
    }
}

void    current_server::build_server(std::vector<std::string>::iterator first, std::vector<std::string>::iterator last)
{
    this->is_auto_index = false;
    this->server_delete_method_in = true;
    this->server_get_method_in = true;
    this->server_post_method_in = true;
    this->host = -1;
    int current_location_index = 0;
    size_t opening = 0;
    size_t closing = 0;
    size_t temp_size;

    std::vector<std::string> serverTempVector(first, last);
    for(size_t i = 1; i < serverTempVector.size(); i++)
    {
        if(!strncmp(serverTempVector[i].c_str(), "host ", 5))
        {
            if(this->host != -1)
            {
                std::cerr << "ERROR: duplicate host !\n";
                exit(1);
            }
            parseHost(split_string(serverTempVector[i], ' ', &temp_size), temp_size);
            // parsePort(split_string(serverTempVector[i], ' ', &temp_size), temp_size); // host and port parsing;
        }
        else if(!strncmp(serverTempVector[i].c_str(), "port ", 5))
        {
            parsePort(split_string(serverTempVector[i], ' ', &temp_size), temp_size);
        }
        else if(!strncmp(serverTempVector[i].c_str(), "server_name", 11))
        {
            parseServerName(split_string(serverTempVector[i], ' ', &temp_size), temp_size);
            if(this->server_names.size() == 0)
                this->server_names.push_back("localhost ");
        }
        else if(!strncmp(serverTempVector[i].c_str(), "error", 5))
        {
            parseErrorPage(split_string(serverTempVector[i], ' ', &temp_size), temp_size);
            if(isFileOrDirectory(this->getServerErrorPageObject()) == "error")
            {
                std::cerr << "ERROR: check the server error path\n";
                exit(1);
            }
        }
        else if(!strncmp(serverTempVector[i].c_str(), "index", 5))
        {
            parseIndexPage(split_string(serverTempVector[i], ' ', &temp_size), temp_size);
            if(isFileOrDirectory(this->requestParserootObject() + this->getIndexObject()) == "error")
            {
                std::cerr << "ERROR: check the server index path\n";
                exit(1);
            }
        }
        else if(!strncmp(serverTempVector[i].c_str(), "root", 4))
        {
            requestParseroot(split_string(serverTempVector[i], ' ', &temp_size), temp_size);
            if(isFileOrDirectory(this->requestParserootObject()) == "error")
            {
                std::cerr << "ERROR: check the server root path\n";
                exit(1);
            }
        }
        else if (!strncmp(serverTempVector[i].c_str(), "autoindex", 9))
        {
            parseAutoIndex(split_string(serverTempVector[i], ' ', &temp_size), temp_size);
        }
        else if (!strncmp(serverTempVector[i].c_str(), "return", 6))
        {
            requestParseredirection(split_string(serverTempVector[i], ' ', &temp_size), temp_size);
        }
        else if (!strncmp(serverTempVector[i].c_str(), "client_max_body_size", 19))
        {
            parseCmds(split_string(serverTempVector[i], ' ', &temp_size), temp_size);
        }
        else if ((!strncmp(serverTempVector[i].c_str(), "location", 8) && serverTempVector[i].back() == '{') || (!strncmp(serverTempVector[i].c_str(), "location", 8) && serverTempVector[i + 1] == "{"))
        {
            opening = i;
            i++;
            while(serverTempVector[i] != "}")
                i++;
            closing = i;
            current_location locationn;
            locationn.setLocationIndex(current_location_index);
            locationn.build_location_object(serverTempVector.begin() + opening, serverTempVector.begin() + closing);   
            this->location.push_back(locationn);
            current_location_index++;          
        }
        else if(serverTempVector[i] != "{" && serverTempVector[i] != "}" && serverTempVector[i][0] != '#' && !serverTempVector[i].empty())
        {
            std::cerr << "ERROR: Wrong Directive [ " << serverTempVector[i] << " ]\n";
            exit(1);
        }
    }
    this->getServerDataFromRootLocation();
    this->checkServerData();
}

void    current_server::setLocationsIndex(std::vector<current_location> location)
{
    for(size_t i = 0; i < location.size();i++)
        location[i].setLocationIndex(i);
}

void    current_server::setServerIndex(int index)
{
    this->server_index = index;
}

size_t current_server::getLocationCount(std::vector<std::string> temp)
{
    size_t count = 0;
    for(size_t i = 0; i < temp.size(); i++)
    {
        if (!strncmp(temp[i].c_str(), "location ", 9) && temp[i].back() == '{')
            count++;
    }
    return (count);
}

void    current_server::restoreRootObject(int i)
{
    if(this->root.size() == 0 && this->location[i].getLocationRootObject().size() != 0)
    {
        this->root = this->location[i].getLocationRootObject();
    }
    else if(this->root.size() != 0 && this->location[i].getLocationRootObject().size() != 0)
    {
        std::cerr << "ERROR: duplicate root path\n";
        exit(1);
    }
    else if(this->root.size() == 0 && this->location[i].getLocationRootObject().size() == 0)
    {
        std::cerr << "ERROR: there's no root in server\n";
        exit(1);   
    }
}

void    current_server::restoreIndexObject(int i)
{
    if(this->index.size() == 0 && this->location[i].getLocationIndexObject().size() != 0)
    {
        this->index = this->location[i].getLocationIndexObject();
    }
    else if(this->index.size() != 0 && this->location[i].getLocationIndexObject().size() != 0)
    {
        std::cerr << "ERROR: duplicate index path\n";
        exit(1);
    }
}

void    current_server::restoreAutoIndex(int i)
{
    if(this->is_auto_index == false && this->location[i].getLocationIsAutoIndexObject() == true)
    {
        this->is_auto_index = true;
    }
    else if(this->is_auto_index == true && this->location[i].getLocationIsAutoIndexObject() == true)
    {
        std::cerr << "ERROR: there's two true autoIndex bool !\n";
        exit(1);
    }
}

void    current_server::restoreCmbs(int i)
{
    this->client_max_body_size = this->location[i].getLocationCmbsObject();
}

void    current_server::getServerDataFromRootLocation(void)
{
    for(size_t i = 0; i < location.size(); i++)
    {
        if(this->location[i].parseLocationNameObject() == "/")
        {
            restoreAutoIndex(i);
            restoreRootObject(i);
            restoreIndexObject(i);
            restoreCmbs(i);
        }
    }
}