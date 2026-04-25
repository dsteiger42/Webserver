/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiContext.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 00:00:00 by rafael            #+#    #+#             */
/*   Updated: 2026/04/25 00:00:00 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGICONTEXT_HPP
# define CGICONTEXT_HPP

# include <string>
# include <ctime>
# include <sys/types.h>

/*
** CgiContext encapsulates the full state of an in-flight CGI process
** for a given client connection.
**
** Lifecycle:
**   1. Created when fork() succeeds inside CGI::launch().
**   2. active = true while the CGI process is running.
**   3. inFd  (inPipe[1])  registered in the main poll() with POLLOUT
**      until the full request body has been written.  Then closed and
**      removed from poll().
**   4. outFd (outPipe[0]) registered in the main poll() with POLLIN
**      until EOF is read (CGI stdout closed).  Then closed, waitpid()
**      called, and the accumulated output handed off to the response
**      builder.
**   5. active = false after step 4.  The client state machine proceeds
**      to serialise the HTTP response into writeBuffer.
**
** Timeout:
**   startTime is set at fork().  cleanup_TimeoutClients() kills the
**   child and closes both fds if (now - startTime) > CGI_TIMEOUT_SEC.
*/

# define CGI_TIMEOUT_SEC 10
# define MAX_CGI_OUTPUT  1000000

struct CgiContext
{
    bool        active;       // true while CGI process is in flight
    pid_t       pid;          // child PID
    int         inFd;         // inPipe[1]  — write request body to CGI stdin
    int         outFd;        // outPipe[0] — read CGI stdout
    std::string bodyToSend;   // full request body to forward
    size_t      bodyOffset;   // bytes already written to inFd
    std::string output;       // accumulated CGI stdout
    time_t      startTime;    // for timeout enforcement

    CgiContext();
    void reset();
};

#endif
