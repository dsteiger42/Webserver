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

#ifndef CIRCULARBUFFER_HPP
# define CIRCULARBUFFER_HPP

# include <cstring>
# include <iostream>
# include <stdexcept>
# include <vector>

class CircularBuffer
{
  private:
	std::vector<char> _buffer;
	size_t _head;     // write pos
	size_t _tail;     // read pos
	size_t _size;     // numbber of bytes in the buffer
	size_t _capacity; // max size of the buffer
  public:
	CircularBuffer(size_t cap);
	bool is_Empty();
	bool is_Full();
	size_t get_Size();
	size_t write(const char *data, size_t len);
	size_t read(char *out, size_t len);
	size_t peek(char *temp, size_t len) const;
	size_t find(const std::string &pattern) const;
	void consume(size_t bytes);
};

#endif