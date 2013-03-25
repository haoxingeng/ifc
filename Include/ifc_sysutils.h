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

/// @file ifc_sysutils.h
/// Declares the utility routines.

/// @addtogroup Utilities
/// @{

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// Utilities Prototype

//-----------------------------------------------------------------------------

/// @name String Utilities
/// @{

/// Determines whether a string is an integer value.
bool IsIntStr(LPCTSTR str);

/// Determines whether a string is a 64-bits integer value.
bool IsInt64Str(LPCTSTR str);

/// Determines whether a string is a floating-point value.
bool IsFloatStr(LPCTSTR str);

/// Determines whether a string is a bool value.
bool IsBoolStr(LPCTSTR str);

/// Converts a string to a number.
/// If the string @a str does not represent a valid number, this function return the @a nDefault.
int StrToInt(LPCTSTR str, int nDefault);

/// Converts a string to a 64-bits number.
/// If the string @a str does not represent a valid number, this function return the @a nDefault.
INT64 StrToInt64(LPCTSTR str, const INT64& nDefault);

/// Converts an integer to a string.
CString IntToStr(int nValue);

/// Converts an integer to a string.
CString IntToStr(const INT64& nValue);

/// Converts a string to a floating-point value.
/// If the string @a str does not represent a valid floating-point value, this function return the @a fDefault.
double StrToFloat(LPCTSTR str, double fDefault = 0);

/// Converts a floating-point value to a string using a specified format.
CString FloatToStr(double fValue, LPCTSTR lpszFormat = TEXT("%f"));

/// Converts a string to a bool value.
/// If the string @a str does not represent a valid bool value, this function return the @a bDefault.
///
/// For example:
/** @verbatim
	The valid string        The converted result
	----------------        --------------------
	"true"             ->   true
	"false"            ->   false
	"True"             ->   true
	"FALSE"            ->   false
	"123.456"          ->   true
	"123"              ->   true
	"0"                ->   false
@endverbatim */
bool StrToBool(LPCTSTR str, bool bDefault = false);

/// Converts a bool value to a string.
///
/// For example:
/** @verbatim
	bValue        bUseBoolStrs    The converted result
	-----------   ------------    --------------------
	true          false           "1"
	false         false           "0"
	true          true            "true"
	false         true            "false"
@endverbatim */
CString BoolToStr(bool bValue, bool bUseBoolStrs = false);

/// Compares two strings without case sensitivity.
/// Returns true if the two strings have the same value; false otherwise.
bool SameText(LPCTSTR str1, LPCTSTR str2);

/// Returns a string with a specified number of repeating characters.
CString StringOfChar(TCHAR ch, int nCount);

/// Ensures a string with the specified character at the end.
CString EnsureEndWith(LPCTSTR lpszSource, TCHAR ch);

/// Compares two strings without case sensitivity.
/// Returns true if the two strings have the same value; false otherwise.
bool SameText(const std::string& str1, const std::string& str2);

/// Trims all whitespaces from the string.
std::string TrimString(const std::string& str);

/// Returns a string that has been converted to uppercase.
std::string UpperCase(const std::string& str);

/// Returns a string that has been converted to lowercase.
std::string LowerCase(const std::string& str);

/// Returns a path with a path delimiter (\) at the end.
CString PathWithSlash(LPCTSTR lpszPath);

/// Returns a path without a path delimiter (\) at the end.
CString PathWithoutSlash(LPCTSTR lpszPath);

/// Returns a string by combining the path and the file name correctly.
CString CombineFileName(LPCTSTR lpszPath, LPCTSTR lpszFileName);

/// Returns a file extension with a separator (.) at the begining.
CString FileExtWithDot(LPCTSTR lpszFileExt);

/// Returns a file extension without a separator (.) at the begining.
CString FileExtWithoutDot(LPCTSTR lpszFileExt);

/// Returns a string by adding thousand separators.
CString AddThousandSep(const INT64& nNumber);

/// Returns the drive portion of a file name.
/// For example:
/** @verbatim
	c:\abc\test.txt            ->   c:
	c:                         ->   c:
	\\localhost\c:\test.txt    ->   \\localhost\c:
@endverbatim */
CString ExtractFileDrive(LPCTSTR lpszFileName);

/// Returns the drive and directory portions of a file name.
/// For example:
/** @verbatim
	ExtractFilePath("C:\\MyDir\\test.txt");        ->   "C:\\MyDir\\"
	ExtractFilePath("C:");                         ->   "C:\\"
@endverbatim */
CString ExtractFilePath(LPCTSTR lpszFileName);

/// Extracts the name and extension parts of a file name.
/// For example:
/** @verbatim
	ExtractFileName("C:\\MyDir\\test.txt");        ->   "test.txt"
@endverbatim */
CString ExtractFileName(LPCTSTR lpszFileName);

