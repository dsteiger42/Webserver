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

std::vector<char*> CGI::buildArguments(const std::string& scriptPath)
{
    args.push_back(scriptPath);
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>(args[0].c_str()));
    argv.push_back(NULL);
    return argv;
}


Response CGI::execute(const Request& req)
{
    (void)req;
    Response res;
    std::vector<char*> argv;
    std::string scriptPath = resolveScriptPath(req.getPath());
    if (!isInsideRoot(scriptPath))
        return makeErrorCode(403);
    if (!checkFile(scriptPath))
        return makeErrorCode(404);
    if (!isExecutable(scriptPath))
        return makeErrorCode(403);
    argv = buildArguments(scriptPath);
    res.setStatusCode(200);
    res.setHeader("Content-Type", "text/plain");    
    res.setBody("CGI OK - execute() foi chamado!");
    return res;
}