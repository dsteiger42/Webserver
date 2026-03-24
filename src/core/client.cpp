#include <core/client.hpp>

Client::Client() : fd(-1), readBuffer(4096), writeBuffer(4096), request(), response()
{
}

Client::Client(int fileD) : fd(fileD), readBuffer(4096), writeBuffer(4096), request(), response()
{
}
