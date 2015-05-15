///////////////////////////////////////////////////////////////////////
// Receiver.cpp - Support receiving messages and dispatching them to //
//              different processing mechanism                       //
// ver 1.0                                                           //                                                    
// Application: CSE687 2015 Project 3 - Message Passing              //
// Platform:    Win7, Visual Studio 2013                             //
// Author:      Wenting Wang, wwang34@syr.edu                        //                                            
///////////////////////////////////////////////////////////////////////

#include "Receiver.h"

using namespace ApplicationHelpers;
using namespace WindowsHelpers;

//----< Client Handler thread starts running this function >-------------------
//----< the socket_ is created for the communication with a specific sender >--
void ClientHandler::operator()(Socket& socket_, Receiver& rcv)
{
	Verbose::show("Begining of CH operator()", always);
	while (true)
	{
		// interpret received command
		std::string command = socket_.recvString();
		Verbose::show("command: " + command, always);
		Verbose::show("WSAgetlastmsg: " + GetLastMsg(false));
		if (command.size() == 0)
			break;
		// used for ex: when a server received a client's file upload request
		if (command == "FILE_UPLOAD")
		{
			if (fileUploadHandling(socket_, rcv))
				Verbose::show("----File Upload Handling passed");
			else
				Verbose::show("----File Upload Handling failed");
			continue;
		}

		// used for ex: when a server received a client's quit request
		if (command == "QUIT"){
			if (quitMsgHandling(socket_, rcv))
				Verbose::show("----Server Quit handling passed", always);
			else
				Verbose::show("----Server Quit handling failed", always);
			continue;
		}

		// used for ex: when a client received a server's file upload reply
		if (command == "FILE_UPLOAD_REPLY"){
			if (fileUploadReplyHandling(socket_, rcv))
				Verbose::show("----Client File Upload Reply accepting passed", always);
			else
				Verbose::show("----Client File Upload Reply accepting failed", always);
			continue;
		}

		// the exit condition for the Client Handler thread
		// used for ex: when a client received a server's quit reply
		if (command == "QUIT_REPLY"){
			if (quitReplyHandling(socket_, rcv))
				Verbose::show("----Client Quit Reply handling passed", always);
			else
				Verbose::show("----Client Quit Reply handling failed", always);
			break;
		}
	}
	
	// we get here if command isn't requesting a processing, e.g., client receive server's "QUIT_REPLY"
	Verbose::show("-- ClientHandler socket connection closing\n", always);
	socket_.shutDown();
	socket_.close();
	Verbose::show("-- ClientHandler thread terminating\n", always);
};

//---------< handling client's file upload request >----------------------
bool ClientHandler::fileUploadHandling(Socket& socket_, Receiver& rcv)
{
	bool ok;
	std::string filename;
	size_t contentLen;
	filename = socket_.recvString();
	contentLen = toOrginType<size_t>(socket_.recvString());
	std::string path = "../Server" + toString(rcv.getNum()) + "/";
	std::string fullfilename = path + filename;
	std::ofstream ofs;

	// used for orginal test
	/*if (FilesInProcessing.count(filename)){
	ofs.open(filename, std::ofstream::binary | std::ofstream::app);
	}
	else{
	FilesInProcessing.insert(filename);
	ofs.open(filename, std::ofstream::binary);
	}*/
	openFileforProcessing(fullfilename, socket_, ofs);
	if (contentLen == 1024)
	{
		const int BufLen = 1024;
		char * buffer = new char[BufLen];
		while (socket_.recvString() != "\n");
		if (!writeToFileBufferData(ok, buffer, contentLen, socket_, ofs)) return false;
		delete[] buffer;
	}
	else{

		// this is the last block of the file
		// Verbose::show("Is receiving last block for file upload of " + filename, always);
		std::string srcAddress, dstAddress, attribStr, mode;

		// reverse the source address and destination address and find other attributes
		while ((attribStr = socket_.recvString()) != "\n"){
			if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
				dstAddress = (attribStr.substr(attribStr.find(':') + 1));
			}
			else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
				srcAddress = (attribStr.substr(attribStr.find(':') + 1));
			}
			else if (attribStr.substr(0, attribStr.find(':')) == "mode"){
				mode = (attribStr.substr(attribStr.find(':') + 1));
			}
		}

		// if the file block is not empty
		//Verbose::show("The msg for " + filename + " come from srcAddress " + dstAddress + " and dstAddress " + srcAddress, always);
		if (contentLen > 0){
			char * buffer = new char[contentLen];
			if (!writeToFileBufferData(ok, buffer, contentLen, socket_, ofs)) return false;
			delete[] buffer;
		}

		// used for orginal test
		/*FilesInProcessing.erase(filename);*/

		// remove the filename and socket pair from the FileSocketPairsInProcessing map
		FileSocketPairsInProcessing.erase(fullfilename);

		// construct a message and push onto the server's receiver's queue
		Message ms_to_process;
		ms_to_process.configureFileUploadMsg(filename, srcAddress, dstAddress, mode);
		ms_to_process.addAttrib("path", path);
		//Verbose::show("When the server receiver's client handing is processing upload msg for " + ms_to_process.getFiletoUpload() + ", dstadress is " + dstAddress, always);
		rcv.enQRequest(ms_to_process);
	}
	
	ofs.close();
	return true;
};
                                                                                                                                                                                                                                                                                                                                                               

