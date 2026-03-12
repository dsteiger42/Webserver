#include <http/cgi/CGI.hpp>
CGI::CGI() {}

CGI::~CGI() {}

std::string CGI::resolveScriptPath(const std::string& path)
{
    std::string finalPath;
    std::string temp;

    temp = path.substr(path.find("/cgi-bin/") + 9);
    finalPath =  router->getDocumentRoot() + "/cgi-bin/" + temp;
    return finalPath;
}

std::vector<char*> CGI::buildArguments(const std::string& scriptPath)
{
    args.clear();
    args.push_back(scriptPath);
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>(args[0].c_str()));
    argv.push_back(NULL);
    return argv;
}

void CGI::buildEnvironment(const Request& req, const std::string& scriptPath)
{
    env.clear();
    std::stringstream ss;
    int length = req.getBody().size();
    ss << length;
    std::string contentLength = ss.str();
    env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("QUERY_STRING=" + req.getQuery());
    env.push_back("CONTENT_LENGTH=" + contentLength);
    env.push_back("CONTENT_TYPE=" + req.getHeader("Content-Type"));
    env.push_back("SCRIPT_FILENAME=" + scriptPath);
    env.push_back("SCRIPT_NAME=" + req.getPath());
    env.push_back("SERVER_PROTOCOL=" + req.getVersion());
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
}

std::vector<char*> CGI::convertEnv(const std::vector<std::string>& env)
{
    std::vector<char*> envp;
    for (size_t i = 0; i < env.size(); i++)
        envp.push_back(const_cast<char*>(env[i].c_str()));
    envp.push_back(NULL);
    return envp;    
}
void CGI::setRouter(Router *r)
{
    router = r; 
}
Response CGI::execute(const Request& req)
{
    // (void)req;
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
    buildEnvironment(req, scriptPath);
    std::vector<char*> envp = convertEnv(env);
    // 4. Criar pipes
    // 5. fork()
    // 6. execve()
    // 7. ler output
    // 8. parsear headers
    // 9. construir Response final

    // Temporário enquanto não implementas o resto:
    res.setStatusCode(200);
    res.setHeader("Content-Type", "text/plain");    
    res.setBody("CGI OK - execute() foi chamado!");
    return res;
}