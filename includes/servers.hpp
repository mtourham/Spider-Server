#ifndef SERVERS_HPP
# define SERVERS_HPP

# include <sys/select.h>
# include <map>
// # include "webserv.hpp"
# include "server.hpp"
# include "parsing.hpp"

class servers
{
    private:
        fd_set                  _fds;
        fd_set                  _rfds;
        fd_set                  _wfds;
        int                     _max_fd;
        std::map<int, server>   _servers;
        std::map<int, server>   _fds_cnxs;
        std::vector<int>        _fds_is_ready;

    public:
        servers();
        servers(servers const & s);
        ~servers();

        servers &   operator=(servers const & s);

        int    setup_server(std::vector<current_server> current_server);
        void    handleSelectError();
        void    run_server();
};

#endif