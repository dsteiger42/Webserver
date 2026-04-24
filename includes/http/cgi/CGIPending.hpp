
#ifndef CGIPENDING_HPP
# define CGIPENDING_HPP

# include <string>
# include <sys/types.h>  // pid_t

struct CGIPending
{
    pid_t       pid;
    int         outPipeFd;    // read end of CGI stdout (parent reads)
    int         inPipeFd;     // write end of CGI stdin (parent writes body), -1 if closed
    int         clientFd;     // fd do cliente dono deste CGI
    int         waitStatus;
    size_t      bodyWritten;
    size_t      startTick;    // tick da iteração do poll quando o CGI começou
    std::string bodyToWrite;
    std::string outputBuffer;
    bool        active;
    
    CGIPending();
};

#endif
