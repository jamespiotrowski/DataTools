#ifndef H_IOSTRM
#define H_IOSTRM
#include <iostream>
#endif // !H_IOSTRM

#ifndef H_BRR
#define H_BRR
#include "Barrier.h"
#endif // !H_BRR

#ifndef H_STRING
#define H_STRING
#include <string>
#include <cstring>
#endif // !H_STRING

#ifndef H_FSTRM
#define H_FSTRM
#include <fstream>
#endif // !H_STRING

#ifndef H_CLM
#define H_CLM
#include "Column.h"
#endif // !H_CLM

#ifndef H_MSG
#define H_MSG
#include "Messaging.h"
#endif // !H_CLM

#ifndef H_STRU
#define H_STRU
#include "StringUtility.h"
#endif // !H_CLM

#ifndef H_ARR
#define H_ARR
#include "Array.h"
#endif // !H_ARR

#ifndef H_CTM
#define H_CTM
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
		wstring line;
		size_t lineNumber = 0;
		void PrintFileLine(wostream &out) const {
			wstring s = L"Line Number: " + to_wstring(lineNumber) + L", Row: {" + line + L"}\n";
			out << s;
		}
		void PrintFileLine() const { PrintFileLine(wcout); }
	};

private:
	wstring delimiter = L"";
	wstring fileName = L"";
	wstring qualifier = L"";
	wstring endline = L"\n";
	locale loc;
	bool headerIncluded = false;
	bool incompleteQualifierDetection = true;
	bool removeCarraigeReturns = true;
	bool trimLeadingAndTrailingWhiteSpace = true;
	bool keepOriginalColumnNames = false;
	wifstream inFile;
	size_t expectedColumns = 0;

	Array<Column> columns;
	Array<DateFormatProcessor> dateMetaData;
	Array<FileLine> malformedRows;
	// Array<Array<wstring>> data;

	struct ThreadArgs_FileLineBatch {
		size_t threadId;
		Array<FileLine>* batch;

		ThreadArgs_FileLineBatch(const size_t& tId, Array<FileLine>* b) : threadId(tId), batch(b) {}
		~ThreadArgs_FileLineBatch() {
			if (batch != nullptr) {
				delete batch;
				batch = nullptr;
			}
		}
	};

	enum class ThreadStatus {
		READY
		, BUSY
		, RESULT_AVAILABLE
	};

	size_t batchSize;
	bool useThreads;
	size_t threadCount;
	thread** threadPool = nullptr;	
	ThreadStatus* threadStatus = nullptr;		
	Array<Column>* threadResults = nullptr;
	size_t activeThreads = 0;

	void CombineColumns(Array<Column>& otherColumnList) {
		for (size_t c = 0; c < expectedColumns; c++) {
			columns[c] = Column::CombineColumns(columns[c], otherColumnList[c]);
		}
	}

	void ProcessThreadResults() {
		for (size_t i = 0; i < threadCount; i++) {
			if (threadStatus[i] == ThreadStatus::RESULT_AVAILABLE) {
				threadPool[i]->join();
				CombineColumns(threadResults[i]);
				threadStatus[i] = ThreadStatus::READY;
				activeThreads -= 1;
			}
		}
	}

	bool IsThreadReady(size_t &readyThread) const {
		for (size_t i = 0; i < threadCount; i++) {
			if (threadStatus[i] == ThreadStatus::READY) {
				readyThread = i;
				return true;
			}
		}
		return false;
	}

	void InterpretRows_T(ThreadArgs_FileLineBatch* ta) {
		Array<Array<wstring>> data;
		Array<Column> columnsCopy = columns;
		for (size_t i = 0; i < ta->batch->GetSize(); i++) {
			data.Add(StringUtility::SplitString((*(ta->batch))[i].line, delimiter, qualifier));
		}

		for (size_t i = 0; i < data.GetSize(); i += 1) {
			for (size_t c = 0; c < expectedColumns; c += 1) {
				columnsCopy[c].DetermineDataType(data[i][c]);
			}
		}

		threadStatus[ta->threadId] = ThreadStatus::RESULT_AVAILABLE;
		threadResults[ta->threadId] = columnsCopy;
		delete ta;
	}

	void InterpretRowsWithThread(Array<FileLine>* b) {
		size_t threadId = 0;
		while (true) {
			if (IsThreadReady(threadId)) {
				ThreadArgs_FileLineBatch* ta = new ThreadArgs_FileLineBatch(threadId, b);
				if (threadPool[threadId] != nullptr) {
					delete threadPool[threadId];
				}
				threadStatus[threadId] = ThreadStatus::BUSY;
				activeThreads += 1;
				threadPool[threadId] = new thread(&DelimitedFileConverter::InterpretRows_T, this, ta);
				PrintMessage(L"DelimitedFileConverter::InterpretRowsWithThread", MessageType::MESSAGE, L"Thread " + to_wstring(threadId) + L" working on batch.");
				return;
			}
			else {
				ProcessThreadResults();
			}
		}
	}


