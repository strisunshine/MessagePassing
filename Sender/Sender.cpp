/////////////////////////////////////////////////////////////////////
// Sender.cpp - Support sending messages and replies to remote end //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 3 - Message Passing            //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////

#include "Sender.h"

using namespace ApplicationHelpers;
using namespace WindowsHelpers;

//----------< default constructor >--------------------------------------------------
//------< a thread is created that pulls requests from the sender blocking queue >---
Sender::Sender(){
	pQ = new BlockingQueue<Message>;
	std::thread t([this] { this->deQRequest(); });
	t.detach();
	Verbose::show("Sender is being created \n");
};

//----------< move constructor >-----------------------------------------------------
Sender::Sender(Sender&& s){
	socketconn = SocketConnecter(std::move(s.socketconn));
	Verbose::show("Sender is being moved \n", always);
};

//----------< assignment operator >--------------------------------------------------
Sender& Sender::operator=(Sender&& s){
	if (this == &s) return *this;
	s.socketconn = SocketConnecter(std::move(s.socketconn));
	Verbose::show("Sender is being move assigned \n", always);
	return *this;
};

bool Sender::sendFile(Message ms){
	bool ok = true;
	//Verbose::show("Plan to open the file " + ms.getFiletoUpload(), always);
	std::ifstream ifs(ms.getFiletoUpload(), std::ifstream::binary);
	//Verbose::show("The ifstream is opened for: " + ms.getFiletoUpload(), always);
	size_t len = ms.getFileUploadBlockLength();
	//Verbose::show("Will go into the while loop inside sendFile for " + ms.getFiletoUpload(), always);
	while (true){
		//std::lock_guard<std::mutex> l(mtx_);
		char * buffer = new char[len];
		ifs.read(buffer, len);

		//if the content is less then the block length, which means it's the last block
		size_t realLen = (size_t)ifs.gcount();
		if (realLen < len){
			ms.setFileUploadBlockLength(realLen);
			//Verbose::show("Is sending the last header string for " + ms.getCommand() + " of " + ms.getFiletoUpload(), always);
			ok = ok && sendFileblockHeader(ms);
			ok = ok && socketconn.send(realLen, buffer);
			delete[] buffer;
			break;
		}
		else{
			ok = ok && sendFileblockHeader(ms);
			ok = ok && socketconn.send(len, buffer);
			delete[] buffer;
		}

		//if the file length happens to be a multiple of the block length
		if (ifs.eof()){
			/*buffer = new char[len];*/
			//Verbose::show("Is sending the last header string for " + ms.getCommand() + " of " + ms.getFiletoUpload(), always);
			ms.setFileUploadBlockLength(0);
			ok = ok && sendFileblockHeader(ms);
			/*socketconn.send(len, buffer);
			delete[] buffer;*/
			break;
		}
	}
	ifs.close();
	if (ok) return true;
	else{
		Verbose::show("There is some error in sending the file: " + ms.getFiletoUpload() +  " \n", always);
		return false;
	}
};

//----------< used by client to send a quit request >------------------------------
bool Sender::sendQuitMsg(Message& ms){
	bool ok = true, eachtime;
	ok = ok && (eachtime = socketconn.sendString(ms.getCommand()));
	if (eachtime == 0) Verbose::show("WSAgetlastmsg: " + GetLastMsg(false), always);
	for (auto attrib : ms.getAttributes()){
		ok = ok && (eachtime = socketconn.sendString(attrib.first + ':' + attrib.second));
		if (eachtime == 0) Verbose::show("WSAgetlastmsg: " + GetLastMsg(false), always);
	}
	ok = ok && (eachtime = socketconn.sendString("\n"));
	if (eachtime == 0) Verbose::show("WSAgetlastmsg: " + GetLastMsg(false), always);
	//socketconn.sendString("");
	if (ok) return true;
	else{
		Verbose::show("There is error in sending quit message \n", always);
		return false;
	}
};

//----------< used by server to send a reply to file upload request >--------------
bool Sender::sendFileUploadReply(Message& ms){
	if (sendFileblockHeader(ms)) return true;
	else{
		Verbose::show("There is error in sending file upload reply \n", always);
		return false;
	}
};

//----------< used by server to send a reply to client's quit request >------------
bool Sender::sendQuitReply(Message& ms){
	if(sendQuitMsg(ms)) return true;
	else{
		Verbose::show("There is error in sending quit message reply \n", always);
		return false;
	}
}

//----------< enque a message to the sender queue >--------------------------------
void Sender::enQRequest(Message ms){
	pQ->enQ(ms);
};

//----------< peek a message from the sender queue >--------------------------------
Message Sender::peekRequest(){
	return pQ->peek();
}

