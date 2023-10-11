#pragma once

#ifndef H_IOSTRM
#define H_IOSTRM
#include <iostream>
#endif // !H_IOSTRM

#ifndef H_STRING
#define H_STRING
#include <string>
#endif // !H_STRING

using namespace std;
/**********************************************************************************************
###############################################################################################
##### Messaging
###############################################################################################
**********************************************************************************************/
enum class MessageType {
	WARNING
	, ERROR
	, MESSAGE
};

wstring ToString(MessageType mt) {
	switch (mt) {
	case(MessageType::ERROR): {
		return L"ERROR";
		break;
	}
	case(MessageType::WARNING): {
		return L"WARNING";
		break;
	}
	case(MessageType::MESSAGE): {
		return L"MESSAGE";
		break;
	}
	default: {
		return L"";
		break;
	}
	}
}

void PrintMessage(wstring function, MessageType mt, wstring message, wostream& out) {
	wstring m = L"[" + ToString(mt) + L" from {" + function + L"} ]: " + message + L"\n";
	out << m;
}


void PrintMessage(wstring function, MessageType mt, wstring message) {
	PrintMessage(function, mt, message, wcout);
}
