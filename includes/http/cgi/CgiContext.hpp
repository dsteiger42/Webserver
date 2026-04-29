/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiContext.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 00:00:00 by rafael            #+#    #+#             */
/*   Updated: 2026/04/27 04:12:11 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGICONTEXT_HPP
# define CGICONTEXT_HPP

# include <string>
# include <sys/types.h>


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
    unsigned long startTime;    // for timeout enforcement

    CgiContext();
    void reset();
};

#endif
