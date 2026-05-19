/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/05/14 05:41:47 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <http/buffer/CircularBuffer.hpp>
# include <http/request/Request.hpp>
# include <http/response/Response.hpp>
# include <iostream>
# include <http/cgi/CgiContext.hpp>

class Client
{
  public:
	int fd;
	long id;
	CircularBuffer writeBuffer;
	Request request;
	Response response;
	unsigned long lastActivity;
	unsigned long requestStart;
    bool drain;
    bool shouldClose;
	CgiContext      cgi;

	Client();
	Client(int fileD, unsigned long tick, size_t maxBodySize);
};

#endif
