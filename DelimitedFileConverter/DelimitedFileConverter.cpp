#include <iostream>
#include <string>
#include <fstream>

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

/**********************************************************************************************
###############################################################################################
##### ARRAY CLASS
###############################################################################################
**********************************************************************************************/
template <class T> class Array {
private:

	/* Array Members */
	unsigned int arraySize = 0;
	unsigned int maxSize = 0;
	T *arr = nullptr;
	unsigned int growthFactor = 100;

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void resize() {
		maxSize += growthFactor;
		T *newArr = new T[maxSize];
		for (unsigned int i = 0; i < arraySize; i++) {
			newArr[i] = arr[i];
		}
		delete[] arr;
		arr = newArr;
		newArr = nullptr;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void clear() {
		if (arr != nullptr) {
			delete[] arr;
		}
		maxSize = 0;
		arraySize = 0; 
		arr = nullptr;
	}

public:

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	Array() : arraySize(0), maxSize(0), arr(nullptr), growthFactor(1) { }

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	~Array() { clear(); }

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void operator=(const Array& copy) {
		clear();
		arraySize = copy.arraySize;
		maxSize = copy.maxSize;
		arr = new T[maxSize];
		growthFactor = 100;
		for (unsigned int i = 0; i < arraySize; i++) {
			arr[i] = copy.arr[i];
		}
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	Array(const Array& copy) {
		(*this) = copy;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void Add(T item) {
		if (arraySize == maxSize) {
			resize();
		}
		arr[arraySize] = item;
		arraySize += 1;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	T& operator[](unsigned int i) {
		return arr[i];
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	unsigned int GetSize() const { return arraySize; }

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void reset() { arraySize = 0; }

};

/**********************************************************************************************
###############################################################################################
##### String Manipulation Class
###############################################################################################
**********************************************************************************************/
class StringUtility {
public: 
	
	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static string SubString(const string& str, const unsigned int& start, const unsigned int& len) {
		string s = "";
		unsigned int nLen = ((start + len) >= str.size()) ? str.size() - start : len;

		if (start >= str.size()) { return s; }

		for (unsigned int i = 0; i < nLen; i++) {
			s += str[start + i];
		}

		return s;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static unsigned int SubStringStartIndex(const string& str, const string& subStr) {
		unsigned int i = 0;
		while (i < str.size()) {
			if (SubString(str, i, subStr.size()) == subStr) {
				return i;
			}
			i += 1;
		}
		return str.size();
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static bool StringContains(const string& str, const string& subStr) {
		unsigned int i = 0;
		while (i < str.size()) {
			if (SubString(str, i, subStr.size()) == subStr) {
				return true;
			}
			i += 1;
		}
		return false;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static string StringReplace(const string& str, const string& subStr, const string& replaceStr) {
		string s = str;
		while (true) {
			unsigned int subStringLocation = SubStringStartIndex(s, subStr);
			if (subStringLocation < s.size()) {
				s = SubString(s, 0, subStringLocation) + replaceStr + SubString(s, subStringLocation + subStr.size(), s.size() - subStringLocation);
			}
			else {
				return s;
			}
		}
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static unsigned int CountSubStringInString(const string& str, const string& subStr, const string& qualifier, bool inQualifiedField) {
		bool justActivatedQualifiedField = false;
		unsigned int i = 0;
		unsigned int shifter = 1;
		unsigned int frequency = 0;
		string s1, s2;
		while (i < str.size()) {
			justActivatedQualifiedField = false;
			shifter = 1;
			/* Handle Qualifer stuff */
			if (qualifier != "") {
				s1 = SubString(str, i, qualifier.size());
				s2 = SubString(str, i + qualifier.size(), qualifier.size());
				/* ....."data"... */
				if ((s1 == qualifier) && !inQualifiedField) {
					inQualifiedField = true;
					justActivatedQualifiedField = true;
				}
				/* ....." Hello ""Good Sir""..."...*/
				else if ((s1 == qualifier) && (s2 == qualifier) && inQualifiedField) {
					/* Move on */
					shifter = (qualifier.size() * 2);
					s1 = "";
				}
				/* ....."data"... */
				else if ((s1 == qualifier) && !(s2 == qualifier) && inQualifiedField) {
					/* Move on */
					inQualifiedField = false;
					justActivatedQualifiedField = true;
				}
			}
			s1 = SubString(str, i, subStr.size());

			/* Check for sub string */
			if (s1 != "" && !inQualifiedField && !justActivatedQualifiedField) {
				if (s1 == subStr) {
					frequency += 1;
				}
			}
			i += shifter;
		}
		return frequency;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static unsigned int CountSubStringInString(const string& str, const string& subStr, const string& qualifier) {
		return CountSubStringInString(str, subStr, qualifier, false);
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static Array<string> SplitString(const string& str, const string& delimiter, const string& qualifier) {
		Array<string> strings;
		if (delimiter == qualifier) {
			PrintMessage("StringUtility::SplitString", ERROR, "Delimiter and qualifer are the same string. Data impossible to interpet.");
			return strings;
		}

		bool inQualifiedField = false;
		bool justActivatedQualifiedField = false;
		bool foundQualifier = false;
		string currentString = "";
		string s1, s2;
		unsigned int i = 0;
		unsigned int shifter = 1;
		unsigned int currentStart = 0;
		while (i < str.size()) {
			foundQualifier = false;
			shifter = 1;
			/* Handle Qualifer stuff */
			if (qualifier != "") {
				s1 = SubString(str, i, qualifier.size());
				s2 = SubString(str, i + qualifier.size(), qualifier.size());
				/* ....."data"... */
				if ((s1 == qualifier) && !inQualifiedField) {
					inQualifiedField = true;
					shifter = (qualifier.size());
					foundQualifier = true;
				}
				/* ....." Hello ""Good Sir""..."...*/
				else if ((s1 == qualifier) && (s2 == qualifier) && inQualifiedField) {
					/* Move on */
					shifter = (qualifier.size() * 2);
					currentString += s1;
					foundQualifier = true;
				}
				/* ....."data"... */
				else if ((s1 == qualifier) && !(s2 == qualifier) && inQualifiedField) {
					/* Move on */
					inQualifiedField = false;
					shifter = (qualifier.size());
					foundQualifier = true;
				}

				if (foundQualifier) {
					i += shifter;
					continue;
				}
			}

			s1 = SubString(str, i, 1);

			currentString += s1;

			if (currentString.size() >= delimiter.size()) {
				if ((StringUtility::SubString(currentString, currentString.size() - delimiter.size(), delimiter.size()) == delimiter) && (!inQualifiedField)) {
					currentString = StringUtility::SubString(currentString, 0, currentString.size() - delimiter.size());
					strings.Add(currentString);
					currentString = "";
				}
			}

			i += shifter;
		}
		strings.Add(currentString);
		return strings;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static bool StringEndsWithinQualifiedField(const string& str, const string& qualifier) {
		return (CountSubStringInString(str, qualifier, "") % 2) == 1;
	}

	static bool IsWhiteSpace(char c) {
		return (c == ' ') || (c == '\r') || (c == '\n') || (c == '\t');
	}

	static string TrimLeftWhiteSpace(const string& s) {
		unsigned int startIndex = 0;
		for (unsigned int i = 0; i < s.size(); i++) {
			if (!IsWhiteSpace(s[i])) {
				startIndex = i;
				break;
			}
		}
		return SubString(s, startIndex, s.size() - startIndex);
	}

	static string TrimRightWhiteSpace(const string& s) {
		if (s.size() == 0) {
			return s;
		}

		unsigned int newLength = s.size();
		for (unsigned int i = s.size(); i > 0; i--) {
			if (!IsWhiteSpace(s[i - 1])) {
				newLength = i;
				break;
			}
		}
		return SubString(s, 0, newLength);
	}

	static string TrimWhiteSpace(const string& s) {
		return TrimLeftWhiteSpace(TrimRightWhiteSpace(s));
	}

	static bool IsUpperCaseLetter(char c) { return (c >= 65 && c <= 90); }
	static bool IsLowerCaseLetter(char c) { return (c >= 97 && c <= 122); }
	static bool IsLetter(char c) { return IsUpperCaseLetter(c) || IsLowerCaseLetter(c); }
	static bool IsDigit(char c) { return (c >= 48 && c <= 57); }
	static bool IsAcceptableColumnCharacter(char c) { return (IsLetter(c) || IsDigit(c)); }
	static char ToUpperCaseLetter(char c) { return IsLowerCaseLetter(c) ? (c - 32) : c; }
	static char ToLowerCaseLetter(char c) { return IsUpperCaseLetter(c) ? (c + 32) : c; }

	static string ToLowerCaseString(const string& s) {
		string newS = "";
		for (unsigned int i = 0; i < s.size(); i++) {
			newS += ToLowerCaseLetter(s[i]);
		}
		return newS;
	}
};

/**********************************************************************************************
###############################################################################################
##### Column CLASS
###############################################################################################
**********************************************************************************************/
enum DateFormat {
	YYYYMMDD
	, DDMMYYYY
	, MMDDYYYY
};

class Column {
public: // private:

	enum ColumnNameFormat {
		PROPER_CASE
		// , CAMEL_CASE
		// , UPPER_CASE
		// , LOWER_CASE
	};

private:
	string FormatColumnName(const string& oldColumnName, ColumnNameFormat columnNameFormat) {
		string newColumnName = "";
		bool setToUpperCase = false;

		if (columnNameFormat == PROPER_CASE) {
			setToUpperCase = true;
			for (unsigned int i = 0; i < oldColumnName.size(); i++) {
				char c = oldColumnName[i];
				if (StringUtility::IsAcceptableColumnCharacter(c)) {
					if (StringUtility::IsLetter(c) && setToUpperCase) {
						newColumnName += StringUtility::ToUpperCaseLetter(c);
						setToUpperCase = false;
					}
					else {
						newColumnName += StringUtility::ToLowerCaseLetter(c);
					}
				}
				else {
					setToUpperCase = true;
				}
			}
		}
		else {
			return oldColumnName;
		}

		return newColumnName;
	}



	static const string defaultColumnName;

	enum ColumnType {
		STRING
		, DECIMAL	// X
		, INTEGER	// X
		, CHARACTER // X
		, TEXT
		, DATE		// X
		, DATETIME  // X
		, BIT		// X
		, BOOLEAN	// X
		, UNDETERMINED 
	};


	/* Main column descriptors */
	string columnName;
	ColumnType columnType = UNDETERMINED;
	unsigned int columnLength = 0;
	unsigned int precision = 0;
	bool nullable = false;

	/* Members used to identify exact type */
	unsigned int instancesCounted = 0;			// Use this to track how many rows there are
	unsigned int nullsCounted = 0;				// Count how many instances were NULL or ""
	unsigned int columnLengthFrequency = 0;		// This is used to determine if string fields are constant lengths

	DateFormat dateFormat;
	string dateDelimiter;

	bool ChangeColumnType(const ColumnType& newColumnType) {
		if (newColumnType == columnType) {
			return true;
		}
		switch (columnType) {
		case(UNDETERMINED): { return true; }
		case(TEXT): { return false; }
		case(STRING): { 
			if (newColumnType == TEXT) {
				return true;
			}
			return false;
		}
		case(DECIMAL): case(CHARACTER): case(DATETIME): case(BOOLEAN): {
			if (newColumnType == STRING || newColumnType == TEXT) {
				return true;
			}
			return false;
		}
		case(INTEGER): {
			if (newColumnType == STRING || newColumnType == TEXT || newColumnType == DECIMAL) {
				return true;
			}
			return false;
		}	
		case(DATE): {
			if (newColumnType == STRING || newColumnType == TEXT || newColumnType == DATETIME) {
				return true;
			}
			return false;
		}
		case(BIT): {
			if (newColumnType == STRING || newColumnType == TEXT || newColumnType == INTEGER || newColumnType == DECIMAL) {
				return true;
			}
			return false;
		}
		}
		return false;
	}

	/* Check for NULL Value */
	static bool IsBlank(const string &s) {
		string t = StringUtility::ToLowerCaseString(s);
		return ((t == "null") || (t == ""));
	}

	/* Check for bit value */
	static bool IsBit(const string &s) { return ((s == "0") || (s == "1"));}

	/* Check for char */
	static bool IsCharacter(const string &s) { return (s.size() == 1); }

	/* Check for Boolean value */
	static bool IsBoolean(const string &s) {
		string t = StringUtility::ToLowerCaseString(s);
		return ((t == "true") || (t == "false"));
	}

	/* Check for int value */
	static bool IsInt(const string &s) {
		if (s.size() == 0) { return false; }
		unsigned int i = (s[0] == '-') ? 1 : 0;
		for (i; i < s.size(); i++) {
			if (!StringUtility::IsDigit(s[i])) {
				return false;
			}
		}
		return true;
	}

	/* Is Decimal */
	static bool IsDecimal(const string &s) {
		if (s.size() == 0) { return false; }
		bool foundDecimal = false;
		unsigned int i = (s[0] == '-') ? 1 : 0;
		for (i; i < s.size(); i++) {
			if (!StringUtility::IsDigit(s[i])) {
				if (s[i] == '.' && !foundDecimal) {
					foundDecimal = true;
				}
				else {
					return false;
				}
			}
		}
		return true;
	}



	/* Is Date */
	static bool IsDate(int d, int m, int y) {
		if (y < 0) { return false; }
		if (m < 1 || m > 12) { return false; }
		if (d < 1 || d > 31) { return false; }

		switch (m) {
		case(1): case(3): case(5): case(7): case(8): case(10): case(12): {
			return ((d >= 1) && (d <= 31));
			break;
		}
		case(2): {
			return ((d >= 1) && (d <= 28)) || ((d == 29) && ((y % 4) == 0));
			break;
		}

		default: {
			return ((d >= 1) && (d <= 30));
			break;
		}
		}
		return false;
	}
	
	static bool IsDate(const string &s, DateFormat dateFormat, const string& dateDelimiter) {
		Array<string> dateParts = StringUtility::SplitString(s, dateDelimiter, "");
		if (dateParts.GetSize() != 3) {
			return false;
		}
		for (unsigned int i = 0; i < dateParts.GetSize(); i++) {
			if (!IsInt(dateParts[i])) {
				return false;
			}
		}
		int y, m, d;
		if (dateFormat == YYYYMMDD) {
			y = stoi(dateParts[0]);
			m = stoi(dateParts[1]);
			d = stoi(dateParts[2]);
		}
		else if (dateFormat == MMDDYYYY) {
			m = stoi(dateParts[0]);
			d = stoi(dateParts[1]);
			y = stoi(dateParts[2]);
		}
		else if (dateFormat == DDMMYYYY) {
			d = stoi(dateParts[0]);
			m = stoi(dateParts[1]);
			y = stoi(dateParts[2]);
		}
		return IsDate(d,m,y);
	}

	/* Is Datetime */
	// This must look something like "<date> HH:mm:ss.ddd"
	static bool IsDateTime(const string &s, DateFormat dateFormat, const string& dateDelimiter) {
		Array<string> dateParts = StringUtility::SplitString(s, dateDelimiter, "");
		if (dateParts.GetSize() != 2) {
			return false;
		}

		if (!IsDate(dateParts[0], dateFormat, dateDelimiter)) {
			return false;
		}

		Array<string> timeParts = StringUtility::SplitString(dateParts[1], ":", "");
		if (timeParts.GetSize() != 3) {
			return false;
		}

		/* Hour */
		if (IsInt(timeParts[0])) {
			int hh = stoi(timeParts[0]);
			if (hh < 0 || hh > 24) {
				return false;
			}
		}
		else {
			return false;
		}

		/* Minute */
		if (IsInt(timeParts[1])) {
			int mm = stoi(timeParts[1]);
			if (mm < 0 || mm > 59) {
				return false;
			}
		}
		else {
			return false;
		}

		/* Second */
		if (IsInt(timeParts[2])) {
			int ss = stoi(timeParts[2]);
			if (ss < 0 || ss >= 60) {
				return false;
			}
		}
		else if(IsDecimal(timeParts[2])) {
			double ss = stod(timeParts[2]);
			if (ss < 0.0 || ss >= 60.0) {
				return false;
			}
		}
		else {
			return false;
		}

		return true;
	}

	/* Is Binary */
	static bool IsText(const string& s) {
		if (s.size() > 4000) {
			return true;
		}
		return false;
	}

	

public:

	Column() {}

	Column(unsigned int columnIndex, const DateFormat& dateFormat, const string& dateDelimiter) : columnName(defaultColumnName + to_string(columnIndex)), dateDelimiter(dateDelimiter), dateFormat(dateFormat) { }

	Column(const string& columnName, const DateFormat& dateFormat, const string& dateDelimiter, const bool& formatColumnName, ColumnNameFormat columnNameFormat) : dateDelimiter(dateDelimiter), dateFormat(dateFormat) {
		if (formatColumnName) {
			this->columnName = FormatColumnName(StringUtility::TrimWhiteSpace(columnName), columnNameFormat);
		}
		else {
			this->columnName = columnName;
		}
	}

	void DetermineDataType(const string& s) {
		instancesCounted += 1;

		if (IsBlank(s)) {
			nullable = true;
			nullsCounted += 1;
		}
		else {
			if (IsBit(s) && ChangeColumnType(BIT)) {
				columnType = BIT;
			}
			else if (IsInt(s) && ChangeColumnType(INTEGER)) {
				columnType = INTEGER;
			}
			else if (IsDecimal(s) && ChangeColumnType(DECIMAL)) {
				columnType = DECIMAL;
				unsigned int totalLen;
				unsigned int decimalLen;
				if (StringUtility::StringContains(s, ".")) {
					Array<string> decimalParts = StringUtility::SplitString(s, ".", "");
					totalLen = decimalParts[1].length() + decimalParts[0].length();
					decimalLen = decimalParts[1].length();
				}
				else {
					totalLen = s.size();
					decimalLen = 0;
				}

				if (totalLen > columnLength) {
					columnLength = totalLen;
				}
				if (decimalLen > precision) {
					precision = decimalLen;
				}
			}
			else if (IsBoolean(s) && ChangeColumnType(BOOLEAN)) {
				columnType = BOOLEAN;
			}
			else if (IsDate(s, dateFormat, dateDelimiter) && ChangeColumnType(DATE)) {
				columnType = DATE;
			}
			else if (IsDateTime(s, dateFormat, dateDelimiter) && ChangeColumnType(DATETIME)) {
				columnType = DATETIME;
			}
			else if (IsCharacter(s) && ChangeColumnType(CHARACTER)) {
				columnType = CHARACTER;
			}
			else if (IsText(s) && ChangeColumnType(TEXT)) {
				columnType = TEXT;
			}
			else {
				columnType = STRING;
				unsigned int totalLen = s.length();
				if (totalLen == columnLength) {
					columnLengthFrequency += 1;
				}
				else {
					columnLengthFrequency = 1;
					if (totalLen > columnLength) {
						columnLength = totalLen;
					}
				}
			}
		}
	}

	string ToString() const {
		bool constantLength = (instancesCounted == (nullsCounted + columnLengthFrequency));
		string s = columnName + " : [ ";
		switch (columnType) {
			case(STRING) : { 
				s += "STRING(" + to_string(columnLength);
				s += (constantLength) ? "c)" : ")";
				break; 
			}
			case(DECIMAL): { 
				s += "DECIMAL(" + to_string(columnLength) + "," + to_string(precision) + ")"; 
				break; 
			}
			case(INTEGER): { s += "INTEGER"; break; }
			case(CHARACTER): { s += "CHARACTER"; break; }
			case(TEXT): { s += "TEXT"; break; }
			case(DATE): { s += "DATE"; break; }
			case(DATETIME): { s += "DATETIME"; break; }
			case(BIT): { s += "BIT"; break; }
			case(BOOLEAN): { s += "BOOLEAN"; break; }
			case(UNDETERMINED): { s += "UNDETERMINED"; break; }
		}
		s += (nullable) ? " NULL" : " NOT NULL";
		s += " ]";
		return s;
	}

};

const string Column::defaultColumnName = "C";

/**********************************************************************************************
###############################################################################################
##### DELIMITED FILE CONVERTER CLASS
###############################################################################################
**********************************************************************************************/
class DelimitedFileConverter {
public:
	struct FileLine {
		string line;
		unsigned int lineNumber = 0;
		void PrintFileLine(ostream &out) const {
			string s = "Line Number: " + to_string(lineNumber) + ", Row: {" + line + "}\n";
			out << s;
		}
		void PrintFileLine() const { PrintFileLine(cout); }
	};

private:
	string delimiter = "";
	string fileName = "";
	string qualifier = "";
	string endline = "\n";
	bool headerIncluded = false;
	bool incompleteQualifierDetection = true;
	bool removeCarraigeReturns = true;
	bool trimLeadingAndTrailingWhiteSpace = true;
	bool keepOriginalColumnNames = false;
	ifstream inFile;
	unsigned int expectedColumns = 0;

	DateFormat dateFormat = MMDDYYYY;
	string dateDelimiter = "/";

	const unsigned int batchSize = 1000;

	Array<Column> columns;
	Array<FileLine> malformedRows;
	Array<Array<string>> data;

public:
	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void PrintOptions(ostream &out) const { 
		string s = "-------------------------------------\n| Delimited File Converter          |\n-------------------------------------\n";
		s += "| - File Name                          : " + fileName + "\n";
		s += "| - Delimiter                          : " + delimiter + "\n";
		s += "| - Qualifier                          : " + qualifier + "\n";
		s += "| - Headers Included                   : " + string((headerIncluded) ? "Yes" : "No") + "\n";
		s += "| - Number of Columns                  : " + string((expectedColumns == 0) ? "Auto-Detect" : to_string(expectedColumns)) + "\n";
		s += "| - Incomplete Qualifier Detection     : " + string((incompleteQualifierDetection) ? "On" : "Off") + "\n";
		s += "| - Remove Carraige Returns            : " + string((removeCarraigeReturns) ? "Yes" : "No") + "\n";
		s += "| - Trim Leading & Trailing WhiteSpace : " + string((removeCarraigeReturns) ? "Yes" : "No") + "\n";
		s += "| - Keep Original Column Names         : " + string((keepOriginalColumnNames) ? "Yes" : "No") + "\n";
		s += "| - Date Format                        : ";
		if (dateFormat == DDMMYYYY) {
			s += "dd" + dateDelimiter + "MM" + dateDelimiter + "yyyy";
		} 
		else if (dateFormat == MMDDYYYY) {
			s += "MM" + dateDelimiter + "dd" + dateDelimiter + "yyyy";
		}
		else if (dateFormat == YYYYMMDD) {
			s += "yyyy" + dateDelimiter + "MM" + dateDelimiter + "dd";
		}
		s += "\n";
		s += "-------------------------------------\n";
		out << s;
	}

	void PrintOptions() const {
		PrintOptions(cout);
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	DelimitedFileConverter(){
		//PrintOptions();
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void operator=(const DelimitedFileConverter& copy) { 
		this->delimiter = copy.delimiter;
		this->fileName = copy.fileName;
		this->qualifier = copy.qualifier;
		this->endline = copy.endline;
		this->headerIncluded = copy.headerIncluded;
		this->expectedColumns = copy.expectedColumns;
		this->malformedRows = copy.malformedRows;


		this->incompleteQualifierDetection = copy.incompleteQualifierDetection;
		this->removeCarraigeReturns = copy.removeCarraigeReturns;
		this->trimLeadingAndTrailingWhiteSpace = copy.trimLeadingAndTrailingWhiteSpace;
		this->keepOriginalColumnNames = copy.keepOriginalColumnNames;
		this->dateFormat = copy.dateFormat;
		this->dateDelimiter = copy.dateDelimiter;
		this->columns = copy.columns;
		this->data = copy.data;

		inFile.open(fileName);
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	DelimitedFileConverter(const DelimitedFileConverter& copy) { 
		(*this) = copy;
		//PrintOptions();
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	DelimitedFileConverter(string fileName, string delimiter, string qualifier, bool headerIncluded, bool winEndLines) : headerIncluded(headerIncluded), delimiter(delimiter), fileName(fileName), qualifier(qualifier) {
		if (winEndLines) {
			endline = "\r\n";
		}
		inFile.open(fileName);
		//PrintOptions();
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	DelimitedFileConverter(string fileName, string delimiter, string qualifier, unsigned int expectedColumns, bool headerIncluded, bool winEndLines) : headerIncluded(headerIncluded), delimiter(delimiter), fileName(fileName), qualifier(qualifier), expectedColumns(expectedColumns) {
		if (winEndLines) {
			endline = "\r\n";
		}
		inFile.open(fileName);
		//PrintOptions();
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	~DelimitedFileConverter(){
		if (inFile.is_open()) {
			inFile.close();
		}
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	bool GetFileLine(string &s) {
		if (!getline(inFile, s)) {
			return false;
		}

		if (removeCarraigeReturns && s.size() > 0) {
			if (s[s.size() - 1] == '\r') {
				s = StringUtility::SubString(s, 0, s.size() - 1);
			}
		}

		return true;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void CompleteRowWithEndlines(FileLine& fl, unsigned int delimiterCount, unsigned int& totalLineCount) {
		Array<FileLine> strings;	// Will be used to add to malformed strings if completed data row turns out malformed.
		string s = "";			// Temp string to read into and do work
		string sLast;
		FileLine f;
		bool incompleteQualifierDetected = false;
		strings.Add(fl);  // Place current string into temp array
		unsigned int nextRowDelimiterCount = 0;
		bool foundAdditionalLines = false;
		/* While the delimiter count is less than what we expect */
		while (delimiterCount < (expectedColumns - 1)) {
			sLast = s;
			/* Get a line. If failed, close file and exit loop */
			if (!GetFileLine(s)) {
				inFile.close();
				s = sLast;
				break;
			}
			else {
				foundAdditionalLines = true;
			}

			totalLineCount += 1;

			f.line = s;
			f.lineNumber = totalLineCount + ((headerIncluded) ? 1 : 0);

			if (incompleteQualifierDetection) {
				nextRowDelimiterCount = StringUtility::CountSubStringInString(s, delimiter, qualifier, false);
				if (nextRowDelimiterCount == (expectedColumns - 1)) {
					unsigned int lc = totalLineCount + ((headerIncluded) ? 0 : -1);
					incompleteQualifierDetected = true;
					PrintMessage("DelimitedFileConverter::CompleteRowWithEndlines", WARNING, "Row with incomplete qualifier detected around line " + to_string(lc) + ". Incomplete Qualifier Detection is currently on. If this is a mistake, rerun the program with this option set to off.");
					break;
				}
			}

			strings.Add(f);	// Place current string into temp array
			delimiterCount += StringUtility::CountSubStringInString(s, delimiter, qualifier, true);	// With new line, count delims and add to current total
			/* If delimiter count is still less than, or equal to the desired ammount */
			if (delimiterCount <= (expectedColumns - 1)) {
				fl.line += "\n" + s;	// Tack it onto the current string and continue.
			}
		}
		/* We broke out of loop above. If delimiter count is what we expect, good to go! */
		if (delimiterCount == (expectedColumns - 1) && !incompleteQualifierDetected) {
			return;
		}
		else {
			if (foundAdditionalLines) {
				fl = f;	// Set string equal to last string read. 
				if (strings.GetSize() > 0) {
					/* All strings in the array are guarenteed to be malformed, except the last one read */
					int os = (incompleteQualifierDetected) ? 0 : 1;
					for (unsigned int i = 0; i < (strings.GetSize() - os); i++) {
						malformedRows.Add(strings[i]);	// Add row to malformed 
					}
				}
			}
		}
		/*
		At this point, the str contains the next row to be processed. 
		All malformed strings have been added to the log
		*/
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void InterpretRows(Array<FileLine>& rows) { // Intentions to possible thread this..
		for (unsigned int i = 0; i < rows.GetSize(); i++) {
			//rows[i].PrintFileLine();
			data.Add(StringUtility::SplitString(rows[i].line, delimiter, qualifier));
		}

		for (unsigned int i = 0; i < data.GetSize(); i += 1) {
			for (unsigned int c = 0; c < expectedColumns; c += 1) {
				columns[c].DetermineDataType(data[i][c]);
			}
		}
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void ProcessFile() {
		if (inFile.is_open()) {
			Array<FileLine> batch;
			string s1;
			FileLine fl;
			unsigned int lineCount = (headerIncluded) ? 0 : 1;
			unsigned int totalLineCount = lineCount;

			// NOTE: If the file has no headers AND expected columns are not provided AND the first data row is malformed, this program probably will not work. 
			if (!headerIncluded && expectedColumns == 0) {
				PrintMessage("DelimitedFileConverter::ProcessFile", WARNING, "Data file has no headers and expected columns is not provided. If the first row is malformed this program will NOT work.");
			}

			if (!GetFileLine(s1)) {
				inFile.close();	// If failed, close file
			}

			/* Get expected columns if needed */
			if (expectedColumns == 0) {
				expectedColumns = StringUtility::CountSubStringInString(s1, delimiter, qualifier) + 1;	// Grab the number of expected columns
			}

			/* If headers are not included */
			if (!headerIncluded) {
				fl.line = s1;
				fl.lineNumber = totalLineCount + ((headerIncluded) ? 1 : 0);
				batch.Add(fl);	// This row contains data. Add this to the batch to be processed below
				for (unsigned int i = 0; i < expectedColumns; i++) {
					columns.Add(Column(i, dateFormat, dateDelimiter));
				}
			}
			else {
				Array<string> columnNames = StringUtility::SplitString(s1, delimiter, qualifier);
				for (unsigned int i = 0; i < expectedColumns; i++) {
					columns.Add(Column(columnNames[i], dateFormat, dateDelimiter, true, Column::PROPER_CASE));
				}
			}
			
			/* Verify some valid amount of columns was determined */
			if (expectedColumns == 0) {
				PrintMessage("DelimitedFileConverter::ProcessFile", ERROR, "Expected columns is 0. Will not process further.");
				return;
			}

			/* Verify that batch either has data AND/OR file is still open*/
			if (batch.GetSize() == 0 && !inFile.is_open()) {
				PrintMessage("DelimitedFileConverter::ProcessFile", ERROR, "Could not read from file. Will not process further.");
				return;
			}

			/* We have something to process */
			unsigned int delimiterCount = 0;
			while (true) {
				/* Load Batch */
				while (lineCount < batchSize && inFile.is_open()) {
					if (!GetFileLine(s1)) {
						inFile.close();
						break;
					}

					lineCount += 1;
					totalLineCount += 1;

					fl.line = s1;
					fl.lineNumber = totalLineCount + ((headerIncluded) ? 1 : 0);

					/* Check the number of delimiters on the line */
					delimiterCount = StringUtility::CountSubStringInString(s1, delimiter, qualifier);
					/* If less, than there is perhaps an endline within a qualifed row */
					if (delimiterCount < (expectedColumns - 1)) {
						/* Row has an unfinished qualifier */
						if (StringUtility::StringEndsWithinQualifiedField(s1, qualifier)) {
							//PrintMessage("DelimitedFileConverter::ProcessFile", WARNING, "Found field contains qualified endlines.");
							CompleteRowWithEndlines(fl, delimiterCount, totalLineCount); // Call function to deal with this
							delimiterCount = StringUtility::CountSubStringInString(fl.line, delimiter, qualifier);	// With resulting string check again
						}
					}
					/* If our delimiter count is greater, add to malformed. */
					if (delimiterCount != (expectedColumns - 1)) {
						//PrintMessage("DelimitedFileConverter::ProcessFile", WARNING, "Malformed Row. ");
						malformedRows.Add(fl);
						continue;
					}

					batch.Add(fl);
				}

				/* Interpret Rows */
				if (batch.GetSize() > 0) {
					InterpretRows(batch);	// Intentions to possibly thread this.
				}

				/* Check for continuation */
				if (!inFile.is_open()) {
					PrintMessage("DelimitedFileConverter::ProcessFile", MESSAGE, "Reached end of file.");
					break;
				}

				/* Reset for next batch */
				lineCount = 0;
				batch.reset();
				cout << totalLineCount << endl;
			}

			for (unsigned int c = 0; c < columns.GetSize(); c += 1) {
				cout << columns[c].ToString() << endl;
			}
		}
		else {
			PrintMessage("DelimitedFileConverter::ProcessFile", ERROR, "File was unable to be opened. Cannot process file.");
		}
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	Array<FileLine> GetMalformedRows() { return malformedRows; }
};

/**********************************************************************************************
###############################################################################################
#####
###############################################################################################
**********************************************************************************************/
int main()
{
	string fileName;
	DelimitedFileConverter dfc;
	Array<DelimitedFileConverter::FileLine> malformedRows;

	fileName = "";
	dfc = DelimitedFileConverter(fileName, ",", "", true, true);
	dfc.PrintOptions();
	dfc.ProcessFile();
	malformedRows = dfc.GetMalformedRows();
	cout << "[MALFORMED ROWS] : ######################################" << endl;
	for (unsigned int i = 0; i < malformedRows.GetSize(); i++) {
		malformedRows[i].PrintFileLine();
	}
	cout << "#########################################################" << endl << endl;


	return 0;
}
