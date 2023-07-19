#include <iostream>
#include <string>
#include <fstream>

using namespace std;

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
##### DELIMITED FILE CONVERTER CLASS
###############################################################################################
**********************************************************************************************/
class DelimitedFileConverter {
private:
	string delimiter = "";
	string fileName = "";
	string qualifier = "";
	string endline = "\n";
	bool headerIncluded = false;
	ifstream inFile;
	unsigned int expectedColumns = 0;
	Array<string> malformedRows;

	const unsigned int batchSize = 10;

public:

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	DelimitedFileConverter(){ }

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
		inFile.open(fileName);
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	DelimitedFileConverter(const DelimitedFileConverter& copy) { 
		(*this) = copy;
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
	static unsigned int CountSubStringInString(const string& str, const string& subStr, const string& qualifier) {
		bool inQualifiedField = false;
		bool justActivatedQualifiedField = false;
		unsigned int i = 0;
		unsigned int shifter = 1;
		unsigned int frequency = 0;
		string s1, s2;
		while (i < str.size()) {
			justActivatedQualifiedField = false;
			shifter = 1;
			s1 = SubString(str, i, qualifier.size());
			s2 = SubString(str, i + qualifier.size(), qualifier.size());
			/* Handle Qualifer stuff */
			if (qualifier != "") {
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
	static unsigned int CountSubStringInString(const string& str, const string& subStr, const string& qualifier, bool inQualifiedField) {
		bool justActivatedQualifiedField = false;
		unsigned int i = 0;
		unsigned int shifter = 1;
		unsigned int frequency = 0;
		string s1, s2;
		while (i < str.size()) {
			justActivatedQualifiedField = false;
			shifter = 1;
			s1 = SubString(str, i, qualifier.size());
			s2 = SubString(str, i + qualifier.size(), qualifier.size());
			/* Handle Qualifer stuff */
			if (qualifier != "") {
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
	static Array<string> SplitString(const string& str, const string& delimiter, const string& qualifier) {
		Array<string> strings;
		if (delimiter == qualifier) {
			cout << "Having the delimiter and qualifer be the same string makes your data impossible to interpet. Good luck with that." << endl;
			return strings;
		}

		bool inQualifiedField = false;
		bool justActivatedQualifiedField = false;
		string currentString = "";
		string s1, s2;
		unsigned int i = 0;
		unsigned int shifter = 1;
		unsigned int currentStart = 0;
		while (i < str.size()) {
			justActivatedQualifiedField = false;
			shifter = 1;
			s1 = SubString(str, i, qualifier.size());
			s2 = SubString(str, i + qualifier.size(), qualifier.size());
			/* Handle Qualifer stuff */
			if (qualifier != "") {
				/* ....."data"... */
				if ((s1 == qualifier) && !inQualifiedField) {
					inQualifiedField = true;
					justActivatedQualifiedField = true;
				}
				/* ....." Hello ""Good Sir""..."...*/
				else if ((s1 == qualifier) && (s2 == qualifier) && inQualifiedField) {
					/* Move on */
					shifter = (qualifier.size() * 2);
				}
				/* ....."data"... */
				else if ((s1 == qualifier) && !(s2 == qualifier) && inQualifiedField) {
					/* Move on */
					inQualifiedField = false;
					justActivatedQualifiedField = true;
				}
			}

			/* If adding to current field */
			if (((s1 != delimiter) || (inQualifiedField)) && !justActivatedQualifiedField) {
				currentString += s1;
			} 
			else if ((s1 == delimiter) && (!inQualifiedField)) {
				strings.Add(currentString);
				currentString = "";
			}

			i += shifter;
		}
		return strings;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static bool StringEndsWithinQualifiedField(const string& str, const string& qualifier) {
		if (qualifier == "") {
			return false;
		}

		bool inQualifiedField = false;
		bool justActivatedQualifiedField = false;
		unsigned int i = 0;
		unsigned int shifter = 1;
		string s1, s2;
		while (i < str.size()) {
			justActivatedQualifiedField = false;
			shifter = 1;
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
			
			i += shifter;
		}
		return inQualifiedField;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void CompleteRowWithEndlines(string& str, unsigned int delimiterCount) {
		Array<string> strings;	// Will be used to add to malformed strings if completed data row turns out malformed.
		string s = "";			// Temp string to read into and do work
		string sLast;
		strings.Add(str);		// Place current string into temp array
		/* While the delimiter count is less than what we expect */
		while (delimiterCount < (expectedColumns - 1)) {
			sLast = s;
			/* Get a line. If failed, close file and exit loop */
			if (!getline(inFile, s)) {
				inFile.close();
				s = sLast;
				break;
			}
			strings.Add(s);	// Place current string into temp array
			delimiterCount += CountSubStringInString(s, delimiter, qualifier, true);	// With new line, count delims and add to current total
			/* If delimiter count is still less than, or equal to the desired ammount */
			if (delimiterCount <= (expectedColumns - 1)) {
				str += "\n" + s;	// Tack it onto the current string and continue.
			}
		}
		/* We broke out of loop above. If delimiter count is what we expect, good to go! */
		if (delimiterCount == (expectedColumns - 1)) {
			return;
		}
		else {
			str = s;	// Set string equal to last string read. 
			if (strings.GetSize() > 0) {
				/* All strings in the array are guarenteed to be malformed, except the last one read */
				for (unsigned int i = 0; i < (strings.GetSize() - 1); i++) {
					malformedRows.Add(strings[i]);	// Add row to malformed 
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
	void InterpretRows(Array<string>& rows) { // Intentions to possible thread this..
		for (unsigned int i = 0; i < rows.GetSize(); i++) {
			cout << "Row: {" + rows[i] + "}" << endl;
		}
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void ProcessFile() {
		if (inFile.is_open()) {
			Array<string> batch;
			string s1;
			unsigned int lineCount = (headerIncluded) ? 0 : 1;
			unsigned int totalLineCount = lineCount;

			// NOTE: If the file has no headers AND expected columns are not provided AND the first data row is malformed, this program probably will not work. 
			if (!headerIncluded && expectedColumns == 0) {
				cout << "[Processing] {WARNING}: : Data file has no headers and expected columns is not provided. If the first row is malformed this program will NOT work." << endl;
			}

			/* Get expected columns if needed */
			if (expectedColumns == 0) {
				/* Get a line */
				if (!getline(inFile, s1)) {
					inFile.close();	// If failed, close file
				}
				else { // getline succeeded
					expectedColumns = CountSubStringInString(s1, delimiter, qualifier) + 1;	// Grab the number of expected columns
					/* If headers are not included */
					if (!headerIncluded) {
						batch.Add(s1);	// This row contains data. Add this to the batch to be processed below
					}
				}
			}
			
			/* Verify some valid amount of columns was determined */
			if (expectedColumns == 0) {
				cout << "[Processing] {ERROR}: : Expected columns is 0. Will not process further." << endl;
				return;
			}

			/* Verify that batch either has data AND/OR file is still open*/
			if (batch.GetSize() == 0 && !inFile.is_open()) {
				cout << "[Processing] {ERROR}: : Could not read from file. Will not process further." << endl;
				return;
			}

			/* We have something to process */
			unsigned int delimiterCount = 0;
			while (true) {
				/* Load Batch */
				while (lineCount < batchSize && inFile.is_open()) {
					if (!getline(inFile, s1)) {
						inFile.close();
						break;
					}
					/* Check the number of delimiters on the line */
					delimiterCount = CountSubStringInString(s1, delimiter, qualifier);
					/* If less, than there is perhaps an endline within a qualifed row */
					if (delimiterCount < (expectedColumns - 1)) {
						CompleteRowWithEndlines(s1, delimiterCount);						// Call function to deal with this
						delimiterCount = CountSubStringInString(s1, delimiter, qualifier);	// With resulting string check again
					}
					/* If our delimiter count is greater, add to malformed. */
					if (delimiterCount != (expectedColumns - 1)) {
						malformedRows.Add(s1);
						continue;
					}

					batch.Add(s1);
					lineCount += 1;
					totalLineCount += 1;
				}

				/* Interpret Rows */
				if (batch.GetSize() > 0) {
					InterpretRows(batch);	// Intentions to possibly thread this.
				}

				/* Check for continuation */
				if (!inFile.is_open()) {
					cout << "[Processing] {NOTE}: Reached end of file." << endl;
					break;
				}

				/* Reset for next batch */
				lineCount = 0;
				batch.reset();
			}
		}
		else {
			cout << "File was unable to be opened. Cannot process file." << endl;
		}
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	Array<string> GetMalformedRows() { return malformedRows; }
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
	Array<string> malformedRows;

	fileName = "Example6.csv";
	dfc = DelimitedFileConverter(fileName, ",", "\"", true, true);
	dfc.ProcessFile();
	malformedRows = dfc.GetMalformedRows();
	cout << "[MALFORMED ROWS] : ######################################" << endl;
	for (unsigned int i = 0; i < malformedRows.GetSize(); i++) {
		cout << "[ROW]{ " + malformedRows[i] + "}[ROW]" << endl;
	}
	cout << "#########################################################" << endl << endl;
	return 0;
}
