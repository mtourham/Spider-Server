#include "../../includes/request.hpp"

void Request::requestParser(std::string &value)
{
    Token token;
    requestLexer requestLexer(value);
    std::string buff;
    bool end_of_section = false;

    uintptr_t p = reinterpret_cast<uintptr_t>(&requestLine);
    std::string *t = reinterpret_cast<std::string *>(p);

    for (int i = 0; token.type != TYPE_EOF;)
    {
        if (i == 0)
            token = requestLexer.get_next_Token(0);
        else
            token = requestLexer.get_next_Token(1);

        if (token.type == TOKEN_END_OF_LINE || token.type == TOKEN_CR)
            i++;
        if (token.type == TOKEN_END_OF_SSECTION)
            end_of_section = true;

        if (i == 0 && !end_of_section)
        {
            *t = token.value;
            t++;
        }
        else if (token.type != TOKEN_END_OF_LINE && token.type != TOKEN_CR &&
                 token.type != TOKEN_END_OF_SSECTION && token.type != TOKEN_TWO_POINT && token.type == false)
        {
            requestLexer.get_next_Token(1);
            buff = requestLexer.get_next_Token(1).value;
            buff.erase(buff.begin());

            if (!buff.empty() && !token.value.empty())
                header.insert(std::make_pair(token.value, buff));

            buff.clear();
        }
        else if (token.type == TOKEN_END_OF_SSECTION)
        {
            buff.clear();
            for (int i = requestLexer.get_index(); i < static_cast<int>(value.length()); i++)
                buff += value[i];
            body = buff;
            break;
        }

        token.value.clear();
    }

    if (header.find("Transfer-Encoding") != header.end())
    {
        if (header["Transfer-Encoding"] == "chunked")
        {
            wait_body = true;
            handleBody(body);
        }
    }
    else if (header.find("Content-Length") != header.end())
    {
        wait_body = true;
        body_size = atol(header["Content-Length"].c_str());
        if ((unsigned long)body.length() >= body_size)
            wait_body = false;
    }
    else if (requestLine.method == "POST" && header.find("Content-Length") != header.end() && header.find("Transfer-Encoding") != header.end())
        wait_body = true;
}


void Request::handleBody(std::string buff)
{
    std::string hex;

    if(header.find("Transfer-Encoding") != header.end())
    {
        if(buff[buff.length() - 1] == 10 && buff[buff.length() - 2] == 13 && buff[buff.length() - 3] == 10
                                && buff[buff.length() - 4] == 13 && buff[buff.length() - 5] == '0' && buff[buff.length() - 6] == 10 && buff[buff.length() - 7] == 13)
        {
        
            buff.erase(buff.length() - 7 , buff.length() - 1);
            wait_body = false;
        }
        if(header["Transfer-Encoding"] == "chunked" )
        {
            if(body_size == 0)
            {
                for(long i = 0 ; i < (long)buff.length(); i++)
                {
                    if(buff[i] == 13 && buff[i + 1] == 10)
                    {
                        buff.erase(0,i + 2);
                        break;
                    }
                    else
                        hex += buff[i];
                }
                std::cerr << "*"<< hex << "*"<< std::endl;
                body_size = std::stoul(hex, NULL, 16);
                body = buff;
            }
            else
            {
                for (int  i = 0; i < (int)buff.length() ; i++)
                {
                    if(buff[i] == 13 && buff[i + 1] == 10)
                    {
                        for(int j = i + 2 ; j < (int)buff.length() ; j++)
                        {
                            if(buff[j] == 13 && buff[j + 1] == 10)
                            {

                                if(!hex.empty())
                                {
                                    std::cerr << hex << std::endl;
                                    body_size = std::stoul(hex, NULL, 16);
                                    i = j + 2;
                                    break;
                                }
                            }
                            hex += buff[j];
                            if((buff[j] < 48 || buff[j] > 57) &&  (buff[j] < 65 || buff[j] > 70) )
                            {
                                hex.clear();
                                break;
                            }
                        }
                    }
                    body += buff[i];
                }
            }
        }
    }
    else if(header.find("Content-Length") != header.end())
    {
        body += buff;
        if((unsigned long)body.length() >= body_size)
            wait_body = false;
    }
}

void Request::errors(current_server &serv)
{
    request_well_formed(serv);
    if(r_error == "")
        location_is_well(serv);
}

void processLocation(const std::string& locationRoot, const std::string& locationPath, std::string& currentPath, std::string& fullPath) {
    if (locationRoot.empty()) {
        if (currentPath[0] == '/')
            currentPath.erase(0, 1);
        fullPath = locationPath + currentPath;
    } else {
        if (locationRoot[locationRoot.length() - 1] == '/' && currentPath[0] == '/')
            currentPath.erase(0, 1);
        fullPath = locationRoot + currentPath;
    }
}

