#include "../../includes/servers.hpp"

servers::servers() {}

servers::servers(servers const & s)
{
    *this = s;
}

servers::~servers() {}

servers &   servers::operator=(servers const & s)
{
    _fds = s._fds;
    _rfds = s._rfds;
    _max_fd = s._max_fd;
    _servers = s._servers;
    _fds_cnxs = s._fds_cnxs;
    return  *this;
}

int servers::setup_server(std::vector<current_server> current_server)
{
    size_t  i;

    for (i = 0; i < current_server.size(); i++)
    {
        server  s(current_server[i].parsePortObject(), current_server[i].parseHostObject());
        for (size_t j = 0; j < s.get_port().size(); j++)
        {
            try
            {
                s.setup_server(current_server[i], j);
                _servers.insert(std::make_pair(s.get_fd_socket(), s));
            }
            catch (std::string const & msg)
            {
                std::cout << msg << '\n';
                if (s.get_error_flag() == 1)
                {
                    s.close();
                    s.set_error_flag(0); 
                }
                _servers.erase(s.get_fd_socket());
            }
        }
    }

    FD_ZERO(&_fds);

    _max_fd = 0;
    for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); it++)
    {
        FD_SET((*it).first, &_fds);
        if (_max_fd < (*it).first)
            _max_fd = (*it).first;
    }
    if (_max_fd == 0)
    {
        std::cerr << "ERROR: couldn't run_server servers\n";
        return 1;
    }
    return 0;
}

void    servers::run_server()
{
    int             r;
    struct timeval  time;

    time.tv_sec = 10;
    time.tv_usec = 0;
    
    while(1)
    {
        FD_ZERO(&_rfds);
        memcpy(&_rfds, &_fds, sizeof(_fds));
        FD_ZERO(&_wfds);
        for (std::vector<int>::iterator it = _fds_is_ready.begin(); it != _fds_is_ready.end(); it++)
            FD_SET(*it, &_wfds);

        r = select(_max_fd + 1, &_rfds, &_wfds, NULL, &time);
        if (r == -1)
        {
            std::cerr << "ERROR: failed to select sockets.\n";
             
            for (std::map<int, server>::iterator it = _fds_cnxs.begin(); it != _fds_cnxs.end(); it++)
                ::close((*it).first);

            _fds_is_ready.clear();
            _fds_cnxs.clear();
            FD_ZERO(&_rfds);
            FD_ZERO(&_wfds);
            FD_ZERO(&_fds);

            _max_fd = 0;
            for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); it++)
            {
                FD_SET((*it).first, &_fds);
                if (_max_fd < (*it).first)
                    _max_fd = (*it).first;
            }
            continue ;
        }
        else if (r == 0)
            continue ;

        // accept connections
        for (std::map<int, server>::iterator it = _servers.begin(); it != _servers.end(); it++)
        {
            if (FD_ISSET((*it).first, &_rfds))
            {
                try
                {
                    (*it).second.accept((*it).first);
                    FD_SET((*it).second.get_fd_connection(), &_fds);
                    _fds_cnxs.insert(std::make_pair((*it).second.get_fd_connection(), (*it).second));
                    if (_max_fd < (*it).second.get_fd_connection())
                        _max_fd = (*it).second.get_fd_connection();
                    std::cout << "host: " << (*it).second.get_host() << ", port: " << (*it).second.get_fd_port((*it).first)
                        << " accept a new connections\n\n";
                }
                catch(const std::string& msg)
                {
                    std::cerr << msg << '\n';
                }
            }
        }

        // receive requests
        for (std::map<int, server>::iterator it = _fds_cnxs.begin(); it != _fds_cnxs.end(); it++)
        {
            if (FD_ISSET((*it).first, &_rfds))
            {
                try
                {
                    (*it).second.receive((*it).first);
                    (*it).second.process((*it).first);
                    _fds_is_ready.push_back((*it).first);
                }
                catch(const std::string& msg)
                {
                    int fd = (*it).first ;
                    std::cerr << msg << "\n";
                    FD_CLR(fd, &_fds);
                    FD_CLR(fd, &_wfds);
                    std::vector<int>::iterator i = std::find(_fds_is_ready.begin(), _fds_is_ready.end(), fd);
                    if (i != _fds_is_ready.end())
                        _fds_is_ready.erase(i);
                    _fds_cnxs[fd].get_fd_port().erase(fd);
                    _fds_cnxs.erase(fd);
                    close(fd);
                    break ;
                }
            }
        }

        // send response
        for (size_t i = 0; i < _fds_is_ready.size(); i++)
        {
            if (FD_ISSET(_fds_is_ready[i], &_wfds))
            {
                try
                {
                        _fds_cnxs[_fds_is_ready[i]].send(_fds_is_ready[i]);
                        if (_fds_cnxs[_fds_is_ready[i]].getRespond(_fds_is_ready[i]).getBodyFlag() == false)
                        {   
                            if (_fds_cnxs[_fds_is_ready[i]].get_request()[_fds_is_ready[i]].get_header()["Connection"] != "keep-alive")
                            {
                                close(_fds_is_ready[i]);
                                _fds_cnxs[_fds_is_ready[i]].get_fd_port().erase(_fds_is_ready[i]);
                                _fds_cnxs.erase(_fds_is_ready[i]);
                                FD_CLR(_fds_is_ready[i], &_fds);
                            }    
                            _fds_is_ready.erase(_fds_is_ready.begin() + i);
                        }
                }
                catch(const std::string& msg)
                {
                    std::cerr << msg << "\n";
                    close(_fds_is_ready[i]);
                    FD_CLR(_fds_is_ready[i], &_fds);
                    _fds_cnxs[_fds_is_ready[i]].get_fd_port().erase(_fds_is_ready[i]);
                    _fds_cnxs.erase(_fds_is_ready[i]);
                    _fds_is_ready.erase(_fds_is_ready.begin() + i);
                }
            }
        }
    }
}
