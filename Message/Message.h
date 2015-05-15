#ifndef MESSAGE_H
#define MESSAGE_H

/////////////////////////////////////////////////////////////////////
// Message.h - Support message construction and configuration      //
// ver 1.0                                                         //
//                                                                 //
// Application: CSE687 2015 Project 3 - Message Passing            //
// Platform:    Win7, Visual Studio 2013                           //
// Author:      Wenting Wang, wwang34@syr.edu                      //                                            
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* ===================
* This package contains one class, Message.
*
* The Message class support buiding different categories of messages. 
* These messages have certain fields as well as a few pairs of 
* attribute name and value. It also contains methods to build some
* static categories of messages that some fileds at set with default
* values. 
*
* Public Interface:
* =================
* Message ms;
* ms.configureFileUploadMsg("test.txt", "::1@9080", "::1@8090");
* ms.configureQuitMsg("::1@9080", "::1@6070");
* ms.setCommand("Quit")
* ms.setFiletoUpload("test.txt")
*
* Maintenance History:
* ====================
* Version 1.0 : 10 Apr 2015
* - first release
*/

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "../ApplicationHelpers/AppHelpers.h"

using namespace ApplicationHelpers;

class Message{
public:
	// set the command
	bool setCommand(const std::string& command);
	// get the command
	std::string getCommand();
	// get the name of the file to be uploaded
	std::string getFiletoUpload();
	// set the name of the file to be uploaded
	bool setFiletoUpload(const std::string& filename);
	// get the block length when send the file block by block
	size_t getFileUploadBlockLength();
	// build a message for file upload used by client
	void configureFileUploadMsg(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, const std::string& mode = "oneway", size_t contentLen = 1024);
	// build a message for quitting used by client
	void configureQuitMsg(const std::string srcAddress, const std::string dstAddress);
	// build a message for file upload reply used by server
	void configureFileUploadReply(const std::string& filename, const std::string& srcAddress, const std::string& dstAddress, const std::string& servernumber, std::string path, const std::string& result, const std::string& mode = "oneway");
	// get the block length when send the file block by block
	void setFileUploadBlockLength(size_t contentLen);
	// add attribute to the message
	bool addAttrib(const std::string& name, const std::string& value);
	// get all the attributes of the message
	std::map<std::string, std::string> getAttributes();
	// clear the files of the message that have been set previously
	void clear();
private:
	std::string command_;
	std::string filename_;
	size_t fileUploadBlockLen_;
	//used a vectored at first, need to discuss pros and cos yet
	/*std::vector<std::pair<std::string, std::string>> attribs_;*/ 
	std::map<std::string, std::string> attribs_;
};











#endif