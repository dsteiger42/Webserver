/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cgi_env.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:13:49 by rafael            #+#    #+#             */
/*   Updated: 2026/03/24 01:20:41 by rafael           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <signal.h>

extern volatile sig_atomic_t g_running;
void handle_Sigint(int sig);