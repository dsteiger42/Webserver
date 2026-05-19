/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CircularBuffer.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rafael <rafael@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 01:31:55 by rafael            #+#    #+#             */
/*   Updated: 2026/05/06 18:59:09 by rafael           ###   ########.fr       */
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
	size_t _head;
	size_t _tail;
	size_t _size;
	size_t _capacity;
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
	void grow(size_t new_capacity);
};

#endif