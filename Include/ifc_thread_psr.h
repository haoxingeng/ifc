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

/// @file ifc_thread_psr.h
/// Defines the thread processor classes.

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"
#include "ifc_thread.h"
#include "ifc_sync_objs.h"

/// The namespace of IFC.
namespace ifc
{

/// @addtogroup Classes
/// @{

///////////////////////////////////////////////////////////////////////////////
// Classes

class CThreadProcessor;
class CDaemonJob;
class CDaemonJobPsr;

///////////////////////////////////////////////////////////////////////////////
// Constant Definitions

/// The default sleep time in millisecond of thread processor.
const int DEF_THREAD_PSR_SHEEP_MSECS = 1;

///////////////////////////////////////////////////////////////////////////////
/// CThreadProcessor - Thread processor class.

class CThreadProcessor
{
public:
	friend class CWorkerThread;

	class CWorkerThread : public CThread
	{
	private:
		CThreadProcessor& m_Processor;
	protected:
		virtual void Execute();
	public:
		CWorkerThread(CThreadProcessor& Processor);
	};

	class CSleepController
	{
	private:
		HANDLE m_hSleepEvent;
		bool m_bNeedSleep;     // Indicates whether the thread processor need sleep or not.
		int m_nSleepMSecs;     // The sleep interval in millisecond.
	public:
		CSleepController();
		~CSleepController();

		void Sleep(int nMilliseconds);
		void InterruptSleep();

		bool& NeedSleep() { return m_bNeedSleep; }
		int& SleepMSecs() { return m_nSleepMSecs; }
	};

private:
	CWorkerThread *m_pWorkerThread;
protected:
	CSleepController m_SleepController;
protected:
	/// Invoked automatically by Stop() before terminate the processor thread.
	virtual void BeforeTerminate() {}
	/// The worker procedure of thread processor.
	virtual void Process() = 0;
public:
	CThreadProcessor();
	virtual ~CThreadProcessor();

	/// Starts the thread processor.
	void Start();
	/// Stops the thread processor.
	void Stop();

	/// Returns the current worker thread.
	CThread& GetWorkerThread() { return *m_pWorkerThread; }
	/// Returns the sleep controller.
	CSleepController& GetSleepController() { return m_SleepController; }
};

typedef CThreadProcessor::CSleepController CThreadPsrSleepCtrl;

///////////////////////////////////////////////////////////////////////////////
/// CDaemonJob - The base class of daemon job.

class CDaemonJob
{
public:
	friend class CDaemonJobPsr;
private:
	CDaemonJobPsr *m_pJobPsr;              // The own job processor.
	UINT m_nLastProcTicks;                 // The last process time.
	UINT m_nProcessCount;                  // The execution count of Process() currently (0-based).
protected:
	UINT m_nDelay;                         // The delay time in millisecond of first process.
	UINT m_nInterval;                      // The interval in millisecond of job execution.
protected:
	/// Invoked automatically by CDaemonJobPsr::Terminate() before terminate the job.
	virtual void BeforeTerminate() {}
	/// The worker procedure of job.
	virtual void Process() = 0;
public:
	CDaemonJob();
	virtual ~CDaemonJob() {}

	CDaemonJobPsr* JobPsr() { return m_pJobPsr; }
	UINT GetProcessCount() const { return m_nProcessCount; }
	UINT GetDelay() const { return m_nDelay; }
	UINT GetInterval() const { return m_nInterval; }
};

///////////////////////////////////////////////////////////////////////////////
/// CDaemonJobPsr - The daemon job processor class.

class CDaemonJobPsr : public CThreadProcessor
{
private:
	typedef CObjectList<CDaemonJob> CJobList;

	CJobList m_JobList;
	CJobList m_AddList;
protected:
	virtual void BeforeTerminate();
	virtual void Process();
public:
	CDaemonJobPsr();
	virtual ~CDaemonJobPsr();

	/// Adds a daemon job to the job processor.
	void AddJob(CDaemonJob *pJob);
	/// Stops the worker thread, and destroy all job objects.
	void Terminate();
};

///////////////////////////////////////////////////////////////////////////////

/// @}

} // namespace ifc
