CC = g++
COMPILERFLAGS = -g -o

.PHONY: all clean

all :
#	$(CC) $(COMPILERFLAGS) client1 client1.cpp
#	$(CC) $(COMPILERFLAGS) server1 server1.cpp
	$(CC) $(COMPILERFLAGS) tcpclient tcpclient.cpp

clean :
	$(RM) server1 client1 tcpclient