//---------< handling client's quit request >-------------------------
bool ClientHandler::quitMsgHandling(Socket& socket_, Receiver& rcv){
	Message quit_reply;
	std::string srcAddress, dstAddress, attribStr;

	// reverse the source address and destination address
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
	}

	// configure a "QUIT" message and enque into the server's receiver
	quit_reply.configureQuitMsg(srcAddress, dstAddress);
	rcv.enQRequest(quit_reply);
	return true;
};

//---------< handling server's quit request reply>-------------------------
bool ClientHandler::quitReplyHandling(Socket& socket_, Receiver& rcv){
	Message ms;
	std::string srcAddress, dstAddress, attribStr;

	// reverse the source address and destination address
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
	}
	// configure a "QUIT_REPLY" message and enque into the client's receiver
	ms.configureQuitMsg(srcAddress, dstAddress);
	ms.setCommand("QUIT_REPLY");
	rcv.enQRequest(ms);
	return true;
};

//---------< handling server's file upload request reply>----------------------
bool ClientHandler::fileUploadReplyHandling(Socket& socket_, Receiver& rcv){
	Message ms;
	std::string result, servernumber, path, filename;
	size_t contentLen;
	filename = socket_.recvString();
	contentLen = toOrginType<size_t>(socket_.recvString());
	std::string srcAddress, dstAddress, attribStr, mode;

	// reverse the source address and destination address and collect all the other attributes
	while ((attribStr = socket_.recvString()) != "\n"){
		if (attribStr.substr(0, attribStr.find(':')) == "srcAddress"){
			dstAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "dstAddress"){
			srcAddress = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "mode"){
			mode = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "result"){
			result = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "servernumber"){
			servernumber = (attribStr.substr(attribStr.find(':') + 1));
		}
		else if (attribStr.substr(0, attribStr.find(':')) == "path"){
			path = (attribStr.substr(attribStr.find(':') + 1));
		}
	}
	// configure a "FILE_UPLOAD_REPLY" message and enque into the client's receiver
	Message file_upload_reply;
	file_upload_reply.configureFileUploadReply(filename, srcAddress, dstAddress, servernumber, path, result, mode);
	rcv.enQRequest(file_upload_reply);
	return true;
};

//----------------< private helper function for check whether the file is in processing >------------------------------
bool ClientHandler::openFileforProcessing(std::string& fileName, Socket& sc, std::ofstream& ofs){
	try{
		// if the file is currently not in processing, just create it, or if it already exists in the drive, override it
		if (FileSocketPairsInProcessing.count(fileName) == 0){
			ofs.open(fileName, std::ofstream::binary);
			FileSocketPairsInProcessing.insert({ fileName, &sc });
		}
		else if (FileSocketPairsInProcessing[fileName] == &sc){
			// if the file is currently in processing and is being written to by the socket_ itself, then append
			// the content to it untill the all the file blocks are received and written into the file                                                         
			ofs.open(fileName, std::ofstream::binary | std::ofstream::app);
		}
		else{
			// if the file is currently in processing and is being written to by another socket_ , then pause
			// and wait until the other socket_has finished writting
			std::unique_lock<std::mutex> l(mtx_);
			cv_.wait(l, [=](){ return FileSocketPairsInProcessing.count(fileName) == 0; });
			ofs.open(fileName, std::ofstream::binary);
		}
	}
	catch (std::exception& ex){
		Verbose::show("  Exception caught:", always);
		Verbose::show(std::string("\n  ") + ex.what() + "\n\n", always);
	}
	return true;
};

