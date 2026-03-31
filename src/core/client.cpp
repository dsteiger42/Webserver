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

#include <core/client.hpp>

Client::Client() : fd(-1), readBuffer(4096), writeBuffer(4096), request(), response()
{
}

Client::Client(int fileD) : fd(fileD), readBuffer(4096), writeBuffer(4096), request(), response()
{
	lastActivity = time(NULL);
}
