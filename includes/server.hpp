#ifndef SERVER_HPP
#define SERVER_HPP
#define RECV_SIZE 1024
#define CHUNK_SIZE 2048
#include <iostream>
#include <unistd.h>
#include <string>
#include <netinet/in.h>
#include <vector>
#include <fcntl.h>
#include <cstring>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <sys/socket.h>
#include "request.hpp"
#include "response.hpp"
#include "cgi_handler.hpp"
#include "parsing.hpp"
#include <string.h>

class server
{
    private:
        std::vector<int>            _port;
        unsigned int                _host;
        int                         _fd_socket;
        int                         _fd_connection;
        struct sockaddr_in          _addr;
        current_server               _server_config;
        int                         _error_flag;
        std::map<int, int>          _fd_port_map;
        std::map<int, std::string>  _request_map;
        std::map<int, response>      _respond;
        std::map<int, Request>      _request;

    public:
        server();
        server(std::vector<int> port, unsigned int host);
        server(server const & s);
        ~server();

        std::vector<int>    get_port() const;
        int                 get_port(int i) const;
        unsigned int        get_host() const;
        std::map<int, response>  get_respond() const;
        std::map<int, Request>  get_request() const;
        int                 get_fd_socket() const;
        int                 get_fd_connection() const;
        int                 get_error_flag() const;
        int                 get_fd_port(int fd);
        std::map<int, int>  get_fd_port();
        server  &           operator=(server const & s);
        void                accept(int fd);
        void                close();
        void                receive(int fd);
        void                send(int fd);
        void                set_server_config(current_server & server_config);
        void                set_error_flag(int error_flag);
        void                setup_server(current_server & server_config, size_t i);
        void                delete_method(std::string & path, int fd);
        current_server       getServerData(void) {return this->_server_config;}
        void                process(int fd);
        void                Get(int current_location_index , std::string path, int fd);
        response             getRespond(int fd);
        void                post_method(current_server &serv, int fd);
        void                multi_part(current_server &serv,int fd);
        current_server       get_server_config();

    private:
        void            set_addr(int i);
};

#endif