/// Returns the extension portions of a file name.
/// For example:
/** @verbatim
	ExtractFileExt("C:\\MyDir\\test.txt");        ->   ".txt"
@endverbatim */
CString ExtractFileExt(LPCTSTR lpszFileName);

/// Returns the name part without extension of a file name.
/// For example:
/** @verbatim
	ExtractFileMainName("C:\\MyDir\\test.txt");   ->   "test"
@endverbatim */
CString ExtractFileMainName(LPCTSTR lpszFileName);

/// Returns a relative path name, relative to a specific base directory.
///
/// Call ExtractRelativePath to convert a fully qualified path name into a relative path name.
/// The @a lpszDestName parameter specifies file name (including path) to be converted.
/// @a lpszBaseName is the fully qualified name of the base directory to which the returned
/// path name should be relative. @a lpszBaseName may or may not include a file name, but it
/// must include the final path delimiter.
///
/// For example:
/** @verbatim
	ExtractRelativePath("c:\\base\\", "c:\\dest\\");          ->   "..\\dest\\"
	ExtractRelativePath("c:\\base\\", "c:\\dest\\file.txt");  ->   "..\\dest\\file.txt"
	ExtractRelativePath("c:\\base\\", "c:\\base\\file.txt");  ->   "file.txt"
@endverbatim */
/// @remarks
///   If you send it what should be a directory without the trailing backslash (c:\\foo instead of c:\\foo\\),
///   the function treats foo as a file and returns undesirable results. You should correctly call it
///   like this:
///   @verbatim ExtractRelativePath("c:\\foo\\", "c:\\bar\\");  ->  "..\\bar\\" @endverbatim
///   The wrong case:
///   @verbatim ExtractRelativePath("c:\\foo\\", "c:\\bar");    ->  ".." @endverbatim
CString ExtractRelativePath(LPCTSTR lpszBaseName, LPCTSTR lpszDestName);

/// Returns the path part of a URL string.
/// For example:
/** @verbatim
	ExtractUrlFilePath("http://www.download.com/file.zip");   ->   "http://www.download.com/"
@endverbatim */
CString ExtractUrlFilePath(LPCTSTR lpszUrl);

/// Returns the file name part of a URL string.
/// For example:
/** @verbatim
	ExtractUrlFileName("http://www.download.com/file.zip");   ->   "file.zip"
@endverbatim */
CString ExtractUrlFileName(LPCTSTR lpszUrl);

/// Returns a new file name by changing the extension of a file name.
/// For example:
/** @verbatim
	ChangeFileExt("c:\\test.txt", ".new");   ->   "c:\\test.new"
@endverbatim */
/// @remarks
///   ChangeFileExt does not rename the actual file, it just returns a new file name string.
CString ChangeFileExt(LPCTSTR lpszFileName, LPCTSTR lpszExt);

/// Strips all the invalid characters from a file name and returns the new string.
/// For example:
/** @verbatim
	ValidateFileName("tes*t.dat?");   ->   "test.dat"
@endverbatim */
/// @remarks
///   The invalid character is:
///   - <tt>\ / : * ? \ " < > | </tt>
///   - The characters which ASCII code is less than 32.
CString ValidateFileName(LPCTSTR lpszFileName);

/// Returns a string to represent a size in KB.
/// For example:
/** @verbatim
	GetSizeString(2049);   ->   "3 KB"
@endverbatim */
CString GetSizeString(const INT64& nBytes, LPCTSTR lpszSuffix = TEXT(" KB"));

/// Returns a string to represent a percent.
CString GetPercentString(const INT64& nPos, const INT64& nMax, LPCTSTR lpszSuffix = TEXT(" %"));

/// Shortens the string to fit in the specified width on a device context.
///
/// @param[in] str
///   The original string to be processed.
/// @param[in] hdc
///   The DC on which to draw the string.
/// @param[in] nWidth
///   The maximum width in pixels.
/// @param[in] lpszSuffix
///   The suffix to be added the end of the original string if the string is shortened.
/// @return
///   The resultant string.
CString RestrictStrWidth(LPCTSTR str, HDC hdc, int nWidth, LPCTSTR lpszSuffix = TEXT("..."));

/// Makes an abbreviate file name from the given file name.
///
/// @param[in] lpszFileName
///   The original file name to be abbreviated.
/// @param[in] nMaxChars
///   Maximum number of characters of the resultant file name.
/// @return
///   The abbreviated file name.
CString AbbreviateFileName(LPCTSTR lpszFileName, int nMaxChars);

/// Returns the width, in pixels, of a string rendered on the specified device context.
int TextWidth(HDC hdc, const CString& str);

