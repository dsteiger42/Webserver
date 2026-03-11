#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <http/CircularBuffer.hpp>
#include <http/Request.hpp>
#include <http/Response.hpp>
#include <iostream>

class Client
{
    public:
        int fd;
        CircularBuffer readBuffer; // the server reads a request from the client
        CircularBuffer writeBuffer; // the server writes a response to the client
        Request request;
        Response response;

        Client(); 
        Client(int fileD);

};

#endif