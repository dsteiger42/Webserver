/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validation_utils.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:32:35 by rafael            #+#    #+#             */
/*   Updated: 2026/03/26 00:27:35 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils/signals/signals.hpp>

volatile sig_atomic_t g_running = 1;

void handle_Sigint(int sig)
{
	(void)sig;
	g_running = 0;
}
