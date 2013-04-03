/****************************************************************************\
*                                                                            *
*  IFC (Iris Foundation Classes) Project                                     *
*  http://github.com/haoxingeng/ifc                                          *
*                                                                            *
*  Copyright 2008 HaoXinGeng (haoxingeng@gmail.com)                          *
*  All rights reserved.                                                      *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
\****************************************************************************/

/// @file ifc_sysutils.cpp

#include "stdafx.h"
#include <windows.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <shellapi.h>

#include "ifc_sysutils.h"

#pragma comment (lib, "version.lib")

namespace ifc
{

using namespace std;

///////////////////////////////////////////////////////////////////////////////

bool IsIntStr(LPCTSTR str)
{
	bool bResult;
	int nLen = (int)_tcsclen(str);
	TCHAR *pStr = (TCHAR*)str;

	bResult = (nLen > 0) && !_istspace(pStr[0]);

	if (bResult)
	{
		TCHAR *endp;
		_tcstol(pStr, &endp, 10);
		bResult = (endp - pStr == nLen);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool IsInt64Str(LPCTSTR str)
{
	bool bResult;
	int nLen = (int)_tcsclen(str);
	TCHAR *pStr = (TCHAR*)str;

	bResult = (nLen > 0) && !_istspace(pStr[0]);

	if (bResult)
	{
		TCHAR *endp;
		_tcstoi64(pStr, &endp, 10);
		bResult = (endp - pStr == nLen);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool IsFloatStr(LPCTSTR str)
{
	bool bResult;
	int nLen = (int)_tcsclen(str);
	TCHAR *pStr = (TCHAR*)str;

	bResult = (nLen > 0) && !_istspace(pStr[0]);

	if (bResult)
	{
		TCHAR *endp;
		_tcstod(pStr, &endp);
		bResult = (endp - pStr == nLen);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool IsBoolStr(LPCTSTR str)
{
	return SameText(str, TRUE_STR) || SameText(str, FALSE_STR) || IsFloatStr(str);
}

//-----------------------------------------------------------------------------

int StrToInt(LPCTSTR str, int nDefault)
{
	if (IsIntStr(str))
		return _tcstol(str, NULL, 10);
	else
		return nDefault;
}

//-----------------------------------------------------------------------------

INT64 StrToInt64(LPCTSTR str, const INT64& nDefault)
{
	if (IsInt64Str(str))
		return _tcstoi64(str, NULL, 10);
	else
		return nDefault;
}

//-----------------------------------------------------------------------------

CString IntToStr(int nValue)
{
	CString strResult;
	strResult.Format(TEXT("%d"), nValue);
	return strResult;
}

//-----------------------------------------------------------------------------

CString IntToStr(const INT64& nValue)
{
	CString strResult;
	strResult.Format(TEXT("%I64d"), nValue);
	return strResult;
}

//-----------------------------------------------------------------------------

double StrToFloat(LPCTSTR str, double fDefault)
{
	if (IsFloatStr(str))
		return _tcstod(str, NULL);
	else
		return fDefault;
}

//-----------------------------------------------------------------------------

CString FloatToStr(double fValue, LPCTSTR lpszFormat)
{
	CString strResult;
	strResult.Format(lpszFormat, fValue);
	return strResult;
}

//-----------------------------------------------------------------------------

bool StrToBool(LPCTSTR str, bool bDefault)
{
	if (IsBoolStr(str))
	{
		if (SameText(str, TRUE_STR))
			return true;
		else if (SameText(str, FALSE_STR))
			return false;
		else
			return (StrToFloat(str, 0) != 0);
	}
	else
		return bDefault;
}

//-----------------------------------------------------------------------------

CString BoolToStr(bool bValue, bool bUseBoolStrs)
{
	if (bUseBoolStrs)
		return (bValue? TRUE_STR : FALSE_STR);
	else
		return (bValue? TEXT("1") : TEXT("0"));
}

//-----------------------------------------------------------------------------

bool SameText(LPCTSTR str1, LPCTSTR str2)
{
	return _tcsicmp(str1, str2) == 0;
}

//-----------------------------------------------------------------------------

CString StringOfChar(TCHAR ch, int nCount)
{
	CString str;
	if (nCount < 0) nCount = 0;
	LPTSTR p = str.GetBuffer(nCount);
	for (int i = 0; i < nCount; i++)
		*p++ = ch;
	str.ReleaseBufferSetLength(nCount);
	return str;
}

//-----------------------------------------------------------------------------

CString EnsureEndWith(LPCTSTR lpszSource, TCHAR ch)
{
	CString strResult(lpszSource);
	if (strResult.IsEmpty() || strResult[strResult.GetLength()-1] != ch)
		strResult += ch;
	return strResult;
}

//-----------------------------------------------------------------------------

bool SameText(const string& str1, const string& str2)
{
	return _stricmp(str1.c_str(), str2.c_str()) == 0;
}

//-----------------------------------------------------------------------------

string TrimString(const string& str)
{
	string strResult;
	int i, nLen;

	nLen = (int)str.size();
	i = 0;
	while (i < nLen && (BYTE)str[i] <= 32) i++;
	if (i < nLen)
	{
		while ((BYTE)str[nLen-1] <= 32) nLen--;
		strResult = str.substr(i, nLen - i);
	}

	return strResult;
}

//-----------------------------------------------------------------------------

string UpperCase(const string& str)
{
	string strResult = str;
	int nLen = (int)strResult.size();
	char c;

	for (int i = 0; i < nLen; i++)
	{
		c = strResult[i];
		if (c >= 'a' && c <= 'z')
			strResult[i] = c - 32;
	}

	return strResult;
}

//-----------------------------------------------------------------------------

string LowerCase(const string& str)
{
	string strResult = str;
	int nLen = (int)strResult.size();
	char c;

	for (int i = 0; i < nLen; i++)
	{
		c = strResult[i];
		if (c >= 'A' && c <= 'Z')
			strResult[i] = c + 32;
	}

	return strResult;
}

//-----------------------------------------------------------------------------

CString PathWithSlash(LPCTSTR lpszPath)
{
	CString strResult(lpszPath);
	int nLen = strResult.GetLength();
	if (nLen > 0 && strResult[nLen-1] != PATH_DELIM)
		strResult += PATH_DELIM;
	return strResult;
}

//-----------------------------------------------------------------------------

CString PathWithoutSlash(LPCTSTR lpszPath)
{
	CString strResult(lpszPath);
	int nLen = strResult.GetLength();
	if (nLen > 0 && strResult[nLen-1] == PATH_DELIM)
		strResult.Delete(nLen-1, 1);
	return strResult;
}

//-----------------------------------------------------------------------------

CString CombineFileName(LPCTSTR lpszPath, LPCTSTR lpszFileName)
{
	return PathWithSlash(lpszPath) + lpszFileName;
}

//-----------------------------------------------------------------------------

CString FileExtWithDot(LPCTSTR lpszFileExt)
{
	CString strResult(lpszFileExt);
	if (!strResult.IsEmpty())
		if (strResult[0] != TEXT('.'))
			strResult.Insert(0, TEXT("."));
	return strResult;
}

//-----------------------------------------------------------------------------

CString FileExtWithoutDot(LPCTSTR lpszFileExt)
{
	CString strResult(lpszFileExt);
	if (!strResult.IsEmpty())
		if (strResult[0] == TEXT('.'))
			strResult.Delete(0);
	return strResult;
}

//-----------------------------------------------------------------------------

CString AddThousandSep(const INT64& nNumber)
{
	CString strResult = IntToStr(nNumber);
	for (int i = strResult.GetLength() - 3; i > 0; i -= 3)
		strResult.Insert(i, TEXT(","));
	return strResult;
}

//-----------------------------------------------------------------------------
// Returns the 0-based position of the last delimiter of the file name.
// Returns -1 if the delimiter is not found.
//-----------------------------------------------------------------------------
int GetLastDelimPos(const CString& strFileName, const CString& strDelims)
{
	int nResult = strFileName.GetLength() - 1;

	for (; nResult >= 0; nResult--)
		if (strDelims.Find(strFileName[nResult], 0) >= 0)
			break;

	return nResult;
}

//-----------------------------------------------------------------------------

CString ExtractFileDrive(LPCTSTR lpszFileName)
{
	CString strResult;
	CString strFileName(lpszFileName);
	int i, j;

	if (strFileName.GetLength() >= 2 && strFileName[1] == DRIVE_DELIM)
		strResult = strFileName.Left(2);
	else if (strFileName.GetLength() >= 2 &&
		strFileName[0] == PATH_DELIM && strFileName[1] == PATH_DELIM)
	{
		j = 0;
		i = 2;
		while (i < strFileName.GetLength() && j < 2)
		{
			if (strFileName[i] == PATH_DELIM) j++;
			if (j < 2) i++;
		}
		if (strFileName[i] == PATH_DELIM) i--;
		strResult = strFileName.Left(i + 1);
	}

	return strResult;
}

//-----------------------------------------------------------------------------

CString ExtractFilePath(LPCTSTR lpszFileName)
{
	CString strFileName(lpszFileName);
	int nPos = GetLastDelimPos(strFileName, CString(PATH_DELIM) + DRIVE_DELIM);
	return PathWithSlash(strFileName.Left(nPos + 1));
}

//-----------------------------------------------------------------------------

CString ExtractFileName(LPCTSTR lpszFileName)
{
	CString strFileName(lpszFileName);
	int nPos = GetLastDelimPos(strFileName, CString(PATH_DELIM) + DRIVE_DELIM);
	return strFileName.Right(strFileName.GetLength() - nPos - 1);
}

//-----------------------------------------------------------------------------

CString ExtractFileExt(LPCTSTR lpszFileName)
{
	CString strFileName(lpszFileName);
	int nPos = GetLastDelimPos(strFileName, CString(PATH_DELIM) + DRIVE_DELIM + TEXT("."));
	if (nPos >= 0 && strFileName[nPos] == TEXT('.'))
		return strFileName.Right(strFileName.GetLength() - nPos);
	else
		return TEXT("");
}

//-----------------------------------------------------------------------------

CString ExtractFileMainName(LPCTSTR lpszFileName)
{
	CString strExt = ExtractFileExt(lpszFileName);
	CString strName = ExtractFileName(lpszFileName);
	return strName.Left(strName.GetLength() - strExt.GetLength());
}

//-----------------------------------------------------------------------------

CString ExtractRelativePath(LPCTSTR lpszBaseName, LPCTSTR lpszDestName)
{
	struct {
		CString operator() (LPCTSTR lpszFileName)
		{
			CString strResult = ExtractFilePath(lpszFileName);
			int nDriveLen = ExtractFileDrive(lpszFileName).GetLength();
			strResult.Delete(0, nDriveLen);
			return strResult;
		}
	} ExtractFilePathNoDrive;

	struct {
		LPTSTR operator() (LPTSTR& pLead)
		{
			LPTSTR pResult = pLead;
			if (pResult == NULL) return pResult;

			pLead = _tcschr(pLead, PATH_DELIM);
			if (pLead != NULL)
			{
				*pLead = 0;
				pLead++;
			}

			return pResult;
		}
	} Next;

	CString strResult;
	CString strBasePath, strDestPath;
	LPTSTR pBaseLead, pDestLead;
	LPTSTR pBasePtr, pDestPtr;

	if (SameText(ExtractFileDrive(lpszBaseName), ExtractFileDrive(lpszDestName)))
	{
		strBasePath = ExtractFilePathNoDrive(lpszBaseName);
		strDestPath = ExtractFilePathNoDrive(lpszDestName);

		pBaseLead = strBasePath.GetBuffer();
		pBasePtr = Next(pBaseLead);
		pDestLead = strDestPath.GetBuffer();
		pDestPtr = Next(pDestLead);

		while (pBasePtr != NULL && pDestPtr != NULL && SameText(pBasePtr, pDestPtr))
		{
			pBasePtr = Next(pBaseLead);
			pDestPtr = Next(pDestLead);
		}

		strResult.Empty();
		while (pBaseLead != NULL)
		{
			strResult += CString(TEXT("..")) + PATH_DELIM;
			Next(pBaseLead);
		}

		if (pDestPtr != NULL && *pDestPtr != 0)
			strResult = strResult + pDestPtr + PATH_DELIM;
		if (pDestLead != NULL)
			strResult += pDestLead;

		strResult += ExtractFileName(lpszDestName);

		strBasePath.ReleaseBuffer();
		strDestPath.ReleaseBuffer();
	}
	else
		strResult = lpszDestName;

	return strResult;
}

//-----------------------------------------------------------------------------

CString ExtractUrlFilePath(LPCTSTR lpszUrl)
{
	CString strUrl(lpszUrl);
	int nPos = GetLastDelimPos(strUrl, TEXT("/\\:"));
	return strUrl.Left(nPos + 1);
}

//-----------------------------------------------------------------------------

CString ExtractUrlFileName(LPCTSTR lpszUrl)
{
	CString strUrl(lpszUrl);
	int nPos = GetLastDelimPos(strUrl, TEXT("/\\:"));
	return strUrl.Right(strUrl.GetLength() - nPos - 1);
}

//-----------------------------------------------------------------------------

CString ChangeFileExt(LPCTSTR lpszFileName, LPCTSTR lpszExt)
{
	CString strResult;
	CString strFileName(lpszFileName);

	int nPos = GetLastDelimPos(strFileName, CString(PATH_DELIM) + DRIVE_DELIM + TEXT("."));
	if (nPos < 0 || strFileName[nPos] != TEXT('.'))
		strResult = strFileName;
	else
		strResult = strFileName.Left(nPos);

	strResult += lpszExt;
	return strResult;
}

//-----------------------------------------------------------------------------

CString ValidateFileName(LPCTSTR lpszFileName)
{
	CString strInvalid(TEXT("\\/:*?\"<>|"));
	CString strFileName(lpszFileName);
	CString strResult;

	for (int i = 0; i < strFileName.GetLength(); i++)
		if (strInvalid.Find(strFileName[i]) < 0 && strFileName[i] >= 32)
			strResult += strFileName[i];

	return strResult;
}

//-----------------------------------------------------------------------------

CString GetSizeString(const INT64& nBytes, LPCTSTR lpszSuffix)
{
	INT64 nKBytes = 0;

	if (nBytes > 0)
	{
		nKBytes = nBytes / 1024;
		if ((nBytes % 1024) != 0) nKBytes++;
	}

	return AddThousandSep(nKBytes) + lpszSuffix;
}

//-----------------------------------------------------------------------------

CString GetPercentString(const INT64& nPos, const INT64& nMax, LPCTSTR lpszSuffix)
{
	CString strResult;

	if (nMax > 0)
		strResult = IntToStr((int)(((double)nPos / nMax) * 100));
	else
		strResult = TEXT("100");

	strResult += lpszSuffix;
	return strResult;
}

//-----------------------------------------------------------------------------

CString RestrictStrWidth(LPCTSTR str, HDC hdc, int nWidth, LPCTSTR lpszSuffix)
{
	CStringW strResult(str);
	CStringW strSrc(str);

	while (TextWidth(hdc, CString(strResult)) > nWidth && !strResult.IsEmpty())
	{
		if (strSrc.GetLength() > 1)
		{
			strSrc.Delete(strSrc.GetLength() - 1, 1);
			strResult = strSrc + CStringW(lpszSuffix);
		}
		else
			strResult.Delete(strResult.GetLength() - 1, 1);
	}

	return CString(strResult);
}

//-----------------------------------------------------------------------------

CString AbbreviateFileName(LPCTSTR lpszFileName, int nMaxChars)
{
	CString strResult(lpszFileName);
	CString strBaseName;
	int nTotalLen, nBaseNameLen, nVolNameLen;
	const TCHAR* lpszCur;
	const TCHAR* lpszBase;

	lpszBase = lpszFileName;
	nTotalLen = lstrlen(lpszFileName);

	strBaseName = ExtractFileName(lpszFileName);
	nBaseNameLen = strBaseName.GetLength();

	// If nMaxChars is more than enough to hold the full path name, we're done.
	// This is probably a pretty common case, so we'll put it first.
	if (nMaxChars >= nTotalLen)
		return strResult;

	// If nMaxChars isn't enough to hold at least the basename, we're done
	if (nMaxChars < nBaseNameLen)
		return strBaseName;

	// Calculate the length of the volume name.  Normally, this is two characters
	// (e.g., "C:", "D:", etc.), but for a UNC name, it could be more (e.g.,
	// "\\server\share").
	//
	// If nMaxChars isn't enough to hold at least <volume_name>\...\<base_name>, the
	// result is the base filename.

	lpszCur = lpszBase + 2;  // Skip "C:" or leading "\\"

	if (lpszBase[0] == '\\' && lpszBase[1] == '\\') // UNC pathname
	{
		// First skip to the '\' between the server name and the share name,
		while (*lpszCur != '\\')
		{
			lpszCur = _tcsinc(lpszCur);
			IFC_ASSERT(*lpszCur != '\0');
		}
	}
	// if a UNC get the share name, if a drive get at least one directory
	IFC_ASSERT(*lpszCur == '\\');

	/*
	// make sure there is another directory, not just c:\filename.ext
	if (nTotalLen - nBaseNameLen > 3)
	{
		lpszCur = _tcsinc(lpszCur);
		while (*lpszCur != '\\')
		{
			lpszCur = _tcsinc(lpszCur);
			IFC_ASSERT(*lpszCur != '\0');
		}
	}
	IFC_ASSERT(*lpszCur == '\\');
	*/

	nVolNameLen = int(lpszCur - lpszBase);
	if (nMaxChars < nVolNameLen + 5 + nBaseNameLen)
		return strBaseName;

	// Now loop through the remaining directory components until something
	// of the form <volume_name>\...\<one_or_more_dirs>\<base_name> fits.
	//
	// Assert that the whole filename doesn't fit -- this should have been
	// handled earlier.

	IFC_ASSERT(nVolNameLen + (int)lstrlen(lpszCur) > nMaxChars);
	while (nVolNameLen + 4 + (int)lstrlen(lpszCur) > nMaxChars)
	{
		do
		{
			lpszCur = _tcsinc(lpszCur);
			IFC_ASSERT(*lpszCur != '\0');
		}
		while (*lpszCur != '\\');
	}

	// Form the resultant string and we're done.
	int cch;
	if (nVolNameLen >= 0 && nVolNameLen < nMaxChars)
		cch = nVolNameLen;
	else cch = nMaxChars;

	strResult = strResult.Left(cch) + TEXT("\\...");
	strResult += lpszCur;

	return strResult;
}

//-----------------------------------------------------------------------------

int TextWidth(HDC hdc, const CString& str)
{
	CSize size;
	::GetTextExtentPoint32(hdc, str, str.GetLength(), &size);
	return size.cx;
}

//-----------------------------------------------------------------------------

int TextHeight(HDC hdc, const CString& str)
{
	CSize size;
	::GetTextExtentPoint32(hdc, str, str.GetLength(), &size);
	return size.cy;
}

//-----------------------------------------------------------------------------

bool LikeString(LPCTSTR lpszValue, LPCTSTR lpszPattern, bool bCaseInsensitive)
{
	const WCHAR MULTI_WILD_CHAR = L'*';
	const WCHAR SINGLE_WILD_CHAR = L'?';

	CStringW strValue(lpszValue);
	CStringW strPattern(lpszPattern);
	LPCWSTR pValue = strValue;
	LPCWSTR pPattern = strPattern;

	while (true)
	{
		if (::CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE | SORT_STRINGSORT,
			pPattern, (int)wcslen(pPattern), CStringW(MULTI_WILD_CHAR), 1) - 2 == 0)
			return true;
		else if (*pValue == '\0' && *pPattern != '\0')
			return false;
		else if (*pValue == '\0')
			return true;
		else
		{
			switch (*pPattern)
			{
			case MULTI_WILD_CHAR:
				{
					int nLen = (int)wcslen(pValue);
					for (int i = 0; i < nLen; i++)
					{
						if (LikeString(CW2T(pValue + i), CW2T(pPattern + 1), bCaseInsensitive))
							return true;
					}
					return false;
				}

			case SINGLE_WILD_CHAR:
				pValue++;
				pPattern++;
				break;

			default:
				{
					bool bFlag = false;
					if (bCaseInsensitive)
					{
						if (::CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE | SORT_STRINGSORT,
							pPattern, 1, pValue, 1) - 2 == 0)
							bFlag = true;
					}
					else
					{
						if (::CompareStringW(LOCALE_USER_DEFAULT, SORT_STRINGSORT,
							pPattern, 1, pValue, 1) - 2 == 0)
							bFlag = true;
					}

					if (bFlag)
					{
						pValue++;
						pPattern++;
					}
					else
						return false;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------

void SplitString(LPCTSTR lpszSource, TCHAR chDelimiter, CStrList& List, bool bTrimResult)
{
	CString strSource(lpszSource);
	int nIndex = 0;
	int nOffset;

	List.Clear();
	if (strSource.IsEmpty()) return;

	while (true)
	{
		nOffset = strSource.Find(chDelimiter, nIndex);
		if (nOffset < 0)   // if not found
		{
			List.Add(strSource.Mid(nIndex));
			break;
		}
		else
		{
			List.Add(strSource.Mid(nIndex, nOffset - nIndex));
			nIndex = nOffset + 1;
		}
	}

	if (bTrimResult)
	{
		for (int i = 0; i < List.GetCount(); i++)
		{
			CString str = List[i];
			str.Trim();
			List.SetString(i, str);
		}
	}
}

//-----------------------------------------------------------------------------

CString FormatString(LPCTSTR lpszFormat, ...)
{
	CString strResult;

	va_list argList;
	va_start(argList, lpszFormat);
	strResult.FormatV(lpszFormat, argList);
	va_end(argList);

	return strResult;
}

//-----------------------------------------------------------------------------

CString GetQuotedStr(LPCTSTR lpszStr, TCHAR chQuote)
{
	CString strResult;
	CString strSrc(lpszStr);

	strResult.Preallocate(strSrc.GetLength());
	strResult = chQuote;

	int nStart = 0, nPos;
	while (true)
	{
		nPos = strSrc.Find(chQuote, nStart);
		if (nPos >= 0)
		{
			strResult += strSrc.Mid(nStart, nPos - nStart) + chQuote + chQuote;
			nStart = nPos + 1;
		}
		else
		{
			strResult += strSrc.Mid(nStart);
			break;
		}
	}

	strResult += chQuote;

	return strResult;
}

//-----------------------------------------------------------------------------

CString ExtractQuotedStr(LPCTSTR& lpszStr, TCHAR chQuote)
{
	CString strResult;
	LPCTSTR lpszStart = lpszStr;

	if (lpszStr == NULL || *lpszStr != chQuote)
		return TEXT("");

	// Calc the character count after converting.

	int nSize = 0;
	lpszStr = _tcsinc(lpszStr);
	while (*lpszStr != '\0')
	{
		if (lpszStr[0] == chQuote)
		{
			if (lpszStr[1] == chQuote)
			{
				nSize++;
				lpszStr += 2;
			}
			else
			{
				lpszStr++;
				break;
			}
		}
		else
		{
			LPCTSTR p = lpszStr;
			lpszStr = _tcsinc(lpszStr);
			nSize += (int)(lpszStr - p);
		}
	}

	// Begin to retrieve the characters.

	LPTSTR pResult = strResult.GetBufferSetLength(nSize);
	lpszStr = lpszStart;
	lpszStr = _tcsinc(lpszStr);
	while (*lpszStr != '\0')
	{
		if (lpszStr[0] == chQuote)
		{
			if (lpszStr[1] == chQuote)
			{
				*pResult++ = *lpszStr;
				lpszStr += 2;
			}
			else
			{
				lpszStr++;
				break;
			}
		}
		else
		{
			LPCTSTR p = lpszStr;
			lpszStr = _tcsinc(lpszStr);
			while (p < lpszStr)
				*pResult++ = *p++;
		}
	}

	return strResult;
}

//-----------------------------------------------------------------------------

CString GetDequotedStr(LPCTSTR lpszStr, TCHAR chQuote)
{
	LPCTSTR lpszStart = lpszStr;
	int nStrLen = (int)_tcsclen(lpszStr);

	CString strResult = ExtractQuotedStr(lpszStr, chQuote);

	if ( (strResult.IsEmpty() || *lpszStr == '\0') &&
		nStrLen > 0 && (lpszStart[0] != chQuote || lpszStart[nStrLen-1] != chQuote) )
		strResult = lpszStart;

	return strResult;
}

//-----------------------------------------------------------------------------

CString FetchStr(CString& strInput, TCHAR chDelimiter, bool bDelete)
{
	CString strResult;

	int nPos = strInput.Find(chDelimiter, 0);
	if (nPos < 0)
	{
		strResult = strInput;
		if (bDelete)
			strInput.Empty();
	}
	else
	{
		strResult = strInput.Mid(0, nPos);
		if (bDelete)
			strInput = strInput.Mid(nPos + 1);
	}

	return strResult;
}

//-----------------------------------------------------------------------------

void GetWindowsVer(OSVERSIONINFO& osvi)
{
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);
}

//-----------------------------------------------------------------------------

OSVERSIONINFO GetWindowsVer()
{
	OSVERSIONINFO osvi;
	GetWindowsVer(osvi);
	return osvi;
}

//-----------------------------------------------------------------------------

bool CheckWindowsVer(DWORD dwMajorVersion, DWORD dwMinorVersion)
{
	OSVERSIONINFO osvi;
	GetWindowsVer(osvi);
	return osvi.dwMajorVersion > dwMajorVersion ||
		(osvi.dwMajorVersion == dwMajorVersion && osvi.dwMinorVersion >= dwMinorVersion);
}

//-----------------------------------------------------------------------------

CString GetComputerName()
{
	const int MAX_SIZE = 256;
	TCHAR szBuffer[MAX_SIZE];
	DWORD nSize = MAX_SIZE;

	::GetComputerName(szBuffer, &nSize);

	return szBuffer;
}

//-----------------------------------------------------------------------------

CString GetWinUserName()
{
	const int MAX_SIZE = 256;
	TCHAR szBuffer[MAX_SIZE];
	DWORD nSize = MAX_SIZE;

	::GetUserName(szBuffer, &nSize);

	return szBuffer;
}

//-----------------------------------------------------------------------------

CString GetWindowsDir()
{
	TCHAR szBuffer[IFC_MAX_PATH];
	::GetWindowsDirectory(szBuffer, IFC_MAX_PATH);
	return PathWithSlash(szBuffer);
}

//-----------------------------------------------------------------------------

CString GetWinTempDir()
{
	const int MAX_SIZE = 256;
	TCHAR szBuffer[MAX_SIZE];
	CString strLongName;
	CString strResult;

	::GetTempPath(MAX_SIZE, szBuffer);
	strResult = PathWithSlash(szBuffer);
	strLongName = GetLongFileName(strResult);
	if (strLongName.GetLength() >= strResult.GetLength())
		strResult = strLongName;

	return strResult;
}

//-----------------------------------------------------------------------------

CString GetWinTempFile(LPCTSTR lpszPrefix)
{
	return GetTempFileName(GetWinTempDir(), lpszPrefix);
}

//-----------------------------------------------------------------------------

CString GetTempFileName(LPCTSTR lpszPath, LPCTSTR lpszPrefix)
{
	TCHAR szFileName[IFC_MAX_PATH];
	CString strPath(PathWithSlash(lpszPath));
	CString strLongName;
	CString strResult;

	ForceDirectories(strPath);
	::GetTempFileName(strPath, lpszPrefix, 0, szFileName);
	strResult = szFileName;
	strLongName = GetLongFileName(strResult);
	if (strLongName.GetLength() >= strResult.GetLength())
		strResult = strLongName;

	return strResult;
}

//-----------------------------------------------------------------------------

CString GetSpecialFolder(int nFolderId)
{
	CString strResult;
	ITEMIDLIST *pItemIdList;
	TCHAR szBuffer[IFC_MAX_PATH];
	IMalloc *pMalloc = NULL;

	if (SUCCEEDED(::SHGetSpecialFolderLocation(0, nFolderId, &pItemIdList)))
	{
		if (::SHGetPathFromIDList(pItemIdList, szBuffer))
			strResult = szBuffer;

		::SHGetMalloc(&pMalloc);
		if (pMalloc != NULL)
		{
			pMalloc->Free(pItemIdList);
			pMalloc->Release();
		}

		strResult = PathWithSlash(strResult);
	}

	return strResult;
}

//-----------------------------------------------------------------------------

CString GetQuickLaunchPath()
{
	return GetSpecialFolder(CSIDL_APPDATA) +
		TEXT("Microsoft\\Internet Explorer\\Quick Launch\\");
}

//-----------------------------------------------------------------------------

CString GetEnvVar(LPCTSTR lpszEnvVar)
{
	CString strResult;
	strResult.GetEnvironmentVariable(lpszEnvVar);
	return strResult;
}

//-----------------------------------------------------------------------------

CRect GetScreenWorkAreaRect()
{
	CRect r;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
	return r;
}

//-----------------------------------------------------------------------------
// The callback function for SelectFolderDlg()
//-----------------------------------------------------------------------------
static int CALLBACK SHBrowseForFolderCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);

	return  0;
}

//-----------------------------------------------------------------------------

bool SelectFolderDlg(HWND hwndOwner, LPCTSTR lpszPrompt, bool bAllowNewFolder, CString& strPath)
{
	bool bResult = false;
	TCHAR szPath[IFC_MAX_PATH];
	LPMALLOC lpMalloc;
	BROWSEINFO sInfo;
	LPITEMIDLIST lpidlRoot;
	LPITEMIDLIST lpidlBrowse;

	if (::SHGetMalloc(&lpMalloc) != NOERROR)
		return bResult;

	strPath = PathWithoutSlash(strPath);

	::SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &lpidlRoot);

	memset(&sInfo, 0, sizeof(BROWSEINFO));
	sInfo.hwndOwner = hwndOwner;
	sInfo.pidlRoot = lpidlRoot;
	sInfo.pszDisplayName = szPath;
	sInfo.lpszTitle = lpszPrompt;
	sInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	if (bAllowNewFolder)
		sInfo.ulFlags |= BIF_EDITBOX | BIF_NEWDIALOGSTYLE | BIF_USENEWUI;
	sInfo.lpfn = SHBrowseForFolderCallbackProc;
	sInfo.lParam = (LPARAM)strPath.GetBuffer(0);

	// Display the dialog.
	lpidlBrowse = ::SHBrowseForFolder(&sInfo);

	if (lpidlBrowse != NULL)
	{
		// Get the selected folder
		if (::SHGetPathFromIDList(lpidlBrowse, szPath))
		{
			strPath = szPath;
			strPath = PathWithSlash(strPath);
		}

		bResult = true;
	}

	if (lpidlBrowse != NULL)
		::CoTaskMemFree(lpidlBrowse);
	if (lpidlRoot != NULL)
		::CoTaskMemFree(lpidlRoot);

	lpMalloc->Release();

	return  bResult;
}

//-----------------------------------------------------------------------------

bool ExecuteFile(LPCTSTR lpszFileName, LPCTSTR lpszParams, LPCTSTR lpszDefaultDir,
	int nShowCmd, HINSTANCE *pHandle)
{
	HINSTANCE hHandle = ::ShellExecute(0, NULL, lpszFileName, lpszParams, lpszDefaultDir, nShowCmd);
	if (pHandle != NULL)
		*pHandle = hHandle;
	return ((LONG_PTR)hHandle > HINSTANCE_ERROR);
}

//-----------------------------------------------------------------------------

bool OpenUrl(LPCTSTR lpszUrl)
{
	CString strUrl(lpszUrl);
	strUrl.Trim();
	return (LONG_PTR)::ShellExecute(0, TEXT("Open"), strUrl, TEXT(""), TEXT(""), SW_SHOW) > HINSTANCE_ERROR;
}

//-----------------------------------------------------------------------------

bool OpenEmailClient(LPCTSTR lpszEmail)
{
	const LPCTSTR PREFIX = TEXT("mailto:");
	CString strEmail(lpszEmail);

	strEmail.Trim();
	if (strEmail.Find(PREFIX) != 0)
		strEmail = PREFIX + strEmail;

	return OpenUrl(strEmail);
}

//-----------------------------------------------------------------------------

void SetStayOnTop(HWND hWnd, bool bStayOnTop)
{
	::SetWindowPos(hWnd, (bStayOnTop? HWND_TOPMOST : HWND_NOTOPMOST),
		0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

//-----------------------------------------------------------------------------

bool SetProcessPrivilege(LPCTSTR lpszPrivilegeName, bool bEnable)
{
	HANDLE hToken = 0;
	LUID luid;
	TOKEN_PRIVILEGES tpNew;

	struct CAutoFinalizer
	{
		HANDLE& m_hToken;
		CAutoFinalizer(HANDLE& hToken) : m_hToken(hToken) {}
		~CAutoFinalizer() { if (m_hToken != 0) CloseHandle(m_hToken); }
	} AutoFinalizer(hToken);

	if (GetWindowsVer().dwPlatformId != VER_PLATFORM_WIN32_NT) return true;

	if (!::OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return false;
	if (!::LookupPrivilegeValue(NULL, lpszPrivilegeName, &luid))
		return false;

	tpNew.PrivilegeCount = 1;
	tpNew.Privileges[0].Luid = luid;
	tpNew.Privileges[0].Attributes = (bEnable? SE_PRIVILEGE_ENABLED : 0);

	if (!::AdjustTokenPrivileges(hToken, false, &tpNew, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
		return false;

	return (::GetLastError() == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------

bool ShutdownWindows()
{
	SetProcessPrivilege(SE_SHUTDOWN_NAME, true);
	return ::ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF, 0) != 0;
}

//-----------------------------------------------------------------------------

bool CreateShortCut(LPCTSTR lpszOriginalFileName, LPCTSTR lpszShortcutFileName,
	LPCTSTR lpszArguments, LPCTSTR lpszWorkingDir, LPCTSTR lpszDescription,
	int nShowCmd, bool bIsFolderShortcut, LPCTSTR lpszIconFileName, int nIconIndex)
{
	HRESULT r;
	IShellLink *psl = NULL;
	IPersistFile *ppf = NULL;

	if (bIsFolderShortcut)
	{
		r = ::CoCreateInstance(CLSID_FolderShortcut, NULL,
			CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, CLSID_FolderShortcut, (PVOID*)&psl);
		if (!SUCCEEDED(r)) psl = NULL;
	}

	/* Folder shortcuts aren't supported prior to Windows 2000/Me.
	   Fall back to creating a normal shell link. */
	if (psl == NULL)
	{
		bIsFolderShortcut = false;
		r = ::CoCreateInstance(CLSID_ShellLink, NULL,
			CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IShellLink, (PVOID*)&psl);
		if (!SUCCEEDED(r)) psl = NULL;
	}

	if (psl != NULL)
	{
		psl->SetPath(lpszOriginalFileName);
		psl->SetArguments(lpszArguments);
		psl->SetShowCmd(nShowCmd);

		if (!CString(lpszWorkingDir).IsEmpty())
			psl->SetWorkingDirectory(lpszWorkingDir);

		if (!CString(lpszDescription).IsEmpty())
			psl->SetDescription(lpszDescription);

		if (lpszIconFileName && !CString(lpszIconFileName).IsEmpty())
			psl->SetIconLocation(lpszIconFileName, nIconIndex);

		r = psl->QueryInterface(IID_IPersistFile, (PVOID*)&ppf);
		if (SUCCEEDED(r))
		{
			OSVERSIONINFO osvi;
			GetWindowsVer(osvi);
			bool bIsWinXPOrHigher = (osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT &&
				osvi.dwMajorVersion >= 5 && osvi.dwMinorVersion >= 1);
			CStringW strFileName;

			if (bIsFolderShortcut && bIsWinXPOrHigher)
				strFileName = ChangeFileExt(lpszShortcutFileName, TEXT(""));
			else
				strFileName = lpszShortcutFileName;

			r = ppf->Save(strFileName, true);
			ppf->Release();

			return SUCCEEDED(r);
		}

		psl->Release();
	}

	return false;
}

//-----------------------------------------------------------------------------

CString SysErrorMessage(int nErrCode)
{
	CString strResult;
	PVOID lpMsgBuf;

	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		nErrCode,
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL);

	strResult = (LPCTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);

	return strResult;
}

//-----------------------------------------------------------------------------

SYSTEMTIME FileTimeToLocalSystemTime(const FILETIME& tFileTime)
{
	SYSTEMTIME tResult;
	FILETIME tLocalFileTime;

	::FileTimeToLocalFileTime(&tFileTime, &tLocalFileTime);
	::FileTimeToSystemTime(&tLocalFileTime, &tResult);

	return tResult;
}

//-----------------------------------------------------------------------------

FILETIME LocalSystemTimeToFileTime(const SYSTEMTIME& tSystemTime)
{
	FILETIME tResult;

	::SystemTimeToFileTime(&tSystemTime, &tResult);
	::LocalFileTimeToFileTime(&tResult, &tResult);

	return tResult;
}

//-----------------------------------------------------------------------------

bool CreateGuid(GUID& Guid)
{
	return ::CoCreateGuid(&Guid) == S_OK;
}

//-----------------------------------------------------------------------------

bool StringToGuid(LPCTSTR str, GUID& Guid)
{
	return SUCCEEDED(::CLSIDFromString(CT2OLE(str), &Guid));
}

//-----------------------------------------------------------------------------

CString GuidToString(const GUID& Guid)
{
	CString strResult;
	LPOLESTR pStr;

	if (SUCCEEDED(::StringFromCLSID(Guid, &pStr)))
	{
		strResult = COLE2T(pStr);
		::CoTaskMemFree(pStr);
	}

	return strResult;
}

//-----------------------------------------------------------------------------

HANDLE GetProcessHandle(LPCTSTR lpszProcessName, DWORD nDesiredAccess)
{
	HANDLE hResult = INVALID_HANDLE_VALUE;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) return hResult;

	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL bOk = Process32First(hSnapshot, &pe); bOk; bOk = Process32Next(hSnapshot, &pe))
	{
		if (_tcsicmp(pe.szExeFile, lpszProcessName) == 0)
		{
			hResult = OpenProcess(nDesiredAccess, FALSE, pe.th32ProcessID);
			break;
		}
	}

	CloseHandle(hSnapshot);
	return hResult;
}

//-----------------------------------------------------------------------------

void TerminateProcesses(LPCTSTR lpszProcessName)
{
	while (true)
	{
		HANDLE hHandle = GetProcessHandle(lpszProcessName, PROCESS_TERMINATE);
		bool bSuccess = (hHandle != INVALID_HANDLE_VALUE);
		if (bSuccess)
		{
			bSuccess = ::TerminateProcess(hHandle, 0) != FALSE;
			CloseHandle(hHandle);
		}

		if (bSuccess)
			Sleep(1);
		else
			break;
	}
}

//-----------------------------------------------------------------------------

CString GetExeFileVersion(LPCTSTR lpszFileName)
{
	CString strResult;
	DWORD nHandle = 0;
	DWORD nVerSize = 0;
	VS_FIXEDFILEINFO *pFileInfo = NULL;

	nVerSize = GetFileVersionInfoSize(lpszFileName, &nHandle);
	if (nVerSize)
	{
		CBuffer VerBuf(nVerSize);
		if (GetFileVersionInfo(lpszFileName, NULL, nVerSize, VerBuf.Data()))
		{
			UINT nSize = sizeof(VS_FIXEDFILEINFO);
			if (VerQueryValue(VerBuf.Data(), TEXT("\\"), (LPVOID*)&pFileInfo, &nSize))
			{
				strResult.Format(TEXT("%d.%d.%d.%d"),
					HIWORD(pFileInfo->dwFileVersionMS),
					LOWORD(pFileInfo->dwFileVersionMS),
					HIWORD(pFileInfo->dwFileVersionLS),
					LOWORD(pFileInfo->dwFileVersionLS)
					);
			}
		}
	}

	return strResult;
}

//-----------------------------------------------------------------------------

bool VerStrToInt64(LPCTSTR lpszVersion, UINT64& nValue)
{
	CStrList StrList;
	SplitString(lpszVersion, '.', StrList);
	bool bResult = (StrList.GetCount() == 4);
	if (bResult)
	{
		nValue = 0;
		INT64_REC *pValue = (INT64_REC*)(&nValue);
		for (int i = 0; i < StrList.GetCount(); i++)
			pValue->words[3-i] = StrToInt(StrList[i], 0);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

CString Int64ToVerStr(UINT64 nValue)
{
	INT64_REC Int64Rec;
	Int64Rec.value = nValue;

	CString strResult;
	strResult.Format(TEXT("%u.%u.%u.%u"),
		(WORD)Int64Rec.words[3],
		(WORD)Int64Rec.words[2],
		(WORD)Int64Rec.words[1],
		(WORD)Int64Rec.words[0]);

	return strResult;
}

//-----------------------------------------------------------------------------

int CompareVersion(LPCTSTR lpszVer1, LPCTSTR lpszVer2)
{
	UINT64 v1, v2;
	if (VerStrToInt64(lpszVer1, v1) && VerStrToInt64(lpszVer2, v2))
		return Compare(v1, v2);
	else
		return CString(lpszVer1).Compare(lpszVer2);
}

//-----------------------------------------------------------------------------
// Obtains the file information using FindFirstFile.
//-----------------------------------------------------------------------------
static bool GetFileFindData(LPCTSTR lpszFileName, WIN32_FIND_DATA& FindData)
{
	HANDLE hFindHandle = ::FindFirstFile(lpszFileName, &FindData);
	bool bResult = (hFindHandle != INVALID_HANDLE_VALUE);
	if (bResult) ::FindClose(hFindHandle);
	return bResult;
}

//-----------------------------------------------------------------------------

INT64 GetFileSize(LPCTSTR lpszFileName)
{
	struct INT64_REC { DWORD lo, hi; };
	INT64 nResult = -1;

	WIN32_FIND_DATA FindData;
	if (GetFileFindData(lpszFileName, FindData))
	{
		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			((INT64_REC*)&nResult)->lo = FindData.nFileSizeLow;
			((INT64_REC*)&nResult)->hi = FindData.nFileSizeHigh;
		}
	}

	return nResult;
}

//-----------------------------------------------------------------------------

CTime GetFileDate(LPCTSTR lpszFileName)
{
	CTime Result;
	WIN32_FIND_DATA FindData;

	if (GetFileFindData(lpszFileName, FindData))
	{
		if ((FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			Result = CTime(FindData.ftLastWriteTime);
	}

	return Result;
}

//-----------------------------------------------------------------------------

bool SetFileDate(LPCTSTR lpszFileName, const CTime& tCreationTime,
	const CTime& tLastAccessTime, const CTime& tLastWriteTime)
{
	bool bResult;
	CFileStream fs;

	bResult = fs.Open(lpszFileName, FM_OPEN_READ_WRITE | FM_SHARE_DENY_NONE);

	if (bResult)
	{
		SYSTEMTIME c, a, w;
		tCreationTime.GetAsSystemTime(c);
		tLastAccessTime.GetAsSystemTime(a);
		tLastWriteTime.GetAsSystemTime(w);

		::SetFileTime(fs.GetHandle(),
			&LocalSystemTimeToFileTime(c),
			&LocalSystemTimeToFileTime(a),
			&LocalSystemTimeToFileTime(w));
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CopyFileToFolder(LPCTSTR lpszFileName, LPCTSTR lpszDestFolder)
{
	CString strDestFolder(lpszDestFolder);
	strDestFolder = PathWithSlash(strDestFolder);
	ForceDirectories(strDestFolder);
	return ::CopyFile(lpszFileName, strDestFolder + ExtractFileName(lpszFileName), false) != 0;
}

//-----------------------------------------------------------------------------

CString AutoRenameFileName(LPCTSTR lpszFullName)
{
	const TCHAR LEFT_SYMBOL = TEXT('(');
	const TCHAR RIGHT_SYMBOL = TEXT(')');

	int nNumber;
	CString strName, strExt, strNumSec;
	CString strResult;

	strResult = lpszFullName;
	strExt = ExtractFileExt(strResult);

	while (FileExists(strResult))
	{
		strName = strResult.Left(strResult.GetLength() - strExt.GetLength());

		// Get strNumSec
		strNumSec.Empty();
		if (strName.GetLength() >= 3)
		{
			if (strName[strName.GetLength() - 1] == RIGHT_SYMBOL)
			{
				for (int i = strName.GetLength() - 3; i >= 0; i--)
					if (strName[i] == LEFT_SYMBOL)
					{
						strNumSec = strName.Mid(i);
						break;
					}
			}
			if (!strNumSec.IsEmpty())
			{
				if (!IsIntStr(strNumSec.Mid(1, strNumSec.GetLength() - 2)))
					strNumSec.Empty();
			}
		}

		if (strNumSec.IsEmpty())
			strResult = strName + LEFT_SYMBOL + TEXT('1') + RIGHT_SYMBOL + strExt;
		else
		{
			nNumber = StrToInt(strNumSec.Mid(1, strNumSec.GetLength() - 2), 0);
			nNumber++;
			strResult = strName.Left(strName.GetLength() - strNumSec.GetLength()) +
				LEFT_SYMBOL + IntToStr(nNumber) + RIGHT_SYMBOL + strExt;
		}
	}

	return strResult;
}

//-----------------------------------------------------------------------------

bool IsFileReadOnly(LPCTSTR lpszFileName)
{
	return (::GetFileAttributes(lpszFileName) & FILE_ATTRIBUTE_READONLY) != 0;
}

//-----------------------------------------------------------------------------

bool FileSetReadOnly(LPCTSTR lpszFileName, bool bReadOnly)
{
	bool bResult = false;
	DWORD nFlags;

	nFlags = ::GetFileAttributes(lpszFileName);
	if (nFlags != (DWORD)(-1))
	{
		if (bReadOnly)
			nFlags |= FILE_ATTRIBUTE_READONLY;
		else
			nFlags &= ~FILE_ATTRIBUTE_READONLY;

		bResult = ::SetFileAttributes(lpszFileName, nFlags) != 0;
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool FileExists(LPCTSTR lpszFileName)
{
	DWORD nFileAttr = ::GetFileAttributes(lpszFileName);
	if (nFileAttr != (DWORD)(-1))
		return ((nFileAttr & FILE_ATTRIBUTE_DIRECTORY) == 0);
	else
	{
		WIN32_FIND_DATA FindData;
		DWORD nLastError = ::GetLastError();
		return
			(nLastError != ERROR_FILE_NOT_FOUND) &&
			(nLastError != ERROR_PATH_NOT_FOUND) &&
			(nLastError != ERROR_INVALID_NAME) &&
			GetFileFindData(lpszFileName, FindData) &&
			(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
	}
}

//-----------------------------------------------------------------------------

bool RenameFile(LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName)
{
	return ::MoveFile(lpszOldFileName, lpszNewFileName) != 0;
}

//-----------------------------------------------------------------------------

bool RemoveFile(LPCTSTR lpszFileName)
{
	DWORD nFileAttr = ::GetFileAttributes(lpszFileName);
	if (nFileAttr != (DWORD)(-1) && (nFileAttr & FILE_ATTRIBUTE_READONLY) != 0)
	{
		nFileAttr &= ~FILE_ATTRIBUTE_READONLY;
		::SetFileAttributes(lpszFileName, nFileAttr);
	}

	return ::DeleteFile(lpszFileName) != 0;
}

//-----------------------------------------------------------------------------

bool DirectoryExists(LPCTSTR lpszDir)
{
	DWORD nAttr;
	nAttr = ::GetFileAttributes(lpszDir);
	return (nAttr != INVALID_FILE_ATTRIBUTES) && ((FILE_ATTRIBUTE_DIRECTORY & nAttr) != 0);
}

//-----------------------------------------------------------------------------

bool CreateDir(LPCTSTR lpszDir)
{
	return ::CreateDirectory(lpszDir, NULL) != 0;
}

//-----------------------------------------------------------------------------

bool RemoveDir(LPCTSTR lpszDir, bool bRecursive)
{
	if (!bRecursive)
		return ::RemoveDirectory(lpszDir) != 0;

	bool bResult = true;
	CString strPath(PathWithSlash(lpszDir));
	HANDLE nFindHandle;
	WIN32_FIND_DATA FindData;

	nFindHandle = FindFirstFile(strPath + TEXT("*.*"), &FindData);
	if (nFindHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			DWORD nAttr = FindData.dwFileAttributes;
			CString strName = FindData.cFileName;
			CString strFileName = strPath + strName;
			bool bIsDir = (nAttr & FILE_ATTRIBUTE_DIRECTORY) != 0;
			bool bIsSpecDir = bIsDir && (strName == TEXT(".") || strName == TEXT(".."));

			if (bIsSpecDir) continue;

			if (bIsDir)
				bResult = RemoveDir(strFileName, true);
			else
				RemoveFile(strFileName);
		}
		while (FindNextFile(nFindHandle, &FindData));

		FindClose(nFindHandle);
	}

	bResult = RemoveDir(strPath, false);
	return bResult;
}

//-----------------------------------------------------------------------------

bool ForceDirectories(LPCTSTR lpszDir)
{
	CString strDir(lpszDir);
	int nLen = strDir.GetLength();

	if (strDir.IsEmpty()) return false;
	if (strDir[nLen-1] == PATH_DELIM)
		strDir.GetBufferSetLength(nLen - 1);

	if (strDir.GetLength() < 3 || DirectoryExists(strDir) ||
		ExtractFilePath(strDir) == strDir) return true;    // avoid 'xyz:\' problem.

	return ForceDirectories(ExtractFilePath(strDir)) && CreateDir(strDir);
}

//-----------------------------------------------------------------------------

CString ExpandFileName(LPCTSTR lpszFileName)
{
	const int MAX_SIZE = 1024;
	TCHAR szBuffer[MAX_SIZE];
	LPTSTR pFileName;

	::GetFullPathName(lpszFileName, MAX_SIZE, szBuffer, &pFileName);

	return szBuffer;
}

//-----------------------------------------------------------------------------

CString GetShortFileName(LPCTSTR lpszFileName)
{
	const int MAX_SIZE = 1024;
	TCHAR szBuffer[MAX_SIZE];

	::GetShortPathName(lpszFileName, szBuffer, MAX_SIZE);

	return szBuffer;
}

//-----------------------------------------------------------------------------

CString GetLongFileName(LPCTSTR lpszFileName)
{
	// WinNT 3.5 does not support API ::GetLongPathName(), so the API is not used here.

	CString strFileName(lpszFileName);
	CString strName, str;
	CString strResult;

	str = PathWithoutSlash(strFileName);
	if (str.GetLength() < 3 || ExtractFilePath(str) == str)
		return strFileName;

	WIN32_FIND_DATA FindData;
	if (GetFileFindData(str, FindData))
		strName = FindData.cFileName;
	else
		strName = ExtractFileName(str);

	strResult = GetLongFileName(ExtractFilePath(str)) + strName;
	if (str.GetLength() != strFileName.GetLength())
		strResult = strResult + TEXT("\\");

	return strResult;
}

//-----------------------------------------------------------------------------

static void DoFindFiles(LPCTSTR lpszPath, LPCTSTR lpszPattern, CStrList& FileList,
	bool bFindFile, bool bFindDir, bool bIncludeHidden, bool bRecursive)
{
	CString strPath(PathWithSlash(lpszPath));
	CString strPattern = (lpszPattern == NULL || *lpszPattern == 0) ? TEXT("*.*") : lpszPattern;
	HANDLE nFindHandle;
	WIN32_FIND_DATA FindData;

	nFindHandle = FindFirstFile(strPath + strPattern, &FindData);
	if (nFindHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString strName = FindData.cFileName;
			CString strFileName = strPath + strName;

			DWORD nAttr = FindData.dwFileAttributes;
			bool bIsHidden = (nAttr & FILE_ATTRIBUTE_HIDDEN) != 0;
			bool bIsDir = (nAttr & FILE_ATTRIBUTE_DIRECTORY) != 0;
			bool bIsDots = bIsDir && (strName == TEXT(".") || strName == TEXT(".."));
			bool bIsSpecFile = (nAttr & FILE_ATTRIBUTE_SYSTEM) != 0;

			if (bIsDots || bIsSpecFile) continue;
			if (!bIncludeHidden && bIsHidden) continue;

			if (bIsDir)
			{
				if (bFindDir)
					FileList.Add(strFileName);
				if (bRecursive)
					DoFindFiles(strFileName, lpszPattern, FileList, bFindFile, bFindDir, bIncludeHidden, bRecursive);
			}
			else
			{
				if (bFindFile)
					FileList.Add(strFileName);
			}
		}
		while (FindNextFile(nFindHandle, &FindData));

		FindClose(nFindHandle);
	}
}

void FindFiles(LPCTSTR lpszPath, LPCTSTR lpszPattern, CStrList& FileList,
	bool bFindFile, bool bFindDir, bool bIncludeHidden, bool bRecursive)
{
	FileList.Clear();
	DoFindFiles(lpszPath, lpszPattern, FileList, bFindFile, bFindDir, bIncludeHidden, bRecursive);
}

//-----------------------------------------------------------------------------

bool SetAutoRunOnStartup(bool bAutoRun, bool bCurrentUser, LPCTSTR lpszAppTitle, LPCTSTR lpszAppParam)
{
	bool bResult;

	CString strValue = GetAppExeName(true);
	if (strValue.Find(' ') >= 0)
		strValue = '"' + strValue + '"';

	if (!CString(lpszAppParam).IsEmpty())
		strValue = strValue + TEXT(" ") + lpszAppParam;

	try
	{
		CRegistry Reg;
		Reg.SetRootKey(bCurrentUser? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE);
		bResult = Reg.OpenKey(TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run\\"), true);
		if (bResult)
		{
			if (bAutoRun)
			{
				if (!Reg.ValueExists(lpszAppTitle) || Reg.ReadString(lpszAppTitle) != strValue)
					Reg.WriteString(lpszAppTitle, strValue);
			}
			else
			{
				if (Reg.ValueExists(lpszAppTitle))
					Reg.DeleteValue(lpszAppTitle);
			}
		}
	}
	catch (IFC_EXCEPT_OBJ e)
	{
		IFC_DELETE_MFC_EXCEPT_OBJ(e);
		bResult = false;
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool AssociateFile(LPCTSTR lpszFileExt, LPCTSTR lpszFileKey, LPCTSTR lpszAppName,
	LPCTSTR lpszFileDesc, int nIconIndex, bool bFlush)
{
	bool bResult;

	CString strAppExeName = GetAppExeName(true);
	if (strAppExeName.Find(' ') >= 0)
		strAppExeName = '"' + strAppExeName + '"';

	try
	{
		CRegistry Reg;
		Reg.SetRootKey(HKEY_CLASSES_ROOT);

		bResult = Reg.OpenKey(TEXT("\\") + CString(lpszFileExt), true);
		if (bResult)
			Reg.WriteString(TEXT(""), lpszFileKey);

		if (bResult)
			bResult = Reg.OpenKey(TEXT("\\") + CString(lpszFileKey), true);
		if (bResult)
			Reg.WriteString(TEXT(""), lpszFileDesc);

		if (bResult)
			bResult = Reg.OpenKey(TEXT("\\") + CString(lpszFileKey) + TEXT("\\Shell\\Open\\Command"), true);
		if (bResult)
			Reg.WriteString(TEXT(""), strAppExeName + TEXT(" \"%1\""));

		if (bResult)
			bResult = Reg.OpenKey(TEXT("\\") + CString(lpszFileKey) + TEXT("\\Shell\\Open with ") + lpszAppName + TEXT("\\Command"), true);
		if (bResult)
			Reg.WriteString(TEXT(""), strAppExeName + TEXT(" \"%1\""));

		if (bResult)
			bResult = Reg.OpenKey(TEXT("\\") + CString(lpszFileKey) + TEXT("\\DefaultIcon"), true);
		if (bResult)
			Reg.WriteString(TEXT(""), strAppExeName + TEXT(",") + IntToStr(nIconIndex));
	}
	catch (IFC_EXCEPT_OBJ e)
	{
		IFC_DELETE_MFC_EXCEPT_OBJ(e);
		bResult = false;
	}

	if (bResult && bFlush)
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

	return bResult;
}

//-----------------------------------------------------------------------------

bool RegisterUrlProtocol(LPCTSTR lpszProtocolName, LPCTSTR lpszExeFileName, int nIconIndex)
{
	bool bResult;

	CString strProtocolName(lpszProtocolName);
	strProtocolName.MakeLower();

	CString strExeFileName(lpszExeFileName);
	if (strExeFileName.Find(' ') >= 0)
		strExeFileName = '"' + strExeFileName + '"';

	try
	{
		CRegistry Reg;
		Reg.SetRootKey(HKEY_CLASSES_ROOT);

		CString strProtocolKey = TEXT("\\") + strProtocolName;

		bResult = Reg.OpenKey(strProtocolKey, true);
		if (bResult)
		{
			Reg.WriteString(TEXT(""), FormatString(TEXT("URL:%s Protocol"), strProtocolName));
			Reg.WriteString(TEXT("URL Protocol"), TEXT(""));
		}

		if (bResult)
			bResult = Reg.OpenKey(strProtocolKey + TEXT("\\DefaultIcon"), true);
		if (bResult)
			Reg.WriteString(TEXT(""), strExeFileName + TEXT(",") + IntToStr(nIconIndex));

		if (bResult)
			bResult = Reg.OpenKey(strProtocolKey + TEXT("\\Shell\\Open\\Command"), true);
		if (bResult)
			Reg.WriteString(TEXT(""), strExeFileName + TEXT(" \"%1\""));

		if (bResult)
		{
			Reg.SetRootKey(HKEY_LOCAL_MACHINE);
			CString strKey = FormatString(TEXT("SOFTWARE\\Microsoft\\Internet Explorer\\ProtocolExecute\\%s\\"), strProtocolName);
			bResult = Reg.OpenKey(strKey, true);
			if (bResult)
				Reg.WriteInteger(TEXT("WarnOnOpen"), 0);
		}
	}
	catch (IFC_EXCEPT_OBJ e)
	{
		IFC_DELETE_MFC_EXCEPT_OBJ(e);
		bResult = false;
	}

	return bResult;
}

//-----------------------------------------------------------------------------

CString EncodeBase64(const char *pSrcData, int nSrcSize)
{
	static const char *s_pTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
	CString strResult;

	if (!pSrcData || nSrcSize <= 0) return TEXT("");

	LPTSTR pDest = strResult.GetBuffer(nSrcSize * 4 / 3 + 4);
	LPTSTR pDestStart = pDest;
	char *pTable = (char*)s_pTable;

	while (nSrcSize > 0)
	{
		DWORD b = 0;
		for (int i = 0; i <= 2; i++)
		{
			b = b << 8;
			if (nSrcSize > 0)
			{
				b = b | (unsigned char)(*pSrcData);
				pSrcData++;
			}
			nSrcSize--;
		}
		for (int i = 3; i >= 0; i--)
		{
			if (nSrcSize < 0)
			{
				pDest[i] = pTable[64];
				nSrcSize++;
			}
			else
			{
				pDest[i] = pTable[b & 0x3F];
			}
			b = b >> 6;
		}
		pDest += 4;
	}

	strResult.ReleaseBufferSetLength((int)(pDest - pDestStart));
	return strResult;
}

//-----------------------------------------------------------------------------

void DecodeBase64(LPCTSTR lpszSrcData, PBYTE pDestBuf, int& nDestSize)
{
	int nSrcLen = (int)_tcsclen(lpszSrcData);
	LPCTSTR pSrcEnd = lpszSrcData + nSrcLen;
	int nWritten = 0;
	bool bOverflow = false;

	while (lpszSrcData < pSrcEnd && (*lpszSrcData) != 0)
	{
		DWORD dwCurr = 0;
		int i;
		int nBits = 0;
		for (i = 0; i < 4; i++)
		{
			if (lpszSrcData >= pSrcEnd)
				break;

			int nCh = *lpszSrcData;

			if (nCh >= 'A' && nCh <= 'Z')
				nCh = nCh - 'A' + 0;    // 0 range starts at 'A'
			else if (nCh >= 'a' && nCh <= 'z')
				nCh = nCh - 'a' + 26;   // 26 range starts at 'a'
			else if (nCh >= '0' && nCh <= '9')
				nCh = nCh - '0' + 52;   // 52 range starts at '0'
			else if (nCh == '+')
				nCh = 62;
			else if (nCh == '/')
				nCh = 63;
			else
				nCh = -1;

			lpszSrcData++;
			if (nCh == -1)
			{
				// skip this char
				i--;
				continue;
			}
			dwCurr <<= 6;
			dwCurr |= nCh;
			nBits += 6;
		}

		if (!bOverflow && nWritten + (nBits/8) > (nDestSize))
			bOverflow = true;

		dwCurr <<= 24 - nBits;
		for (i = 0; i < nBits/8; i++)
		{
			if (!bOverflow)
			{
				*pDestBuf = (BYTE) ((dwCurr & 0x00ff0000) >> 16);
				pDestBuf++;
			}
			dwCurr <<= 8;
			nWritten++;
		}
	}

	nDestSize = nWritten;

	if (bOverflow)
		IFC_ASSERT(false);
}

//-----------------------------------------------------------------------------

void DecodeBase64(LPCTSTR lpszSrcData, CBuffer& DestBuf)
{
	int nSrcLen = (int)_tcsclen(lpszSrcData);
	int nDestLen = nSrcLen;

	DestBuf.SetSize(nDestLen);
	DecodeBase64(lpszSrcData, (PBYTE)DestBuf.Data(), nDestLen);
	DestBuf.SetSize(nDestLen);
	DestBuf[nDestLen] = 0;
}

//-----------------------------------------------------------------------------

CString EncodeBase16(const char *pSrcData, int nSrcSize)
{
	static const char *pTable = "0123456789ABCDEF";
	CString strResult;

	if (!pSrcData || nSrcSize <= 0) return TEXT("");

	LPTSTR d = strResult.GetBufferSetLength(nSrcSize * 2);
	char *s = (char*)pSrcData;

	while (nSrcSize > 0)
	{
		*d++ = pTable[((*s) >> 4) & 0x0F];
		*d++ = pTable[(*s) & 0x0F];
		s++;
		nSrcSize--;
	}

	strResult.ReleaseBuffer();
	return strResult;
}

//-----------------------------------------------------------------------------

void DecodeBase16(LPCTSTR lpszSrcData, PBYTE pDestBuf, int& nDestSize)
{
	int nSrcLen = (int)_tcsclen(lpszSrcData);

	if ((nSrcLen % 2) != 0) nSrcLen--;
	if (nDestSize < nSrcLen / 2)
		IFC_ASSERT(false);

	static struct {
		char operator() (char ch)
		{
			if (ch >= '0' && ch <= '9')
				return (ch - '0');
			if (ch >= 'A' && ch <= 'F')
				return (ch - 'A' + 10);
			if (ch >= 'a' && ch <= 'f')
				return (ch - 'a' + 10);
			return 0;
		}
	} GetHexValue;

	int nRead = 0;
	int nWritten = 0;
	while (nRead < nSrcLen)
	{
		char ch1 = GetHexValue((char)*lpszSrcData++);
		char ch2 = GetHexValue((char)*lpszSrcData++);

		*pDestBuf++ = (BYTE)(ch1*16 + ch2);
		nWritten++;
		nRead += 2;
	}

	nDestSize = nWritten;
}

//-----------------------------------------------------------------------------

void DecodeBase16(LPCTSTR lpszSrcData, CBuffer& DestBuf)
{
	int nSrcLen = (int)_tcsclen(lpszSrcData);
	int nDestLen = nSrcLen / 2;

	DestBuf.SetSize(nDestLen);
	DecodeBase16(lpszSrcData, (PBYTE)DestBuf.Data(), nDestLen);
	DestBuf.SetSize(nDestLen);
	DestBuf[nDestLen] = 0;
}

//-----------------------------------------------------------------------------

CString UrlEncode(LPCTSTR lpszSource)
{
	static struct {
		BYTE operator() (BYTE x)
		{
			return x > 9 ? x + 55: x + 48;
		}
	} ToHex;

	CStringA src(lpszSource);
	CStringA dst;
	int srclen = src.GetLength();
	BYTE buf[4] = {0};

	for (int i = 0; i < srclen; i++)
	{
		BYTE c = src[i];

		if (isalnum(c) || c == ':' || c == '/' || c == '.')
			dst += c;
		else if (isspace(c))
			dst += '+';
		else
		{
			buf[0] = '%';
			buf[1] = ToHex(c >> 4);
			buf[2] = ToHex(c % 16);
			dst += (char*)buf;
		}
	}

	return CString(CA2T(dst));
}

//-----------------------------------------------------------------------------

CString UrlDecode(LPCTSTR lpszSource)
{
	CStringA src(lpszSource);
	CStringA dst;
	int srclen = src.GetLength();

	for (int i = 0; i < srclen; i++)
	{
		if (src[i] == '%')
		{
			if (isxdigit(src[i + 1]) && isxdigit(src[i + 2]))
			{
				char c1 = src[++i];
				char c2 = src[++i];
				c1 = c1 - 48 - ((c1 >= 'A') ? 7 : 0) - ((c1 >= 'a') ? 32 : 0);
				c2 = c2 - 48 - ((c2 >= 'A') ? 7 : 0) - ((c2 >= 'a') ? 32 : 0);
				dst += (unsigned char)(c1 * 16 + c2);
			}
		}
		else
		{
			if (src[i] == '+')
			{
				dst += ' ';
			}
			else
			{
				dst += src[i];
			}
		}
	}

	return CString(CA2T(dst));
}

//-----------------------------------------------------------------------------

CStringA Utf8Encode(LPCTSTR lpszSource)
{
	CStringA strResult;
	CStringW strSource = CT2W(lpszSource);

	int n = WideCharToMultiByte(CP_UTF8, 0, strSource, -1, NULL, 0, NULL, NULL);
	if (n > 0)
	{
		LPSTR p = strResult.GetBufferSetLength(n);
		WideCharToMultiByte(CP_UTF8, 0, strSource, -1, p, n, NULL, NULL);
		strResult.ReleaseBuffer();
	}

	return strResult;
}

//-----------------------------------------------------------------------------

UINT GetTickDiff(UINT nOldTickCount, UINT nNewTickCount)
{
	if (nNewTickCount >= nOldTickCount)
		return (nNewTickCount - nOldTickCount);
	else
		return (UINT(-1) - nOldTickCount + nNewTickCount);
}

//-----------------------------------------------------------------------------

void Randomize()
{
	srand((unsigned int)time(NULL));
}

//-----------------------------------------------------------------------------

int GetRandom(int nMin, int nMax)
{
	IFC_ASSERT((nMax - nMin) < MAXLONG);
	return nMin + (int)(((double)rand() / ((double)RAND_MAX + 0.1)) * (nMax - nMin + 1));
}

//-----------------------------------------------------------------------------

INT64 GetRandom64(INT64 nMin, INT64 nMax)
{
	IFC_ASSERT((nMax - nMin) < MAXLONGLONG);
	return nMin + (INT64)(((double)rand() / ((double)RAND_MAX + 0.1)) * (nMax - nMin + 1));
}

//-----------------------------------------------------------------------------

void GenerateRandomList(int nStartNumber, int nEndNumber, int *pRandomList)
{
	if (nStartNumber > nEndNumber || !pRandomList) return;

	int nCount = nEndNumber - nStartNumber + 1;

	if (rand() % 2 == 0)
		for (int i = 0; i < nCount; i++)
			pRandomList[i] = nStartNumber + i;
	else
		for (int i = 0; i < nCount; i++)
			pRandomList[nCount - i - 1] = nStartNumber + i;

	for (int i = nCount - 1; i >= 1; i--)
		Swap(pRandomList[i], pRandomList[rand()%i]);
}

//-----------------------------------------------------------------------------

CString GetAppPath()
{
	return ExtractFilePath(GetAppExeName(true));
}

//-----------------------------------------------------------------------------

CString GetAppSubPath(LPCTSTR lpszSubFolder)
{
	return PathWithSlash(GetAppPath() + lpszSubFolder);
}

//-----------------------------------------------------------------------------

CString GetAppExeName(bool bIncludePath)
{
	TCHAR szBuffer[IFC_MAX_PATH];
	::GetModuleFileName(NULL, szBuffer, IFC_MAX_PATH);
	return bIncludePath? szBuffer : ExtractFileName(szBuffer);
}

//-----------------------------------------------------------------------------

CString GetModuleExeName(HMODULE hModule, bool bIncludePath)
{
	if (hModule == NULL)
		hModule = GetSelfModuleHandle();

	TCHAR szBuffer[IFC_MAX_PATH];
	::GetModuleFileName(hModule, szBuffer, IFC_MAX_PATH);
	return bIncludePath? szBuffer : ExtractFileName(szBuffer);
}

//-----------------------------------------------------------------------------

HMODULE GetSelfModuleHandle()
{
#ifdef IFC_USE_MFC
	return ::AfxGetInstanceHandle();
#else
	MEMORY_BASIC_INFORMATION mbi;
	return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0) ?
		(HMODULE)mbi.AllocationBase : NULL);
#endif
}

//-----------------------------------------------------------------------------

int ParseCommandLine(LPCTSTR lpszCmdLine, CStrList& ArgList)
{
	struct {
		CString operator() (CString& strSrc)
		{
			if (strSrc.IsEmpty()) return TEXT("");

			int nPos = 0;
			while (_istspace(strSrc[nPos])) nPos++;

			int nStart = nPos;
			if (strSrc[nPos] == '\"')
			{
				while (strSrc[nPos])
				{
					if (strSrc[nPos] == '\"' && _istspace(strSrc[nPos+1]))
						break;
					else
						nPos++;
				}
				if (nPos >= strSrc.GetLength()) nPos = strSrc.GetLength() - 1;
			}
			else
			{
				while (strSrc[nPos] && !_istspace(strSrc[nPos])) nPos++;
				nPos--;
			}

			CString strResult = strSrc.Mid(nStart, nPos - nStart + 1);
			strSrc = strSrc.Mid(nPos + 1);

			return strResult;
		}
	} GetToken;

	struct {
		CString operator() (const CString& strSrc)
		{
			CString strResult = strSrc;
			if (!strResult.IsEmpty())
			{
				if (strResult[0] == '\"')
					strResult.Delete(0);
				if (strResult[strResult.GetLength()-1] == '\"')
					strResult.Delete(strResult.GetLength()-1);
			}
			return strResult;
		}
	} DequotedStr;

	CString strLine(lpszCmdLine);
	ArgList.Clear();

	while (true)
	{
		CString strToken = GetToken(strLine);
		if (!strToken.IsEmpty())
		{
			CString str = DequotedStr(strToken);
			if (!str.IsEmpty())
				ArgList.Add(str);
		}
		else
			break;
	}

	if (ArgList.GetCount() > 0)
		ArgList.Delete(0);

	return ArgList.GetCount();
}

//-----------------------------------------------------------------------------

int ParseCommandLine(CStrList& ArgList)
{
	return ParseCommandLine(::GetCommandLine(), ArgList);
}

#ifdef IFC_USE_MFC

//-----------------------------------------------------------------------------

int ShowMessage(LPCTSTR lpszMsg, UINT nType)
{
	return ::AfxMessageBox(lpszMsg, nType);
}

void ShowMessage(INT64 nValue)
{
	CString strMsg;
	strMsg.Format(TEXT("%I64d"), nValue);
	ShowMessage(strMsg);
}

#else

//-----------------------------------------------------------------------------

int ShowMessage(LPCTSTR lpszMsg, LPCTSTR lpszCaption, UINT nType)
{
	CString strCaption(lpszCaption);
	if (strCaption.IsEmpty())
		strCaption.LoadString(128);   // suppose IDR_MAINFRAME == 128.
	return ::MessageBox(::GetActiveWindow(), lpszMsg, strCaption, MB_OK | MB_ICONINFORMATION);
}

void ShowMessage(INT64 nValue, LPCTSTR lpszCaption)
{
	CString strMsg;
	strMsg.Format(TEXT("%I64d"), nValue);
	ShowMessage(strMsg, lpszCaption);
}

#endif

///////////////////////////////////////////////////////////////////////////////

} // namespace ifc
