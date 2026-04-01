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

#include <http/routing/file_handler.hpp>

Response Router::make_ErrorCode(size_t code)
{
	Response res(_config.errorPages);
	res.set_StatusCode(code);
	std::string path = _documentRoot + res.get_FilePath();
	std::string page;
	if (!read_File(path, page))
	{
		std::cout << "aqui11111\n";
		std::stringstream ss;
		ss << "<h1>" << code << " "
			<< "Error Ocurred"
			<< "</h1>";
		res.set_Body(ss.str());
		return (res);
	}
	res.set_Body(page);
	return (res);
}
