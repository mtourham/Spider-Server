
#include "../../includes/server.hpp"

server::server(){}

server::server(std::vector<int> port, unsigned int host)
    : _host(host) ,_error_flag(1)
{
    _port = port;
}

server::server(server const & s)
    : _error_flag(1)
{
    *this = s;
}

response server::getRespond(int fd)
{
    return _respond[fd];
}

server::~server() {}

std::vector<int>    server::get_port() const
{
   return _port;
}

int server::get_port(int i) const
{
    return _port[i];
}

unsigned int    server::get_host() const
{
    return _host;
}

int server::get_fd_socket() const
{
    return _fd_socket;
}

int server::get_fd_connection() const
{
    return _fd_connection;
}

int server::get_error_flag() const
{
    return _error_flag;
}

int server::get_fd_port(int fd)
{
    return _fd_port_map[fd];
}

std::map<int, int>  server::get_fd_port()
{
    return _fd_port_map;
}

std::map<int, response>  server::get_respond() const
{
    return _respond;
}

std::map<int, Request>  server::get_request() const
{
    return  _request;
}

server  & server::operator=(server const & s)
{
    _port = s._port;
    _host = s._host;
    _fd_socket = s._fd_socket;
    _fd_connection = s._fd_connection;
    _addr = s._addr;
    _server_config = s._server_config;
    _request_map = s._request_map;
    _request = s._request;
    _respond = s._respond;
    _fd_port_map = s._fd_port_map;
    return *this;
}

