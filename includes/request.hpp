#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include "requestLexer.hpp"
#include "parsing.hpp"

struct Start_line
{
    std::string method;
    std::string path;
    std::string httpV;
    std::string full_path;
    int current_location_index;
    std::string query;
};

class Request 
{

    private:
        Start_line requestLine;
        std::string body;
        std::map<std::string,std::string> header;
        std::string r_error;
        bool         wait_body;
        unsigned long          body_size;
    public:
        Request(): wait_body(false),body_size(0){}
        Request(Request const & r){*this = r;}
        Request & operator=(Request const & r)
        {
            this->requestLine.method = r.requestLine.method;
            this->requestLine.path = r.requestLine.path;
            this->requestLine.full_path = r.requestLine.full_path;
            this->requestLine.httpV = r.requestLine.httpV;
            this->requestLine.current_location_index = r.requestLine.current_location_index;
            this->requestLine.query = r.requestLine.query;
            this->body = r.body;
            this->header = r.header;
            this->r_error = r.r_error;
            this->body_size = r.body_size;
            this->wait_body = r.wait_body;
            return (*this); 
        }
        ~Request(){}

        void    requestParser(std::string &value);
        void    handleBody(std::string buffer);
        void    errors(current_server &serv);
        void    request_well_formed(current_server &serv);
        void    location_is_well(current_server &serv);
        // void    parseRequestLine(Token& token);
        void    parseHeader(Token& token, std::string& value, std::map<std::string, std::string>& headerMap);
        void    parseBody(std::string& value, requestLexer& lexer, std::string& bodyStr);
        void    clear();

        Start_line &get_start_line(){return requestLine;}
        std::map<std::string,std::string> &get_header(){return header;}
        std::string &get_body(){return body;}
        std::string &get_error(){return r_error;}
        bool &get_wait_body(){return wait_body;}
};


#endif