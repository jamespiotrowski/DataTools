#pragma once

#ifndef H_ARR
#include "Array.h"
#endif // !H_ARR

#ifndef H_MSG
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
