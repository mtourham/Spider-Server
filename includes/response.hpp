# ifndef RESPOND_HPP
# define RESPOND_HPP
# include "parsing.hpp"
# include "sys/stat.h"
# include <cstdio>
# include <dirent.h>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/uio.h>
# include <unistd.h>
# include <string>

class response
{
    private:
        bool			bodyFlag;
        int             chunkPosition;
        int				current_location_index;
        int             fd;
        std::string		httpVersion;
	    std::string		ContentLenght;
        std::string		statusCode;
        std::string     location;
        std::string		Body;
		std::string		pathSave;
        std::string		statusDescription;
        std::string		content_type;
		std::string		tempAutoIndexFile;
        std::string		finalString;
        std::string     expireDate;
        std::vector<std::string>     cookies;
        current_server	server;
    
    public:
        response() {};
        response(current_server const & s): bodyFlag(false), server(s) {};
        response(response const & r) {*this = r;}

        void        setRespondServer(current_server s) {this->server = s;}
		void		setRespondLocationIndex(int current_location_index) {this->current_location_index = current_location_index;}
        void        setFd(int fd) {this->fd = fd;}
        void        sethttpVersion(std::string httpVersion);
        void        setstatusCode(std::string statusCode);
        void        setstatusDescription(std::string statusDescription);
        void        setContentLenght(std::string ContentLenght);
        void        setBody(std::string Body);
        void        setLocation(std::string  location);
        void        setCookies(std::string Cookie);
        void        setExpires(std::string expires);
        void        setFinalString(std::string finalString);
        void        setRespond(std::string path, std::string httpVersion, std::string error);
        void		setContentType(std::string const & content_type);
        void        setBodyFlag(bool v);
        void        setBodyChunked(std::string s);
        void        setChunkPosition(int chunk);
        std::string setErrorBody(std::string status_code);

		int			getRespondLocationIndex(void) {return (this->current_location_index);}
        std::string gethttpVersion(void);
        std::string getstatusCode(void);
        std::string getstatusDescription(void);
        std::string getContentLenght(void);
        std::string getBody(void);
        std::string getfinalString(void);
        int         getFd(void);
        std::string getCookies(void);
        std::string getExpires(void);
        std::string getLocation(void);
        size_t      getContentLenght_sizet(void);
        std::string	getFileType(const std::string& fileName);
        bool        getBodyFlag();
        std::string & getPathSave(void);
        int         getChunkPosition(void);
        response     getRespond(void);

        bool        isAmongErrorCodes(int error_code);
        std::string fileToSring(std::string path);
        std::string	errorStringToString(int error);
        std::string	mergeRespondStrings(void);
        void        recoverBody(int status_code);
        void        letItBe(std::string httpVersion, std::string statusCode, std::string statusDesc);
        
        void		cleanAll(void);
        std::string	parseAutoIndexPage(std::string path);
        std::string chunkedFileToString(std::string path);

        

        
};

bool theFileExists(const std::string& fileName);
bool isDirectory(const char* path);
std::string	isFileOrDirectory(std::string path);


#endif