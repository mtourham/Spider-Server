#include "../../includes/webserv.hpp"

int current_location::getLocationindexObject(void)
{
    return (this->current_location_index);
}

bool    current_location::getLocationIsAutoIndexObject(void) const 
{
    return (this->is_auto_index);
}

std::string current_location::getLocationIndexObject(void) const 
{
    return (this->index);
}
std::string current_location::getLocationRedirectionObject(void) const
{
    return (this->redirection);
}
std::string current_location::getLocationErrorPageObject(void) const 
{
    return (this->error_page);
}
std::string current_location::getLocationRootObject(void) const 
{
    return (this->root);
}
std::string current_location::parseLocationNameObject(void) const 
{
    return this->location_name;
}
std::vector<int>    current_location::getLocationErrorCodesObject(void) 
{
    return (this->error_codes);
}
std::vector<std::string>	current_location::getLocationMethodsObject(void)
{
    return (this->httpMethods);
}
bool    current_location::getLocationHas301Code(void) 
{
    return (this->has_301_code);
}
std::string current_location::parseUploadObject(void) 
{
    return (this->upload);
}
bool    current_location::getHasCgi(void) 
{
    return (location_has_cgi);
}
bool    current_location::getHasRedirection(void)
{
    return (this->location_has_redirection);
}
void    current_location::setLocationIndex(size_t i) {
    this->current_location_index = i;
}


current_location::current_location(current_location const  & s)
{

    this->upload = s.upload;
    this->location_name = s.location_name;
    this->root = s.root;
    this->current_location_index = s.current_location_index;
    this->client_max_body_size = s.client_max_body_size;
    this->location_has_cgi = s.location_has_cgi;
    this->cgiExtension = s.cgiExtension;
    this->error_page = s.error_page;
    this->cgiPaths = s.cgiPaths;
    this->httpMethods = s.httpMethods;
    this->index = s.index;
    this->error_codes = s.error_codes;
    this->location_has_redirection = s.location_has_redirection;
    this->redirection = s.redirection;
    this->is_auto_index = s.is_auto_index;
    this->has_301_code = s.has_301_code;
}


current_location & current_location::operator=(current_location const & s)
{
    this->client_max_body_size = s.client_max_body_size;
    this->location_name = s.location_name;
    this->httpMethods = s.httpMethods;
    this->error_codes = s.error_codes;
    this->root = s.root;
    this->cgiExtension = s.cgiExtension;
    this->index = s.index;
    this->location_has_redirection = s.location_has_redirection;
    this->has_301_code = s.has_301_code;
    this->cgiPaths = s.cgiPaths;
    this->upload = s.upload;
    this->redirection = s.redirection;
    this->location_has_cgi = s.location_has_cgi;
    this->is_auto_index = s.is_auto_index;
    this->error_page = s.error_page;
    this->current_location_index = s.current_location_index;
    return (*this);
}

bool current_location::isCGI(std::string path)
{
    if(!strrchr(path.c_str(), '.'))
        return (false);
    return (!strcmp(strrchr(path.c_str(), '.'), ".php") || !strcmp(strrchr(path.c_str(), '.'), ".rb") || !strcmp(strrchr(path.c_str(), '.'), ".py") ? true : false);
}

void    current_location::checkCgiAllowed(void)
{
    bool ext = false;
    std::string path_holder;
    std::string ext_holder;

    for(size_t i = 0; i < this->cgiExtension.size(); i++)
    {
        if(this->cgiExtension[i] != ".php" && this->cgiExtension[i] != ".rb" && this->cgiExtension[i] != ".py")
        {
            std::cerr << "ERROR: CGI extention not allowed\n";
            exit(1);
        }
    }
    for(size_t i = 0; i < this->cgiPaths.size(); i++)
    {
        ext = false;
        if(isFileOrDirectory(this->cgiPaths[i]) != "file")
        {
            std::cerr << "ERROR: " << this->cgiPaths[i] << " Check This Path\n";
            exit(1);
        }
        if(!strcmp(strrchr(this->cgiPaths[i].c_str(), '/') + 1, "php") || !strcmp(strrchr(this->cgiPaths[i].c_str(), '/') + 1, "php-cgi"))
        {
            for(size_t i = 0; i < this->cgiExtension.size(); i++)
            {
                if(this->cgiExtension[i] == ".php")
                    ext = true;
            }
            if(ext == false)
            {
                std::cerr << "ERROR: there's no extention of  >> " << this->cgiPaths[i] << std::endl;
                exit(1);
            }
        }
        else if(!strcmp(strrchr(this->cgiPaths[i].c_str(), '/') + 1, "python") || !strcmp(strrchr(this->cgiPaths[i].c_str(), '/') + 1, "python3"))
        {
            for(size_t i = 0; i < this->cgiExtension.size(); i++)
            {
                if(this->cgiExtension[i] == ".py")
                    ext = true;
            }
            if(ext == false)
            {
                std::cerr << "ERROR: there's no extention of  >> " << this->cgiPaths[i] << std::endl;
                exit(1);
            }
        }
        else if(!strcmp(strrchr(this->cgiPaths[i].c_str(), '/') + 1, "ruby"))
        {
            for(size_t i = 0; i < this->cgiExtension.size(); i++)
            {
                if(this->cgiExtension[i] == ".rb")
                    ext = true;
            }
            if(ext == false)
            {
                std::cerr << "ERROR: there's no extention of >> " << this->cgiPaths[i] << std::endl;
                exit(1);
            }
        }
        else
        {
            std::cerr << "ERROR: cgi is not supported\n";
            exit(1);
        }
    }
}

