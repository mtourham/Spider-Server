#include "../../includes/webserv.hpp"

bool response::isAmongErrorCodes(int error_code)
{
    std::vector<int> errors = server.getErrorCodesObject();
    for(size_t i = 0;i < errors.size(); i++)
    {
        if(error_code == errors[i])
            return (true);
    }
    return (false);
}


std::string response::chunkedFileToString(std::string path)
{
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) 
    {
        // perror("open");
        std::cerr << "Error opening file " << path << std::endl;
        return "error";
    }
    // Move file pointer to current chunk position
    lseek(fd, this->chunkPosition, SEEK_SET);
    fcntl(fd, F_SETFL, O_NONBLOCK);
    char buffer[CHUNK_SIZE];
    if (getfinalString().size() > 0)
        return this->finalString;
    int bytes_read = read(fd, buffer, CHUNK_SIZE);
    if (bytes_read == 0)
    {
        bodyFlag = false;
        this->pathSave.clear();
        this->cleanAll();
        this->chunkPosition = 0;
        close(fd);
        return ("0\r\n\r\n");
    }
    else if (bytes_read == -1)
    {
        this->bodyFlag = false;
            std::cerr << "Error: while chunking";
        close(fd);
        return "error";
    }
    
    std::string content(buffer, bytes_read);
    
    // Update current chunk position
    this->chunkPosition += bytes_read;
    
    close(fd);
    finalString.clear();
    return content;
}

std::string	response::parseAutoIndexPage(std::string path)
{
    std::string temp;
    DIR *dir;
    struct dirent *ent;
    
    temp = "<html>\n<head>\n<title>Directory listing</title>\n</head>\n<body>\n<ul>\n";
    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (ent->d_type == DT_REG || ent->d_type == DT_DIR) {
            std::string type = (ent->d_type == DT_REG) ? "file" : "directory";
            temp = temp + "<li><a href=\"" + ent->d_name + "\">" + ent->d_name + "</a> (" + type + ")</li>\n";
        }
    }
    closedir (dir);
    }
    else
    {
        std::cout << "Unable to open directory";
        return "";
    }
    temp = temp + "</ul>\n";
    temp = temp + "</body>\n";
    temp = temp + "</html>\n";
    
    return (temp);
}

std::string     response::fileToSring(std::string path)
{
    std::ifstream file(path);
    if(!file.good())
        return (setErrorBody("404"));
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    return (content);
}


std::string		response::setErrorBody(std::string status_code)
{
    if(isFileOrDirectory(this->server.getServerErrorPageObject()) == "directory" && this->isAmongErrorCodes(atoi(status_code.c_str())))
    {
        std::string ret;
        if(this->server.getServerErrorPageObject().back() == '/')
        {
            ret = fileToSring(this->server.getServerErrorPageObject() + status_code + ".html");
            return (ret);
        }
        else
        {
            ret = fileToSring(this->server.getServerErrorPageObject() + "/" + status_code + ".html");
            return (ret);
        }
    }
    else if(this->isAmongErrorCodes(atoi(status_code.c_str())))
        return (fileToSring(this->server.getServerErrorPageObject()));    
    return ("");   
}

std::string		response::mergeRespondStrings(void)
{
	this->finalString =  this->gethttpVersion() + " " + this->getstatusCode() + " " + this->getstatusDescription() + "\r\nContent-Length: " + this->getContentLenght() + "\r\n";
    if(this->content_type.size())
        this->finalString += this->content_type + "\r\n";
    if(!this->cookies.empty())
	    this->finalString = this->finalString + this->getCookies() + "\r\n";
	if(!this->location.empty()) 
	    this->finalString = this->finalString + this->getLocation() + "\r\n";
    if(!this->expireDate.empty())
	    this->finalString = this->finalString + this->getExpires() + "\r\n";
    this->finalString = this->finalString +  "\r\n" + this->getBody();
	return (this->finalString);
}

