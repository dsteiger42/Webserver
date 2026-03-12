#include <http/cgi/CGI.hpp>
CGI::CGI() {}

CGI::~CGI() {}

std::string CGI::resolveScriptPath(const std::string& path)
{
    std::string finalPath;
    std::string temp;

    temp = path.substr(path.find("/cgi-bin/") + 9);
    finalPath =  router.getDocumentRoot() + "/cgi-bin/" + temp;
    return finalPath;
}

Response CGI::execute(const Request& req)
{
    (void)req;
    Response res;
    std::string scriptPath = resolveScriptPath(req.getPath());
    if (!isInsideRoot(scriptPath))
        return makeErrorCode(403);
    if (!checkFile(scriptPath))
        return makeErrorCode(404);
    if (!isExecutable(scriptPath))
        return makeErrorCode(403);
    res.setStatusCode(200);
    res.setHeader("Content-Type", "text/plain");    
    res.setBody("CGI OK - execute() foi chamado!");
    return res;
}