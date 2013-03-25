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

/// @file ifc_thread_psr.cpp

#include "stdafx.h"
#include "ifc_thread_psr.h"
#include "ifc_sysutils.h"
#include "ifc_exceptions.h"
#include "ifc_errmsgs.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// CThreadProcessor::CWorkerThread

CThreadProcessor::CWorkerThread::CWorkerThread(CThreadProcessor& Processor) :
	m_Processor(Processor)
{
	SetFreeOnTerminate(false);
}

//-----------------------------------------------------------------------------

void CThreadProcessor::CWorkerThread::Execute()
{
	while (!GetTerminated())
	{
		try
		{
			m_Processor.m_SleepController.NeedSleep() = true;
			m_Processor.Process();
		}
		catch (...)
		{
			m_Processor.m_SleepController.NeedSleep() = true;
		}

		if (m_Processor.m_SleepController.NeedSleep())
			m_Processor.m_SleepController.Sleep(m_Processor.m_SleepController.SleepMSecs());
	}
}

///////////////////////////////////////////////////////////////////////////////
// CThreadProcessor::CSleepController

CThreadProcessor::CSleepController::CSleepController() :
	m_bNeedSleep(true),
	m_nSleepMSecs(DEF_THREAD_PSR_SHEEP_MSECS)
{
	m_hSleepEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CThreadProcessor::CSleepController::~CSleepController()
{
	CloseHandle(m_hSleepEvent);
}

//-----------------------------------------------------------------------------

void CThreadProcessor::CSleepController::Sleep(int nMilliseconds)
{
	if (nMilliseconds != 0)
		::WaitForSingleObject(m_hSleepEvent, nMilliseconds);
	ResetEvent(m_hSleepEvent);
}

//-----------------------------------------------------------------------------

void CThreadProcessor::CSleepController::InterruptSleep()
{
	SetEvent(m_hSleepEvent);
}

///////////////////////////////////////////////////////////////////////////////
// CThreadProcessor

CThreadProcessor::CThreadProcessor() :
	m_pWorkerThread(NULL)
{
	// nothing
}

//-----------------------------------------------------------------------------

CThreadProcessor::~CThreadProcessor()
{
	Stop();
}

//-----------------------------------------------------------------------------

void CThreadProcessor::Start()
{
	if (!m_pWorkerThread)
	{
		m_pWorkerThread = new CWorkerThread(*this);
		m_pWorkerThread->Run();
	}
}

//-----------------------------------------------------------------------------

void CThreadProcessor::Stop()
{
	if (m_pWorkerThread)
	{
		m_SleepController.InterruptSleep();
		BeforeTerminate();
		m_pWorkerThread->Terminate();
		m_pWorkerThread->WaitFor();
		delete m_pWorkerThread;
		m_pWorkerThread = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
// CDaemonJob

CDaemonJob::CDaemonJob() :
	m_pJobPsr(NULL),
	m_nLastProcTicks(0),
	m_nProcessCount(0),
	m_nDelay(0),
	m_nInterval(0)
{
	// nothing
}

///////////////////////////////////////////////////////////////////////////////
// CDaemonJobPsr

CDaemonJobPsr::CDaemonJobPsr() :
	m_JobList(false, true),
	m_AddList(true, true)
{
	const int LOOP_INTERVAL = 1;   // ms
	m_SleepController.SleepMSecs() = LOOP_INTERVAL;
}

//-----------------------------------------------------------------------------

CDaemonJobPsr::~CDaemonJobPsr()
{
	Terminate();
}

//-----------------------------------------------------------------------------

void CDaemonJobPsr::BeforeTerminate()
{
	for (int i = m_JobList.GetCount() - 1; i >= 0; i--)
		m_JobList[i]->BeforeTerminate();
}

//-----------------------------------------------------------------------------

void CDaemonJobPsr::Process()
{
	while (true)
	{
		CDaemonJob *pJob = m_AddList.Extract(0);
		if (pJob)
			m_JobList.Add(pJob);
		else
			break;
	}

	UINT nCurTicks = GetTickCount();
	for (int i = m_JobList.GetCount() - 1; i >= 0; i--)
	{
		CDaemonJob& Job = *(m_JobList[i]);

		if (Job.m_nLastProcTicks == 0)
			Job.m_nLastProcTicks = nCurTicks;

		if (Job.m_nProcessCount == 0 && GetTickDiff(Job.m_nLastProcTicks, nCurTicks) < Job.m_nDelay)
			continue;

		if (Job.m_nProcessCount == 0 || GetTickDiff(Job.m_nLastProcTicks, nCurTicks) >= Job.m_nInterval)
		{
			Job.m_nLastProcTicks = nCurTicks;
			try
			{
				Job.Process();
			}
			catch (CException* e)
			{
				e->Delete();
			}
			catch (...)
			{}

			Job.m_nProcessCount++;
		}
	}
}

//-----------------------------------------------------------------------------

void CDaemonJobPsr::AddJob(CDaemonJob *pJob)
{
	if (pJob)
	{
		pJob->m_pJobPsr = this;
		m_AddList.Add(pJob);
	}
}

//-----------------------------------------------------------------------------

void CDaemonJobPsr::Terminate()
{
	Stop();
	m_JobList.Clear();
	m_AddList.Clear();
}

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)
