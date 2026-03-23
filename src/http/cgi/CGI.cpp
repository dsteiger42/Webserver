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

std::string CGI::resolve_ScriptPath(const std::string &path)
{

	std::string locPath = router->matchLocation(path).path;
	std::string relativePath = path.substr(locPath.size() - 1);
	if (!relativePath.empty() && relativePath[0] == '/')
        relativePath.erase(0, 1);
	std::string fullPath = router->get_DocumentRoot();
	if (!fullPath.empty() && fullPath[fullPath.size() - 1] != '/')
		fullPath += '/';
	return (fullPath + relativePath);
}

std::vector<char *> CGI::build_Arguments(const std::string &scriptPath)
{
	_args.clear();
	_args.push_back(scriptPath);
	std::vector<char *> argv;
	argv.push_back(const_cast<char *>(_args[0].c_str()));
	argv.push_back(NULL);
	return (argv);
}

void CGI::build_Environment(const Request &req, const std::string &scriptPath)
{
	int	length;

	env.clear();
	std::stringstream ss;
	length = req.get_Body().size();
	ss << length;
	std::string contentLength = ss.str();
	env.push_back("REQUEST_METHOD=" + req.get_Method());
	env.push_back("QUERY_STRING=" + req.get_Query());
	env.push_back("CONTENT_LENGTH=" + contentLength);
	env.push_back("CONTENT_TYPE=" + req.get_Header("Content-Type"));
	env.push_back("SCRIPT_FILENAME=" + scriptPath);
	env.push_back("SCRIPT_NAME=" + req.get_Path());
	env.push_back("SERVER_PROTOCOL=" + req.get_Version());
	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("REDIRECT_STATUS=200");

}

std::vector<char *> CGI::convert_Env(const std::vector<std::string> &env)
{
	std::vector<char *> envp;
	for (size_t i = 0; i < env.size(); i++)
		envp.push_back(const_cast<char *>(env[i].c_str()));
	envp.push_back(NULL);
	return (envp);
}

void CGI::create_Pipes(int inPipe[2], int outPipe[2])
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

void CGI::execute_ChildProcess(int inPipe[2], int outPipe[2],
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

std::string CGI::handle_ParentProcess(int inPipe[2], int outPipe[2],
	const Request &req)
{
    std::string buff;
    size_t bytes;
    char temp[4096];
	close(inPipe[0]);
	close(outPipe[1]);
    if (!req.get_Body().empty())
	    write(inPipe[1], req.get_Body().c_str(), req.get_Body().size());
	close(inPipe[1]);
    while((bytes = read(outPipe[0], temp, sizeof(temp))) > 0)
    {
        buff.append(temp, bytes);
    }
    close(outPipe[0]);
    return buff;
}


CGI::CGIResult CGI::parse_CGIOutput(const std::string& output)
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
	std::string scriptPath = resolve_ScriptPath(req.get_Path());
	if (!is_InsideRoot(scriptPath, router->get_DocumentRoot()))
		return (router->make_ErrorCode(403));
	if (!check_File(scriptPath))
		return (router->make_ErrorCode(404));
	if (!is_Executable(scriptPath))
		return (router->make_ErrorCode(403));
	argv = build_Arguments(scriptPath);
	build_Environment(req, scriptPath);
	std::vector<char *> envp = convert_Env(env);
	create_Pipes(inPipe, outPipe);
	pid = fork();
	if (pid == -1)
		return (router->make_ErrorCode(500));
	if (pid == 0)
		execute_ChildProcess(inPipe, outPipe, scriptPath, &argv[0], &envp[0]);
	if (pid > 0)
    {
		output = handle_ParentProcess(inPipe, outPipe, req);
        waitpid(pid, NULL, 0);
    }
	CGIResult result = parse_CGIOutput(output);
	res.set_StatusCode(result.status);
	res.set_Header("Content-Type", result.contentType);
	if (result.headers.count("Location"))
    	res.set_Header("Location", result.headers["Location"]);
	res.set_Body(result.body);
	return (res);
}