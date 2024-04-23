#ifndef CGI_HANDLER
# define CGI_HANDLER
# include <vector>
# include <unistd.h>
# include <fcntl.h>
# include "request.hpp"
# include "parsing.hpp"
# include "response.hpp"
# include <sys/wait.h>


# define CGI_BUFFER 2048

class cgi_handler
{
    private:
        std::vector<std::string>    _env;
        current_server               _server_config;
        Request                     _request;
        current_location             _location;
        

    public:
        cgi_handler(current_server server_config, Request request, int port);
        cgi_handler(cgi_handler const & c);
        ~cgi_handler();

        cgi_handler &   operator=(cgi_handler const & c);
        void            init_env(int port);
        void            exec(response & response);
        void            generate_response(std::string & cgi_response, response & response);
        void            set_content_length(response &response);
        void            parse_header(const std::string &header, response &response);
        void            set_internal_server_error(response &response);
        char**          get_argv();
    
    private:
        cgi_handler();
        char**          vector_to_ptr();

};



#endif