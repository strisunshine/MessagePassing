/////////////////////////////////////////////////////////////////////
// Message.cpp - Support message construction and configuration    //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 3 - Message Passing            //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////

#include "Message.h"

//set the command
bool Message::setCommand(const std::string& command){
	command_ = command;
	return true;
};

// get the command
std::string Message::getCommand(){
	return command_;
};

// get the name of the file to be uploaded
std::string Message::getFiletoUpload(){
	return filename_;
};

// set the name of the file to be uploaded
bool Message::setFiletoUpload(const std::string& filename){
	filename_ = filename;
	return true;
};

// get the block length when send the file block by block
size_t Message::getFileUploadBlockLength(){
	return fileUploadBlockLen_;
};

// build a message for file upload used by the client
void Message::configureFileUploadMsg(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, const std::string& mode, size_t contentLen){
	clear();
	command_ = "FILE_UPLOAD";
	filename_ = filename;
	fileUploadBlockLen_ = contentLen;
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
	attribs_.insert({ "mode", mode });
}

// build a message for quitting used by the client
void Message::configureQuitMsg(const std::string srcAddress, const std::string dstAddress){
	clear();
	command_ = "QUIT";
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
}

// build a message for file upload reply used by the server
void Message::configureFileUploadReply(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, const std::string& servernumber, std::string path, const std::string& result, const std::string& mode){
	clear();
	setFiletoUpload(filename);
	setCommand("FILE_UPLOAD_REPLY");
	attribs_.insert({ "srcAddress", srcAddress });
	attribs_.insert({ "dstAddress", dstAddress });
	attribs_.insert({ "mode", mode });
	attribs_.insert({ "result", result });
	attribs_.insert({ "servernumber", servernumber });
	attribs_.insert({ "path", path});
};

// get the block length when send the file block by block
void Message::setFileUploadBlockLength(size_t contentLen){
	fileUploadBlockLen_ = contentLen;
}
// add attribute to the message
bool Message::addAttrib(const std::string& name, const std::string& value){
	std::pair<std::string, std::string> attrib = { name, value };
	attribs_.insert(attrib);
	return true;
};
// get all the attributes of the message
std::map<std::string, std::string> Message::getAttributes(){
	return attribs_;
};
// clear the files of the message that have been set previously
void Message::clear(){
	command_ = "";
	filename_ = "";
	fileUploadBlockLen_ = 0;
	attribs_.clear();
};


#ifdef TEST_MESSAGE

int main()
{
	Message ms;
	ms.configureFileUploadMsg("SameFile.txt", "::1@9080", "::1@8090");
	ms.configureQuitMsg("::1@9080", "::1@8090");
	ms.configureFileUploadReply("SameFile.txt", "::1@9080", "::1@8090", "1", "../Server", "success");
	return 0;
}


#endif