void current_location::build_location_object(std::vector<std::string>::iterator first, std::vector<std::string>::iterator last)
{
    size_t temp_size;

    this->location_has_cgi = false;
    this->is_auto_index = false;
    this->location_has_redirection = false;
    this->client_max_body_size = 0;
    std::vector<std::string> locationVec(first, last);
    for(size_t i = 0; i < locationVec.size(); i++)
    {
        if ((!strncmp(locationVec[i].c_str(), "location", 8) && locationVec[i].back() == '{') || (!strncmp(locationVec[i].c_str(), "location", 8) && locationVec[i + 1] == "{"))
        {
            parseLocationName(split_string(locationVec[i], ' ', &temp_size), temp_size);
        }
        else if(!strncmp(locationVec[i].c_str(), "index", 5))
        {
            parseIndexPage(split_string(locationVec[i], ' ', &temp_size), temp_size);
        }
        else if(!strncmp(locationVec[i].c_str(), "error", 5))
        {
            parseErrorPage(split_string(locationVec[i], ' ', &temp_size), temp_size);
            if(isFileOrDirectory(this->getLocationErrorPageObject()) == "error")
            {
                std::cerr << "ERROR: check the location error path\n";
                exit(1);
            }
        }
        else if (!strncmp(locationVec[i].c_str(), "autoindex", 9))
        {
            parseAutoIndex(split_string(locationVec[i], ' ', &temp_size), temp_size);
        }
        else if(!strncmp(locationVec[i].c_str(), "root", 4))
        {
            if(locationVec[i][locationVec[i].size() - 1] == '/')
                locationVec[i].erase(locationVec[i].size() - 1);
            requestParseroot(split_string(locationVec[i], ' ', &temp_size), temp_size);
            if(isFileOrDirectory(this->getLocationRootObject()) == "error")
            {
                std::cerr << "ERROR: check the location root path\n";
                exit(1);
            }
        }
        else if (!strncmp(locationVec[i].c_str(), "client_max_body_size", 19))
        {
            parseLocationCmds(split_string(locationVec[i], ' ', &temp_size), temp_size);
        }
        else if (!strncmp(locationVec[i].c_str(), "return ", 7))
        {
            requestParseredirection(split_string(locationVec[i], ' ', &temp_size), temp_size);
            this->location_has_redirection = true;
        }
        else if (!strncmp(locationVec[i].c_str(), "cgi_path ", 9))
        {
            this->location_has_cgi = true;
            parseCgiPath(split_string(locationVec[i], ' ', &temp_size), temp_size);
 
            checkCgiAllowed();
        }
        else if (!strncmp(locationVec[i].c_str(), "methods", 7))
        {
            parseMethods(split_string(locationVec[i], ' ', &temp_size), temp_size);
        }
        else if (!strncmp(locationVec[i].c_str(), "upload ", 7))
        {
            parseUpload(split_string(locationVec[i], ' ', &temp_size), temp_size);
        }
        else if (!strncmp(locationVec[i].c_str(), "cgi_exec ", 9))
        {
            parseCgiExec(split_string(locationVec[i], ' ', &temp_size), temp_size);
        }
        else if(locationVec[i] != "{" && locationVec[i] != "}" && locationVec[i][0] != '#' && !locationVec[i].empty())
        {
            std::cerr << "ERROR: parsing error << " << locationVec[i] << " >>\n";
            exit(1);
        }
    }
}

std::string *split_string(std::string split, char c, size_t *index_save)
{
    std::vector<std::string> Tokens;
    size_t start = 0;
    size_t end = 0;
    while ((end = split.find(c, start)) != std::string::npos)
    {
        Tokens.push_back(split.substr(start, end - start));
        start = end + 1;
        while (start < split.length() && (split[start] == c || isspace(split[start]))) {
            ++start;
        }
    }
    Tokens.push_back(split.substr(start));
    std::string *ret = new std::string[Tokens.size()]; // ret string allocation;
    std::copy(Tokens.begin(), Tokens.end(), ret);
    for(size_t i = 0; i < Tokens.size(); i++)
    {
        ret[i].erase(0, ret[i].find_first_not_of(" \t\r\n"));
        ret[i].erase(ret[i].find_last_not_of(" \t\r\n") + 1);
    }
    *index_save = Tokens.size();
    return (ret);
}

current_location current_server::getOneLocationObject(int index) const
{
    return (this->location[index]);
}