void Request::location_is_well(current_server& serv) {
    size_t founded = 0;
    long index = requestLine.current_location_index = -1;
    size_t indexOfCharacters = 0;
    bool allowed_method = false;

    for (size_t i = 0; i < (size_t)serv.getServerLocationsObject().size(); ++i) {
        founded = requestLine.path.find(serv.getServerLocationsObject()[i].parseLocationNameObject());
        if (founded != (size_t)-1 && indexOfCharacters < serv.getServerLocationsObject()[i].parseLocationNameObject().length()) {
            indexOfCharacters = serv.getServerLocationsObject()[i].parseLocationNameObject().length();
            index = (long)i;
        }
    }

    if (index != -1) {
        requestLine.current_location_index = index;

        if (!serv.getServerLocationsObject()[index].getLocationRedirectionObject().empty())
            r_error = "301";

        const std::vector<std::string>& allowedMethods = serv.getServerLocationsObject()[index].getLocationMethodsObject();
        if (!allowedMethods.empty()) {
            allowed_method = false;
            for (size_t i = 0; i < allowedMethods.size(); ++i) {
                if (allowedMethods[i] == requestLine.method) {
                    allowed_method = true;
                    break;
                }
            }
            if (!allowed_method)
                r_error = "405";
        } else if (requestLine.method != "POST" && requestLine.method != "DELETE" && requestLine.method != "GET") {
            r_error = "405";
        }

        processLocation(serv.getServerLocationsObject()[index].getLocationRootObject(),
                        serv.getServerLocationsObject()[index].getLocationRootObject(),
                        requestLine.path,
                        requestLine.full_path);

        if (!requestLine.full_path.empty()) {
            size_t query_position = requestLine.full_path.find("?");
            if (query_position != std::string::npos) {
                requestLine.query = requestLine.full_path.substr(query_position + 1, requestLine.full_path.find("#") - query_position - 1);
                requestLine.full_path.erase(query_position);
            }
        }

        if (header.find("Content-Type") != header.end() && header["Content-Type"] == "application/x-www-form-urlencoded")
            requestLine.query = body;
    } else {
        r_error = "404";
        for (size_t i = 0; i < (size_t)serv.getServerLocationsObject().size(); ++i) {
            founded = requestLine.path.find(serv.getServerLocationsObject()[i].parseLocationNameObject());
            if (founded != (size_t)-1 && indexOfCharacters <= serv.getServerLocationsObject()[i].parseLocationNameObject().length()) {
                indexOfCharacters = serv.getServerLocationsObject()[i].parseLocationNameObject().length();
                index = (long)i;
            }
        }
    }
}


int allowed_character(char c)
{
    std::string valid = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    for(int i = 0 ; i < (int)valid.length();i++)
    {
        if(c == valid[i])
            return 1;
    }
    return 0;
}

int check_hexa(char c)
{
    std::string valid = "ABCDEF0123456789";
    for(int i = 0 ; i < (int)valid.length();i++)
    {
        if(c == valid[i])
            return 1;
    }
    return 0;
}

void Request::request_well_formed(current_server &serv)
{
    std::string tmp_path = requestLine.path;
    if(header.find("Transfer-Encoding") != header.end())
    {
        if(header["Transfer-Encoding"] != "chunked")
        {
            r_error = "501 Not Implemented";
            return;
        }
            
    }
    else if((requestLine.method == "POST" && header.find("Transfer-Encoding") == header.end() && header.find("Content-Length") == header.end())
        || (requestLine.method == "POST" && header.find("Transfer-Encoding") != header.end() && header.find("Content-Length") != header.end()))
        {
            r_error = "400";
            return;
        }
        
    if(body.size() > 0)
    {
        if(serv.getCmbsObject()  > 0 && (size_t)serv.getCmbsObject()  <  body.size())
        {
            r_error = "413 Request Entity Too Large";
            return;
        }
    }
    if(tmp_path.length() > 2048)
        r_error = "414 Request-URI Too Long";
    for(int i = 0 ; i < (int)tmp_path.length() ; i++)
    {
        if(allowed_character(tmp_path[i]) == 0)
        {
            r_error = "400";
            break;
        }
        if(tmp_path[i] == '%' && i + 1  < (int)tmp_path.length() && i + 2  < (int)tmp_path.length())
        {
            if( check_hexa(tmp_path[i + 1]) == 1 && check_hexa(tmp_path[i + 2]) == 1)
            {
                r_error = "400";
                break;
            } 
        }
    }
}

void Request::clear()
{
    requestLine.method.clear();
    requestLine.path.clear();
    requestLine.httpV.clear();
    requestLine.full_path.clear();
    requestLine.current_location_index = -1;
    requestLine.query.clear();
    wait_body = false;
    body.clear();
    header.clear();
    r_error.clear();
}
      