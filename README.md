# IP-Calculator_Server-Client
A simple IP Calculator between a client and server.

Each child thread sends a request to the server with the IP address and subnet mask to determine the rest of the network parameters (network address, broadcast address, min host, max host, and the number of hosts) using stream sockets. After the child threads receive the network parameters from the server, they write them on a memory location available to the main thread. Finally, the main thread prints the information about the network parameters for each number in the input file.
