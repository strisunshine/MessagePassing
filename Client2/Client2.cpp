///////////////////////////////////////////////////////////////////////////
// Client2.cpp - Used for testing concurrent communication, representing //
//             one of the clients that send messages and get replies     // 
//             from the servers. It can also disconnect with one         //
//             server and connect with another server.                   //            
//                                                                       //
// ver 1.0                                                               //                                                    
// Application: CSE687 2015 Project 3 - Message Passing                  //
// Platform:    Win7, Visual Studio 2013                                 //
// Author:      Wenting Wang, wwang34@syr.edu                            //                                            
///////////////////////////////////////////////////////////////////////////

#include "Client2.h"

using namespace ApplicationHelpers;

//-------------< test stub >------------------------------------------------
#ifdef TEST_CLIENT2

struct Cosmetic
{
	~Cosmetic()
	{
		std::cout << "\n  press key to exit: ";
		std::cin.get();
		std::cout << "\n\n";
	}
};

int main(int argc, char* argv[]){
	try{
		Verbose v(false);
		SocketSystem ss;
		size_t port = 7060, server1port = 8090, server2port = 6070;
		// set Client2's own port 
		if (argc > 6) port = toOrginType<size_t>(argv[6]);
		// set the first server's port it will connect to
		if (argc > 7) server1port = toOrginType<size_t>(argv[7]);
		// set the second server's port it will connect to
		if (argc > 8) server2port = toOrginType<size_t>(argv[8]);
		Verbose::show("Setting up Client2 (Listening port is " + toString(port) + ") \n", always);
		Client2 client2(port);
		Message ms;
		// for original test on different clients sending files with the same name to a server
		/*Verbose::show("\n Request uploading SameFile.txt to Server1", always);
		ms.configureFileUploadMsg("SameFile.txt", "::1@7060", "::1@8090");
		client2.makeRequest(ms);*/
		title("Client2 starts sending messages");
		if (argc > 1){
			Verbose::show("\n Request uploading " + toString(argv[1]) + " to Server1", always);
			ms.configureFileUploadMsg(argv[1], "::1@" + toString(port), "::1@" + toString(server1port));
			client2.makeRequest(ms);
		}
		if (argc > 2){
			Verbose::show("\n Request uploading " + toString(argv[2]) + " to Server1", always);
			ms.configureFileUploadMsg(argv[2], "::1@" + toString(port), "::1@" + toString(server1port));
			client2.makeRequest(ms);
		}

		Verbose::show("\n Request uploading " + toString("1morefiletoServer1") + " to Server1", always);
		ms.configureFileUploadMsg("1morefiletoServer1", "::1@" + toString(port), "::1@" + toString(server1port));
		client2.makeRequest(ms);
		Verbose::show("\n Request uploading 2morefiletoServer1 to Server1", always);
		ms.configureFileUploadMsg("2morefiletoServer1", "::1@" + toString(port), "::1@" + toString(server1port));
		client2.makeRequest(ms);

		//ms.configureQuitMsg("::1@7060", "::1@8090");
		//client2.makeRequest(ms);
		if (argc > 3){
			Verbose::show("\n Request uploading " + toString(argv[3]) + " to Server2", always);
			ms.configureFileUploadMsg(argv[3], "::1@" + toString(port), "::1@" + toString(server2port));
			client2.makeRequest(ms);
		}
		if (argc > 4){
			Verbose::show("\n Request uploading " + toString(argv[4]) + " to Server2", always);
			ms.configureFileUploadMsg(argv[4], "::1@" + toString(port), "::1@" + toString(server2port));
			client2.makeRequest(ms);
		}
		if (argc > 5){
			Verbose::show("\n Request uploading " + toString(argv[5]) + " to Server2", always);
			ms.configureFileUploadMsg(argv[5], "::1@" + toString(port), "::1@" + toString(server2port));
			client2.makeRequest(ms);
		}
		Verbose::show("\n Request uploading " + toString("1morefiletoServer2") + " to Server2", always);
		ms.configureFileUploadMsg("1morefiletoServer2", "::1@" + toString(port), "::1@" + toString(server2port));
		client2.makeRequest(ms);
		Verbose::show("\n Request uploading 2morefiletoServer1 to Server2", always);
		ms.configureFileUploadMsg("2morefiletoServer2", "::1@" + toString(port), "::1@" + toString(server2port));
		client2.makeRequest(ms);

		ms.configureQuitMsg("::1@" + toString(port), "::1@" + toString(server2port));
		client2.makeRequest(ms);
		while (client2.getSender().sendThreadStateValid() || client2.getReceiver().receiveThreadStateValid());
		/*while (1);*/
	}
	catch (std::exception& ex)
	{
		Verbose::show("  Exception caught:", always);
		Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
	}
	{
		Cosmetic aForCosmeticAction;
    }
}
#endif