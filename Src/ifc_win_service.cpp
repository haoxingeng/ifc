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

/// @file ifc_win_service.cpp

#include "stdafx.h"
#include <iostream>

#include "ifc_win_service.h"
#include "ifc_sysutils.h"

using namespace std;

namespace ifc
{

///////////////////////////////////////////////////////////////////////////////

CWinService* CWinService::s_pService = NULL;

//-----------------------------------------------------------------------------

CWinService::CWinService(LPCTSTR lpszServiceName, LPCTSTR lpszServiceDisplayName,
	LPCTSTR lpszServiceDescription, LPCTSTR lpszServiceArguments,
	LPCTSTR lpszServiceDependencies, bool bCanPause, bool bCanStop)
{
	m_strServiceName = lpszServiceName;
	m_strServiceDisplayName = lpszServiceDisplayName;
	m_strServiceDescription = lpszServiceDescription;
	m_strServiceArguments = ValidateServiceArgs(lpszServiceArguments);
	m_strServiceDependencies = lpszServiceDependencies;

	memset(&m_ServiceStatus, 0, sizeof(m_ServiceStatus));
	m_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	m_hStatusHandle = NULL;

	m_bCanPause = bCanPause;
	m_bCanStop = bCanStop;
	m_bDebugMode = false;

	s_pService = this;
}

//-----------------------------------------------------------------------------

CWinService::~CWinService()
{
	s_pService = NULL;
}

//-----------------------------------------------------------------------------

bool CWinService::InternalStopService(SC_HANDLE hSvcHandle)
{
	const int MAX_WAIT_MS = 30*1000;
	const int WAIT_STEP_MS = 50;

	SERVICE_STATUS SvcStatus;
	::QueryServiceStatus(hSvcHandle, &SvcStatus);

	if (SvcStatus.dwCurrentState != SERVICE_STOPPED)
	{
		DWORD nStartTicks = GetTickCount();
		if (::ControlService(hSvcHandle, SERVICE_CONTROL_STOP, &SvcStatus))
		{
			Sleep(1000);

			while (::QueryServiceStatus(hSvcHandle, &SvcStatus))
			{
				if (SvcStatus.dwCurrentState == SERVICE_STOPPED)
					break;

				if (GetTickDiff(nStartTicks, GetTickCount()) > MAX_WAIT_MS)
					break;

				Sleep(WAIT_STEP_MS);
			}
		}
	}

	return (SvcStatus.dwCurrentState == SERVICE_STOPPED);
}

//-----------------------------------------------------------------------------

void WINAPI CWinService::ServiceMainCallback(DWORD argc, LPTSTR *argv)
{
	if (s_pService)
		s_pService->ServiceMain(argc, argv);
}

//-----------------------------------------------------------------------------

void WINAPI CWinService::ServiceCtrlCallback(DWORD nCtrlCode)
{
	if (s_pService)
		s_pService->ServiceControl(nCtrlCode);
}

//-----------------------------------------------------------------------------

BOOL WINAPI CWinService::ConsoleCtrlCallback(DWORD nCtrlType)
{
	if (s_pService)
		return s_pService->ConsoleControl(nCtrlType);
	else
		return FALSE;
}

//-----------------------------------------------------------------------------

CString CWinService::ValidateServiceArgs(LPCTSTR lpszArgs)
{
	CString strCmdLine = GetQuotedStr(GetAppExeName()) + TEXT(" ") + lpszArgs;
	CStrList StrList;
	ParseCommandLine(strCmdLine, StrList);

	CString strResult;
	for (int i = 0; i < StrList.GetCount(); ++i)
	{
		CString str = StrList[i];
		str.Trim();
		if (str.Find(' ') >= 0)
			str = '\"' + str + '\"';

		if (!str.IsEmpty())
		{
			if (!strResult.IsEmpty()) strResult += ' ';
			strResult += str;
		}
	}

	return strResult;
}

//-----------------------------------------------------------------------------

bool CWinService::IsServiceArguments(DWORD argc, LPTSTR *argv)
{
	CString strArgs;
	for (UINT i = 1; i < argc; ++i)
	{
		CString str = argv[i];
		str.Trim();
		if (str.Find(' ') >= 0)
			str = '\"' + str + '\"';

		if (!strArgs.IsEmpty()) strArgs += ' ';
		strArgs += str;
	}

	return SameText(ValidateServiceArgs(strArgs), m_strServiceArguments);
}

//-----------------------------------------------------------------------------

int CWinService::StartDispatcher()
{
	int nResult = 0;
	SERVICE_TABLE_ENTRY DispatchTable[] = {
		{ (LPTSTR)(LPCTSTR)m_strServiceName, ServiceMainCallback },
		{ NULL, NULL }
	};

	if (!::StartServiceCtrlDispatcher(DispatchTable))
	{
		nResult = ::GetLastError();
		LogEvent(nResult, TEXT("StartServiceCtrlDispatcher failed."));
	}

	return nResult;
}

//-----------------------------------------------------------------------------

void CWinService::ServiceMain(DWORD argc, LPTSTR *argv)
{
	try
	{
		m_hStatusHandle = ::RegisterServiceCtrlHandler(m_strServiceName, ServiceCtrlCallback);
		if (m_hStatusHandle && ReportStatusToSCM(SERVICE_START_PENDING))
		{
			if (OnInitialize(argc, argv))
			{
				ReportStatusToSCM(SERVICE_RUNNING);
				OnStart();
			}
		}
	}
	catch (IFC_EXCEPT_OBJ e)
	{
		IFC_DELETE_MFC_EXCEPT_OBJ(e);
	}
	catch (...)
	{}

	OnFinalize();
	ReportStatusToSCM(SERVICE_STOPPED);
}

//-----------------------------------------------------------------------------

void CWinService::ServiceControl(DWORD nCtrlCode)
{
	switch (nCtrlCode)
	{
	case SERVICE_CONTROL_STOP:
		ReportStatusToSCM(SERVICE_STOP_PENDING);
		OnStop();
		return;

	case SERVICE_CONTROL_PAUSE:
		ReportStatusToSCM(SERVICE_PAUSE_CONTINUE);
		OnPause();
		return;

	case SERVICE_CONTROL_CONTINUE:
		ReportStatusToSCM(SERVICE_CONTINUE_PENDING);
		OnContinue();
		return;

	case SERVICE_CONTROL_SHUTDOWN:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;
	}

	ReportStatusToSCM(m_ServiceStatus.dwCurrentState);
}

//-----------------------------------------------------------------------------

bool CWinService::ConsoleControl(DWORD nCtrlType)
{
	switch (nCtrlType)
	{
	case CTRL_BREAK_EVENT:
	case CTRL_C_EVENT:
		OnStop();
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------

void CWinService::Debug(DWORD argc, LPTSTR *argv)
{
	wcout << "Debugging " << m_strServiceName.GetString() << "..." << endl;

	::SetConsoleCtrlHandler(ConsoleCtrlCallback, TRUE);

	try
	{
		if (OnInitialize(argc, argv))
			OnStart();
	}
	catch (IFC_EXCEPT_OBJ e)
	{
		IFC_DELETE_MFC_EXCEPT_OBJ(e);
	}
	catch (...)
	{}

	OnFinalize();
}

//-----------------------------------------------------------------------------

void CWinService::LogEvent(DWORD nCode, LPCTSTR lpszMsg)
{
	if (!m_bDebugMode)
	{
		CString strMsg;
		strMsg.Format(TEXT("%s error: %d"), m_strServiceName, nCode);

		LPCTSTR lpszStrings[2];
		lpszStrings[0] = strMsg;
		lpszStrings[1] = lpszMsg;

		HANDLE hEventSource = ::RegisterEventSource(NULL, m_strServiceName);
		if (hEventSource != NULL)
		{
			::ReportEvent(
				hEventSource,          // handle of event source
				EVENTLOG_ERROR_TYPE,   // event type
				0,                     // event category
				0,                     // event ID
				NULL,                  // current user's SID
				2,                     // strings in lpszStrings
				0,                     // no bytes of raw data
				(LPCTSTR*)lpszStrings, // array of error strings
				NULL);                 // no raw data

			::DeregisterEventSource(hEventSource);
		}
	}
}

//-----------------------------------------------------------------------------

bool CWinService::ReportStatusToSCM(DWORD nCurrentState, DWORD nWin32ExitCode, DWORD nWaitHint)
{
	static DWORD nCheckPoint = 1;
	bool bResult = true;

	if (!m_bDebugMode)
	{
		if (nWaitHint == 0 && (
			nCurrentState == SERVICE_CONTINUE_PENDING ||
			nCurrentState == SERVICE_PAUSE_PENDING ||
			nCurrentState == SERVICE_START_PENDING ||
			nCurrentState == SERVICE_STOP_PENDING) )
		{
			nWaitHint = 3000;
		}

		if (nCurrentState == SERVICE_START_PENDING)
			m_ServiceStatus.dwControlsAccepted = 0;
		else
		{
			m_ServiceStatus.dwControlsAccepted = 0;
			if (m_bCanPause)
				m_ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
			if (m_bCanStop)
				m_ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_STOP;
		}

		m_ServiceStatus.dwCurrentState = nCurrentState;
		m_ServiceStatus.dwWin32ExitCode = nWin32ExitCode;
		m_ServiceStatus.dwWaitHint = nWaitHint;

		if ((nCurrentState == SERVICE_RUNNING) || (nCurrentState == SERVICE_STOPPED))
			m_ServiceStatus.dwCheckPoint = 0;
		else
			m_ServiceStatus.dwCheckPoint = nCheckPoint++;

		if (m_hStatusHandle)
		{
			if (!(bResult = ::SetServiceStatus(m_hStatusHandle, &m_ServiceStatus) != 0))
				LogEvent(GetLastError(), TEXT("SetServiceStatus"));
		}
	}

	return bResult;
}

//-----------------------------------------------------------------------------

void CWinService::OnRunAsNonService(DWORD argc, LPTSTR *argv)
{
	if (argc == 1)
	{
		Install();
		Stop();
		Start();
	}
}

//-----------------------------------------------------------------------------

bool CWinService::InstallService(LPCTSTR lpszServiceName, LPCTSTR lpszServiceDisplayName,
	 LPCTSTR lpszServiceDescription, LPCTSTR lpszExeFileName, LPCTSTR lpszServiceArguments,
	 LPCTSTR lpszServiceDependencies)
{
	bool bResult = false;
	SC_HANDLE hSvcHandle;
	SC_HANDLE hScmHandle;

	CString strCmdLine = lpszExeFileName;
	if (strCmdLine.Find(' ') >= 0)
		strCmdLine = TEXT("\"") + strCmdLine + TEXT("\"");

	if (lpszServiceArguments && *lpszServiceArguments)
		strCmdLine += FormatString(TEXT(" %s"), lpszServiceArguments);

	hScmHandle = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
	if (hScmHandle)
	{
		hSvcHandle = ::CreateService(
			hScmHandle,                 // SCM database
			lpszServiceName,            // name of service
			lpszServiceDisplayName,     // name to display
			SERVICE_QUERY_STATUS | SERVICE_CHANGE_CONFIG | SERVICE_START,  // desired access
			SERVICE_WIN32_OWN_PROCESS,  // service type
			SERVICE_AUTO_START,//SERVICE_DEMAND_START,       // start type
			SERVICE_ERROR_NORMAL,       // error control type
			strCmdLine,                 // service's binary path name
			NULL,                       // no load ordering group
			NULL,                       // no tag identifier
			lpszServiceDependencies,	// dependencies
			NULL,                       // LocalSystem account
			NULL);                      // no password

		if (hSvcHandle)
		{
			SC_ACTION scAction[2];
			SERVICE_FAILURE_ACTIONS sfa;
			sfa.lpsaActions = scAction;
			sfa.lpRebootMsg = NULL;
			sfa.lpCommand = NULL;
			sfa.dwResetPeriod = 0;
			sfa.cActions = 2;
			scAction[0].Delay = 60 * 1000;
			scAction[0].Type = SC_ACTION_REBOOT;
			scAction[1].Delay = 60 * 1000;
			scAction[1].Type = SC_ACTION_RESTART;

			SERVICE_DESCRIPTION sd;
			sd.lpDescription = (LPTSTR)lpszServiceDescription;

			::ChangeServiceConfig2(hSvcHandle, SERVICE_CONFIG_FAILURE_ACTIONS, &sfa);
			::ChangeServiceConfig2(hSvcHandle, SERVICE_CONFIG_DESCRIPTION, &sd);

			::CloseServiceHandle(hSvcHandle);

			bResult = true;
		}

		::CloseServiceHandle(hScmHandle);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CWinService::UninstallService(LPCTSTR lpszServiceName)
{
	bool bResult = false;
	SC_HANDLE hSvcHandle;
	SC_HANDLE hScmHandle;

	hScmHandle = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hScmHandle)
	{
		hSvcHandle = ::OpenService(hScmHandle, lpszServiceName, DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);
		if (hSvcHandle)
		{
			InternalStopService(hSvcHandle);
			bResult = ::DeleteService(hSvcHandle) != 0;
			::CloseServiceHandle(hSvcHandle);
		}
		::CloseServiceHandle(hScmHandle);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CWinService::IsServiceInstalled(LPCTSTR lpszServiceName)
{
	bool bResult = false;
	SC_HANDLE hSvcHandle;
	SC_HANDLE hScmHandle;

	hScmHandle = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hScmHandle)
	{
		hSvcHandle = ::OpenService(hScmHandle, lpszServiceName, SERVICE_QUERY_CONFIG);
		if (hSvcHandle != NULL)
		{
			bResult = true;
			::CloseServiceHandle(hSvcHandle);
		}
		::CloseServiceHandle(hScmHandle);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CWinService::StartService(LPCTSTR lpszServiceName, DWORD argc, LPTSTR *argv)
{
	bool bResult = false;
	SC_HANDLE hSvcHandle;
	SC_HANDLE hScmHandle;

	hScmHandle = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hScmHandle)
	{
		hSvcHandle = ::OpenService(hScmHandle, lpszServiceName, SERVICE_START);
		if (hSvcHandle)
		{
			bResult = ::StartService(hSvcHandle, argc, (LPCTSTR*)argv) != 0;
			::CloseServiceHandle(hSvcHandle);
		}
		::CloseServiceHandle(hScmHandle);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CWinService::StopService(LPCTSTR lpszServiceName)
{
	bool bResult = false;
	SC_HANDLE hSvcHandle;
	SC_HANDLE hScmHandle;

	hScmHandle = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hScmHandle)
	{
		hSvcHandle = ::OpenService(hScmHandle, lpszServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
		if (hSvcHandle)
		{
			bResult = InternalStopService(hSvcHandle);
			::CloseServiceHandle(hSvcHandle);
		}
		::CloseServiceHandle(hScmHandle);
	}

	return bResult;
}

//-----------------------------------------------------------------------------

bool CWinService::GetServiceFileName(LPCTSTR lpszServiceName, CString& strServiceFileName)
{
	const int BUF_SIZE = 4096;

	bool bResult = false;
	SC_HANDLE hSvcHandle;
	SC_HANDLE hScmHandle;

	CString str;
	DWORD nBytes = 0;
	LPQUERY_SERVICE_CONFIG lpServiceConfig = (LPQUERY_SERVICE_CONFIG)str.GetBuffer(BUF_SIZE);

	strServiceFileName.Empty();

	hScmHandle = ::OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (hScmHandle)
	{
		hSvcHandle = ::OpenService(hScmHandle, lpszServiceName, SERVICE_QUERY_CONFIG);
		if (hSvcHandle)
		{
			bResult = ::QueryServiceConfig(hSvcHandle, lpServiceConfig, BUF_SIZE, &nBytes) != 0;
			if (!bResult)
			{
				lpServiceConfig = (LPQUERY_SERVICE_CONFIG)str.GetBuffer(nBytes);
				bResult = ::QueryServiceConfig(hSvcHandle, lpServiceConfig, nBytes, &nBytes) != 0;
			}
			::CloseServiceHandle(hSvcHandle);
		}
		::CloseServiceHandle(hScmHandle);
	}

	if (bResult)
	{
		CString strFileName = lpServiceConfig->lpBinaryPathName;
		bResult = !strFileName.IsEmpty();
		if (bResult)
		{
			if (strFileName[0] == '\"')
			{
				strFileName.TrimLeft('\"');
				strFileName = strFileName.Left(strFileName.Find('\"'));
			}
			else
			{
				int nPos = strFileName.Find(' ');
				if (nPos >= 0)
					strFileName = strFileName.Left(nPos);
			}

			strServiceFileName = strFileName;
		}
	}

	return bResult;
}

//-----------------------------------------------------------------------------

int CWinService::Main(int argc, LPTSTR *argv)
{
	int nResult = 0;
	bool bHandled = false;

	if ((argc > 1) && ((*argv[1] == '-') || (*argv[1] == '/')))
	{
		CString strArg = argv[1] + 1;
		strArg.MakeLower();

		if (strArg == TEXT("install"))
		{
			Install();
			bHandled = true;
		}
		else if (strArg == TEXT("uninstall" || strArg == TEXT("remove")))
		{
			Uninstall();
			bHandled = true;
		}
		else if (strArg == TEXT("restart"))
		{
			Stop();
			Start();
			bHandled = true;
		}
		else if (strArg == TEXT("debug"))
		{
			m_bDebugMode = true;
			Debug(argc, argv);
			bHandled = true;
		}
		else if (strArg == TEXT("help") || strArg == TEXT("?"))
		{
			CString strMsg = FormatString(
				TEXT("Usage:\n")
				TEXT("%s -install         to install the service.\n")
				TEXT("%s -uninstall       to remove the service.\n")
				TEXT("%s -restart         to restart the service.\n")
				TEXT("%s -debug [params]  to run as a console app for debugging.\n")
				TEXT("%s -help            to show the help information.\n"),
				m_strServiceName,
				m_strServiceName,
				m_strServiceName,
				m_strServiceName,
				m_strServiceName);

			wcout << strMsg.GetString();
			bHandled = true;
		}
	}

	if (!bHandled)
	{
		if (IsServiceArguments(argc, argv))
			nResult = StartDispatcher();
		else
			OnRunAsNonService(argc, argv);
	}

	return nResult;
}

//-----------------------------------------------------------------------------

bool CWinService::Install()
{
	return CWinService::InstallService(m_strServiceName, m_strServiceDisplayName,
		m_strServiceDescription, GetAppExeName(true), m_strServiceArguments,
		m_strServiceDependencies);
}

//-----------------------------------------------------------------------------

bool CWinService::Uninstall()
{
	return CWinService::UninstallService(m_strServiceName);
}

//-----------------------------------------------------------------------------

bool CWinService::IsInstalled()
{
	return CWinService::IsServiceInstalled(m_strServiceName);
}

//-----------------------------------------------------------------------------

bool CWinService::Start()
{
	return CWinService::StartService(m_strServiceName, 0, NULL);
}

//-----------------------------------------------------------------------------

bool CWinService::Stop()
{
	return CWinService::StopService(m_strServiceName);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ifc