public:
	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void PrintOptions(wostream &out) const { 
		wstring s = L"-------------------------------------\n| Delimited File Converter          |\n-------------------------------------\n";
		s += L"| - File Name                          : " + fileName + L"\n";
		s += L"| - Delimiter                          : " + delimiter + L"\n";
		s += L"| - Qualifier                          : " + qualifier + L"\n";
		s += L"| - Headers Included                   : " + wstring((headerIncluded) ? L"Yes" : L"No") + L"\n";
		s += L"| - Number of Columns                  : " + wstring((expectedColumns == 0) ? L"Auto-Detect" : to_wstring(expectedColumns)) + L"\n";
		s += L"| - Incomplete Qualifier Detection     : " + wstring((incompleteQualifierDetection) ? L"On" : L"Off") + L"\n";
		s += L"| - Remove Carraige Returns            : " + wstring((removeCarraigeReturns) ? L"Yes" : L"No") + L"\n";
		s += L"| - Trim Leading & Trailing WhiteSpace : " + wstring((removeCarraigeReturns) ? L"Yes" : L"No") + L"\n";
		s += L"| - Keep Original Column Names         : " + wstring((keepOriginalColumnNames) ? L"Yes" : L"No") + L"\n";
		s += L"-------------------------------------\n";
		out << s;
	}

	void PrintOptions() const {
		PrintOptions(wcout);
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	DelimitedFileConverter() = delete;

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
		this->loc = copy.loc;
		
		this->incompleteQualifierDetection = copy.incompleteQualifierDetection;
		this->removeCarraigeReturns = copy.removeCarraigeReturns;
		this->trimLeadingAndTrailingWhiteSpace = copy.trimLeadingAndTrailingWhiteSpace;
		this->keepOriginalColumnNames = copy.keepOriginalColumnNames;
		this->columns = copy.columns;
		//this->data = copy.data;
		this->dateMetaData = copy.dateMetaData;

		inFile.imbue(loc);
		inFile.open(fileName, ios::in);
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
	DelimitedFileConverter(
		wstring fileName
		, locale loc
		, size_t expectedColumns = 0
		, wstring delimiter = L","
		, wstring qualifier = L"\""
		, bool headerIncluded = true
		, bool winEndLines = true
		, bool incompleteQualifierDetection = true
		, bool removeCarraigeReturns = true
		, bool trimWhiteSpace = true
		, bool keepOriginalColumnNames = false
		, bool useThreads = false
		, size_t numThreads = 4
		, size_t ProcessingBatchSize = 25
	) : fileName(fileName)
		, loc(loc)
		, expectedColumns(expectedColumns)
		, delimiter(delimiter)
		, qualifier(qualifier)
		, headerIncluded(headerIncluded)
		, incompleteQualifierDetection(incompleteQualifierDetection)
		, removeCarraigeReturns(removeCarraigeReturns)
		, trimLeadingAndTrailingWhiteSpace(trimWhiteSpace)
		, keepOriginalColumnNames(keepOriginalColumnNames)
		, useThreads(useThreads)
		, threadCount(numThreads)
		, batchSize(ProcessingBatchSize)
	{
		if (winEndLines) {
			endline = L"\r\n";
		}
		inFile.imbue(loc);
		inFile.open(fileName, ios::in);

		if (useThreads) {
			threadPool = new thread * [numThreads];
			threadStatus = new ThreadStatus[numThreads];
			threadResults = new Array<Column>[numThreads];
			for (size_t i = 0; i < numThreads; i++) {
				threadStatus[i] = ThreadStatus::READY;
				threadPool[i] = nullptr;
			}
		}

	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	~DelimitedFileConverter(){
		if (inFile.is_open()) {
			inFile.close();
		}
		if (useThreads) {
			if (threadPool != nullptr) {
				for (size_t i = 0; i < threadCount; i++) {
					if (threadPool[i] != nullptr) {
						delete threadPool[i];
						threadPool[i] = nullptr;
					}
				}
				delete[] threadPool;
			}

			delete[] threadStatus;
			delete[] threadResults;
		}
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	bool GetFileLine(wstring &s) {
		if (!getline(inFile, s)) {
			return false;
		}

		if (removeCarraigeReturns && s.size() > 0) {
			if (s[s.size() - 1] == L'\r') {
				s = StringUtility::SubString(s, 0, s.size() - 1);
			}
		}

		return true;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void CompleteRowWithEndlines(FileLine& fl, size_t delimiterCount, size_t& totalLineCount) {
		Array<FileLine> strings;	// Will be used to add to malformed strings if completed data row turns out malformed.
		wstring s = L"";			// Temp string to read into and do work
		wstring sLast;
		FileLine f;
		bool incompleteQualifierDetected = false;
		strings.Add(fl);  // Place current string into temp array
		size_t nextRowDelimiterCount = 0;
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
					size_t lc = totalLineCount + ((headerIncluded) ? 0 : -1);
					incompleteQualifierDetected = true;
					PrintMessage(L"DelimitedFileConverter::CompleteRowWithEndlines", MessageType::WARNING, L"Row with incomplete qualifier detected around line " + to_wstring(lc) + L". Incomplete Qualifier Detection is currently on. If this is a mistake, rerun the program with this option set to off.");
					break;
				}
			}

			strings.Add(f);	// Place current string into temp array
			delimiterCount += StringUtility::CountSubStringInString(s, delimiter, qualifier, true);	// With new line, count delims and add to current total
			/* If delimiter count is still less than, or equal to the desired ammount */
			if (delimiterCount <= (expectedColumns - 1)) {
				fl.line += L"\n" + s;	// Tack it onto the current string and continue.
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
					for (size_t i = 0; i < (strings.GetSize() - os); i++) {
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
		Array<Array<wstring>> data;
		for (size_t i = 0; i < rows.GetSize(); i++) {
			data.Add(StringUtility::SplitString(rows[i].line, delimiter, qualifier));
		}

		for (size_t i = 0; i < data.GetSize(); i += 1) {
			for (size_t c = 0; c < expectedColumns; c += 1) {
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
			bool deleteUnusedBatch = false;
			Array<FileLine>* batch = new Array<FileLine>();
			wstring s1;
			FileLine fl;
			size_t lineCount = (headerIncluded) ? 0 : 1;
			size_t totalLineCount = lineCount;

			// NOTE: If the file has no headers AND expected columns are not provided AND the first data row is malformed, this program probably will not work. 
			if (!headerIncluded && expectedColumns == 0) {
				PrintMessage(L"DelimitedFileConverter::ProcessFile", MessageType::WARNING, L"Data file has no headers and expected columns is not provided. If the first row is malformed this program will NOT work.");
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
				batch->Add(fl);	// This row contains data. Add this to the batch to be processed below
				for (size_t i = 0; i < expectedColumns; i++) {
					columns.Add(Column(i));
				}
			}
			else {
				Array<wstring> columnNames = StringUtility::SplitString(s1, delimiter, qualifier);
				for (size_t i = 0; i < expectedColumns; i++) {
					columns.Add(Column(columnNames[i], true, Column::ColumnNameFormat::PROPER_CASE));
				}
			}

			/* Verify some valid amount of columns was determined */
			if (expectedColumns == 0) {
				PrintMessage(L"DelimitedFileConverter::ProcessFile", MessageType::ERROR, L"Expected columns is 0. Will not process further.");
				return;
			}

			/* Verify that batch either has data AND/OR file is still open*/
			if (batch->GetSize() == 0 && !inFile.is_open()) {
				PrintMessage(L"DelimitedFileConverter::ProcessFile", MessageType::ERROR, L"Could not read from file. Will not process further.");
				delete batch;
				return;
			}

			/* We have something to process */
			size_t delimiterCount = 0;
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

					//wcout << fl.line << endl;

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

					batch->Add(fl);
				}

				/* Interpret Rows */
				if (batch->GetSize() > 0) {
					if (useThreads) {
						InterpretRowsWithThread(batch);
						batch = nullptr;
						batch = new Array<FileLine>(); // The old one is going to be deleted by the thread
					}
					else {
						InterpretRows(*batch);	// Intentions to possibly thread this.
					}
				}
				else {
					deleteUnusedBatch = true;
				}

				/* Check for continuation */
				if (!inFile.is_open()) {
					PrintMessage(L"DelimitedFileConverter::ProcessFile", MessageType::MESSAGE, L"Reached end of file.");
					break;
				}

				/* Reset for next batch */
				lineCount = 0;
				if (!useThreads) {
					batch->reset();
				}
				PrintMessage(L"DelimitedFileConverter::ProcessFile", MessageType::MESSAGE, L"Lines analyzed: " + to_wstring(totalLineCount) + L"...");
			}

			while (activeThreads > 0 && useThreads) {
				ProcessThreadResults();
			}

			if ((batch != nullptr && !useThreads) || deleteUnusedBatch) {
				delete batch;
			}
		}
		else {
			PrintMessage(L"DelimitedFileConverter::ProcessFile", MessageType::ERROR, L"File was unable to be opened. Cannot process file.");
		}
		auto end = std::chrono::system_clock::now();
		elapsed_seconds = end - start;
		PrintMessage(L"DelimitedFileConverter::ProcessFile", MessageType::MESSAGE, L"elapsed time: " + to_wstring((int)elapsed_seconds.count()) + L"s");
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	Array<FileLine> GetMalformedRows() { return malformedRows; }

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void GetTableStructureAsScript(const SqlType& sqlType, const wstring& fileName, const wstring& tableName) { 
		wstring newTableName = tableName; // StringUtility::StringReplace(StringUtility::StringReplace(tableName, "[", ""), "]", "");
		if (columns.GetSize() > 0) {
			wstring s = L"";
			if (sqlType == SqlType::TRANSACT_SQL) {
				s = L"CREATE TABLE " + newTableName + L" (\n";
				/* First Column */
				s += L"\t[" + columns[0].GetColumnName() + L"] "; /* Name */
				s += columns[0].GetSqlTypeAsString(sqlType) + L"\n"; /* SQL Type */
				/* Remainder of columns */
				for (size_t i = 1; i < columns.GetSize(); i++) {
					s += L"\t,[" + columns[i].GetColumnName() + L"] "; /* Name */
					s += columns[i].GetSqlTypeAsString(sqlType) + L"\n"; /* SQL Type */
				}
				s += L");";
			}
			wofstream outFile;
			outFile.imbue(loc);
			outFile.open(fileName);
			if (outFile.is_open()) {
				outFile << s;
				outFile.close();
			}
			else {
				PrintMessage(L"DelimitedFileConverter::GetTableStructureAsScript", MessageType::ERROR, L"Failed to open script file. File name provided may be invalid.");
			}
		}
		else {
			PrintMessage(L"DelimitedFileConverter::GetTableStructureAsScript", MessageType::ERROR, L"There are no columns. Cannot generate SQL Script.");
		}
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	wstring NullIf(const wstring& s, const wstring& n) {
		if (s == n) {
			return L"NULL";
		}
		return s;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	wstring GetSqlValueString(const SqlType& sqlType, const FileLine& fileLine) {
		Array<wstring> values = StringUtility::SplitString(fileLine.line, delimiter, qualifier);
		if (values.GetSize() != columns.GetSize()) {
			PrintMessage(L"DelimitedFileConverter::GetSqlValueString", MessageType::ERROR, L"Number of values in string does not equal number of columns.");
			return wstring();
		}

		if (sqlType == SqlType::TRANSACT_SQL) {
			wstring s = L"(";
			for (size_t i = 0; i < columns.GetSize(); i++) {
				// If string, need quotes
				if (columns[i].IsStringType()) { 
					// Need to qualify quotes with replace ( ' --> '' )
					s += NullIf(L"'" + NullIf(StringUtility::StringReplace(values[i], L"'", L"''"), L"") + L"'", L"'NULL'");
				}
				// If bool, need to convert to BIT 
				else if (columns[i].IsBoolean()) {
					s += NullIf(ConvertBooleanValueToString(values[i]), L"");
				}
				// Should be fine from here
				else {
					s += NullIf(values[i], L"");
				}

				if (i != (columns.GetSize() - 1)) {
					s += L",";
				}
			}
			s += L")";
			return s;
		}

		PrintMessage(L"DelimitedFileConverter::GetSqlValueString", MessageType::ERROR, L"Requested SQL Type is currently not implemented.");
		return wstring();
	}
	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	void GetDataAsSqlScript(const SqlType& sqlType, const wstring& outFileName, const wstring& tableName) {
		wstring newTableName = tableName; // StringUtility::StringReplace(StringUtility::StringReplace(tableName, "[", ""), "]", "");
		if (columns.GetSize() == 0) {
			PrintMessage(L"DelimitedFileConverter::GetDataAsSqlScript", MessageType::ERROR, L"There are no columns. Cannot generate SQL Script.");
			return;
		}

		inFile.open(fileName, ios::in);
		if (!inFile.is_open()) {
			PrintMessage(L"DelimitedFileConverter::GetDataAsSqlScript", MessageType::ERROR, L"Failed to open the data file.");
			return;
		}

		wofstream outFile;
		outFile.imbue(loc);
		outFile.open(outFileName);
		if (!outFile.is_open()) {
			PrintMessage(L"DelimitedFileConverter::GetDataAsSqlScript", MessageType::ERROR, L"Failed to open the output file.");
			inFile.close();
			return;
		}
		
		std::chrono::duration<double> elapsed_seconds;
		auto start = std::chrono::system_clock::now();
		if (inFile.is_open()) {
			Array<FileLine> batch;
			wstring s1;
			FileLine fl;
			size_t lineCount = (headerIncluded) ? 0 : 1;
			size_t totalLineCount = lineCount;

			if (!GetFileLine(s1)) {
				inFile.close();	// If failed, close file
			}

			/* If headers are not included */
			if (!headerIncluded) {
				fl.line = s1;
				fl.lineNumber = totalLineCount + ((headerIncluded) ? 1 : 0);
				batch.Add(fl);	// This row contains data. Add this to the batch to be processed below
			}
			
			/* Verify that batch either has data AND/OR file is still open*/
			if (batch.GetSize() == 0 && !inFile.is_open()) {
				PrintMessage(L"DelimitedFileConverter::GetDataAsSqlScript", MessageType::ERROR, L"Could not read from file. Will not process further.");
				return;
			}

			const int batchLimiter = 500;
			int currentBatchCount = 0;

			wstring columnList = L"(";
			for (size_t i = 0; i < columns.GetSize(); i++) {
				columnList += L"[" + columns[i].GetColumnName() + L"]";
				if (i != (columns.GetSize() - 1)) {
					columnList += L",";
				}
			}
			columnList += L")";

			/* We have something to process */
			size_t delimiterCount = 0;
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

					//wcout << fl.line << endl;

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
						PrintMessage(L"DelimitedFileConverter::GetDataAsSqlScript", MessageType::WARNING, L"Malformed row occured while generating insert script. Skipping. Line Number: " + to_wstring(fl.lineNumber));
						continue;
					}

					batch.Add(fl);
				}

				/* Interpret Rows */
				if (batch.GetSize() > 0) {
					wstring insertLine;
					for (size_t i = 0; i < batch.GetSize(); i++) {
						if (currentBatchCount == 0) {
							insertLine = L"\nINSERT INTO " + newTableName + L" VALUES\n\t";
						}
						else {
							insertLine = L"\t,";
						}
						insertLine += GetSqlValueString(sqlType, batch[i]);
						currentBatchCount += 1;
						if (currentBatchCount >= batchLimiter) {
							insertLine += L";";
							currentBatchCount = 0;
						}
						insertLine += L"\n";
						outFile << insertLine;
					}
				}

				/* Check for continuation */
				if (!inFile.is_open()) {
					PrintMessage(L"DelimitedFileConverter::GetDataAsSqlScript", MessageType::MESSAGE, L"Reached end of file.");
					break;
				}

				/* Reset for next batch */
				lineCount = 0;
				batch.reset();
				PrintMessage(L"DelimitedFileConverter::GetDataAsSqlScript", MessageType::MESSAGE, L"Lines converted to SQL: " + to_wstring(totalLineCount) + L"...");
			}
			if (currentBatchCount != 0) {
				outFile << L";";
			}
			outFile.close();
		}
		else {
			PrintMessage(L"DelimitedFileConverter::GetDataAsSqlScript", MessageType::ERROR, L"File was unable to be opened. Cannot process file.");
		}
		auto end = std::chrono::system_clock::now();
		elapsed_seconds = end - start;
		PrintMessage(L"DelimitedFileConverter::GetDataAsSqlScript", MessageType::MESSAGE, L"elapsed time: " + to_wstring((int)elapsed_seconds.count()) + L"s");
	}

	size_t GetColumnCount() const { return columns.GetSize(); }

	Column operator[](size_t i) const { return columns.At(i); }

};

/**********************************************************************************************
###############################################################################################
#####
###############################################################################################
**********************************************************************************************/

int main(int argc, char* argv[])
{
	if (argc == 3) {
		string l_name = argv[1];
		locale loc(l_name);
		wifstream inFile;
		inFile.imbue(loc);
		inFile.open(argv[2]);
		if (inFile.is_open()) {
			wstring line;
			Array<wstring> lines;
			while (getline(inFile, line)) {
				lines.Add(line);
				//wcout << line << endl;
			}
			inFile.close();

			wstring Encoding = L"en_US.UTF8";
			wstring InputFile = L"", Delimiter = L",", Qualifier = L"\"", OS = L"Windows", SqlTableFile = L"", SqlDataFile = L"", MalformedRowsFile = L"", DataAnalysisFile = L"", SqlTableName = L"[dbo].[temp]";
			bool HeaderIncluded = true, IncompleteQualifierDetection = true, RemoveCarraigeReturns = true, TrimWhiteSpace = true, KeepOriginalColumnNames = false, UseThreads = false;
			size_t NumThreads = 4, ProcessingBatchSize = 25, ExpectedNumberOfColumns = 0;
			SqlType sqlType = SqlType::TRANSACT_SQL;

			for (size_t i = 0; i < lines.GetSize(); i++) {
				wstring cleanedLine = StringUtility::StringReplace(lines[i], L"\t", L"");
				Array<wstring> option = StringUtility::SplitString(cleanedLine, L":", L"\"");
				if (option.GetSize() >= 2) {
					if (option[0] == L"InputFile")					{ InputFile = option[1]; }
					else if (option[0] == L"SqlTableFile")			{ SqlTableFile = option[1]; }
					else if (option[0] == L"SqlDataFile")			{ SqlDataFile = option[1]; }
					else if (option[0] == L"Delimiter")				{ Delimiter = option[1]; }
					else if (option[0] == L"Qualifier")				{ Qualifier = option[1]; }
					else if (option[0] == L"Encoding")				{ Encoding = option[1]; }
					else if (option[0] == L"OS")					{ OS = option[1]; }
					else if (option[0] == L"SqlType")				{ sqlType = ConvertSqlTypeStringToSqlType(option[1]); }
					else if (option[0] == L"NumThreads")			{ NumThreads = stoi(option[1]); }
					else if (option[0] == L"ProcessingBatchSize")	{ ProcessingBatchSize = stoi(option[1]); }
					else if (option[0] == L"ExpectedNumberOfColumns") { ExpectedNumberOfColumns = stoi(option[1]); }
					else if (option[0] == L"HeaderIncluded")		{ HeaderIncluded = StringUtility::ConvertStringToBool(option[1]); }
					else if (option[0] == L"IncompleteQualifierDetection") { IncompleteQualifierDetection = StringUtility::ConvertStringToBool(option[1]); }
					else if (option[0] == L"RemoveCarraigeReturns") { RemoveCarraigeReturns = StringUtility::ConvertStringToBool(option[1]); }
					else if (option[0] == L"TrimWhiteSpace")		{ TrimWhiteSpace = StringUtility::ConvertStringToBool(option[1]); }
					else if (option[0] == L"KeepOriginalColumnNames") { KeepOriginalColumnNames = StringUtility::ConvertStringToBool(option[1]); }
					else if (option[0] == L"UseThreads")			{ UseThreads = StringUtility::ConvertStringToBool(option[1]); }
					else if (option[0] == L"MalformedRowsFile")		{ MalformedRowsFile = option[1]; }
					else if (option[0] == L"DataAnalysisFile")		{ DataAnalysisFile = option[1]; }
					else if (option[0] == L"SqlTableName")			{ SqlTableName = option[1]; }
				}
			}

			if (InputFile == L"") {
				PrintMessage(L"main()", MessageType::ERROR, L"No input file provided! Please check your argument file and make sure at least the input file name is provided.");
				return 1;
			}

			OS = StringUtility::ToLowerCaseString(OS);

			locale locDfc(StringUtility::WStringToString(Encoding));
			DelimitedFileConverter dfc(InputFile, locDfc, ExpectedNumberOfColumns, Delimiter, Qualifier, HeaderIncluded, (OS == L"windows")
				, IncompleteQualifierDetection, RemoveCarraigeReturns, TrimWhiteSpace, KeepOriginalColumnNames, UseThreads, NumThreads, ProcessingBatchSize);

			dfc.PrintOptions();

			dfc.ProcessFile();

			/*** MALFORMED ROWS ***/
			if (MalformedRowsFile != L"") {
				Array<DelimitedFileConverter::FileLine> mRows = dfc.GetMalformedRows();
				if (mRows.GetSize() > 0) {
					PrintMessage(L"main()", MessageType::MESSAGE, L"Malformed Row(s) detected. Writing to file.");
					wofstream outFile;
					outFile.open(MalformedRowsFile);
					if (outFile.is_open()) {
						for (size_t i = 0; i < mRows.GetSize(); i++) {
							mRows[i].PrintFileLine(outFile);
						}
						outFile.close();

					}
					else {
						PrintMessage(L"main()", MessageType::ERROR, L"Unable to open the malformed rows file.");
					}
				}
			}

			/*** Data Analysis File ***/
			if (DataAnalysisFile != L"") {
				if (dfc.GetColumnCount() > 0) {
					PrintMessage(L"main()", MessageType::MESSAGE, L"Writing to data analysis file.");
					wofstream outFile;
					outFile.open(DataAnalysisFile);
					if (outFile.is_open()) {
						for (size_t i = 0; i < dfc.GetColumnCount(); i++) {
							outFile << dfc[i].ToString() << endl;
						}
						outFile.close();

					}
					else {
						PrintMessage(L"main()", MessageType::ERROR, L"Unable to open the data anlysis file.");
					}
				}
			}

			/*** SQL Table File ***/
			if (SqlTableFile != L"") {
				if (dfc.GetColumnCount() > 0) {
					PrintMessage(L"main()", MessageType::MESSAGE, L"Writing to SQL table file.");
					dfc.GetTableStructureAsScript(sqlType, SqlTableFile, SqlTableName);
				}
			}

			/*** SQL Data File ***/
			if (SqlDataFile != L"") {
				if (dfc.GetColumnCount() > 0) {
					PrintMessage(L"main()", MessageType::MESSAGE, L"Writing to SQL data file.");
					dfc.GetTableStructureAsScript(sqlType, SqlTableFile, SqlTableName);
					dfc.GetDataAsSqlScript(sqlType, SqlDataFile, SqlTableName);
				}
			}
		}
	}
	else {
		PrintMessage(L"main()", MessageType::MESSAGE, L"USAGE: DelimitedFileConverter.exe <encoding> <argument file>");
		PrintMessage(L"main()", MessageType::ERROR, L"The current iteration of this program only works when an encoding and an argument file is provided.");
	}

	return 0;
}
