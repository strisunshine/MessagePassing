///////////////////////////////////////////////////////////////////////////
// Server2.cpp - Used for testing concurrent communication, representing //
//             one of the servers that can receive and process messages  // 
//             sent by different clients concurrently and send back      //
//             replies to the clients                                    //          
//                                                                       //
// ver 1.0                                                               //                                                    
// Application: CSE687 2015 Project 3 - Message Passing                  //
// Platform:    Win7, Visual Studio 2013                                 //
// Author:      Wenting Wang, wwang34@syr.edu                            //                                            
///////////////////////////////////////////////////////////////////////////

#include "Server2.h"

#ifdef TEST_SERVER2

int main(int argc, char* argv[]){

	try{
		Verbose v(false);
		SocketSystem ss;	
		size_t port = 6070;
		if (argc > 1) port = toOrginType<size_t>(argv[1]);
		Verbose::show("Starting Server2 (port: " + toString(port) + ") \n", always);
		Server2 server2;

		while (1);
	}
	catch (std::exception& ex)
	{
		Verbose::show("  Exception caught:", always);
		Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
	}
	return 0;
}

#endif