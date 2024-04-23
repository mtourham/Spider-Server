#include "includes/webserv.hpp"


int main(int argc, char **argv)
{
    if(argc == 2)
    {
        try
        {
            config config(argv[1]);
            servers     s;
            if (s.setup_server(config.getServersObject()))
            {
                return 1;
            }
            s.run_server();
        }
        catch(const std::string& msg)
        {
            std::cerr << msg << '\n';
        }
    }
    else
        std::cerr << "ERROR: bad args number\n";
    
    return 0;
}