/// Returns the height, in pixels, of a string rendered on the specified device context.
int TextHeight(HDC hdc, const CString& str);

/// Determines whether or not a given string matches a specified pattern.
///
/// @param[in] lpszValue
///   The string to compare.
/// @param[in] lpszPattern
///   The pattern string.
/// @param[in] bCaseInsensitive
///   Specifies whether strings should be compared in case-sensitive manner or not.
/// @return
///   Returns true if the strings match; false otherwise.
/// @remarks
///   A pattern can include these wildcard characters:
///   - '<tt>*</tt>': Any string of zero or more characters.
///   - '<tt>?</tt>': Any single character.
///
/// For example:
/** @verbatim
	LikeString("abcdefg", "abc*", true);   ->   true
	LikeString("abc", "abc*", true);       ->   true
	LikeString("abcd", "abc?", true);      ->   true
	LikeString("abc", "abcd", true);       ->   false
@endverbatim */
bool LikeString(LPCTSTR lpszValue, LPCTSTR lpszPattern, bool bCaseInsensitive);

/// Splits a string with a delimiter character into a string list.
///
/// @param[in] lpszSource
///   The string to be splited.
/// @param[in] chDelimiter
///   The delimiter character.
/// @param[in] List
///   Stores the separated strings.
/// @param[in] bTrimResult
///   Indicates whether to trim the result list or not.
///
/// For example (Suppose the delimiter is the comma <tt>","</tt>.) :
/** @verbatim
	""          -> []
	" "         -> [" "]
	","         -> ["", ""]
	"a,b,c"     -> ["a", "b", "c"]
	",a,,b,c,"  -> ["", "a", "", "b", "c", ""]
@endverbatim */
void SplitString(LPCTSTR lpszSource, TCHAR chDelimiter, CStrList& List, bool bTrimResult = false);

/// Formats the string as sprintf does.
CString FormatString(LPCTSTR lpszFormat, ...);

/// Converts a string to a quoted string.
/// For example:
/** @verbatim
	abc         ->     "abc"
	ab'c        ->     "ab'c"
	ab"c        ->     "ab""c"
	(empty)     ->     ""
@endverbatim */
CString GetQuotedStr(LPCTSTR lpszStr, TCHAR chQuote = TEXT('"'));

/// Converts a quoted string to an unquoted string.
///
/// ExtractQuotedStr removes the quote characters from the beginning and end of a quoted string,
/// and reduces pairs of quote characters within the string to a single quote character.
/// The @a chQuote parameter defines what character to use as a quote character. If the first
/// character in @a lpszStr is not the value of the @a chQuote parameter, ExtractQuotedStr returns
/// an empty string.
///
/// The function copies characters from @a lpszStr to the result string until the second solitary
/// quote character or the first null character in @a lpszStr. The @a lpszStr parameter is updated
/// to point to the first character following the quoted string. If @a lpszStr does not contain a
/// matching end quote character, the @a lpszStr parameter is updated to point to the terminating
/// null character.
///
/// For example:
/** @verbatim
	lpszStr(before)    Returned string        lpszStr(after)
	---------------    ---------------        ---------------
	"abc"               abc                    '\0'
	"ab""c"             ab"c                   '\0'
	"abc"123            abc                    123
	abc"                (empty)                abc"
	"abc                abc                    '\0'
	(empty)             (empty)                '\0'
@endverbatim */
CString ExtractQuotedStr(LPCTSTR& lpszStr, TCHAR chQuote = TEXT('"'));

/// Converts a quoted string to an unquoted string.
///
/// GetDequotedStr removes the quote characters from the beginning and end of a quoted string, and
/// reduces pairs of quote characters within the string to a single quote character. The chQuote
/// parameter defines what character to use as a quote character. If the @a lpszStr parameter does
/// not begin and end with the quote character, GetDequotedStr returns @a lpszStr unchanged.
///
/// For example:
/** @verbatim
	"abc"     ->     abc
	"ab""c"   ->     ab"c
	"abc      ->     "abc
	abc"      ->     abc"
	(empty)   ->    (empty)
@endverbatim */
CString GetDequotedStr(LPCTSTR lpszStr, TCHAR chQuote = TEXT('"'));

/// Fetchs a string from the input string.
///
/// @param[in] strInput
///   The source string to be fetched from.
/// @param[in] chDelimiter
///   The delimiter character.
/// @param[in] bDelete
///   Determines whether to delete the fetched string from the input string or not.
/// @return
///   Returns the fetched string.
///
/// For example:
/** @verbatim
	strInput(before)   chDelimiter  bDelete       strInput(after)   Result(after)
	----------------   -----------  ----------    ---------------   -------------
	"abc def"           ' '         false         "abc def"         "abc"
	"abc def"           ' '         true          "def"             "abc"
	" abc"              ' '         false         " abc"            ""
	" abc"              ' '         true          "abc"             ""
	""                  ' '         true/false    ""                ""
@endverbatim */
CString FetchStr(CString& strInput, TCHAR chDelimiter = ' ', bool bDelete = true);

