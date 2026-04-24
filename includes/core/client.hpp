/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/04/24 03:27:41 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <http/buffer/CircularBuffer.hpp>
# include <http/request/Request.hpp>
# include <http/response/Response.hpp>
# include <http/cgi/CGIPending.hpp>
# include <iostream>

class Client
{
  public:
    int fd;
    CircularBuffer readBuffer;
    CircularBuffer writeBuffer;
    Request        request;
    Response       response;
    size_t         lastActivityTick;
    size_t         requestStartTick;
    size_t         drainStartTick;
    bool           drain;
    CGIPending     cgi;

    Client();
    Client(int fileD);
};

#endif