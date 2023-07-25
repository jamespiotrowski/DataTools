#pragma once

#ifndef H_IOSTRM
#include <iostream>
#endif // !H_IOSTRM

#ifndef H_STRING
#include <string>
#endif // !H_STRING

using namespace std;

/**********************************************************************************************
###############################################################################################
##### Messaging
###############################################################################################
**********************************************************************************************/
enum MessageType {
	WARNING
	, ERROR
	, MESSAGE
};

string ToString(MessageType mt) {
	switch (mt) {
	case(ERROR): {
		return "ERROR";
		break;
	}
	case(WARNING): {
		return "WARNING";
		break;
	}
	case(MESSAGE): {
		return "MESSAGE";
		break;
	}
	default: {
		return "";
		break;
	}
	}
}

void PrintMessage(string function, MessageType mt, string message, ostream& out) {
	string m = "[" + ToString(mt) + " from {" + function + "} ]: " + message + "\n";
	out << m;
}


void PrintMessage(string function, MessageType mt, string message) {
	PrintMessage(function, mt, message, cout);
}
