/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/24 03:28:18 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <core/client.hpp>


CGIPending::CGIPending() : pid(-1), outPipeFd(-1), inPipeFd(-1), clientFd(-1),  waitStatus(0), bodyWritten(0), startTick(0), active(false)
{
}

Client::Client() : fd(-1), readBuffer(65536), writeBuffer(4096),
    request(), response(), lastActivityTick(0), requestStartTick(0), drainStartTick(0), drain(false), cgi()
    {
    }
Client::Client(int fileD) : fd(fileD), readBuffer(65536), writeBuffer(4096),
    request(), response(), lastActivityTick(0), requestStartTick(0), drainStartTick(0), drain(false), cgi()
{
}