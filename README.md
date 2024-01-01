# SocketProgramming

Socket Programming Learning

## Single Reactor Single Thread

![](/images/sreactorsthread.jpg)
- The Reactor object listens for events through select/poll/epoll (IO multiplexing interface). After receiving the event, it distributes it through dispatch. The specific distribution to the Acceptor object or the Handler object depends on the type of event received.
- If it is a connection establishment event, it will be handled by the Acceptor object. The Acceptor object will obtain the connection through the accept method and create a Handler object to handle subsequent response events.
- If it is not a connection establishment event, the Handler object corresponding to the current connection will respond.
- The Handler object completes read -> process -> send.

## Single Reactor Multi Thread

![](/images/sreactormthread.jpg)
- The Handler object is not responsible for processing, but is only responsible for receiving and sending data. After the Handler object reads the data through read, it will send the data to the Processor object in the child thread for processing.
- The Processor object in the sub-thread performs processing. After processing, the result is sent to the Handler object in the main thread, and then the Handler sends the response result to the client through the send method.

## Multi Reactor Multi Thread

![](/images/mreactormthread.jpg)
- The MainReactor object in the main thread monitors the connection establishment event through select. After receiving the event, it obtains the connection through accept in the Acceptor object and assigns the new connection to a child thread.
- The SubReactor object in the child thread adds the connection allocated by the MainReactor object to select to continue monitoring, and creates a Handler to handle the connection's response event.
- If a new event occurs, the SubReactor object will call the Handler object corresponding to the current connection to respond.
- The Handler object completes read -> process -> send.