/// @}

//-----------------------------------------------------------------------------

/// @name System Utilities
/// @{

/// Obtains extended information about the version of the operating system that is currently running.
void GetWindowsVer(OSVERSIONINFO& osvi);

/// Obtains extended information about the version of the operating system that is currently running.
OSVERSIONINFO GetWindowsVer();

/// Determines whether the currently running os version number is greater than or equal to the
/// specified version number.
/// @return
///   Returns true if greater than or equal to, false otherwise.
bool CheckWindowsVer(DWORD dwMajorVersion, DWORD dwMinorVersion);

/// Returns the NetBIOS name of the local computer.
/// @note This function is a wrapper of the Windows API "GetComputerName".
CString GetComputerName();

/// Returns the name of the user associated with the current thread.
/// @note This function is a wrapper of the Windows API "GetUserName".
CString GetWinUserName();

/// Returns the path of the Windows directory.
/// @note This function is a wrapper of the Windows API "GetWindowsDirectory".
CString GetWindowsDir();

/// Returns the path of the directory designated for temporary files.
/// @note This function is a wrapper of the Windows API "GetTempPath".
CString GetWinTempDir();

/// Creates a name for a temporary file.
/// If a unique file name is generated, an empty file is created and the handle to it is released;
/// otherwise, only a file name is generated.
///
/// @remarks
///   - The generated temporary file is placed in the Windows temporary directory.
///   - The function uses up to the first three characters of @a lpszPrefix as the prefix of the file name.
/// @note
///   This function is a wrapper of the Windows API "GetTempFileName".
CString GetWinTempFile(LPCTSTR lpszPrefix = TEXT(""));

/// Creates a name for a temporary file under the specified path.
CString GetTempFileName(LPCTSTR lpszPath, LPCTSTR lpszPrefix = TEXT(""));

/// Returns the location of a special folder.
///
/// @param[in] nFolderId
///   Value specifying the folder for which to retrieve the location.
///   The following shows the possible values.
///   - CSIDL_DESKTOP
///   - CSIDL_PROGRAMS
///   - CSIDL_RECENT
///   - CSIDL_SENDTO
///   - CSIDL_STARTMENU
///   - CSIDL_STARTUP
///   - CSIDL_TEMPLATES
///   - CSIDL_APPDATA
///   - ...
/// @return
///   Returns the path with backslash if successful; otherwise returns an empty string.
/// @note
///   To obtain the "Quick Launch" path, Write code like this:
///   @code
///   GetSpecialFolder(CSIDL_APPDATA) + "Microsoft\\Internet Explorer\\Quick Launch\\".
///   @endcode
CString GetSpecialFolder(int nFolderId);

/// Returns the location of the "Quick Launch" folder.
CString GetQuickLaunchPath();

/// Returns the value of the specified environment variable.
CString GetEnvVar(LPCTSTR lpszEnvVar);

/// Returns the boundaries of the usable work area on the primary monitor.
CRect GetScreenWorkAreaRect();

/// Displays a dialog box that allows a user to select a folder.
///
/// @param[in] hwndOwner
///   Handle to the owner window for the dialog box.
/// @param[in] lpszPrompt
///   The string that is displayed above the tree view control in the dialog box.
/// @param[in] bAllowNewFolder
///   Indicates whether the "New Folder" button is visible.
/// @param[in,out] strPath
///   Specifies the initial path, and stores the selected path.
/// @return
///   Returns true if the user clicked OK button; false otherwise.
bool SelectFolderDlg(HWND hwndOwner, LPCTSTR lpszPrompt, bool bAllowNewFolder, CString& strPath);

/// Executes a specified file.
///
/// @param[in] lpszFileName
///   Specifies the file to execute.
/// @param[in] lpszParams
///   If the @a lpszFileName parameter specifies an executable file, @a lpszParams is point to a string
///   that specifies the parameters to be passed to the application.
/// @param[in] lpszDefaultDir
///   The string that specifies the default directory.
/// @param[in] nShowCmd
///   Specifies how an application is to be displayed when it is opened. It may be SW_SHOW, SW_SHOWNORMAL,
///   SW_MINIMIZE, SW_MAXIMIZE, and so on.
/// @param[out] pHandle
///   If @a pHandle is not NULL, it will be store the value returned by API ShellExecute.
/// @return
///   Returns true is the operation is successful; false otherwise.
/// @note
///   This function performs an operation by calling the Windows API "ShellExecute".
bool ExecuteFile(LPCTSTR lpszFileName, LPCTSTR lpszParams, LPCTSTR lpszDefaultDir, int nShowCmd,
	HINSTANCE *pHandle = NULL);

