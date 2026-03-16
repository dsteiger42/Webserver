#include <http/cgi/CGI.hpp>

CGI::CGI()
{
}

CGI::~CGI()
{
}

void CGI::setRouter(Router *r)
{
	router = r;
}

std::string CGI::resolveScriptPath(const std::string &path)
{
	std::string finalPath;
	std::string temp;
	temp = path.substr(path.find("/cgi-bin/") + 9);
	finalPath = router->getDocumentRoot() + "/cgi-bin/" + temp;
	return (finalPath);
}

std::vector<char *> CGI::buildArguments(const std::string &scriptPath)
{
	args.clear();
	args.push_back(scriptPath);
	std::vector<char *> argv;
	argv.push_back(const_cast<char *>(args[0].c_str()));
	argv.push_back(NULL);
	return (argv);
}

void CGI::buildEnvironment(const Request &req, const std::string &scriptPath)
{
	int	length;

	env.clear();
	std::stringstream ss;
	length = req.getBody().size();
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
	env.push_back("REDIRECT_STATUS=200");

}

std::vector<char *> CGI::convertEnv(const std::vector<std::string> &env)
{
	std::vector<char *> envp;
	for (size_t i = 0; i < env.size(); i++)
		envp.push_back(const_cast<char *>(env[i].c_str()));
	envp.push_back(NULL);
	return (envp);
}

void CGI::createPipes(int inPipe[2], int outPipe[2])
{
	if (pipe(inPipe) == -1)
	{
		std::cerr << "Error creating inPipe" << std::endl;
		return ;
	}
	if (pipe(outPipe) == -1)
	{
		std::cerr << "Error creating inPipe" << std::endl;
		return ;
	}
}

void CGI::executeChildProcess(int inPipe[2], int outPipe[2],
	const std::string &scriptPath, char *const argv[], char *const envp[])
{
	close(inPipe[1]);
	close(outPipe[0]);
	if (dup2(inPipe[0], STDIN_FILENO) == -1)
		return ;
	if (dup2(outPipe[1], STDOUT_FILENO) == -1)
		return ;
	if (execve(scriptPath.c_str(), argv, envp) == -1)
		exit(1);
}

std::string CGI::handleParentProcess(int inPipe[2], int outPipe[2],
	const Request &req)
{
    std::string buff;
    size_t bytes;
    char temp[4096];
	close(inPipe[0]);
	close(outPipe[1]);
    if (!req.getBody().empty())
	    write(inPipe[1], req.getBody().c_str(), req.getBody().size());
	close(inPipe[1]);
    while((bytes = read(outPipe[0], temp, sizeof(temp))) > 0)
    {
        buff.append(temp, bytes);
    }
    close(outPipe[0]);
    return buff;
}


CGI::CGIResult CGI::parseCGIOutput(const std::string& output)
{
	CGIResult result;

	result.status = 200;
	result.contentType = "text/plain";
	size_t pos = output.find("\r\n\r\n");
	size_t sepLen = 4;
	if (pos == std::string::npos)
    {
        pos = output.find("\n\n");
        sepLen = 2;
    }
    if (pos == std::string::npos)
    {
        result.body = output;
        return result;
    }
	std::string header = output.substr(0, pos);
	result.body = output.substr(pos + sepLen);
	std::istringstream stream(header);
	std::string line;
	while (std::getline(stream, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (line.empty())
			break ;
		size_t colon = line.find(':');
		std::string key = line.substr(0, colon);
		std::string value = line.substr(colon + 1);
		while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
			value.erase(value.begin());
		result.headers[key] = value;
		if (key == "Content-Type")
            result.contentType = value;
		if (key == "Status")
        {
            std::istringstream ss(value);
            ss >> result.status;
        }
		if (key == "Location")
		{
			result.headers["Location"] = value;
			//se cgi nao enviou o status troca para 302
			if (result.status == 200)
				result.status = 302;
		}
    }
    return result;	
}
	


Response CGI::execute(const Request &req)
{
	Response res;
	int inPipe[2];
	int outPipe[2];
	std::vector<char *> argv;
	pid_t pid;
	std::string output;
	std::string scriptPath = resolveScriptPath(req.getPath());
	if (!isInsideRoot(scriptPath))
	{
		std::cout << "root cgi" << std::endl;
		return (makeErrorCode(403));
	}
	if (!checkFile(scriptPath))
		return (makeErrorCode(404));
	if (!isExecutable(scriptPath))
	{
		std::cout << "executable" << std::endl;
		return (makeErrorCode(403));
	}
	argv = buildArguments(scriptPath);
	buildEnvironment(req, scriptPath);
	std::vector<char *> envp = convertEnv(env);
	// 4. Criar pipes
	createPipes(inPipe, outPipe);
    // 5. fork()
	pid = fork();
	if (pid == -1)
		return (makeErrorCode(500));
    // 6. execve()
	if (pid == 0)
		executeChildProcess(inPipe, outPipe, scriptPath, &argv[0], &envp[0]);
	// 7. ler output
	if (pid > 0) // father
    {
		output = handleParentProcess(inPipe, outPipe, req);
        waitpid(pid, NULL, 0);
    }
	// 8. parsear headers
	CGIResult result = parseCGIOutput(output);
	
	// 9. construir Response final
	res.setStatusCode(result.status);
	res.setHeader("Content-Type", result.contentType);
	if (result.headers.count("Location"))
    	res.setHeader("Location", result.headers["Location"]);
	res.setBody(result.body);
	return (res);
}