#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

int main()
{
    pid_t pid = fork();

    if (pid == 0)
    {
        char *argv[] = {(char *)"./test.cgi", NULL};
        char *envp[] = {(char *)"REQUEST_METHOD=GET", NULL};

        execve("tests/test.cgi", argv, envp);

        perror("execve");
    }
    else
    {
        waitpid(pid, NULL, 0);
        std::cout << "child finished\n";
    }
}