/// Opens an URL using the default program.
/// @return
///   Returns true is the operation is successful; false otherwise.
///
/// For example:
/** @code
	OpenUrl("http://www.abc.com");
	OpenUrl("www.abc.com");
	OpenUrl("file:///c:\\");
@endcode */
bool OpenUrl(LPCTSTR lpszUrl);

/// Opens a Email client using the default program.
/// @return
///   Returns true is the operation is successful; false otherwise.
///
/// For example:
/** @code
	OpenEmailClient("haoxingeng@gmail.com");
	OpenEmailClient("mailto:haoxingeng@gmail.com");
@endcode */
bool OpenEmailClient(LPCTSTR lpszEmail);

/// Sets the specified window as top-most-window or not.
void SetStayOnTop(HWND hWnd, bool bStayOnTop);

/// Enables or disables a privilege for the current process.
///
/// @param[in] lpszPrivilegeName
///   The string that specifies the name of the privilege.
///   The possible values: SE_SHUTDOWN_NAME, SE_BACKUP_NAME, etc.
/// @param[in] bEnable
///   Specifies whether to enable the privilege or disable the privilege.
/// @return
///   Returns true if successful; false otherwise.
bool SetProcessPrivilege(LPCTSTR lpszPrivilegeName, bool bEnable);

/// Shuts down the system.
/// This function enables the SE_SHUTDOWN_NAME privilege before shutting down the system.
bool ShutdownWindows();

/// Creates a shortcut to a file or a folder.
///
/// @param[in] lpszOriginalFileName
///   The path and name of the existing file.
/// @param[in] lpszShortcutFileName
///   The file name of the shortcut to be created. This parameter must end with the extension ".lnk".
/// @param[in] lpszArguments
///   The command-line arguments.
/// @param[in] lpszWorkingDir
///   The working directory.
/// @param[in] lpszDescription
///   The description of the shortcut.
/// @param[in] nShowCmd
///   The show command for the shortcut.
/// @param[in] bIsFolderShortcut
///   Specifies whether the shortcut is linked to a folder or not.
/// @param[in] lpszIconFileName
///   Specifies the icon file.
/// @param[in] nIconIndex
///   Specifies the 0-based icon index.
/// @return
///   Returns true if successful; false otherwise.
///
/// For example:
/** @code
	CreateShortCut("c:\\test.txt", "c:\\test.lnk", "", "c:\\", "This is test.txt", SW_SHOW, false);
	CreateShortCut("c:\\mydir", "c:\\mydir.lnk", "", "", "This is mydir folder", SW_SHOW, true);
@endcode */
bool CreateShortCut(LPCTSTR lpszOriginalFileName, LPCTSTR lpszShortcutFileName, LPCTSTR lpszArguments,
	LPCTSTR lpszWorkingDir, LPCTSTR lpszDescription, int nShowCmd, bool bIsFolderShortcut,
	LPCTSTR lpszIconFileName = NULL, int nIconIndex = 0);

/// Returns the error message for the Window API GetLastError.
/// @note This function is a wrapper of the Windows API "FormatMessage".
CString SysErrorMessage(int nErrCode);

/// Converts the FILETIME type to SYSTEMTIME type.
SYSTEMTIME FileTimeToLocalSystemTime(const FILETIME& tFileTime);

/// Converts the SYSTEMTIME type to FILETIME type.
FILETIME LocalSystemTimeToFileTime(const SYSTEMTIME& tSystemTime);

/// Creates a GUID.
/// @note This function is a wrapper of the Windows API "CoCreateGuid".
bool CreateGuid(GUID& Guid);

/// Converts a string generated by the GuidToString() function back into the original GUID.
/// @note This function is a wrapper of the Windows API "CLSIDFromString".
bool StringToGuid(LPCTSTR str, GUID& Guid);

/// Converts a GUID to a string.
/// @note This function is a wrapper of the Windows API "StringFromCLSID".
CString GuidToString(const GUID& Guid);

/// Gets the process handle by the specified process name.
/// @note If the function fails, the return value is a INVALID_HANDLE_VALUE.
HANDLE GetProcessHandle(LPCTSTR lpszProcessName, DWORD nDesiredAccess = PROCESS_ALL_ACCESS);

/// Kills the processes of the specified name.
///
/// For example:
/** @code
	TerminateProcesses("notepad.exe");
@endcode */
void TerminateProcesses(LPCTSTR lpszProcessName);

/// Retrieves the file version information of the specified executable file (*.exe,*.dll,*.ocx, etc.).
/// @return
///   Returns the version string such as "1.0.1.2".
CString GetExeFileVersion(LPCTSTR lpszFileName);