void    response::recoverBody(int status_code)
{
    if(status_code == 404)
    {
        this->setFinalString("HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 22\r\n\r\n<h1>404 Not Found</h1>\r\n");
        this->setContentLenght(std::to_string(this->getfinalString().size()));
    }
    else if(status_code == 501)
    {
        this->setFinalString("HTTP/1.1 501 Not Implementedr\nContent-Type: text/html\r\nContent-Length: 29\r\n\r\n<h1>501 Not Implemented</h1>\r\n");
        this->setContentLenght(std::to_string(this->getfinalString().size()));
    }
    else if(status_code == 400)
    {
        this->setFinalString("HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\nContent-Length: 22\r\n\r\n<h1>\n400 Bad Request\n</h1>");
        this->setContentLenght(std::to_string(this->getfinalString().size()));
    }
    else if(status_code == 413)
    {
        this->setFinalString("HTTP/1.1 413 Request Entity Too Large\r\nContent-Type: text/html\r\nContent-Length: 41\r\n\r\n<h1>\n413 Request Entity Too Large\n</h1>\r\n");
        this->setContentLenght(std::to_string(this->getfinalString().size()));
    }
    else if(status_code == 414)
    {
        this->setFinalString("HTTP/1.1 414 Request-URI Too Longer\nContent-Type: text/html\r\nContent-Length: 37\r\n\r\n<h1>\n414 Request-URI Too Long\n</h1>\r\n");
        this->setContentLenght(std::to_string(this->getfinalString().size()));
    }
    else if (status_code == 403)
    {
        this->setFinalString("HTTP/1.1 403 Error Forbidden\r\nContent-Type: text/html\r\nContent-Length: 31\r\n\r\n<h1>\n 403 Error Forbidden</h1>\r\n");
        this->setContentLenght(std::to_string(this->getfinalString().size()));
        
    }
    else if (status_code == 405)
    {
        this->setFinalString("HTTP/1.1 405 Error Forbidden\r\nContent-Type: text/html\r\nContent-Length: 31\r\n\r\n<h1>\n 405 Error Forbidden</h1>\r\n");
        this->setContentLenght(std::to_string(this->getfinalString().size()));
        
    }
}

void	response::setRespond(std::string path, std::string httpVersion, std::string error)
{
    if(!error.empty())
    {
        if(error == "501 Not Implemented")
        {
            this->letItBe(httpVersion, "501", "Not Implemented");
            return;
        }
        else if(error == "403")
        {
            this->letItBe(httpVersion, "403", "Forbidden");
            return;
        }
        else if(error == "400")
        {
            this->letItBe(httpVersion, "400", "Bad Request");
            return;
        }
        else if(error == "413 Request Entity Too Large")
        {
            this->letItBe(httpVersion, "413", "Request Entity Too Large");
            return;
        }
        else if(error == "414 Request-URI Too Long")
        {
            this->letItBe(httpVersion, "414", "Request-URI Too Long");
            return;
        }
        else if(error == "400 Bad Request")
        {
            this->letItBe(httpVersion, "400", "Bad Request");
            return;
        }
        else if(error == "404")
        {
            this->letItBe(httpVersion, "404", "Not Found");
            return;
        }
        else if(error == "405")
        {
            this->letItBe(httpVersion, "405", "Method Not Allowed");
            return;
        }
        else if(error == "301")
        {
			current_location location = server.getOneLocationObject(this->current_location_index);
	        int index;

			this->bodyFlag = false;
			this->sethttpVersion(httpVersion);
			this->setstatusCode("301");
			this->setstatusDescription("Moved Permanently");
			this->setContentLenght(std::to_string(this->getBody().size()));
	        
	        if(location.getLocationRedirectionObject().substr(0, 5) == "http:" || location.getLocationRedirectionObject().substr(0, 6) == "https:")
	        {
	            this->setLocation(location.getLocationRedirectionObject());
	           	this->mergeRespondStrings();
                std::cout << this->finalString << '\n';
                return ;
	        }
	        else
	        {
				index = this->server.getLocationByName(location.getLocationRedirectionObject());
				if(index != -1)
				{
					if(path[path.size() - 1] == '/')
						path = path.substr(0, path.length() - 1);
					this->setLocation(location.getLocationRedirectionObject() + '/');
					this->mergeRespondStrings();
					return ;
				}
				else
				{
					this->setRespond(path, this->httpVersion, "404");
					return ;
				}
			return ;
			}
		}
		else if(error == "201") // << HERE
		{
			this->bodyFlag = false;
            this->sethttpVersion(httpVersion);
            this->setstatusCode("201");
            this->setstatusDescription("Created");
            setContentType("text/plain");
            this->setBody("Created");
            this->setContentLenght(std::to_string(this->getBody().size()));
			this->mergeRespondStrings();
            return ;
        } 
    }
    if(theFileExists(path) == false || isFileOrDirectory(path) == "error")
    {
        this->letItBe(httpVersion, "404", "Not Found");
        return;
    }
    else
    { 
        cleanAll();
        this->pathSave = path;
        if(this->getBodyFlag() == false)
        {
            this->pathSave = path;
            this->setChunkPosition(0);
            this->setContentType(getFileType(path));
            this->sethttpVersion(httpVersion);
            this->setstatusCode("200");
            this->setstatusDescription("OK");
            this->setContentLenght(std::to_string(fileToSring(path).size()));
            this->Body.clear();
            if(getfinalString().size() == 0)
		        this->mergeRespondStrings();
        }
        return ;
    }
}
