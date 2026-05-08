/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiContext.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/25 00:00:00 by rafael            #+#    #+#             */
/*   Updated: 2026/04/25 05:33:10 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <http/cgi/CgiContext.hpp>
#include <unistd.h>

CgiContext::CgiContext() : active(false), pid(-1), inFd(-1), outFd(-1),
	bodyOffset(0), startTime(0)
{
}

void CgiContext::reset()
{
	if (inFd != -1)
	{
		close(inFd);
		inFd = -1;
	}
	if (outFd != -1)
	{
		close(outFd);
		outFd = -1;
	}
	active = false;
	pid = -1;
	bodyOffset = 0;
	bodyToSend.clear();
	output.clear();
	startTime = 0;
}