/// Converts the version string to a 64bit integer value.
///
/// For example:
/** @verbatim
	"1.2.3.4"  ->  281483566841860
@endverbatim */
bool VerStrToInt64(LPCTSTR lpszVersion, UINT64& nValue);

/// Converts the 64bit integer value to a version string.
///
/// For example:
/** @verbatim
	281483566841860  ->  "1.2.3.4"
@endverbatim */
CString Int64ToVerStr(UINT64 nValue);

/// Compares two version numbers.
/// @return
///   - -1, if @a lpszVer1 < @a lpszVer2.
///   -  0, if @a lpszVer1 = @a lpszVer2.
///   -  1, if @a lpszVer1 > @a lpszVer2.
///
/// For example:
/** @code
	int r;
	r = CompareVersion("1.0.0.3", "1.0.0.2");
	ASSERT(r == 1);
	r = CompareVersion("1.0.0.3", "1.0.0.13");
	ASSERT(r == -1);
@endcode */
int CompareVersion(LPCTSTR lpszVer1, LPCTSTR lpszVer2);

/// @}

//-----------------------------------------------------------------------------

/// @name File Utilities
/// @{

/// Retrieves the size in bytes of the specified file.
/// @return
///   - The size in bytes of the file, or
///   - (-1), if an error occurs.
INT64 GetFileSize(LPCTSTR lpszFileName);

/// Rtrieves the date and time a file was last modified.
CTime GetFileDate(LPCTSTR lpszFileName);

/// Sets the date and time of a file.
bool SetFileDate(LPCTSTR lpszFileName, const CTime& tCreationTime,
	const CTime& tLastAccessTime, const CTime& tLastWriteTime);

/// Copies the specified file to a destination folder.
bool CopyFileToFolder(LPCTSTR lpszFileName, LPCTSTR lpszDestFolder);

/// Returns a unique file name by adding a suffix at the end of the specified name automatically.
/// For example:
/** @verbatim
	AutoRenameFileName("c:\\test.txt");

	If the file "c:\test.txt" already exists, the function returns "c:\\test(1).txt".
	Otherwise, the function returns "c:\\test.txt".
	If "c:\test.txt" and "c:\test(1).txt" exist, the function returns "c:\\test(2).txt".
@endverbatim */
CString AutoRenameFileName(LPCTSTR lpszFullName);

/// Determines whether a file is read-only.
/// @return
///   Returns true if the file is read-only; false otherwise.
bool IsFileReadOnly(LPCTSTR lpszFileName);

/// Sets the file permissions to read-only.
/// @return
///   Returns true if successful; false otherwise.
bool FileSetReadOnly(LPCTSTR lpszFileName, bool bReadOnly);

/// Tests if a specified file exists.
/// @return
///   Returns true if the file exists; false otherwise.
bool FileExists(LPCTSTR lpszFileName);

/// Changes a file name.
/// @return
///   Returns true if successful; false otherwise.
bool RenameFile(LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName);

/// Deletes a file even if it is readonly.
/// @return
///   Returns true if successful; false otherwise.
bool RemoveFile(LPCTSTR lpszFileName);

/// Determines whether a specified directory exists.
/// @return
///   Returns true if the directory exists; false otherwise.
bool DirectoryExists(LPCTSTR lpszDir);

/// Creates a new directory.
/// @return
///   Returns true if successful; false otherwise.
bool CreateDir(LPCTSTR lpszDir);

/// Deletes an existing empty directory.
/// @param[in] lpszDir
///   The path name of the directory to be removed.
/// @param[in] bRecursive
///   Determines whether to remove the directory recursively.
/// @return
///   Returns true if successful; false otherwise.
bool RemoveDir(LPCTSTR lpszDir, bool bRecursive = false);

/// Creates a new directory, including the creation of parent directories as needed.
/// @return
///   Returns true if successful; false otherwise.
///
/// For example:
/** @verbatim
	ForceDirectories("c:\\a\\b\\c\\d\\e\\f\\g\\");
@endverbatim */
bool ForceDirectories(LPCTSTR lpszDir);

/// Returns the full path name for a relative file name.
/// For example:
/** @verbatim
	ExpandFileName("test.dat");               ->   "c:\\test.dat"     (if the current directory is c:\)
	ExpandFileName("c:\\a\\..\\test.dat");    ->   "c:\\test.dat"
@endverbatim */
CString ExpandFileName(LPCTSTR lpszFileName);

/// Returns the short path form of the specified file name.
/// For example:
/** @verbatim
	GetShortFileName("C:\\Program Files");   ->   "C:\\PROGRA~1"
@endverbatim */
CString GetShortFileName(LPCTSTR lpszFileName);

