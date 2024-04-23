#include "../../includes/webserv.hpp"

config::config(config & c)
{
    this->servers = c.getServersObject();
}

config & config::operator=(config const & c)
{
    this->server_size = c.getServersCount();
    this->servers = c.getServersObject();
    return (*this);
}

size_t config::getServersCount(void) const
{
    return (this->server_size);
}

std::vector<current_server> config::getServersObject(void) const
{
    return (this->servers);
}

config::config(std::string filename)
{
    std::vector<std::string> temp;
    std::vector<current_server> holder;

    std::ifstream file(filename);
    if(!file.good())
    {
        std::cerr << "ERROR: cannot open file!\n";
        exit(1);
    }
    checkBrackets(filename);
    std::string line;
    int         opening;
    int         closing;
    int         server_index = 0;
    while(std::getline(file, line))
    {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n;") + 1);
        std::replace(std::begin(line),std::end(line),'\t',' ');
        std::replace(std::begin(line),std::end(line),'\n',' ');
        temp.push_back(line);
    }
    file.close();
	for(std::vector<std::string>::size_type i = 0; i < temp.size(); i++)
	{
		if ((!strncmp(temp[i].c_str(), "server", 6) && temp[i].back() == '{') || (!strncmp(temp[i].c_str(), "server", 6) && temp[i + 1] == "{"))
		{
			opening = i;
			while(temp[i] != "}")
			{
				if ((!strncmp(temp[i].c_str(), "location", 8) && temp[i].back() == '{') || (!strncmp(temp[i].c_str(), "location", 8) && temp[i + 1] == "{"))
    			{
                	while(temp[i] != "}")
                    	i++;
            	}
                i++;
            }
            closing = i;
            current_server server;
            server.build_server(temp.begin() + opening, temp.begin() + closing + 1);
            server.setServerIndex(server_index);
            this->servers.push_back(server);
            server_index += 1;
        }
		else if(!temp[i].empty())
        {
            std::cerr << "ERROR: wrong directive [" << temp[i] << "]\n";
            exit(1);
        }
    }
    if(serverHasDuplicatePorts())
    {
    	std::cerr << "ERROR: server has duplicate ports\n";
    	exit(1);
    }
}

bool    config::serverHasDuplicatePorts()
{
    for (size_t i = 0; i < servers.size(); i++)
    {
        const current_server & server1 = servers[i];
        for (size_t j = i + 1; j < servers.size(); j++)
        {
            const current_server & server2 = servers[j];
            for (size_t k = 0; k < server1.parsePortObject().size(); k++)
            {
                int port = server1.parsePortObject()[k];
                for (size_t l = 0; l < server2.parsePortObject().size(); l++)
                {
                    int port2 =  server2.parsePortObject()[l];
                    if (port == port2)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void    config::checkBrackets(std::string f)
{
    std::ifstream file(f);
    std::stack<char> brackets;
    char c;

    while (file.get(c))
	{
        if (c == '(' || c == '{' || c == '[')
            brackets.push(c);
		else if (c == ')' || c == '}' || c == ']')
		{
            if (brackets.empty())
			{
                std::cerr << "ERROR: unclosed bracket !\n";
                exit(1);
            }
            char top = brackets.top();
            if ((c == ')' && top == '(') || (c == '}' && top == '{') || (c == ']' && top == '['))
                brackets.pop();
			else
			{
                std::cerr << "unclosed bracket !\n";
                exit(1);
            }
        }
    }
}

void    config::serversIndexInit()
{
    for(size_t i = 0;i < this->servers.size(); i++)
        this->servers[i].setServerIndex(i);
}

int    config::getServersCount(std::vector<std::string> temp)
{
    int count = 0;
    for(std::vector<std::string>::size_type i = 0; i < temp.size(); i++)
        {
            if ((!strncmp(temp[i].c_str(), "server", 6) && temp[i].back() == '{') || (!strncmp(temp[i].c_str(), "server", 6) && temp[i + 1] == "{"))
                count++;  
        }
        return (count);
}