void server::setup_server(current_server & server_config, size_t i)
{
    int optval = 1;

    _fd_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd_socket == -1)
    {
        _error_flag = 0;
        throw(std::string("ERROR: failed to create the socket by the host: ") 
            + std::to_string(_host) + std::string(" on port: ") + std::to_string(_port[i]));
    }
    //Allow socket descriptor to be reuseable
    if (setsockopt(_fd_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
        throw(std::string("ERROR: faild to set socket option (setsockopt()) for _fd_socket by the host: ")
            + std::to_string(_host) + std::string(" on port: ") + std::to_string(_port[i]));
    set_addr(i);
    if (bind(_fd_socket, (struct sockaddr*)&_addr, sizeof(_addr)) == -1)
        throw(std::string("ERROR: failed to bind the socket by the host: ")
            + std::to_string(_host) + std::string(" on port: ") + std::to_string(_port[i]));
    if (listen(_fd_socket, 100) == -1)
        throw(std::string("ERROR: failed to listen by the host: ")
            + std::to_string(_host) + std::string(" on port: ") + std::to_string(_port[i]));
    set_server_config(server_config);
    std::cout << "host: " << _host << " is listening on port " << _port[i] << "...\n\n";
    _fd_port_map.insert(std::make_pair(_fd_socket, _port[i]));
}

void    server::set_addr(int i)
{
    memset((char *)&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = htonl(_host);
    _addr.sin_port = htons(_port[i]);
}

void    server::set_error_flag(int error_flag)
{
    _error_flag = error_flag;
}


void server::accept(int fd)
{
    int optval = 1;

    _fd_connection = ::accept(_fd_socket, NULL, NULL);
    if (setsockopt(_fd_connection, SOL_SOCKET, SO_NOSIGPIPE, &optval, sizeof(optval)) == -1)
        throw(std::string("ERROR: faild to set socket option for _fd_socket."));
    if (_fd_connection == -1)
        throw(std::string("ERROR: connection faild."));
    if (fcntl(_fd_connection, F_SETFL, O_NONBLOCK) == -1)
        throw(std::string("ERROR: fcntl() failed."));
    _fd_port_map.insert(std::make_pair(_fd_connection, _fd_port_map[fd]));
}

void    server::close()
{
    ::close(_fd_socket);
}

void    server::receive(int fd)
{
    int     r;
    char    buffer[RECV_SIZE] = {0};

    r = recv(fd, buffer, RECV_SIZE, 0);

    if (r == -1)
        throw(std::string("NOTE: connection closed by client."));
    else if (r == 0)
        throw(std::string("ERROR: failed to receive data, closing connection."));
    if (_request_map.find(fd) != _request_map.end())
    {
        _request_map[fd].clear();
        _request_map[fd] = std::string(buffer, r);
    }
    else
    {
        _request_map.insert(std::make_pair(fd, std::string(buffer, r)));
        _request.insert(std::make_pair(fd, Request()));
        _respond.insert(std::make_pair(fd, response(_server_config)));
    }
}

void    server::send(int fd)
{
    if(_respond[fd].getBody().empty())
        _respond[fd].recoverBody(atoi(_respond[fd].getstatusCode().c_str()));
    
    if(_respond[fd].getBodyFlag() == true)
        _respond[fd].setFinalString(_respond[fd].chunkedFileToString(_respond[fd].getPathSave()));
    if ((::send(fd, _respond[fd].getfinalString().c_str(), _respond[fd].getfinalString().size(), 0)) == -1)
    {
        _respond[fd].cleanAll();
        _request[fd].clear();
        _respond.erase(fd);
        _request.erase(fd);
        throw(std::string("ERROR: failed to send."));
    }
    _respond[fd].cleanAll();
}

void    server::set_server_config(current_server  & server_config)
{
    _server_config = server_config;
}

int is_path_exist(std::string & path)
{
    if (access(path.c_str(), F_OK) == 0)
        return 1;
    return 0;
}

int is_file_or_dir(std::string & path)
{
    struct stat file_info;

    if (stat(path.c_str(), &file_info) == -1)
        return -1;
    if (file_info.st_mode & S_IFDIR)
        return 2;
    if (file_info.st_mode & S_IFREG)
        return 1;
    return -1;
}


int remove_dir(std::string path)
{
    DIR*    dir = opendir(path.c_str());
    if (dir == NULL)
        return 1;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        std::string full_path = std::string(path) + "/" + entry->d_name;

        struct stat st;
        if (lstat(full_path.c_str(), &st) == 0)
        {
            if (S_ISDIR(st.st_mode))
            {
                if (remove_dir(full_path) == 1)
                    return 1;
            }
            else 
                unlink(full_path.c_str());
        }
    }

    closedir(dir);
    rmdir(path.c_str());
    return 0;
}

// Helper function to handle file deletion
bool delete_file(const std::string& path, response& response) {
    if (remove(path.c_str()) == 0) {
        response.setstatusCode("204");
        response.setContentLenght("0");
        response.setstatusDescription("No Content");
        response.mergeRespondStrings();
        return true;
    }
    return false;
}

// Helper function to handle directory deletion
bool delete_directory(const std::string& path, response& response) {
    if (remove_dir(path) == 0) {
        response.setstatusCode("204");
        response.setstatusDescription("No Content");
        response.setContentLenght("0");
        response.mergeRespondStrings();
        return true;
    }
    return false;
}

// Helper function to set response for internal server error
void internal_server_error_response(response& response) {
    response.setstatusCode("500");
    response.setstatusDescription("Internal Server Error");
    response.setContentType("text/html");
    response.setBody("<h1>Internal Server Error</h1>"); // Set error page
    response.mergeRespondStrings();
}

// Helper function to set response for forbidden access
void forbidden_response(response& response) {
    response.setstatusCode("403");
    response.setstatusDescription("Forbidden");
    response.setContentType("text/html");
    response.setBody("<h1>Forbidden</h1>"); // Set error page
    response.mergeRespondStrings();
}

// Helper function to set response for conflict
void conflict_response(response& response) {
    response.setstatusCode("409");
    response.setstatusDescription("Conflict");
    response.setContentType("text/html");
    response.setBody("<h1>Conflict</h1>"); // Set error page
    response.mergeRespondStrings();
}

// Main delete method function
void server::delete_method(std::string& path, int fd) {
    if (is_path_exist(path)) {
        int r = is_file_or_dir(path);
        if (r == 1) { // Handle file cases
            if (delete_file(path, _respond[fd])) {
                return;
            }
            internal_server_error_response(_respond[fd]);
            return;
        } else if (r == 2) { // Handle directory cases
            if (path.back() == '/') {
                struct Start_line start_line = _request[fd].get_start_line();
                if (access(path.c_str(), W_OK) == 0 &&
                    path != _server_config.getServerLocationsObject()[start_line.current_location_index].getLocationRootObject() &&
                    path < _server_config.getServerLocationsObject()[start_line.current_location_index].getLocationRootObject()) {
                    if (delete_directory(path, _respond[fd])) {
                        return;
                    }
                    internal_server_error_response(_respond[fd]);
                    return;
                }
                forbidden_response(_respond[fd]);
                return;
            } else {
                conflict_response(_respond[fd]);
                return;
            }
        } else { // Error
            internal_server_error_response(_respond[fd]);
            return;
        }
    }
    _respond[fd].setstatusCode("404");
    _respond[fd].setstatusDescription("Not Found");
    _respond[fd].setContentType("text/html");
    _respond[fd].setBody("<h1>Not Found</h1>");
    _respond[fd].mergeRespondStrings();
}

void server::Get(int current_location_index , std::string path, int fd) 
{
    if(!strcmp(strrchr(path.c_str(), '/'), "/favicon.ico"))
    {
        if(!strcmp(strrchr(path.c_str(), '/'), "/favicon.ico"))
        {   
            return ;
        }
    }
    current_location location = _server_config.getOneLocationObject(current_location_index);
    std::string isFOrD = isFileOrDirectory(path);

    if(_respond[fd].getstatusCode() == "200" && _respond[fd].getBodyFlag() == false)
    {
        if(isFOrD == "file")
        {
            if(location.getHasCgi() && location.isCGI(_request[fd].get_start_line().full_path)) // check if the extention of file compatible with extentions 
            {
                cgi_handler cgi(_server_config, _request[fd], _fd_port_map[fd]);
                cgi.exec(_respond[fd]);
                return ;
            }    
            if(_respond[fd].fileToSring(path).size() >= 50000 && isFOrD == "file")
            {
                if(_respond[fd].getBodyFlag() == true)
                    return ;
                _respond[fd].setBodyFlag(true);
                    return;
            }
            else
            {
                _respond[fd].setBody(_respond[fd].fileToSring(path));
                _respond[fd].setContentLenght(std::to_string(_respond[fd].fileToSring(path).size()));
                _respond[fd].mergeRespondStrings();
            }
            return ;
        }
        else if(isFOrD == "directory")
        {
            if(path[path.size() - 1] != '/')
            {
                _respond[fd].sethttpVersion(_respond[fd].gethttpVersion());
			    _respond[fd].setstatusCode("301");
			    _respond[fd].setstatusDescription("Moved Permanently");
			    _respond[fd].setContentLenght(std::to_string(_respond[fd].getBody().size()));
                _respond[fd].setLocation(_request[fd].get_start_line().path + '/');
                _respond[fd].mergeRespondStrings();
                std::cout << _respond[fd].getfinalString() << std::endl;
                return ;
            }
            if(isFileOrDirectory(path + location.getLocationIndexObject()) == "file")
            {
                Get(current_location_index, path + location.getLocationIndexObject(), fd);
            }
            else if (location.getLocationIndexObject().size() > 0 &&
                isFileOrDirectory(path + location.getLocationIndexObject()) == "error")
            {
                _respond[fd].setRespond(path, _respond[fd].gethttpVersion(), "404");
                return ;
            }
            else if(location.getLocationIsAutoIndexObject())
            {
                _respond[fd].setBody(_respond[fd].parseAutoIndexPage(path));
                _respond[fd].setContentLenght(std::to_string(_respond[fd].getBody().size()));
                _respond[fd].mergeRespondStrings();
                return ;
            }
            else
            {
                _respond[fd].setRespond(path, _respond[fd].gethttpVersion(), "403");
                return ;
            }
        }
        else
        {
            _respond[fd].setRespond(path, _respond[fd].gethttpVersion(), "404");
            return ;
        }
    }
}

void    server::process(int fd)
{
    if(!_request[fd].get_wait_body())
        _request[fd].requestParser(_request_map[fd]);
    else
    {
        _request[fd].handleBody(_request_map[fd]);
    }
    if(!_request[fd].get_wait_body())
    {

        _request[fd].errors(_server_config);
        _respond[fd].setRespondLocationIndex(_request[fd].get_start_line().current_location_index);
        _respond[fd].setFd(fd);
        _respond[fd].setRespond(_request[fd].get_start_line().full_path, _request[fd].get_start_line().httpV, _request[fd].get_error());
        if(_request[fd].get_error().empty())
        {
            if(_request[fd].get_start_line().method == "GET")
            {
                Get(_request[fd].get_start_line().current_location_index, _request[fd].get_start_line().full_path, fd);
            }
            else if(_request[fd].get_start_line().method == "POST")
            {
                post_method(_server_config,fd);
            }
            else if(_request[fd].get_start_line().method == "DELETE")
            {
                delete_method(_request[fd].get_start_line().full_path, fd);
            }
        }
        _request[fd].clear();
        _request_map.erase(fd);
    }
}

current_server   server::get_server_config()
{
    return _server_config;
}