bool ClientHandler::writeToFileBufferData(bool& ok, char * buffer, int BufLen, Socket& sc, std::ofstream& ofs){
	if (sc.waitForData(100, 10))
	{
		ok = sc.recv(BufLen, buffer);
		if (sc == INVALID_SOCKET || !ok)
			return false;
		ofs.write(buffer, BufLen);
		Verbose::show("server recvd " + toString(BufLen) + " bytes");
	}
	else
	{
		Verbose::show("client wait for server timed-out", always);
	}
	return true;
}


//------< parameterized constructor, with port number, ip protocol, registration number and binded sender specified >------------
//------< for ex: if the receiver and a sender is owned by the same client/server, then the sender is binded to this receiver >--
//------< a thread is created that pulls requests from the sender blocking queue >-----------------------------------------------
Receiver::Receiver(size_t port, Socket::IpVer ipv, int qNum, Sender& s) : socketlstn(port, ipv), qNum_(qNum), s_(&s){
	socketlstn.start(ch, *this);
	std::thread t([this] { this->deQRequest(); });
	t.detach();
};

//----------< enque a message to the receiver queue >--------------------------------------------------
void Receiver::enQRequest(Message& ms){
	// for original test
	/*switch (qNum_){
	case 1:
	receiver1Q.get().enQ(ms);
	case 2:
	receiver2Q.get().enQ(ms);
	case 3:
	receiver3Q.get().enQ(ms);
	case 4:
	receiver4Q.get().enQ(ms);
	}*/
	receiverQ.enQ(ms);
};

//--------< deque a message from the receiver queue and process it >-------------------------------------
//--------< it loops in a thread created in the receiver constructor >-----------------------------------
//--------< the loop breaks only when the it receives a quit reply message from the server>--------------
void Receiver::deQRequest(){
	do{
		Message ms;
		// for original test
		/*switch (qNum_){
		case 1:
		ms = receiver1Q.get().deQ();
		case 2:
		ms = receiver2Q.get().deQ();
		case 3:
		ms = receiver3Q.get().deQ();
		case 4:
		ms = receiver4Q.get().deQ();
		}*/
		ms = receiverQ.deQ();
		if (ms.getCommand() == "FILE_UPLOAD"){
			std::string path = ms.getAttributes()["path"];
			Verbose::show(ms.getCommand() + " of " + ms.getFiletoUpload() + " has completed, the file stored under " + path + "\n", always);
			// add specific attributes for file upload reply
			ms.setCommand("FILE_UPLOAD_REPLY");
			ms.addAttrib("result", "success");
			ms.addAttrib("servernumber", toString(qNum_));
			s_->enQRequest(ms);
			continue;
		}
		else if (ms.getCommand() == "FILE_UPLOAD_REPLY"){
			std::string result, servernumber, path;
			result = ms.getAttributes()["result"];
			servernumber = ms.getAttributes()["servernumber"];
			path = ms.getAttributes()["path"];
			// display the server's reply of file upload on client's screen
			if (result == "success") Verbose::show(ms.getFiletoUpload() + " has been successfully uploaded to Server" + servernumber + " under " + path + ".\n", always);
			else Verbose::show("The upload of " + ms.getFiletoUpload() + " to Server" + servernumber + " has failed.\n", always);
		}
		else if (ms.getCommand() == "QUIT"){
			Verbose::show("Quit has been received.\n", always);
			ms.setCommand("QUIT_REPLY");
			s_->enQRequest(ms);
		}
		else if (ms.getCommand() == "QUIT_REPLY"){
			Verbose::show("Client receiver is going to stop.\n", always);
			isReceiveThreadEnded = true;
			break;
		}
	} while (true);
};

//--< get the registeration number for the client/server that is using the receiver >-----------------
int Receiver::getNum(){
	return qNum_;
}

//----------< get the reference to the socket listener >----------------------------------------------
SocketListener& Receiver::getSocketListener(){
	return socketlstn;
};

//----------< check if the receiver thread state is valid >----------------------------
bool Receiver::receiveThreadStateValid(){
	return isReceiveThreadEnded == false;
}

Receiver::~Receiver(){
	//delete s_;
}

//-------------< test stub >------------------------------------------------
#ifdef TEST_RECEIVER

int main(){
	/*Sender s;*/
	Verbose v(true);
	SocketSystem ss;
	Receiver r(8090, Socket::IP6, 1);
	std::cout << "\n  press key to exit: ";
	while (std::cin.get() != '\n')
	{
	}
}
#endif