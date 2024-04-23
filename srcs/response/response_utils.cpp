#include "../../includes/webserv.hpp"

std::string response::gethttpVersion(void)
{
    return (this->httpVersion);
}

std::string response::getstatusCode(void)
{
    return (this->statusCode);
}

std::string response::getstatusDescription(void)
{
    return (this->statusDescription);
}

std::string response::getContentLenght(void)
{
    return (this->ContentLenght);
}

size_t response::getContentLenght_sizet(void)
{
    return (this->Body.size());
}

bool response::getBodyFlag(void)
{
    return (this->bodyFlag);
}

void response::setBodyFlag(bool v)
{
    this->bodyFlag = v;
}   

std::string response::getBody(void)
{
    return (this->Body);
}

std::string response::getfinalString(void)
{
    return (this->finalString);
}

void    response::sethttpVersion(std::string httpVersion)
{
    this->httpVersion = httpVersion;
}

void    response::setstatusCode(std::string statusCode)
{
    this->statusCode = statusCode;
}

void    response::setstatusDescription(std::string statusDescription)
{
    this->statusDescription = statusDescription;
}

void    response::setContentLenght(std::string ContentLenght)
{
    this->ContentLenght = ContentLenght;
}

void    response::setBody(std::string Body)
{
    this->Body = Body;
    this->ContentLenght = std::to_string(Body.size());
}

void    response::setFinalString(std::string finalString)
{
    this->finalString = finalString;
}

std::string & response::getPathSave(void)
{
    return (this->pathSave);
}

bool theFileExists(const std::string& fileName)
{
    std::ifstream infile(fileName);
    if(infile.good())
        return (true);
    return (false);
}

std::string response::getFileType(const std::string& fileName)
{
    std::string ext = fileName.substr(fileName.find_last_of(".") + 1);

    if (ext == "txt")
        return "text/plain";
    else if (ext == "jpg" || ext == "jpeg")
        return "image/jpeg";
    else if (ext == "png")
        return "image/png";
    else if (ext == "pdf")
        return "application/pdf";
    else if (ext == "html")
        return "text/html";
    else if (ext == "css")
        return "text/css";
    else if(ext == "mp4")
        return "video/mp4";
    else if (ext == "mp3")
        return ("audio/mp3");
    else if (ext == "xml")
        return ("application/xml");
    else if (ext == "json")
        return ("application/json");
    else
        return "text/html";
}


void    response::cleanAll(void)
{
    this->Body.clear();
    this->httpVersion.clear();
    this->statusCode.clear();
    this->statusDescription.clear();
    this->ContentLenght.clear();
    this->finalString.clear();
    this->content_type.clear();
    this->location.clear();
    this->cookies.clear();
    this->expireDate.clear();
}

std::string	isFileOrDirectory(std::string path) // need to fix this one !!
{
    std::ifstream init(path);
    if (isDirectory(path.c_str()))
	{
        return ("directory");
    }
	else if(init.good())
        return ("file");
	return ("error");
}

std::string response::getLocation(void)
{
    return ("Location: " + this->location);
}

void	response::setExpires(std::string expires)
{
	this->expireDate = expires;
}

std::string response::getExpires(void)
{
	return ("Expires: " + this->expireDate);
}

std::string response::getCookies(void)
{
    std::string ret;
    for(size_t i = 0; i < this->cookies.size(); i++)
    {
        ret += "Set-Cookie: " + this->cookies[i] + "\r\n";
    }
	return (ret);
}

void	response::setCookies(std::string Cookie)
{
	this->cookies.push_back(Cookie);
}


void    response::setLocation(std::string location)
{
    this->location = location;
}

void    response::setContentType(std::string const & content_type)
{
    this->content_type = "Content-Type: " + content_type;
}

void    response::letItBe(std::string httpVersion, std::string statusCode, std::string statusDesc) {
    this->bodyFlag = false;
    this->sethttpVersion(httpVersion);
    this->setstatusCode(statusCode);
    this->setstatusDescription(statusDesc);
    this->setBody(this->setErrorBody(this->getstatusCode()));
    this->setContentLenght(std::to_string(this->getBody().size()));
    this->mergeRespondStrings();
}

int response::getChunkPosition(void)
{
    return (this->chunkPosition);
}

void response::setChunkPosition(int chunk)
{
    this->chunkPosition = chunk;
}

bool isDirectory(const char* path) {
    struct stat file_stat;
    if (stat(path, &file_stat) != 0) {
        return false;
    }
    return S_ISDIR(file_stat.st_mode);
}

response response::getRespond(void)
{
    return (*this);    
}