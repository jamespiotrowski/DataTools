#pragma once

#ifndef H_ALG
#define H_ALG
#include <algorithm>
#endif

#ifndef H_ARR
#define H_ARR
#include "Array.h"
#endif // !H_ARR

#ifndef H_MSG
#define H_MSG
#include "Messaging.h"
#endif // !H_CLM

using namespace std;
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
	static wstring SubString(const wstring& str, const size_t& start, const size_t& len) {
		wstring s = L"";
		size_t nLen = (((size_t)start + (size_t)len) >= str.size()) ? str.size() - start : len;

		if (start >= str.size()) { return s; }

		for (size_t i = 0; i < nLen; i++) {
			s += str[(size_t)start + i];
		}

		return s;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static size_t SubStringStartIndex(const wstring& str, const wstring& subStr) {
		size_t i = 0;
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
	static bool StringContains(const wstring& str, const wstring& subStr) {
		size_t i = 0;
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
	static wstring StringReplace(const wstring& str, const wstring& subStr, const wstring& replaceStr) {
		if (subStr.length() == 0) { return str; }
		wstring s = L"";
		size_t curIndex = 0;
		while ((curIndex + subStr.length()) <= str.length()) {
			wstring curSubStr = SubString(str, curIndex, subStr.length());
			if (curSubStr == subStr) {
				s += replaceStr;
				curIndex += (subStr.length());
			}
			else {
				s += curSubStr[0];
				curIndex += 1;
			}
		}
		while (curIndex < str.length()) {
			s += str[curIndex++];
		}
		return s;
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static size_t CountSubStringInString(const wstring& str, const wstring& subStr, const wstring& qualifier, bool inQualifiedField) {
		bool justActivatedQualifiedField = false;
		size_t i = 0;
		size_t shifter = 1;
		size_t frequency = 0;
		wstring s1, s2;
		while (i < str.size()) {
			justActivatedQualifiedField = false;
			shifter = 1;
			/* Handle Qualifer stuff */
			if (qualifier != L"") {
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
					s1 = L"";
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
			if (s1 != L"" && !inQualifiedField && !justActivatedQualifiedField) {
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
	static size_t CountSubStringInString(const wstring& str, const wstring& subStr, const wstring& qualifier) {
		return CountSubStringInString(str, subStr, qualifier, false);
	}

	/****************************************************************
	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-

	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-	-
	****************************************************************/
	static Array<wstring> SplitString(const wstring& str, const wstring& delimiter, const wstring& qualifier) {
		Array<wstring> strings;
		if (delimiter == qualifier) {
			PrintMessage(L"StringUtility::SplitString", MessageType::ERROR, L"Delimiter and qualifer are the same string. Data impossible to interpet.");
			return strings;
		}

		bool inQualifiedField = false;
		bool justActivatedQualifiedField = false;
		bool foundQualifier = false;
		wstring currentString = L"";
		wstring s1, s2;
		size_t i = 0;
		size_t shifter = 1;
		size_t currentStart = 0;
		while (i < str.size()) {
			foundQualifier = false;
			shifter = 1;
			/* Handle Qualifer stuff */
			if (qualifier != L"") {
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
					currentString = L"";
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
	static bool StringEndsWithinQualifiedField(const wstring& str, const wstring& qualifier) {
		return (CountSubStringInString(str, qualifier, L"") % 2) == 1;
	}

	static bool IsWhiteSpace(const wchar_t& c) {
		return (c == L' ') || (c == L'\r') || (c == L'\n') || (c == L'\t');
	}

	static wstring TrimLeftWhiteSpace(const wstring& s) {
		size_t startIndex = 0;
		for (size_t i = 0; i < s.size(); i++) {
			if (!IsWhiteSpace(s[i])) {
				startIndex = i;
				break;
			}
		}
		return SubString(s, startIndex, s.size() - startIndex);
	}

	static wstring TrimRightWhiteSpace(const wstring& s) {
		if (s.size() == 0) {
			return s;
		}

		size_t newLength = s.size();
		for (size_t i = s.size(); i > 0; i--) {
			if (!IsWhiteSpace(s[i - 1])) {
				newLength = i;
				break;
			}
		}
		return SubString(s, 0, newLength);
	}

	static wstring TrimWhiteSpace(const wstring& s) {
		return TrimLeftWhiteSpace(TrimRightWhiteSpace(s));
	}

	static bool IsUpperCaseLetter(const wchar_t& c) { return (c >= 65 && c <= 90); }
	static bool IsLowerCaseLetter(const wchar_t& c) { return (c >= 97 && c <= 122); }
	static bool IsLetter(const wchar_t& c) { return IsUpperCaseLetter(c) || IsLowerCaseLetter(c); }
	static bool IsDigit(const wchar_t& c) { return (c >= 48 && c <= 57); }
	static bool IsAcceptableColumnCharacter(const wchar_t& c) { return (IsLetter(c) || IsDigit(c)); }
	static wchar_t ToUpperCaseLetter(const wchar_t& c) { return IsLowerCaseLetter(c) ? (c - 32) : c; }
	static wchar_t ToLowerCaseLetter(const wchar_t& c) { return IsUpperCaseLetter(c) ? (c + 32) : c; }

	static wstring ToLowerCaseString(const wstring& s) {
		wstring newS = s;
		transform(newS.begin(), newS.end(), newS.begin(), ::tolower);
		return newS;
	}

	static bool ContainsUnicodeChars(const wstring& str) {
		for (size_t i = 0; i < str.size(); i++) {
			if (((unsigned int)str[i]) > 127) {
				return true;
			}
		}
		return false;
	}

	static bool ConvertStringToBool(wstring b) {
		b = ToLowerCaseString(b);
		if (b == L"t" || b == L"true" || b == L"1") {
			return true;
		}
		return false;
	}

	static string WStringToString(const wstring& wstr) {
		string str;
		transform(wstr.begin(), wstr.end(), std::back_inserter(str), [](wchar_t c) {
			return (char)c;
		});
		return str;
	}

};
