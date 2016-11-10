# Makefile for the socket programming example
#
CXXFLAGS  += -std=c++11

simple_server_objects = ServerSocket.o Socket.o servidor.o
simple_client_objects = ClientSocket.o Socket.o cliente.o


all : servidor cliente

servidor: $(simple_server_objects)
	g++ -std=c++11 -o servidor $(simple_server_objects)


cliente: $(simple_client_objects)
	g++ -std=c++11 -o cliente $(simple_client_objects)


Socket: Socket.cpp
ServerSocket: ServerSocket.cpp
ClientSocket: ClientSocket.cpp
servidor: servidor.cpp
cliente: cliente.cpp


clean:
	rm -f *.o servidor cliente
	
