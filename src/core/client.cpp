/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/05/14 05:41:43 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <core/client.hpp>

Client::Client() : fd(-1), writeBuffer(4096), request(), response(), drain(false), shouldClose(false)
{
}

Client::Client(int fileD, unsigned long tick, size_t maxBodySize) : fd(fileD), writeBuffer(4096), request(maxBodySize), response(), lastActivity(tick), requestStart(tick), drain(false), shouldClose(false)
{
}
