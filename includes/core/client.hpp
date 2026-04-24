/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/03/24 02:59:09 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <http/buffer/CircularBuffer.hpp>
# include <http/request/Request.hpp>
# include <http/response/Response.hpp>
# include <iostream>

class Client
{
  public:
	int fd;
	CircularBuffer writeBuffer; // the server writes a response to the client
	Request request;
	Response response;
	time_t lastActivity;
	time_t requestStart;
    bool drain;

	Client();
	Client(int fileD);
};

#endif
