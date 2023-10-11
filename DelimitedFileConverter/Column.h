#pragma once

#ifndef H_DT
#define H_DT
#include "Date.h"
#endif // !H_DT

#ifndef H_STRING
#define H_STRING
#include <string>
#include <cstring>
#endif // !H_STRING

#ifndef H_ARR
#define H_ARR
#include "Array.h"
#endif // !H_ARR

#ifndef H_STRU
#define H_STRU
#include "StringUtility.h"
#endif // !H_CLM

using namespace std;

/**********************************************************************************************
###############################################################################################
##### Column CLASS
###############################################################################################
**********************************************************************************************/


enum class SqlType {
	TRANSACT_SQL
	, MY_SQL
};

SqlType ConvertSqlTypeStringToSqlType(wstring sqlType) {
	sqlType = StringUtility::ToLowerCaseString(sqlType);
	
	if (sqlType == L"transact-sql" || sqlType == L"t-sql" || sqlType == L"transact sql" || sqlType == L"t sql" || sqlType == L"transactsql" || sqlType == L"tsql") {
		return SqlType::TRANSACT_SQL;
	}

	if (sqlType == L"my-sql" || sqlType == L"mysql" || sqlType == L"my sql") {
		return SqlType::MY_SQL;
	}

	return SqlType::TRANSACT_SQL;
}

wstring ConvertBooleanValueToString(wstring s) {
	s = StringUtility::ToLowerCaseString(s);
	if (s == L"1" || s == L"true") { return L"1"; }
	if (s == L"0" || s == L"false") { return L"0"; }
	PrintMessage(L"ConvertBooleanValueToString", MessageType::ERROR, L"Tried to represent a non-boolean value as boolean. Value: " + s);
	return wstring();
}

class Column {
public:

	enum class ColumnNameFormat {
		PROPER_CASE
		// , CAMEL_CASE
		// , UPPER_CASE
		// , LOWER_CASE
	};

