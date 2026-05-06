_This project has been created as part of the 42 curriculum by dsteiger and raamorim._

DESCRIPTION:
    This project is a fully functional HTTP/1.1 webserver written in C++, capable of serving dynamic content via CGI and maintaining persistent connections.
    The goal is to understand how the web works at a low level by implementing sockets, protocols, and request handling from scratch.

    Concepts:
        - Server: A computer that listens for incoming requests over a network and sends back a response.

        - Web Server: A server that specifically understands HTTP, the language browsers use to ask for web pages, images, and other resources.

        - HTTP Protocol: The set of rules that define how a client and server communicate. A client sends a request following these rules, and the server replies following the same rules. Similar to how humans have to know the same language to communicate.

        - Sockets: The endpoint of a network connection. A socket is what the server opens to start listening for incoming connections.

        - IP & Port:  An IP address identifies a machine on the network. A port identifies a specific service running on that machine. Together they tell the network exactly where to deliver a message, like a street address and an apartment number.

        - Methods: The action the client wants to perform. GET means "give me this resource" (ex: browsing to youtube), POST means "here is some data to process" (ex: commenting on a video), DELETE means "remove this resource" (ex: deleting the comment on a video), and so on. 

        - CGI (Common Gateway Interface): A standard for running external programs on the server to generate dynamic responses. Instead of sending a file, the server executes a script and sends its output back to the client.

        - Request: The message sent by the client to the server. It contains a method, a path (which resource it wants), headers (metadata), and sometimes a body (extra data).

        - Response: The message the server sends back. It contains a status code (200 OK, 404 Not Found, etc.), headers, and usually a body with the requested content.


    Extra Concepts:
        --- TCP vs UDP ---

        TCP:
            - checks if every packet arrived in full
            - reliable

        UDP:
            - faster
            - might lose data, as it does not check if the packets arrived in full
            
        Design Choice: TCP
            - used in almost every web server in the world
            - chosen for his security/reliability


        --- HTTP vs HTTPS ---

        HTTP:
            - older and outdated version
            - messages are not encrypted and can be read by others

        HTTPS:
            - HTTP with a security measure
            - uses TLS (Transport Layer Security) to encrypt messages sent over the Internet.

        Design Choice: HTTP/1.1
            - HTTP because the subject requested
            - 1.1 because its simple, text-based design makes it practical to implement and debug at a low level, while newer versions like HTTP/2 and HTTP/3 introduce significantly more complexity (binary framing, and QUIC/TLS integration) that goes beyond the scope of this project.


        --- Select() vs Poll() vs Epoll() ---
        These are file monitors. They help monitor each client connected and trying to connect.


        Select():
            - Monitors up to FD_SETSIZE (typically 1024) file descriptors.
            - Uses three separate fd_sets: read, write, and exception.
            - Modifies the fd_sets in-place, so they must be reset on every call.
            - O(n) scan. Iterates over all monitored FDs each call, even idle ones.
            - Available on virtually all POSIX systems (widest portability).

        Poll():
            - No hard FD limit (uses a pollfd array you size yourself).
            - Combines events/revents in one struct. No need to rebuild sets each call.
            - Still O(n). Kernel scans all entries regardless of activity.
            - Cleaner API than select(). Easier to add/remove descriptors.
            - Widely supported. Good cross-platform option (Linux, macOS, BSD).

        Epoll():
            - Linux-only. Scales to hundreds of thousands of FDs efficiently.
            - O(1) for event retrieval. Kernel only returns active FDs.
            - Edge-triggered (EPOLLET) or level-triggered (default) modes.
            - Uses epoll_create / epoll_ctl / epoll_wait, which is a more complex setup.
            - Internally uses a red-black tree + ready list. No repeated full scans.

        Design Choice: Poll()
            - No arbitrary FD limit like select()'s 1024 cap.
            - Simpler API than epoll(). No separate create/ctl/wait lifecycle.
            - Good enough for moderate connection counts (hundreds, not millions).
            - Portable: Works on Linux, macOS, BSD without platform-specific code.
            - O(n) cost is acceptable when N stays manageable for this use case.
            - Overall, more capable than select() and simpler than epoll().

INSTRUCTIONS:
    - To compile, write "make".
    - To execute, write the program's name followed by the configuration file. Ex: "./Webserv webserver.conf".
    - To test connections, execute the program, open another terminal and write "nc localhost 8080" or any other port listening.
    - To test connections via browser, write "http://localhost:8080", or any other port listening.
    - To test cgi via browser, write "http://localhost:8080/cgi-bin/test.py", or any other script's name.


RESOURCES:
    - Youtube: used to learn all the concepts related to Web Servers.
    - AI: was used mainly for writing scripts running different tests and to help with some concepts.
    - Github: looked at several projects to see the logic behind them.
    - RFCs: helped understanding the HTTP protocol.
