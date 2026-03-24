#include <http/cgi/CGI.hpp>
#include <http/routing/Router.hpp>

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

Response CGI::execute(const Request &req)
{
	Response res;
	int inPipe[2];
	int outPipe[2];
	int status;
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
        waitpid(pid, &status, 0);
    }
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		return (router->make_ErrorCode(500));
	CGIResult result = parse_CGIOutput(output);
	res.set_StatusCode(result.status);
	res.set_Header("Content-Type", result.contentType);
	if (result.headers.count("Location"))
    	res.set_Header("Location", result.headers["Location"]);
	res.set_Body(result.body);
	return (res);
}