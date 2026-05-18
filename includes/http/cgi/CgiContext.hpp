/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiContext.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 00:00:00 by rafael            #+#    #+#             */
/*   Updated: 2026/05/06 19:00:34 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGICONTEXT_HPP
# define CGICONTEXT_HPP

# include <string>
# include <sys/types.h>


# define CGI_TIMEOUT_SEC 10
# define MAX_CGI_OUTPUT  200000000

struct CgiContext
{
    bool        active;
    pid_t       pid;
    int         inFd;
    int         outFd;   
    std::string bodyToSend;
    size_t      bodyOffset;
    std::string output;
    unsigned long startTime;
    CgiContext();
    void reset();
};

#endif