/// Returns the long path form of the specified file name.
/// For example:
/** @verbatim
	GetLongFileName("C:\\PROGRA~1\\COMMON~1\\");   ->   "C:\\Program Files\\Common Files\\"
@endverbatim */
CString GetLongFileName(LPCTSTR lpszFileName);

/// @}

//-----------------------------------------------------------------------------

/// @name Registry Utilities
/// @{

/// Sets whether the application to be run automatically on window startup.
///
/// @param[in] bAutoRun
///   Specifies whether to run the application automatically.
/// @param[in] bCurrentUser
///   Specifies whether this operation is affect the current user.
/// @param[in] lpszAppTitle
///   The application title to be written to the registry.
/// @param[in] lpszAppParam
///   The command-line arguments for the application.
/// @return
///   Returns true if successful; false otherwise.
///
/// For example:
/** @verbatim
	SetAutoRunOnStartup(true, true, "MyApp", "");
@endverbatim */
bool SetAutoRunOnStartup(bool bAutoRun, bool bCurrentUser, LPCTSTR lpszAppTitle,
	LPCTSTR lpszAppParam = TEXT(""));

/// Associates the application with a specified file extension.
///
/// @param[in] lpszFileExt
///   The file extension including the period.
/// @param[in] lpszFileKey
///   The key name of the file extension.
/// @param[in] lpszAppName
///   The application name to be used to display on the context menu.
/// @param[in] lpszFileDesc
///   A simple description of the application.
/// @param[in] nIconIndex
///   The 0-based icon index.
/// @param[in] bFlush
///   Determines whether to flush the system cache after the operation.
/// @return
///   Returns true if successful; false otherwise.
///
/// For example:
/** @code
	AssociateFile(".edf", "EDiaryFile", "EDiary", "This is EDiary file", 0, true);
@endcode */
bool AssociateFile(LPCTSTR lpszFileExt, LPCTSTR lpszFileKey, LPCTSTR lpszAppName, LPCTSTR lpszFileDesc,
	int nIconIndex = 0, bool bFlush = false);

/// Registers an URL protocol.
///
/// @param[in] lpszProtocolName
///   The protocol name. (e.g. "svn", "telnet")
/// @param[in] lpszExeFileName
///   The executable handler file name including path.
/// @param[in] nIconIndex
///   The 0-based icon index.
/// @return
///   Returns true if successful; false otherwise.
///
/// For example:
/** @code
	RegisterUrlProtocol("svn", "c:\\test\\mytest.exe", 0);
@endcode */
bool RegisterUrlProtocol(LPCTSTR lpszProtocolName, LPCTSTR lpszExeFileName, int nIconIndex = 0);

/// @}

//-----------------------------------------------------------------------------

/// @name Algorithm Utilities
/// @{

/// Performs base-64 encoding.
///
/// @param[in] pSrcData
///   The buffer containing the data to be encoded.
/// @param[in] nSrcSize
///   The length in bytes of the data to be encoded.
/// @return
///   Returns the encoded data.
CString EncodeBase64(const char *pSrcData, int nSrcSize);

/// Performs base-64 decoding.
///
/// @param[in] lpszSrcData
///   The string containing the data to be decoded.
/// @param[in] pDestBuf
///   The buffer to receive the decoded data.
/// @param[in,out] nDestSize
///   Refer to a variable that contains the length in bytes of @a pDestBuf. If the function succeeds,
///   the variable receives the number of bytes written to the buffer.
/// @remarks
///   The recommended size of the buffer @a pDestBuf is <b><tt>(_tcsclen(lpszSrcData))</tt></b>.
void DecodeBase64(LPCTSTR lpszSrcData, PBYTE pDestBuf, int& nDestSize);

/// Performs base-64 decoding.
///
/// @param[in] lpszSrcData
///   The string containing the data to be decoded.
/// @param[in] DestBuf
///   The buffer to receive the decoded data.
void DecodeBase64(LPCTSTR lpszSrcData, CBuffer& DestBuf);

/// Performs base-16 encoding.
///
/// @param[in] pSrcData
///   The buffer containing the data to be encoded.
/// @param[in] nSrcSize
///   The length in bytes of the data to be encoded.
/// @return
///   Returns the encoded data.
CString EncodeBase16(const char *pSrcData, int nSrcSize);

/// Performs base-16 decoding.
///
/// @param[in] lpszSrcData
///   The string containing the data to be decoded.
/// @param[in] pDestBuf
///   The buffer to receive the decoded data.
/// @param[in,out] nDestSize
///   Refer to a variable that contains the length in bytes of @a pDestBuf. If the function succeeds,
///   the variable receives the number of bytes written to the buffer.
/// @remarks
///   The recommended size of the buffer @a pDestBuf is <b><tt>(_tcsclen(lpszSrcData) / 2)</tt></b>.
void DecodeBase16(LPCTSTR lpszSrcData, PBYTE pDestBuf, int& nDestSize);

