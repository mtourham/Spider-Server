#include "../../includes/webserv.hpp"

int is_file_or_dir_(std::string & path)
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

size_t remove_header(std::string & request, size_t i,std::string &buffer,std::string &filename,std::string upload)
{
    long pos = 0;
    if(i >= request.length() || !filename.empty())
    {
        std::ofstream post(upload + filename);
        post << buffer;
        post.close();
        filename.clear();
        buffer.clear();
    }
    else
    {
        
        pos =request.find("filename=" , i);
        if(pos != -1)
        {
            for(size_t t = pos + 10; request[t] != '"' ;t++)
                filename += request[t];
            pos = request.find("\r\n\r\n",pos);
            if(pos != -1)
                return pos + 4;
        }
        else
        {
            pos = request.find("\r\n\r\n",i);
            if(pos != -1)
                return pos + 4;
        }
    }
    return i ;
}

void server::multi_part(current_server &serv, int fd) {
    bool is_boundary = false;
    std::string boundary;
    std::string filename;
    std::string buffer;
    std::string buffer01;

    if (_request[fd].get_header().find("Content-Type") != _request[fd].get_header().end())
        buffer01 = _request[fd].get_header()["Content-Type"];

    for (size_t i = buffer01.find("=") + 1; i < (size_t)buffer01.length(); ++i) {
        if (buffer01[i] != '-')
            boundary += _request[fd].get_header().find("Content-Type")->second[i];
    }

    for (size_t i = 0; i < (size_t)_request[fd].get_body().length(); ++i) {
        i = remove_header(_request[fd].get_body(), i, buffer, filename, serv.getServerLocationsObject()[_request[fd].get_start_line().current_location_index].parseUploadObject());

        for (; i < (size_t)_request[fd].get_body().length(); ++i) {
            if ((_request[fd].get_body()[i] == 13 && _request[fd].get_body()[i + 1] == 10 && _request[fd].get_body()[i + 2] == '-')
                || (_request[fd].get_body()[i] == 10 && _request[fd].get_body()[i + 1] == '-')
                || (_request[fd].get_body()[i] == 13 && _request[fd].get_body()[i + 1] == '-')) {
                if (_request[fd].get_body().find(boundary, _request[fd].get_body().find(boundary) + i) < i + 32) {
                    break;
                    is_boundary = true;
                }
                is_boundary = false;
            }

            if (is_boundary == false)
                buffer += _request[fd].get_body()[i];
        }
    }
}

void server::post_method(current_server &serv, int fd) {
    std::string extention;
    std::string boundary;
    std::string buffer;
    int is_dir_or_not = 0;
    std::string error = "";
    DIR* dir;
    struct dirent* entry;

    if (!_request[fd].get_body().empty()) {
        if (!serv.getServerLocationsObject()[_request[fd].get_start_line().current_location_index].parseUploadObject().empty()) {
            if (_request[fd].get_header().find("Content-Type") != _request[fd].get_header().end()) {
                if (strncmp(_request[fd].get_header()["Content-Type"].c_str(), "multipart", 9) == 0)
                    multi_part(serv, fd);
                else {
                    struct timeval tp;
                    gettimeofday(&tp, NULL);
                    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
                    buffer = _request[fd].get_header()["Content-Type"];
                    for (int i = 0; i < (int)buffer.length(); ++i)
                        if (buffer[i] == '/')
                            for (int j = i + 1; j < (int)buffer.length(); ++j)
                                extention += buffer[j];
                    std::ofstream post(serv.getServerLocationsObject()[_request[fd].get_start_line().current_location_index].parseUploadObject() + std::to_string(ms) + '.' + extention);
                    post << _request[fd].get_body();
                    post.close();
                }
            }
            error = "201";
        } else {
            is_dir_or_not = is_file_or_dir_(_request[fd].get_start_line().full_path);
            if (is_dir_or_not == -1) {
                error = "404";
            } else if (is_dir_or_not == 2) {
                bool is_found = false;
                if (_request[fd].get_start_line().full_path[_request[fd].get_start_line().full_path.length() - 1] != '/') {
                    error = "301";
                } else {
                    if ((dir = opendir(_request[fd].get_start_line().full_path.c_str())) != 0) {
                        while ((entry = readdir(dir)) != 0) {
                            if (entry->d_type == DT_REG) {
                                if (strncmp(entry->d_name, "index.php", 9) == 0 || strncmp(entry->d_name, "index.py", 8) == 0)
                                    is_found = true;
                            }
                        }
                        closedir(dir);
                    }
                }
                if (!is_found && error == "") {
                    error = "403";
                } else if (!serv.getServerLocationsObject()[_request[fd].get_start_line().current_location_index].parseCgiPathObject(_request[fd].get_start_line().full_path + "").empty() && is_found) {
                    _request[fd].get_start_line().full_path += "index.php";
                    cgi_handler cgi(_server_config, _request[fd], _fd_port_map[fd]);
                    cgi.exec(_respond[fd]);
                    return;
                } else if (error == "")
                    error = "403";
            } else if (_request[fd].get_start_line().path.find_last_of(".php") != std::string::npos ||
                       _request[fd].get_start_line().path.find_last_of(".py") != std::string::npos) {
                if (!serv.getServerLocationsObject()[_request[fd].get_start_line().current_location_index].parseCgiPathObject(_request[fd].get_start_line().full_path).empty()) {
                    cgi_handler cgi(_server_config, _request[fd], _fd_port_map[fd]);
                    cgi.exec(_respond[fd]);
                    return;
                } else
                    error = "403";
            } else
                error = "403";
        }
    }
    std::cout << error << std::endl;
    _respond[fd].setRespond(_request[fd].get_start_line().full_path, _request[fd].get_start_line().httpV, error);
}
