#ifndef SERVER1_H
#define SERVER1_H
/////////////////////////////////////////////////////////////////////////
// Server1.h - Used for testing concurrent communication, representing //
//             one of the servers that can receive and process messages// 
//             sent by different clients concurrently and send back    //
//             replies to the clients                                  //          
//                                                                     //
// ver 1.0                                                             //                                                    
// Application: CSE687 2015 Project 3 - Message Passing                //
// Platform:    Win7, Visual Studio 2013                               //
// Author:      Wenting Wang, wwang34@syr.edu                          //                                            
/////////////////////////////////////////////////////////////////////////

#include "../Sockets/Sockets/Sockets.h"
#include "../Message/Message.h"
#include "../Receiver/Receiver.h"
#include "../Sender/Sender.h"

#include <string>

class Server1{
public:
	Server1(size_t port = 8090, Socket::IpVer ipv = Socket::IP6, int qNum = 1) : r(Receiver(port, ipv, qNum, s)){}

	//----------< configure its own listener port and ip protocol >--------------------------------
	bool configureListener(size_t port, Socket::IpVer ipv){
		r.getSocketListener() = std::move(SocketListener(port, ipv));
	};

	//----------< users call this method to make a request, such as uploading a file, quitting >---
	void makeRequest(Message& ms){
		s.enQRequest(ms);
	};

	//----------< close its connection with a server that it's currently connected to >-----------
	bool closeConnectionToServer(){ return false; };

	// for original test
	/*bool closeConnection(std::string ip);*/
private:
	Sender s;
	Receiver r;
};

#endif