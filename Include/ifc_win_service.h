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

/// @file ifc_win_service.h
/// Defines the Windows Service classes.

#pragma once

#include <windows.h>
#include <winsvc.h>

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"
#include "ifc_exceptions.h"

/// The namespace of IFC.
namespace ifc
{

/// @addtogroup Classes
/// @{

///////////////////////////////////////////////////////////////////////////////
// CWinService - The windows service class.

class CWinService
{
private:
	static CWinService* s_pService;
	CString m_strServiceName;
	CString m_strServiceDisplayName;
	CString m_strServiceDescription;
	CString m_strServiceArguments;
	CString m_strServiceDependencies;
	SERVICE_STATUS m_ServiceStatus;
	SERVICE_STATUS_HANDLE m_hStatusHandle;
	bool m_bCanPause;
	bool m_bCanStop;
	bool m_bDebugMode;
private:
	static bool InternalStopService(SC_HANDLE hSvcHandle);
	static void WINAPI ServiceMainCallback(DWORD argc, LPTSTR *argv);
	static void WINAPI ServiceCtrlCallback(DWORD nCtrlCode);
	static BOOL WINAPI ConsoleCtrlCallback(DWORD nCtrlType);

	CString ValidateServiceArgs(LPCTSTR lpszArgs);
	bool IsServiceArguments(DWORD argc, LPTSTR *argv);
	int StartDispatcher();
	void ServiceMain(DWORD argc, LPTSTR *argv);
	void ServiceControl(DWORD nCtrlCode);
	bool ConsoleControl(DWORD nCtrlType);
	void Debug(DWORD argc, LPTSTR *argv);
protected:
	/// Writes the message to the event log.
	void LogEvent(DWORD nCode, LPCTSTR lpszMsg);
	/// Reports the service status to SCM.
	bool ReportStatusToSCM(DWORD nCurrentState, DWORD nWin32ExitCode = NO_ERROR, DWORD nWaitHint = 0);
protected:
	/// Performs initialization before running.
	virtual bool OnInitialize(DWORD argc, LPTSTR *argv) { return true; }
	/// Performs finalization after running.
	virtual void OnFinalize() {}
	/// Override the method to start the service.
	virtual void OnStart() {}
	/// Override the method to stop the service.
	virtual void OnStop() {}
	/// Override the method to pause the service.
	virtual void OnPause() {}
	/// Override the method to continue the service.
	virtual void OnContinue() {}
	/// Override the method to run the program as non-service.
	virtual void OnRunAsNonService(DWORD argc, LPTSTR *argv);
public:
	/// Installs the service.
	static bool InstallService(LPCTSTR lpszServiceName, LPCTSTR lpszServiceDisplayName,
		LPCTSTR lpszServiceDescription, LPCTSTR lpszExeFileName, LPCTSTR lpszServiceArguments,
		LPCTSTR lpszServiceDependencies);
	/// Stops and removes the service.
	static bool UninstallService(LPCTSTR lpszServiceName);
	/// Indicates whether the service has been installed or not.
	static bool IsServiceInstalled(LPCTSTR lpszServiceName);
	/// Starts the service.
	static bool StartService(LPCTSTR lpszServiceName, DWORD argc, LPTSTR *argv);
	/// Stops the service.
	static bool StopService(LPCTSTR lpszServiceName);
	/// Gets the full filename including the path of the service.
	static bool GetServiceFileName(LPCTSTR lpszServiceName, CString& strServiceFileName);
public:
	/// Constructor.
	///
	/// @param[in] lpszServiceName
	///   The name of the service being registered.
	/// @param[in] lpszServiceDisplayName
	///   The display name to be used by user interface programs to identify the service.
	/// @param[in] lpszServiceDescription
	///   The service description to be used by user interface programs.
	/// @param[in] lpszServiceArguments
	///   The command arguments of the service program. For example, the RPC service
	///   needs the arguments "-k rpcss".
	/// @param[in] lpszServiceDependencies
	///   Pointer to a double null-terminated array of null-separated names of services or
	///   load ordering groups that the system must start before this service. Specify NULL
	///   or an empty string if the service has no dependencies.
	/// @param[in] bCanPause
	///   Determines whether the service can be paused and contined.
	/// @param[in] bCanStop
	///   Determines whether the service can be stopped.
	CWinService(LPCTSTR lpszServiceName, LPCTSTR lpszServiceDisplayName,
		LPCTSTR lpszServiceDescription, LPCTSTR lpszServiceArguments = TEXT(""),
		LPCTSTR lpszServiceDependencies = TEXT(""),
		bool bCanPause = false, bool bCanStop = true);
	/// Destructor
	virtual ~CWinService();

	/// The starting point of execution for the service program.
	/// @remarks
	///   Call this method in your C++ main function.
	int Main(int argc, LPTSTR *argv);

	/// Installs the service into the Service Control Manager (SCM) database.
	bool Install();
	/// Stops and removes the service.
	bool Uninstall();
	/// Indicates whether the service has been installed or not.
	bool IsInstalled();
	/// Starts the service.
	bool Start();
	/// Stops the service.
	bool Stop();
};

///////////////////////////////////////////////////////////////////////////////

/// @}

} // namespace ifc
