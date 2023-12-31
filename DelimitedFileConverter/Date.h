#pragma once

#ifndef H_STRING
#define H_STRING
#include <string>
#include <cstring>
#endif // !H_STRING

#ifndef H_ARR
#define H_ARR
#include "Array.h"
#endif // !H_ARR

#ifndef H_CTM
#define H_CTM
#include <ctime>
#include <chrono>
#endif // !H_CTM

#ifndef H_STRU
#define H_STRU
#include "StringUtility.h"
#endif // !H_CLM

using namespace std;

/*
YY	or	YYYY
m	or	MM
D	or	DD
H	or  HH
M	or	MM
S	or  SS

Minimum possible acceptable string YY/m/d 
Maximun possible acceptable string YYYY/mm/DD HH:MM:SS.XX....XX pm

*/

enum class DateFormat {
	YYYYMMDD
	, YYYYDDMM
	, MMYYYYDD
	, MMDDYYYY
	, DDMMYYYY
	, DDYYYYMM
	, UNSURE
	, INVALID
};

/*******************************/
class Date {
private:
	DateFormat dateFormat;
	char day;
	char month;
	int year;
};

/*******************************/
class Time {
private:
	char hour;
	char minute;
	double second;
};

/*******************************/
class DateTime {
private:
	Date date;
	Time time;
};

/*******************************/
/*                             */
/*******************************/
class DateFormatProcessor {
private:

	/*******************************/
	/*                             */
	/*******************************/
	Array<DateFormat> possibleDateFormats;
	DateFormat dateFormat = DateFormat::UNSURE;
	bool hasTime = false;
	wstring delimiter;

	/*******************************/
	/*                             */
	/*******************************/
	static bool IsPositiveInt(const wstring &s) {
		if (s.size() == 0) { return false; }
		for (size_t i = 0; i < s.size(); i++) {
			if (!StringUtility::IsDigit(s[i])) {
				return false;
			}
		}
		return true;
	}