	enum class ColumnType {
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

private:
	wstring FormatColumnName(const wstring& oldColumnName, ColumnNameFormat columnNameFormat) {
		wstring newColumnName;
		bool setToUpperCase = false;

		if (columnNameFormat == ColumnNameFormat::PROPER_CASE) {
			setToUpperCase = true;
			for (size_t i = 0; i < oldColumnName.size(); i++) {
				wchar_t c = oldColumnName[i];
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

	static const wstring defaultColumnName;

	/* Main column descriptors */
	wstring columnName;
	ColumnType columnType = ColumnType::UNDETERMINED;
	DateFormatProcessor dateFormatProcessor;

	size_t columnLength = 0;
	size_t precision = 0;
	bool nullable = false;
	bool unicode = false;

	/* Members used to identify exact type */
	size_t instancesCounted = 0;			// Use this to track how many rows there are
	size_t nullsCounted = 0;				// Count how many instances were NULL or ""
	size_t columnLengthFrequency = 0;		// This is used to determine if string fields are constant lengths

	bool ChangeColumnType(const ColumnType& newColumnType) {
		if (newColumnType == columnType) {
			return true;
		}

		switch (columnType) {
		case(ColumnType::UNDETERMINED): { return true; }
		case(ColumnType::TEXT): { return false; }
		case(ColumnType::STRING): {
			if (newColumnType == ColumnType::TEXT) {
				return true;
			}
			return false;
		}
		case(ColumnType::DECIMAL): case(ColumnType::CHARACTER): case(ColumnType::DATETIME): case(ColumnType::BOOLEAN): {
			if (newColumnType == ColumnType::STRING || newColumnType == ColumnType::TEXT) {
				return true;
			}
			return false;
		}
		case(ColumnType::INTEGER): {
			if (newColumnType == ColumnType::STRING || newColumnType == ColumnType::TEXT || newColumnType == ColumnType::DECIMAL) {
				return true;
			}
			return false;
		}
		case(ColumnType::DATE): {
			if (newColumnType == ColumnType::STRING || newColumnType == ColumnType::TEXT || newColumnType == ColumnType::DATETIME) {
				return true;
			}
			return false;
		}
		case(ColumnType::BIT): {
			if (newColumnType == ColumnType::STRING || newColumnType == ColumnType::TEXT || newColumnType == ColumnType::INTEGER || newColumnType == ColumnType::DECIMAL) {
				return true;
			}
			return false;
		}
		}
		return false;
	}

	/* Check for NULL Value */
	static bool IsBlank(const wstring &s) {
		wstring t = StringUtility::ToLowerCaseString(s);
		return ((t == L"null") || (t == L""));
	}

	/* Check for bit value */
	static bool IsBit(const wstring &s) { return ((s == L"0") || (s == L"1")); }

	/* Check for char */
	static bool IsCharacter(const wstring &s) { return (s.size() == 1); }

	/* Check for Boolean value */
	static bool IsBoolean(const wstring &s) {
		wstring t = StringUtility::ToLowerCaseString(s);
		return ((t == L"true") || (t == L"false"));
	}

	/* Check for int value */
	static bool IsInt(const wstring &s) {
		if (s.size() == 0) { return false; }
		size_t i = (s[0] == L'-') ? 1 : 0;
		for (i; i < s.size(); i++) {
			if (!StringUtility::IsDigit(s[i])) {
				return false;
			}
		}
		return true;
	}

	/* Is Decimal */
	static bool IsDecimal(const wstring &s) {
		if (s.size() == 0) { return false; }
		bool foundDecimal = false;
		size_t i = (s[0] == L'-') ? 1 : 0;
		for (i; i < s.size(); i++) {
			if (!StringUtility::IsDigit(s[i])) {
				if (s[i] == L'.' && !foundDecimal) {
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
	static bool IsText(const wstring& s) {
		if (s.size() > 4000) {
			return true;
		}
		return false;
	}

public:

	Column() {}

	Column(size_t columnIndex) : columnName(defaultColumnName + to_wstring(columnIndex)) { }

	Column(const wstring& columnName, const bool& formatColumnName, ColumnNameFormat columnNameFormat) {
		if (formatColumnName) {
			this->columnName = StringUtility::StringReplace(StringUtility::StringReplace(FormatColumnName(StringUtility::TrimWhiteSpace(columnName), columnNameFormat), L"]", L""), L"[", L"");
		}
		else {
			this->columnName = StringUtility::StringReplace(StringUtility::StringReplace(columnName, L"]", L""), L"[", L"");
		}
	}

	void DetermineDataType(const wstring& s) {
		instancesCounted += 1;

		bool previousSuccess = false;

		if (IsBlank(s)) {
			nullable = true;
			nullsCounted += 1;
		}
		else { // 93 s

			size_t totalLen = s.length();
			if (totalLen == columnLength) {
				columnLengthFrequency += 1;
			}
			else {
				columnLengthFrequency = 1;
				if (totalLen > columnLength) {
					columnLength = totalLen;
				}
			}

			if (ChangeColumnType(ColumnType::BIT) && IsBit(s)) {
				columnType = ColumnType::BIT;
				previousSuccess = true;
			}
			else if (ChangeColumnType(ColumnType::DECIMAL) && IsDecimal(s) && (!IsInt(s) || columnType == ColumnType::DECIMAL)) {
				columnType = ColumnType::DECIMAL;
				previousSuccess = true;
				size_t totalLen;
				size_t decimalLen;
				if (StringUtility::StringContains(s, L".")) {
					Array<wstring> decimalParts = StringUtility::SplitString(s, L".", L"");
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
			else if (ChangeColumnType(ColumnType::INTEGER) && IsInt(s)) {
				columnType = ColumnType::INTEGER;
				previousSuccess = true;
			}
			else if (ChangeColumnType(ColumnType::BOOLEAN) && IsBoolean(s)) {
				columnType = ColumnType::BOOLEAN;
				previousSuccess = true;
			}
			else if (ChangeColumnType(ColumnType::DATE) || ChangeColumnType(ColumnType::DATETIME)) {
				dateFormatProcessor.ProcessDate(s);
				if (dateFormatProcessor.IsPossibleDateTime()) {
					columnType = ColumnType::DATETIME;
					previousSuccess = true;
				}
				else if (dateFormatProcessor.IsPossibleDate()) {
					columnType = ColumnType::DATE;
					previousSuccess = true;
				}
			}

			if (!previousSuccess && ChangeColumnType(ColumnType::CHARACTER) && IsCharacter(s)) {
				columnType = ColumnType::CHARACTER;
			}
			else if (!previousSuccess && ChangeColumnType(ColumnType::TEXT) && IsText(s)) {
				columnType = ColumnType::TEXT;
				if (!unicode && StringUtility::ContainsUnicodeChars(s)) {
					unicode = true;
				}
			}
			else if(!previousSuccess){
				columnType = ColumnType::STRING;
				if (!unicode && StringUtility::ContainsUnicodeChars(s)) {
					unicode = true;
				}
			}
		}
	}

	wstring ToString() const {
		bool constantLength = (instancesCounted == (nullsCounted + columnLengthFrequency));
		wstring s = columnName + L" : [ ";
		switch (columnType) {
		case(ColumnType::STRING): {
			s += ((unicode) ? L"LSTRING(" : L"STRING(") + to_wstring(columnLength);
			s += (constantLength) ? L"c)" : L")";
			break;
		}
		case(ColumnType::DECIMAL): {
			s += L"DECIMAL(" + to_wstring(columnLength) + L"," + to_wstring(precision) + L")";
			break;
		}
		case(ColumnType::INTEGER): { s += L"INTEGER"; break; }
		case(ColumnType::CHARACTER): { s += L"CHARACTER"; break; }
		case(ColumnType::TEXT): { s += ((unicode) ? L"LTEXT" : L"TEXT"); break; }
		case(ColumnType::DATE): { s += L"DATE"; break; }
		case(ColumnType::DATETIME): { s += L"DATETIME"; break; }
		case(ColumnType::BIT): { s += L"BIT"; break; }
		case(ColumnType::BOOLEAN): { s += L"BOOLEAN"; break; }
		case(ColumnType::UNDETERMINED): { s += L"UNDETERMINED"; break; }
		}
		s += (nullable) ? L" NULL" : L" NOT NULL";
		s += L" ]";
		return s;
	}

	wstring GetColumnName() const { return columnName; }

	wstring GetSqlTypeAsString(SqlType sqlType) const {
		wstring s;
		bool constantLength = (instancesCounted == (nullsCounted + columnLengthFrequency));
		if (sqlType == SqlType::TRANSACT_SQL) {
			switch (columnType) {
			case(ColumnType::STRING): {
				if (constantLength) {
					s = (unicode ? L"[NCHAR](" : L"[CHAR](") + to_wstring(columnLength) + L")";
				}
				else {
					s = (unicode ? L"[NVARCHAR](" : L"[VARCHAR](") + to_wstring(columnLength) + L")";
				}
				break;
			}
			case(ColumnType::DECIMAL): {
				s = L"[DECIMAL](" + to_wstring(columnLength) + L"," + to_wstring(precision) + L")";
				break;
			}
			case(ColumnType::CHARACTER): { s = (unicode ? L"[NCHAR](1)" : L"[CHAR](1)"); break; }
			case(ColumnType::TEXT): { s = L"[TEXT]"; break; }
			case(ColumnType::DATE): { s = L"[DATE]"; break; }
			case(ColumnType::DATETIME): {
				if (columnLength < 20) { s = L"[SMALLDATETIME]"; }
				else if (columnLength < 24) { s = L"[DATETIME]"; }
				else { s = L"[DATETIME2]"; }
				break; 
			}
			case(ColumnType::BIT): { s = L"[BIT]"; break; }
			case(ColumnType::BOOLEAN): { s = L"[BIT]"; break; }
			case(ColumnType::INTEGER): {
				if (columnLength < 3) { s = L"[TINYINT]"; }
				else if (columnLength < 5) { s = L"[SMALLINT]"; }
				else if (columnLength < 10) { s = L"[INT]"; }
				else { s = L"[BIGINT]"; }
				break; 
			}
			case(ColumnType::UNDETERMINED): { s = L"[???]"; break; }
			}

			s += (nullable) ? L" NULL" : L" NOT NULL";
		}
		return s;
	}

	bool IsStringType() const {
		return (columnType == ColumnType::STRING || columnType == ColumnType::CHARACTER || columnType == ColumnType::TEXT || columnType == ColumnType::DATE || columnType == ColumnType::DATETIME);
	}

	bool IsBoolean() const {
		return (columnType == ColumnType::BOOLEAN);
	}

	static Column CombineColumns(Column& c1, Column& c2) {
		Column newColumn = c1;
		if (newColumn.ChangeColumnType(c2.columnType)) {
			newColumn.columnType = c2.columnType;
		}
		newColumn.columnLength = (c1.columnLength > c2.columnLength) ? c1.columnLength : c2.columnLength;
		newColumn.precision = (c1.precision > c2.precision) ? c1.precision : c2.precision;
		newColumn.nullable = (c1.nullable || c2.nullable);
		newColumn.unicode = (c1.unicode || c2.unicode);
		newColumn.instancesCounted = c1.instancesCounted + c2.instancesCounted;
		newColumn.nullsCounted = c1.nullsCounted + c2.nullsCounted;
		newColumn.columnLengthFrequency = c1.columnLengthFrequency + c2.columnLengthFrequency;
		return newColumn;
	}

};

const wstring Column::defaultColumnName = L"C";
