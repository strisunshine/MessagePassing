#ifndef CLIENT2_H
#define CLIENT2_H
/////////////////////////////////////////////////////////////////////////
// Client2.h - Used for testing concurrent communication, representing //
//             one of the clients that send messages and get replies   // 
//             from the servers. It can also disconnect with one       //
//             server and connect with another server.                 //            
//                                                                     //
// ver 1.0                                                             //                                                    
// Application: CSE687 2015 Project 3 - Message Passing                //
// Platform:    Win7, Visual Studio 2013                               //
// Author:      Wenting Wang, wwang34@syr.edu                          //                                            
/////////////////////////////////////////////////////////////////////////

#include <string>

#include "../Message/Message.h"
#include "../Receiver/Receiver.h"
#include "../Sender/Sender.h"

class Client2{
public:
	//-----------< constructor >--------------------------------------------------------------------------
	Client2(size_t port = 7060, Socket::IpVer ipv = Socket::IP6, int qNum = 4) : r(Receiver(port, ipv, 4, s)){}

	// for original test
	/*bool connectTo(std::string ip, size_t port);*/

	//----------< configure its own listener port and ip protocol >--------------------------------
	bool configureListener(size_t port, Socket::IpVer ipv){
		r.getSocketListener() = std::move(SocketListener(port, ipv));
	};

	//----------< users call this method to make a request, such as uploading a file, quitting >---
	void makeRequest(Message& ms){
		s.enQRequest(ms);
	};

	//----------< close its connection with a server that it's currently connected to >------------
	bool closeConnectionToServer(){
		s.getSocketConnecter().shutDown();
		s.getSocketConnecter().close();
	};

	//----------< get a reference to its sender member object >------------------------------------
	Sender& getSender(){
		return s;
	}

	//----------< get a reference to its receiver member object >------------------------------------
	Receiver& getReceiver(){
		return r;
	};
private:
	Sender s;
	Receiver r;
};

#endif