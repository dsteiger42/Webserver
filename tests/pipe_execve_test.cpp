// tests/pipe_execve_test.cpp
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <string>

int main()
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0) // filho
    {
        // fecha lado de leitura
        close(pipefd[0]);

        // redireciona stdout para o pipe
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            return 1;
        }
        close(pipefd[1]);

        // argv e envp mínimos
        char *argv[] = {(char *)"tests/test.cgi", NULL};
        char *envp[] = {(char *)"REQUEST_METHOD=GET", NULL};

        // executa o script
        execve("tests/test.cgi", argv, envp);

        // se execve falhar
        perror("execve");
        return 1;
    }
    else // pai
    {
        // fecha lado de escrita
        close(pipefd[1]);

        // lê tudo do pipe
        char buffer[1024];
        ssize_t n;
        std::string output;

        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0)
        {
            output.append(buffer, n);
        }

        close(pipefd[0]);

        // espera pelo filho
        waitpid(pid, NULL, 0);

        // imprime output do script
        std::cout << "CGI OUTPUT:\n" << output << std::endl;
    }

    return 0;
}