	/*******************************/
	/*                             */
	/*******************************/
	static bool IsPositiveDecimal(const wstring &s) {
		if (s.size() == 0) { return false; }
		bool foundDecimal = false;
		for (size_t i = 0; i < s.size(); i++) {
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
		
	/*******************************/
	/*                             */
	/*******************************/
	static bool IsPossibleDay(int d) { return (d > 0 && d < 32); }
	static bool IsPossibleMonth(int m) { return (m > 0 && m < 13); }
	static bool IsPossibleYear(int y) { return (y >= 0); }

	/*******************************/
	/*                             */
	/*******************************/
	static bool IsDate(int y, int m, int d) {
		if (!IsPossibleYear(y)) { return false; }
		if (!IsPossibleMonth(m)) { return false; }
		if (!IsPossibleDay(d)) { return false; }

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

	/*******************************/
	/*                             */
	/*******************************/
	static bool IsHour(int h) { return (h >= 0 && h <= 23); }
	static bool IsMinute(int m) { return (m >= 0 && m < 60); }
	static bool IsSecond(double s) { return (s >= 0.0 && s < 60.0); }

	/*******************************/
	/*                             */
	/*******************************/
	static bool IsTime(wstring t) {
		// HH:MM:SS.dddd...ddd PM/AM
		// HH:MM:SS.dddd...ddd
		size_t numSpaces = StringUtility::CountSubStringInString(t, L" ", L"");
		bool am_pm_included = false;
		bool pm = false;
		wstring timeString;
		if (numSpaces == 1) {
			Array<wstring> tp = StringUtility::SplitString(t, L" ", L"");
			wstring amPm = StringUtility::ToLowerCaseString(tp[1]);
			if (amPm == L"pm") {
				am_pm_included = true;
				pm = true;
			}
			else if (amPm == L"am") {
				am_pm_included = true;
			}
			else {
				return false;
			}
			timeString = tp[0];
		}
		else if (numSpaces > 1) {
			return false;
		}
		else {
			timeString = t;
		}

		Array<wstring> timeParts = StringUtility::SplitString(timeString, L":", L"");
		if (timeParts.GetSize() != 3) {
			return false;
		}

		int h = (IsPositiveInt(timeParts[0])) ? stoi(timeParts[0]) : -1;
		int m = (IsPositiveInt(timeParts[1])) ? stoi(timeParts[1]) : -1;
		double s = (IsPositiveDecimal(timeParts[2])) ? stod(timeParts[2]) : -1.0;

		if (h == -1 || m == -1 || s == -1.0) {
			return false;
		}

		if (am_pm_included && pm) {
			h += 12;
		}

		return (IsHour(h) && IsMinute(m) && IsSecond(s));
	}

	/*******************************/
	/*                             */
	/*******************************/
	static Array<DateFormat> GetPossibleDateFormats(wstring d, wstring& delimiter) {
		Array<DateFormat> df;
		// Count number of delimiters
		bool identifiedDelimiter = false;
		int nonDigitChars = 0;
		int digitChars = 0;
		// For each char in the string
		for (unsigned int i = 0; i < d.size(); i += 1) {
			if (StringUtility::IsDigit(d[i])) { // If this is a digit
				digitChars += 1;					// Count and continue
			}
			else {							// Else, not a digit
				if (!identifiedDelimiter) {		// If we have not found a delimiter yet
					identifiedDelimiter = true;		// Indicate that we have a delimiter
					delimiter = wstring(1,d[i]);			// Save it
					nonDigitChars += 1;				// Count and continue
				}
				else {							// We already found a delimiter earlier
					if (wstring(1, d[i]) != delimiter) {		// if they are not the same
						return df;						// Return false..?
					}
					else {							// They are the same
						nonDigitChars += 1;				// Count and continue
					}
				}
			}
		}
		// Need at least 4 to 8 digits (YYMD or YYYYMMDD) AND Need exactly 2 delimiters
		if (digitChars < 4 || digitChars > 8 || nonDigitChars != 2 || delimiter.size() != 1) {
			return df;
		}

		// Now get date parts
		Array<wstring> dateParts = StringUtility::SplitString(d, delimiter, L"");
		int datePart1 = (IsPositiveInt(dateParts[0])) ? stoi(dateParts[0]) : -1;
		int datePart2 = (IsPositiveInt(dateParts[1])) ? stoi(dateParts[1]) : -1;
		int datePart3 = (IsPositiveInt(dateParts[2])) ? stoi(dateParts[2]) : -1;
		// If failed return
		if (datePart1 == -1 || datePart2 == -1 || datePart3 == -1) {
			return df;
		}
		// Get possible date formats
		if (IsDate(datePart1, datePart2, datePart3)) { // YYYY MM DD
			df.Add(DateFormat::YYYYMMDD);
		}
		if (IsDate(datePart1, datePart3, datePart2)) { // YYYY DD MM
			df.Add(DateFormat::YYYYDDMM);
		}
		if (IsDate(datePart2, datePart1, datePart3)) { // MM YYYY DD
			df.Add(DateFormat::MMYYYYDD);
		}
		if (IsDate(datePart3, datePart1, datePart2)) { // MM DD YYYY
			df.Add(DateFormat::MMDDYYYY);
		}
		if (IsDate(datePart3, datePart2, datePart1)) { // DD MM YYYY
			df.Add(DateFormat::DDMMYYYY);
		}
		if (IsDate(datePart2, datePart3, datePart1)) { // DD YYYY MM
			df.Add(DateFormat::DDYYYYMM);
		}
		return df;
	}

	/*******************************/
	/*                             */
	/*******************************/
	void SplitDateParts(const wstring& s, int& y, int& m, int& d, DateFormat df) {
		Array<wstring> dateParts = StringUtility::SplitString(s, delimiter, L"");

		y = (df == DateFormat::YYYYDDMM || df == DateFormat::YYYYMMDD) ? stoi(dateParts[0]) :
			((df == DateFormat::DDMMYYYY || df == DateFormat::MMDDYYYY) ? stoi(dateParts[2]) : stoi(dateParts[1]));

		m = (df == DateFormat::MMDDYYYY || df == DateFormat::MMYYYYDD) ? stoi(dateParts[0]) :
			((df == DateFormat::DDYYYYMM || df == DateFormat::YYYYDDMM) ? stoi(dateParts[2]) : stoi(dateParts[1]));

		d = (df == DateFormat::DDMMYYYY || df == DateFormat::DDYYYYMM) ? stoi(dateParts[0]) :
			((df == DateFormat::YYYYMMDD || df == DateFormat::MMYYYYDD) ? stoi(dateParts[2]) : stoi(dateParts[1]));
	}


public:
	/*******************************/
	/*                             */
	/*******************************/
	void ProcessDate(wstring d) {
		// If already invalid, dont process
		if (dateFormat == DateFormat::INVALID) { return; }
		// If string does not meet minimum length, dont process.
		if (d.size() < 6) {
			dateFormat = DateFormat::INVALID;
			return; 
		}
		// Check for time
		wstring dateString = L"";
		wstring timeString = L"";
		size_t numSpaces = StringUtility::CountSubStringInString(d, L" ", L"");
		if (numSpaces == 1 || numSpaces == 2) { // If there might be a time here
			Array<wstring> dateParts = StringUtility::SplitString(d, L" ", L""); // split
			dateString = dateParts[0]; // grab the date part
			timeString = dateParts[1]; // grab the time part
			if (numSpaces == 2) {
				timeString += L" " + dateParts[2];
			}
			hasTime = true;
		}
		else {
			dateString = d;
		}
		// If time identified and it is not valid, dont process
		if (hasTime && !IsTime(timeString)) {
			hasTime = false;
			dateFormat = DateFormat::INVALID;
			return;
		}
		// Grab all possible formats for the date
		Array<DateFormat> df = GetPossibleDateFormats(dateString, delimiter);

		if (df.GetSize() > 0) {
			/* If date format already determined */
			if (dateFormat != DateFormat::UNSURE) {
				if (!df.exists(dateFormat)) { // if this format is not in the possible formats
					dateFormat = DateFormat::INVALID; // done, dont process
					return;
				}
				else {
					return; // We are done, no changes
				}
			}
			else { // dateFormat is still unsure
				if (possibleDateFormats.GetSize() > 0) { // If there are multiple possible formats
					for (unsigned int i = 0; i < df.GetSize(); i++) { // for each new format
						if (!possibleDateFormats.exists(df[i])) { // if the new format is not found in the old formats
							dateFormat = DateFormat::INVALID; // then this is not a possible date
							return;
						}
					}
					// if we have reached this point, then all new formats were found in the old formats
					possibleDateFormats = df;
				}
				else {
					possibleDateFormats = df;
				}
			}

			// If narrowed down to 1, we have the only possible date format.
			if (possibleDateFormats.GetSize() == 1) {
				dateFormat = possibleDateFormats[0];
			}
		}
		else {
			dateFormat = DateFormat::INVALID;
			return;
		}
	}

	/*******************************/
	/*                             */
	/*******************************/
	bool IsNotDate() const { return (dateFormat == DateFormat::INVALID); }

	/*******************************/
	/*                             */
	/*******************************/
	bool IsDate() const { 
		return (dateFormat != DateFormat::INVALID && dateFormat != DateFormat::UNSURE && !hasTime);
	}

	/*******************************/
	/*                             */
	/*******************************/
	bool IsDateTime() const {
		return (dateFormat != DateFormat::INVALID && dateFormat != DateFormat::UNSURE && hasTime);
	}

	/*******************************/
	/*                             */
	/*******************************/
	bool IsPossibleDate() const {
		return (dateFormat != DateFormat::INVALID && (!hasTime));
	}

	/*******************************/
	/*                             */
	/*******************************/
	bool IsPossibleDateTime() const {
		return (dateFormat != DateFormat::INVALID && hasTime);
	}

};
