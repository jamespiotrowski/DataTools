#ifndef H_IOSTRM
#include <iostream>
#endif // !H_IOSTRM

#ifndef H_STRING
#include <string>
#endif // !H_STRING

#ifndef H_FSTRM
#include <fstream>
#endif // !H_STRING

#ifndef H_CLM
#include "Column.h"
#endif // !H_CLM

#ifndef H_MSG
#include "Messaging.h"
#endif // !H_CLM

#ifndef H_STRU
#include "StringUtility.h"
#endif // !H_CLM

#ifndef H_ARR
#include "Array.h"
#endif // !H_ARR

#ifndef H_CTM
#include <ctime>
#include <chrono>
#endif // !H_CTM


using namespace std;


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

	const unsigned int batchSize = 1000;

	Array<Column> columns;
	Array<DateFormatProcessor> dateMetaData;
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
		this->columns = copy.columns;
		this->data = copy.data;
		this->dateMetaData = copy.dateMetaData;

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
		std::chrono::duration<double> elapsed_seconds;
		auto start = std::chrono::system_clock::now();
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
					columns.Add(Column(i));
				}
			}
			else {
				Array<string> columnNames = StringUtility::SplitString(s1, delimiter, qualifier);
				for (unsigned int i = 0; i < expectedColumns; i++) {
					columns.Add(Column(columnNames[i], true, Column::PROPER_CASE));
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
		auto end = std::chrono::system_clock::now();
		elapsed_seconds = end - start;
		std::cout << "elapsed time: " << (int)elapsed_seconds.count() << "s" << std::endl;
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

	fileName = "DateExample.txt";
	dfc = DelimitedFileConverter(fileName, "\t", "", true, true);
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
