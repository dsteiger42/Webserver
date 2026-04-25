/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/25 00:00:00 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTER_HPP
# define ROUTER_HPP

# include <algorithm>
# include <config/parser/parser.hpp>
# include <ctime>
# include <dirent.h>
# include <http/cgi/CGI.hpp>
# include <http/request/Request.hpp>
# include <http/response/Response.hpp>
# include <sstream>
# include <utils/http/mime.hpp>
# include <utils/utils.hpp>

class Router
{
  private:
    ServerConfig _config;
    std::string  _path;
    std::string  _query;
    std::string  _method;
    std::string  _documentRoot;
    std::string  _absolutePath;

  public:
    CGI *cgi;

    Router(ServerConfig &sc);
    ~Router();
    Router(const Router &other);
    Router &operator=(const Router &other);

    std::string        get_Path()         const;
    std::string        get_Query()        const;
    std::string        get_Method()       const;
    std::string        get_AbsolutePath() const;
    std::string        get_DocumentRoot() const;
    const ServerConfig &get_Config()      const;

    Response make_ErrorCode(size_t code);

    /*
    ** Returns true if the request targets a CGI location.
    ** Does NOT execute the CGI — purely a classification check.
    ** Used by the server to decide between the sync and async paths.
    */
    bool is_CgiRequest(const Request &request);

    Response handle_GET(const Request &request, Location &location);
    Response handle_DELETE(const Request &request, Location &location);
    Response handle_POST(const Request &request, Location &location);

    bool     validate_Path(const std::string &path);
    void     split_PathQuery(const std::string &path);
    std::vector<std::string> split_Path(const std::string &path);
    bool     build_FinalPath(std::string &path);
    Response handle_Request(const Request &request);
    Location &matchLocation(const std::string &path);
    Response redirect(int redirectCode, std::string redirectUrl);
};

#endif
