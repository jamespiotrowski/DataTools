#pragma once

#ifndef H_DT
#include "Date.h"
#endif // !H_DT

#ifndef H_STRING
#include <string>
#endif // !H_STRING

#ifndef H_ARR
#include "Array.h"
#endif // !H_ARR

#ifndef H_STRU
#include "StringUtility.h"
#endif // !H_CLM

using namespace std;

/**********************************************************************************************
###############################################################################################
##### Column CLASS
###############################################################################################
**********************************************************************************************/

class Column {
public:

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
	DateFormatProcessor dateFormatProcessor;

	unsigned int columnLength = 0;
	unsigned int precision = 0;
	bool nullable = false;

	/* Members used to identify exact type */
	unsigned int instancesCounted = 0;			// Use this to track how many rows there are
	unsigned int nullsCounted = 0;				// Count how many instances were NULL or ""
	unsigned int columnLengthFrequency = 0;		// This is used to determine if string fields are constant lengths

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
	static bool IsBit(const string &s) { return ((s == "0") || (s == "1")); }

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

	/* Is Text */
	static bool IsText(const string& s) {
		if (s.size() > 4000) {
			return true;
		}
		return false;
	}

public:

	Column() {}

	Column(unsigned int columnIndex) : columnName(defaultColumnName + to_string(columnIndex)) { }

	Column(const string& columnName, const bool& formatColumnName, ColumnNameFormat columnNameFormat) {
		if (formatColumnName) {
			this->columnName = FormatColumnName(StringUtility::TrimWhiteSpace(columnName), columnNameFormat);
		}
		else {
			this->columnName = columnName;
		}
	}

	void DetermineDataType(const string& s) {
		instancesCounted += 1;

		bool dateSucceeded = false;

		if (IsBlank(s)) {
			nullable = true;
			nullsCounted += 1;
		}
		else { // 93 s

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

			if (ChangeColumnType(BIT) && columnType != BIT && IsBit(s)) {
				columnType = BIT;
			}
			else if (ChangeColumnType(INTEGER) && columnType != INTEGER && IsInt(s)) {
				columnType = INTEGER;
			}
			else if (ChangeColumnType(DECIMAL) && IsDecimal(s)) {
				columnType = DECIMAL;
				unsigned int totalLen;
				unsigned int decimalLen;
				if (StringUtility::StringContains(s, ".")) {
					Array<string> decimalParts = StringUtility::SplitString(s, ".", "");
					// Account for negative here?
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
			else if (ChangeColumnType(BOOLEAN) && columnType != BOOLEAN && IsBoolean(s)) {
				columnType = BOOLEAN;
			}
			else if (ChangeColumnType(DATE) || ChangeColumnType(DATETIME)) {
				dateFormatProcessor.ProcessDate(s);
				if (dateFormatProcessor.IsPossibleDateTime()) {
					columnType = DATETIME;
					dateSucceeded = true;
				}
				else if (dateFormatProcessor.IsPossibleDate()) {
					columnType = DATE;
					dateSucceeded = true;
				}
			}
			if (!dateSucceeded && ChangeColumnType(CHARACTER) && IsCharacter(s)) {
				columnType = CHARACTER;
			}
			else if (!dateSucceeded && ChangeColumnType(TEXT) && IsText(s)) {
				columnType = TEXT;
			}
			else if(!dateSucceeded){
				columnType = STRING;
			}
		}
	}

	string ToString() const {
		bool constantLength = (instancesCounted == (nullsCounted + columnLengthFrequency));
		string s = columnName + " : [ ";
		switch (columnType) {
		case(STRING): {
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