/// Performs base-16 decoding.
///
/// @param[in] lpszSrcData
///   The string containing the data to be decoded.
/// @param[in] DestBuf
///   The buffer to receive the decoded data.
void DecodeBase16(LPCTSTR lpszSrcData, CBuffer& DestBuf);

/// Encodes the source string to a URL-encoded string.
CString UrlEncode(LPCTSTR lpszSource);

/// Decodes a URL-encoded string.
CString UrlDecode(LPCTSTR lpszSource);

/// Converts the source string to a utf-8 string.
CStringA Utf8Encode(LPCTSTR lpszSource);

/// @}

//-----------------------------------------------------------------------------

/// @name Other Utilities
/// @{

/// Returns the min value.
template <typename T>
inline const T& Min(const T& a, const T& b) { return ((a < b)? a : b); }

/// Returns the max value.
template <typename T>
inline const T& Max(const T& a, const T& b) { return ((a < b)? b : a); }

/// Returns a value that is guaranteed to be within a specified range.
template <typename T>
inline const T& EnsureRange(const T& Value, const T& Min, const T& Max)
	{ return (Value > Max) ? Max : (Value < Min ? Min : Value); }

/// Exchanges two values.
template <typename T>
inline void Swap(T& v1, T& v2)
	{ T temp; temp = v1; v1 = v2; v2 = temp; }

/// Compares two values.
template <typename T>
inline int Compare(const T& v1, const T& v2)
{
	if (v1 < v2) return -1;
	else if (v1 > v2) return 1;
	else return 0;
}

/// Performs byte swapping (i.e. conversion between little-endian and big-endian representations)
/// of different data types.
template<typename T>
inline void ByteSwap(T& v)
{
	BYTE *s = (BYTE*)&v;
	for (BYTE *d = s + sizeof(v) - 1; s < d; ++s, --d)
		*s ^= *d, *d ^= *s, *s ^= *d;
}

/// Returns a CRect object from the top-left coordinate, and width and height values of a rectangle.
inline CRect GetBounds(int nLeft, int nTop, int nWidth, int nHeight)
	{ return CRect(nLeft, nTop, nLeft + nWidth, nTop + nHeight); }

/// Calculates the elapsed tick count using the @a nOldTickCount and nNewTickCount.
///
/// @a nOldTickCount and @a nNewTickCount are the starting and ending tick counts as returned by the
/// Windows API GetTickCount.
///
/// GetTickDiff exists because a ticks value can rollover to zero (0) when it exceeds the maximum
/// value represented in the DWORD data type. GetTickDiff handles this situation in calculating the
/// elapsed tick count.
UINT GetTickDiff(UINT nOldTickCount, UINT nNewTickCount);

/// Randomize the random seed.
void Randomize();

/// Returns a random number in the range @a nMin to @a nMax.
int GetRandom(int nMin, int nMax);

/// Returns a 64-bit random number in the range @a nMin to @a nMax.
INT64 GetRandom64(INT64 nMin, INT64 nMax);

/// Generates a random number list of non-repetition in the range @a nStartNumber to @a nEndNumber.
///
/// @param[in] nStartNumber
///   The lower bound of the random number list.
/// @param[in] nEndNumber
///   The upper bound of the random number list.
/// @param[in] pRandomList
///   Stores the generated random number list.
/// @remarks
///   The element count of @a pRandomList must greater than or equal to (nEndNumber - nStartNumber + 1).
void GenerateRandomList(int nStartNumber, int nEndNumber, int *pRandomList);

/// Returns the path of this application.
CString GetAppPath();

/// Returns the sub path of this application.
CString GetAppSubPath(LPCTSTR lpszSubFolder = TEXT(""));

/// Return the executable file name of this application.
CString GetAppExeName(bool bIncludePath = true);

/// Returns the file name of this module (dll/ocx/...).
CString GetModuleExeName(HMODULE hModule = NULL, bool bIncludePath = true);

/// Parses the specified command line, stores the arguments to @a ArgList, and returns the number of arguments.
/// For example:
/** @code
	c:\test.exe abc "def" "xy z"     ->  ["abc", "def", "xy z"]
@endcode */
int ParseCommandLine(LPCTSTR lpszCmdLine, CStrList& ArgList);

/// Parses the current command line, stores the arguments to @a ArgList, and returns the number of arguments.
int ParseCommandLine(CStrList& ArgList);

/// A simple message box.
int ShowMessage(LPCTSTR lpszMsg, UINT nType = (MB_OK | MB_ICONINFORMATION));
/// A simple message box to display an integer.
void ShowMessage(INT64 nValue);

/// @}

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)

/// @}
