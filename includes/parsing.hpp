#ifndef PARSING_HPP
#define PARSING_HPP
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <stack>

class current_location
{
private:
    bool has_301_code;
    bool location_has_cgi;
    bool is_auto_index;
    bool location_has_redirection;
    std::string root;
    int client_max_body_size;
    std::string index;
    std::string redirection;
    std::string location_name;
    std::vector<int> error_codes;
    std::vector<std::string> cgiExtension;
    std::vector<std::string> cgiPaths;
    std::vector<std::string> httpMethods;
    std::string upload;
    int current_location_index;
    std::string error_page;

public:
    current_location(){};
    ~current_location(){};
    current_location(current_location const &s);
    current_location &operator=(current_location const &s);

    void setLocationIndex(size_t i);

    int getLocationCmbsObject(void);
    int getLocationindexObject(void);
    bool getLocationIsAutoIndexObject(void) const;
    std::string getLocationIndexObject(void) const;
    std::string getLocationRedirectionObject(void) const;
    std::string getLocationErrorPageObject(void) const;
    std::string getLocationRootObject(void) const;
    std::string parseLocationNameObject(void) const;
    std::vector<int> getLocationErrorCodesObject(void);
    std::vector<std::string> getLocationMethodsObject(void);
    bool getLocationHas301Code(void);
    bool getHasCgi(void);
    bool getHasRedirection(void);

    std::string parseUploadObject(void);
    std::string parseCgiPathObject(std::string path);
    void requestParseroot(std::string *keys, size_t size);
    void parseAutoIndex(std::string *keys, size_t size);
    void build_location_object(std::vector<std::string>::iterator first, std::vector<std::string>::iterator last);
    void parseCgiExec(std::string *Keys, size_t size);
    void parseLocationCmds(std::string *keys, size_t size);
    void parseErrorPage(std::string *keys, size_t size);
    void parseIndexPage(std::string *keys, size_t size);
    void parseMethods(std::string *Keys, size_t size);
    void parseCgiPath(std::string *Keys, size_t size);
    void requestParseredirection(std::string *keys, size_t size);
    void parseUpload(std::string *Keys, size_t size);
    void parseLocationName(std::string *Keys, size_t size);
    bool isCGI(std::string path);
    void checkCgiAllowed(void);
};

class current_server : public current_location
{
private:
    std::vector<int> port;
    int host;
    int server_index;
    bool server_get_method_in;
    int client_max_body_size;
    size_t location_count;
    std::string root;
    std::string index;
    std::vector<std::string> server_names;
    std::vector<int> error_codes;
    std::string error_page;
    std::string redirection;
    std::vector<current_location> location;
    bool server_post_method_in;
    bool server_delete_method_in;
    bool is_auto_index;
public:
    current_server(){};
    current_server(std::vector<std::string>::iterator first, std::vector<std::string>::iterator last)
    {
        this->build_server(first, last);
    }
    ~current_server(){};
    current_server(current_server const &s);
    current_server &operator=(current_server const &s);
    std::vector<int> parsePortObject(void) const;
    int parseHostObject(void) const;
    std::vector<std::string> parseServerNamesObject(void) const;
    void parseCmds(std::string *keys, size_t size);
    void parseErrorPage(std::string *keys, size_t size);
    void parseHost(std::string *keys, size_t size);
    void parsePort(std::string *Port, size_t temp_size);
    void parseIndexPage(std::string *keys, size_t size);
    void parseAutoIndex(std::string *keys, size_t size);
    void parseServerName(std::string *keys, size_t size);

    void requestParseroot(std::string *keys, size_t size);
    void requestParseredirection(std::string *keys, size_t size);
    std::string requestParserootObject(void) const;
    std::string requestParseredirectionObject(void) const;
    int convertIpToInt(std::string host);
    
    void setLocationsIndex(std::vector<current_location> location);
    void setServerIndex(int index);

    void checkServerData(void);
    int getCmbsObject(void) const;
    int getServerIndexObject(void) const;
    bool getIsAutoIndexObject(void) const;
    std::string getIndexObject(void) const;
    std::string getServerErrorPageObject(void) const;
    std::vector<current_location> getServerLocationsObject(void) const;
    std::vector<int> getErrorCodesObject(void) const;
    current_location getOneLocationObject(int index) const;
    void build_server(std::vector<std::string>::iterator first, std::vector<std::string>::iterator last);
    bool is_digits(const std::string &str);
    size_t getLocationCount(void) const;
    size_t getLocationCount(std::vector<std::string> temp);
    std::vector<current_location> getServerLocation(void) const;
    int getLocationByName(std::string name) const;
    void getServerDataFromRootLocation(void);

    void restoreCmbs(int i);
    void restoreAutoIndex(int i);
    void restoreRootObject(int i);
    void restoreIndexObject(int i);
    void restoreServerMethods(int i);
};

class config
{
private:
    int server_size;
    std::vector<current_server> servers;

public:
    config(){};
    config(config &c);
    config(std::string filename);
    ~config(){};
    config &operator=(config const &c);
    
    void checkBrackets(std::string file);
    bool serverHasDuplicatePorts();
    void serversIndexInit();
    void parseServerName(std::string *keys, size_t size);

    std::vector<current_server> getServersObject(void) const;
    int getServersCount(std::vector<std::string> temp);
    size_t getServersCount(void) const;
};

std::string *split_string(std::string split, char c, size_t *index_save);

#endif