//--------< deque a message from the sender queue and send it >---------------------
//--------< it loops in a thread created in the sender constructor >----------------
//--------< the loop breaks only when the it receives a quit message >--------------
void Sender::deQRequest(){
	do{
		Message ms = pQ->deQ();
		std::string dstAddr = ms.getAttributes()["dstAddress"];

		//if the sender is already connected to an address same with the destination address of this request, then no need to reconnect
		if (connectedAddress != dstAddr){
			if (connectedAddress != ""){
				//socketconn.shutDown();
			}
			std::string ip;
			size_t port;
			ip = dstAddr.substr(0, dstAddr.find('@'));
			port = toOrginType<size_t>(dstAddr.substr(dstAddr.find('@') + 1));
			bool connRes = false; int count=0;
			while (!connRes && count <10){
				connRes = socketconn.connect(ip, port);
				count++;
				Sleep(100);
			}
			if (connRes){
				connectedAddress = dstAddr;
				Verbose::show("-- The Client/Server itself is currently connected to " + dstAddr + "\n", always);
			}
		}
		else Verbose::show("Connection to " + dstAddr + " is the same with last time's: " + connectedAddress, always);

		//the if-else condition below is used to dispatch different commands to different sending mechanism
		if (ms.getCommand() == "FILE_UPLOAD"){
			//std::thread sendFileThread(&Sender::sendFile, *this, ms);
			std::string srcAddress = ms.getAttributes()["srcAddress"];
			//Verbose::show("When the client is just prepared to send the file " + ms.getFiletoUpload() + ", the dstAdress:" + dstAddr + ", the srcAddress:" + srcAddress, always);
			Verbose::show("The client is going to send the file: " + ms.getFiletoUpload(), always);
			sendFile(ms);
			//const Message cms = ms;
			//std::thread sendFileThread([=] { this->sendFile(cms); });
			//HANDLE hT5 = sendFileThread.native_handle();
			//WaitForSingleObject(hT5, INFINITE);
			//sendFileThread.detach();  // detach - deQRequest() won't access thread again
		}
		else if (ms.getCommand() == "QUIT"){  // used by the client to send quit quit request
			sendQuitMsg(ms);
			//set the flag for sender thread ended state
			isSenderThreadEnded = true;
			Verbose::show("Sender is going to stop. \n", always);
			break;
		}
		else if (ms.getCommand() == "FILE_UPLOAD_REPLY"){  // used by the server to send file upload request reply
			Verbose::show("The server is going to send FILE_UPLOAD_REPLY", always);
			sendFileUploadReply(ms);
		}
		else if (ms.getCommand() == "QUIT_REPLY"){  // used by the server to send quit request reply
			Verbose::show("The server is going to send QUIT_REPLY", always);
			sendQuitReply(ms);
		}
	} while (true);
};

//----------< check if the sender thread state is valid >----------------------------
bool Sender::sendThreadStateValid(){
	return isSenderThreadEnded == false;
}

//----------< get the reference to the socket connector >-----------------------------
SocketConnecter& Sender::getSocketConnecter(){ return socketconn; };

//----------< destructor >------------------------------------------------------------
Sender::~Sender(){
	socketconn.shutDown();
	socketconn.close();
	delete pQ;
}

//----------< a private function to send the header for each file block >-------------
bool Sender::sendFileblockHeader(Message& ms){
	bool ok = true, eachtime;
	std::string dstAddress = ms.getAttributes()["dstAddress"];
	ok = ok && (eachtime=socketconn.sendString(ms.getCommand()));
	if(eachtime==0) Verbose::show("WSAgetlastmsg: " + GetLastMsg(false), always);
	ok = ok && (eachtime = socketconn.sendString(ms.getFiletoUpload()));
	if (eachtime == 0) Verbose::show("WSAgetlastmsg: " + GetLastMsg(false), always);
	ok = ok && (eachtime = socketconn.sendString(toString(ms.getFileUploadBlockLength())));
	if (eachtime == 0) Verbose::show("WSAgetlastmsg: " + GetLastMsg(false), always);
	for (auto attrib : ms.getAttributes()){
		ok = ok && (eachtime = socketconn.sendString(attrib.first + ':' + attrib.second));
		if (eachtime == 0) Verbose::show("WSAgetlastmsg: " + GetLastMsg(false), always);
	}
	ok = ok && (eachtime = socketconn.sendString("\n"));
	if (eachtime == 0) Verbose::show("WSAgetlastmsg: " + GetLastMsg(false), always);
	if (ok) return true;
	else{
		Verbose::show("There is error in sending the file block header for " + ms.getFiletoUpload() + " \n");
		return false;
	}
};

#ifdef TEST_SENDER
int main()
{
	try
	{
		Verbose v(true);
		SocketSystem ss;
		Sender s;
		Message ms;
		ms.configureFileUploadMsg("client1toServer1First.txt", "::1@9080", "::1@8090");
		s.enQRequest(ms);
		ms.configureFileUploadMsg("pr1s15.pdf", "::1@9080", "::1@8090");
		s.enQRequest(ms);
		/*ms.configureFileUploadMsg("C+++How+to+Program+Fifth.chm", "::1@9080", "::1@8090");
		s.enQRequest(ms);*/
		ms.configureFileUploadMsg("kola.jpg", "::1@9080", "::1@8090");
		s.enQRequest(ms);
		ms.configureFileUploadMsg("science.txt", "::1@9080", "::1@8090");
		s.enQRequest(ms);
		ms.configureFileUploadMsg("TestFileCatalogue.exe", "::1@9080", "::1@8090");
		s.enQRequest(ms);
		ms.configureFileUploadMsg("MT4S03sol.doc", "::1@9080", "::1@8090");
		s.enQRequest(ms);	
		ms.configureQuitMsg("::1@9080", "::1@8090");
		s.enQRequest(ms);
		while (s.sendThreadStateValid());
	}
	catch (std::exception& ex)
	{
		Verbose::show("  Exception caught:", always);
		Verbose::show(std::string("\n  ") + ex.what() + "\n\n");
	}
	std::cout << "\n  press key to exit: ";
}
#endif