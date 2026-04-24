#include <unistd.h>
#include <iostream>

int main()
{
    const char *script = "tests/test.cgi";

    char *argv[] = {
        (char *)script,
        NULL
    };

    char *envp[] = {
        (char *)"REQUEST_METHOD=GET",
        NULL
    };

    execve(script, argv, envp);

    perror("execve failed");
    return 1;
}