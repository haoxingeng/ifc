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

/// @file ifc_graphics.cpp

#include "stdafx.h"
#include "ifc_graphics.h"
#include "ifc_sysutils.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// CGraphicsGlobalVars

class CGraphicsGlobalVars
{
public:
	int m_nScreenLogPixels;
	CCriticalSection m_nBitmapImageLock;
	HPEN m_StockPen;
	HBRUSH m_StockBrush;
	HFONT m_StockFont;
	HICON m_StockIcon;
	FONT_DATA m_DefFontData;
	PEN_DATA m_DefPenData;
	BRUSH_DATA m_DefBrushData;
	CResourceManager m_FontManager;
	CResourceManager m_PenManager;
	CBrushResourceManager m_BrushManager;

private:
	void InitScreenLogPixels()
	{
		HDC hDC = ::GetDC(0);
		m_nScreenLogPixels = ::GetDeviceCaps(hDC, LOGPIXELSY);
		::ReleaseDC(0, hDC);
	}

	void InitStockResources()
	{
		m_StockPen = (HPEN)::GetStockObject(BLACK_PEN);
		m_StockBrush = (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
		m_StockFont = (HFONT)::GetStockObject(SYSTEM_FONT);
		m_StockIcon = (HICON)::LoadIcon(0, IDI_APPLICATION);
	}

	void InitDefFontData()
	{
		const TCHAR* const S_FONT_SUBSTITUTES =
			TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\FontSubstitutes");

		CString strName;
		memset(&m_DefFontData, 0, sizeof(m_DefFontData));

		m_DefFontData.nHeight = -MulDiv(8, m_nScreenLogPixels, 72);
		if (GetWindowsVer().dwPlatformId == VER_PLATFORM_WIN32_WINDOWS &&
			GetDefFontCharSet() == SHIFTJIS_CHARSET)
			strName = TEXT("Tahoma");

		CRegistry Reg;
		Reg.SetRootKey(HKEY_LOCAL_MACHINE);
		if (Reg.OpenKeyReadOnly(S_FONT_SUBSTITUTES))
		{
			strName = Reg.ReadString(TEXT("MS Shell Dlg 2"));
			Reg.CloseKey();
		}

		if (!strName.IsEmpty())
			_tcscpy_s(m_DefFontData.szName, LF_FACESIZE, strName);
	}

	void InitDefPenData()
	{
		m_DefPenData.hHandle = 0;
		m_DefPenData.nColor = CL_BLACK;
		m_DefPenData.nWidth = 1;
		m_DefPenData.nStyle = PST_SOLID;
	}

	void InitDefBrushData()
	{
		m_DefBrushData.hHandle = 0;
		m_DefBrushData.nColor = CL_WHITE;
		m_DefBrushData.pBitmap = NULL;
		m_DefBrushData.nStyle = BST_SOLID;
		m_DefBrushData.bOwnsBitmap = false;
	}

private:
	CGraphicsGlobalVars() :
		m_nScreenLogPixels(0),
		m_StockPen(0),
		m_StockBrush(0),
		m_StockFont(0),
		m_StockIcon(0),
		m_FontManager(sizeof(FONT_DATA)),
		m_PenManager(sizeof(PEN_DATA)),
		m_BrushManager(sizeof(BRUSH_DATA))
	{
		InitScreenLogPixels();
		InitStockResources();
		InitDefFontData();
		InitDefPenData();
		InitDefBrushData();
	}

public:
	~CGraphicsGlobalVars()
	{

	}

	static CGraphicsGlobalVars& Instance()
	{
		static CGraphicsGlobalVars obj;
		return obj;
	}
};

static inline CGraphicsGlobalVars& GlobalVars()
{
	return CGraphicsGlobalVars::Instance();
}

///////////////////////////////////////////////////////////////////////////////
// Misc Routines

COLORREF ColorToRGB(TCOLOR nColor)
{
	if ((INT32)nColor < 0)
		return ::GetSysColor(nColor & 0x000000FF);
	else
		return nColor;
}

//-----------------------------------------------------------------------------

FONT_CHARSET GetDefFontCharSet()
{
	FONT_CHARSET nResult = DEFAULT_CHARSET;

	HDC hDisplayDC = GetDC(0);
	if (hDisplayDC != 0)
	{
		if (::SelectObject(hDisplayDC, GlobalVars().m_StockFont) != 0)
		{
			TEXTMETRIC TextMetric;
			if (::GetTextMetrics(hDisplayDC, &TextMetric))
				nResult = TextMetric.tmCharSet;
		}
		::ReleaseDC(0, hDisplayDC);
	}

	return nResult;
}

//-----------------------------------------------------------------------------

static WORD GetResHashCode(const void *pBuffer, int nBytes)
{
	WORD nResult;

	__asm
	{
		MOV     EDX, pBuffer
		MOV     ECX, nBytes
		XOR     EAX, EAX
	_1:
		ROL     AX, 5
		XOR     AL, [EDX]
		INC     EDX
		DEC     ECX
		JNE     _1
		MOV     nResult, AX
	}

	return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// CFont

CFont::CFont()
{
	Init();
}

CFont::CFont(const CFont& src)
{
	Init();
	*this = src;
}

CFont::~CFont()
{
	GlobalVars().m_FontManager.FreeResource(m_pResource);
}

//-----------------------------------------------------------------------------

void CFont::Init()
{
	GlobalVars().m_DefFontData.hHandle = 0;
	m_pResource = GlobalVars().m_FontManager.AllocResource(&GlobalVars().m_DefFontData);
	m_nColor = CL_BLACK;
	m_nPixelsPerInch = GlobalVars().m_nScreenLogPixels;
}

//-----------------------------------------------------------------------------

CFont& CFont::operator = (const CFont& rhs)
{
	if (this == &rhs) return *this;

	CAutoLocker Locker1(m_pOwnerLock);
	CAutoLocker Locker2(rhs.m_pOwnerLock);

	GlobalVars().m_FontManager.AssignResource(this, rhs.m_pResource);
	SetColor(rhs.m_nColor);
	if (m_nPixelsPerInch != rhs.m_nPixelsPerInch)
		SetSize(rhs.GetSize());

	return *this;
}

//-----------------------------------------------------------------------------

void CFont::GetData(FONT_DATA& Data)
{
	Data = m_pResource->Font;
	Data.hHandle = 0;
}

//-----------------------------------------------------------------------------

void CFont::SetData(const FONT_DATA& Data)
{
	CAutoLocker Locker(m_pOwnerLock);
	GlobalVars().m_FontManager.ChangeResource(this, &Data);
}

//-----------------------------------------------------------------------------

HFONT CFont::GetHandle()
{
	if (m_pResource->hHandle == 0)
	{
		CAutoLocker Locker(GlobalVars().m_FontManager.m_Lock);
		if (m_pResource->hHandle == 0)
		{
			FONT_DATA& FontData = m_pResource->Font;
			LOGFONT LogFont;

			LogFont.lfHeight = FontData.nHeight;
			LogFont.lfWidth = 0; // have font mapper choose
			LogFont.lfEscapement = FontData.nOrientation;
			LogFont.lfOrientation = FontData.nOrientation;
			LogFont.lfWeight = (FontData.nStyle & FS_BOLD) ? FW_BOLD : FW_NORMAL;
			LogFont.lfItalic = (FontData.nStyle & FS_ITALIC) ? 1 : 0;
			LogFont.lfUnderline = (FontData.nStyle & FS_UNDERLINE) ? 1 : 0;
			LogFont.lfStrikeOut = (FontData.nStyle & FS_STRIKE_OUT) ? 1 : 0;

			if (FontData.nCharset == DEFAULT_CHARSET && GlobalVars().m_DefFontData.nCharset != DEFAULT_CHARSET)
				LogFont.lfCharSet = GlobalVars().m_DefFontData.nCharset;
			else
				LogFont.lfCharSet = (BYTE)FontData.nCharset;

			if (SameText(FontData.szName, TEXT("Default")))
				_tcscpy_s(LogFont.lfFaceName, LF_FACESIZE, GlobalVars().m_DefFontData.szName);
			else
				_tcscpy_s(LogFont.lfFaceName, LF_FACESIZE, FontData.szName);

			if (GetWindowsVer().dwPlatformId == VER_PLATFORM_WIN32_WINDOWS &&
				GetDefFontCharSet() == SHIFTJIS_CHARSET &&
				_tcscmp(FontData.szName, GlobalVars().m_DefFontData.szName) == 0 &&
				FontData.nCharset == GlobalVars().m_DefFontData.nCharset)
			{
				LogFont.lfCharSet = SHIFTJIS_CHARSET;
			}

			LogFont.lfQuality = DEFAULT_QUALITY;

			// Everything else as default
			// Only True Type fonts support the angles
			LogFont.lfOutPrecision = (LogFont.lfOrientation != 0) ? OUT_TT_ONLY_PRECIS : OUT_DEFAULT_PRECIS;
			LogFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;

			switch (FontData.nPitch)
			{
			case FP_VARIABLE: LogFont.lfPitchAndFamily = VARIABLE_PITCH; break;
			case FP_FIXED:    LogFont.lfPitchAndFamily = FIXED_PITCH;    break;
			default:          LogFont.lfPitchAndFamily = DEFAULT_PITCH;  break;
			}

			m_pResource->hHandle = ::CreateFontIndirect(&LogFont);
		}
	}

	return (HFONT)m_pResource->hHandle;
}

//-----------------------------------------------------------------------------

FONT_CHARSET CFont::GetCharset() const
{
	return m_pResource->Font.nCharset;
}

//-----------------------------------------------------------------------------

int CFont::GetHeight() const
{
	return m_pResource->Font.nHeight;
}

//-----------------------------------------------------------------------------

CString CFont::GetName() const
{
	return CString(m_pResource->Font.szName);
}

//-----------------------------------------------------------------------------

int CFont::GetOrientation() const
{
	return m_pResource->Font.nOrientation;
}

//-----------------------------------------------------------------------------

FONT_PITCH CFont::GetPitch() const
{
	return m_pResource->Font.nPitch;
}

//-----------------------------------------------------------------------------

int CFont::GetSize() const
{
	return -MulDiv(GetHeight(), 72, m_nPixelsPerInch);
}

//-----------------------------------------------------------------------------

FONT_STYLES CFont::GetStyle() const
{
	return m_pResource->Font.nStyle;
}

//-----------------------------------------------------------------------------

void CFont::SetHandle(HFONT hValue)
{
	FONT_DATA FontData = GlobalVars().m_DefFontData;
	if (hValue != 0)
	{
		LOGFONT LogFont;
		if (::GetObject(hValue, sizeof(LogFont), &LogFont) != 0)
		{
			FontData.nHeight = LogFont.lfHeight;
			if (LogFont.lfWeight >= FW_BOLD)
				FontData.nStyle |= FS_BOLD;
			if (LogFont.lfItalic == 1)
				FontData.nStyle |= FS_ITALIC;
			if (LogFont.lfUnderline == 1)
				FontData.nStyle |= FS_UNDERLINE;
			if (LogFont.lfStrikeOut == 1)
				FontData.nStyle |= FS_STRIKE_OUT;

			FontData.nCharset = (FONT_CHARSET)LogFont.lfCharSet;
			_tcscpy_s(FontData.szName, LF_FACESIZE, LogFont.lfFaceName);

			switch (LogFont.lfPitchAndFamily & 0x0F)
			{
			case VARIABLE_PITCH: FontData.nPitch = FP_VARIABLE; break;
			case FIXED_PITCH:    FontData.nPitch = FP_FIXED;    break;
			default:             FontData.nPitch = FP_DEFAULT;  break;
			}

			FontData.nOrientation = LogFont.lfOrientation;
			FontData.hHandle = hValue;
		}
	}

	SetData(FontData);
}

//-----------------------------------------------------------------------------

void CFont::SetCharset(FONT_CHARSET nValue)
{
	if (nValue != GetCharset())
	{
		FONT_DATA Data;
		GetData(Data);
		Data.nCharset = nValue;
		SetData(Data);
	}
}

//-----------------------------------------------------------------------------

void CFont::SetColor(TCOLOR nValue)
{
	if (nValue != m_nColor)
	{
		m_nColor = nValue;
		OnChanged();
	}
}

//-----------------------------------------------------------------------------

void CFont::SetHeight(int nValue)
{
	if (nValue != GetHeight())
	{
		FONT_DATA Data;
		GetData(Data);
		Data.nHeight = nValue;
		SetData(Data);
	}
}

//-----------------------------------------------------------------------------

void CFont::SetName(LPCTSTR lpszValue)
{
	if (_tcslen(lpszValue) > 0)
	{
		FONT_DATA Data;
		GetData(Data);
		memset(Data.szName, 0, sizeof(Data.szName));
		_tcscpy_s(Data.szName, LF_FACESIZE, lpszValue);
		SetData(Data);
	}
}

//-----------------------------------------------------------------------------

void CFont::SetOrientation(int nValue)
{
	if (nValue != GetOrientation())
	{
		FONT_DATA Data;
		GetData(Data);
		Data.nOrientation = nValue;
		SetData(Data);
	}
}

//-----------------------------------------------------------------------------

void CFont::SetPitch(FONT_PITCH nValue)
{
	if (nValue != GetPitch())
	{
		FONT_DATA Data;
		GetData(Data);
		Data.nPitch = nValue;
		SetData(Data);
	}
}

//-----------------------------------------------------------------------------

void CFont::SetSize(int nValue)
{
	SetHeight(-MulDiv(nValue, m_nPixelsPerInch, 72));
}

//-----------------------------------------------------------------------------

void CFont::SetStyle(FONT_STYLES nValue)
{
	if (nValue != GetStyle())
	{
		FONT_DATA Data;
		GetData(Data);
		Data.nStyle = nValue;
		SetData(Data);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CPen

CPen::CPen()
{
	Init();
}

CPen::CPen(const CPen& src)
{
	Init();
	*this = src;
}

CPen::~CPen()
{
	GlobalVars().m_PenManager.FreeResource(m_pResource);
}

//-----------------------------------------------------------------------------

void CPen::Init()
{
	m_pResource = GlobalVars().m_PenManager.AllocResource(&GlobalVars().m_DefPenData);
	m_nMode = PM_COPY;
}

//-----------------------------------------------------------------------------

CPen& CPen::operator = (const CPen& rhs)
{
	if (this == &rhs) return *this;

	CAutoLocker Locker1(m_pOwnerLock);
	CAutoLocker Locker2(rhs.m_pOwnerLock);

	GlobalVars().m_PenManager.AssignResource(this, rhs.m_pResource);
	SetMode(rhs.GetMode());

	return *this;
}

//-----------------------------------------------------------------------------

void CPen::GetData(PEN_DATA& Data)
{
	Data = m_pResource->Pen;
	Data.hHandle = 0;
}

//-----------------------------------------------------------------------------

void CPen::SetData(const PEN_DATA& Data)
{
	CAutoLocker Locker(m_pOwnerLock);
	GlobalVars().m_PenManager.ChangeResource(this, &Data);
}

//-----------------------------------------------------------------------------

HPEN CPen::GetHandle()
{
	static const WORD PEN_STYLE_MAP[PST_COUNT] = {
		PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT, PS_DASHDOTDOT, PS_NULL,
		PS_INSIDEFRAME, PS_USERSTYLE, PS_ALTERNATE
	};

	if (m_pResource->hHandle == 0)
	{
		CAutoLocker Locker(GlobalVars().m_PenManager.m_Lock);
		if (m_pResource->hHandle == 0)
		{
			PEN_DATA& PenData = m_pResource->Pen;
			LOGPEN LogPen;

			LogPen.lopnStyle = PEN_STYLE_MAP[PenData.nStyle];
			LogPen.lopnWidth.x = PenData.nWidth;
			LogPen.lopnColor = ColorToRGB(PenData.nColor);

			m_pResource->hHandle = ::CreatePenIndirect(&LogPen);
		}
	}

	return (HPEN)m_pResource->hHandle;
}

//-----------------------------------------------------------------------------

TCOLOR CPen::GetColor() const
{
	return m_pResource->Pen.nColor;
}

//-----------------------------------------------------------------------------

PEN_STYLE CPen::GetStyle() const
{
	return m_pResource->Pen.nStyle;
}

//-----------------------------------------------------------------------------

int CPen::GetWidth() const
{
	return m_pResource->Pen.nWidth;
}

//-----------------------------------------------------------------------------

void CPen::SetHandle(HPEN hValue)
{
	static const PEN_STYLE PEN_STYLE_MAP[PST_COUNT] = {
		PST_SOLID, PST_DASH, PST_DOT, PST_DASHDOT, PST_DASHDOTDOT, PST_CLEAR,
		PST_INSIDEFRAME, PST_USERSTYLE, PST_ALTERNATE
	};

	PEN_DATA PenData = GlobalVars().m_DefPenData;
	if (hValue != 0)
	{
		const int LOG_PEN_SIZE = sizeof(LOGPEN);
		const int EXT_LOG_PEN_SIZE = sizeof(EXTLOGPEN);

		LOGPEN LogPen;
		EXTLOGPEN ExtLogPen;
		int nPenSize;

		nPenSize = ::GetObject(hValue, 0, NULL);  // Determine size of structure
		if (nPenSize == LOG_PEN_SIZE)
		{
			if (::GetObject(hValue, LOG_PEN_SIZE, &LogPen) != 0)
			{
				PenData.nStyle = PEN_STYLE_MAP[LogPen.lopnStyle];
				PenData.nWidth = LogPen.lopnWidth.x;
				PenData.nColor = LogPen.lopnColor;
				PenData.hHandle = hValue;
			}
		}
		else if (nPenSize >= EXT_LOG_PEN_SIZE - 4)  // Extended Logical Pen
		{
			if (nPenSize > EXT_LOG_PEN_SIZE)  // With optional extended dash style
			{
				CBuffer ExtLogPenBuf(nPenSize);
				EXTLOGPEN *pExtLogPen = (EXTLOGPEN*)ExtLogPenBuf.Data();
				if (::GetObject(hValue, nPenSize, ExtLogPenBuf.Data()) != 0)
				{
					PenData.nStyle = PEN_STYLE_MAP[pExtLogPen->elpPenStyle & PS_STYLE_MASK];
					PenData.nWidth = pExtLogPen->elpWidth;
					PenData.nColor = pExtLogPen->elpColor;
					PenData.hHandle = hValue;
				}
			}
			else  // Without optional extended dash style
			{
				if (::GetObject(hValue, EXT_LOG_PEN_SIZE, &ExtLogPen) != 0)
				{
					PenData.nStyle = PEN_STYLE_MAP[ExtLogPen.elpPenStyle & PS_STYLE_MASK];
					PenData.nWidth = ExtLogPen.elpWidth;
					PenData.nColor = ExtLogPen.elpColor;
					PenData.hHandle = hValue;
				}
			}
		}
	}

	SetData(PenData);
}

//-----------------------------------------------------------------------------

void CPen::SetColor(TCOLOR nValue)
{
	if (nValue != GetColor())
	{
		PEN_DATA Data;
		GetData(Data);
		Data.nColor = nValue;
		SetData(Data);
	}
}

//-----------------------------------------------------------------------------

void CPen::SetMode(PEN_MODE nValue)
{
	if (nValue != GetMode())
	{
		m_nMode = nValue;
		OnChanged();
	}
}

//-----------------------------------------------------------------------------

void CPen::SetStyle(PEN_STYLE nValue)
{
	if (nValue != GetStyle())
	{
		PEN_DATA Data;
		GetData(Data);
		Data.nStyle = nValue;
		SetData(Data);
	}
}

//-----------------------------------------------------------------------------

void CPen::SetWidth(int nValue)
{
	if (nValue != GetWidth() && nValue >= 0)
	{
		PEN_DATA Data;
		GetData(Data);
		Data.nWidth = nValue;
		SetData(Data);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CBrush

CBrush::CBrush()
{
	Init();
}

CBrush::CBrush(const CBrush& src)
{
	Init();
	*this = src;
}

CBrush::~CBrush()
{
	GlobalVars().m_BrushManager.FreeResource(m_pResource);
}

//-----------------------------------------------------------------------------

void CBrush::Init()
{
	m_pResource = GlobalVars().m_BrushManager.AllocResource(&GlobalVars().m_DefBrushData);
}

//-----------------------------------------------------------------------------

CBrush& CBrush::operator = (const CBrush& rhs)
{
	if (this == &rhs) return *this;

	CAutoLocker Locker1(m_pOwnerLock);
	CAutoLocker Locker2(rhs.m_pOwnerLock);

	GlobalVars().m_BrushManager.AssignResource(this, rhs.m_pResource);

	return *this;
}

//-----------------------------------------------------------------------------

void CBrush::GetData(BRUSH_DATA& Data)
{
	Data = m_pResource->Brush;
	Data.hHandle = 0;
	Data.pBitmap = NULL;
}

//-----------------------------------------------------------------------------

void CBrush::SetData(const BRUSH_DATA& Data)
{
	CAutoLocker Locker(m_pOwnerLock);
	GlobalVars().m_BrushManager.ChangeResource(this, &Data);
}

//-----------------------------------------------------------------------------

HBRUSH CBrush::GetHandle()
{
	if (m_pResource->hHandle == 0)
	{
		CAutoLocker Locker(GlobalVars().m_BrushManager.m_Lock);
		if (m_pResource->hHandle == 0)
		{
			BRUSH_DATA& BrushData = m_pResource->Brush;
			LOGBRUSH LogBrush;

			if (BrushData.pBitmap != NULL)
			{
				LogBrush.lbStyle = BS_PATTERN;
				// BrushData.pBitmap->SetHandleType(BM_DDB);  // TODO
				// LogBrush.lbHatch = BrushData.pBitmap->GetHandle();  // TODO
			}
			else
			{
				LogBrush.lbHatch = 0;
				switch (BrushData.nStyle)
				{
				case BST_SOLID: LogBrush.lbStyle = BS_SOLID;   break;
				case BST_CLEAR: LogBrush.lbStyle = BS_HOLLOW;  break;
				default:
					LogBrush.lbStyle = BS_HATCHED;
					LogBrush.lbHatch = (BrushData.nStyle - BST_HORIZONTAL);
				}
			}

			LogBrush.lbColor = ColorToRGB(BrushData.nColor);
			m_pResource->hHandle = ::CreateBrushIndirect(&LogBrush);
		}
	}

	return (HBRUSH)m_pResource->hHandle;
}

//-----------------------------------------------------------------------------

TCOLOR CBrush::GetColor() const
{
	return m_pResource->Brush.nColor;
}

//-----------------------------------------------------------------------------

BRUSH_STYLE CBrush::GetStyle() const
{
	return m_pResource->Brush.nStyle;
}

//-----------------------------------------------------------------------------

CBitmap* CBrush::GetBitmap() const
{
	return m_pResource->Brush.pBitmap;
}

//-----------------------------------------------------------------------------

void CBrush::SetHandle(HBRUSH hValue)
{
	BRUSH_DATA BrushData = GlobalVars().m_DefBrushData;
	if (hValue != 0)
	{
		LOGBRUSH LogBrush;
		if (::GetObject(hValue, sizeof(LogBrush), &LogBrush) != 0)
		{
			switch (LogBrush.lbStyle)
			{
			case BS_SOLID:  BrushData.nStyle = BST_SOLID;  break;
			case BS_HOLLOW: BrushData.nStyle = BST_CLEAR;  break;
			case BS_PATTERN:
				if (BrushData.pBitmap == NULL)
				{
					BrushData.pBitmap = new CBitmap();
					BrushData.bOwnsBitmap = true;
				}
				// BrushData.pBitmap->SetHandle(LogBrush.lbHatch);  // TODO
				break;
			default:
				BrushData.nStyle = (BRUSH_STYLE)(LogBrush.lbHatch + BST_HORIZONTAL);
				break;
			}

			BrushData.nColor = LogBrush.lbColor;
			m_pResource->hHandle = hValue;
		}
	}

	SetData(BrushData);
}

//-----------------------------------------------------------------------------

void CBrush::SetColor(TCOLOR nValue)
{
	if (nValue != GetColor() || GetStyle() == BST_CLEAR)
	{
		BRUSH_DATA Data;
		GetData(Data);
		Data.nColor = nValue;
		if (Data.nStyle == BST_CLEAR)
			Data.nStyle = BST_SOLID;
		SetData(Data);
	}
}

//-----------------------------------------------------------------------------

void CBrush::SetStyle(BRUSH_STYLE nValue)
{
	if (nValue != GetStyle() || (nValue == BST_CLEAR && GetColor() != CL_WHITE))
	{
		BRUSH_DATA Data;
		GetData(Data);
		Data.nStyle = nValue;
		if (Data.nStyle == BST_CLEAR)
			Data.nColor = CL_WHITE;
		SetData(Data);
	}
}

//-----------------------------------------------------------------------------

void CBrush::SetBitmap(CBitmap *pValue)
{
	BRUSH_DATA Data = GlobalVars().m_DefBrushData;
	Data.pBitmap = pValue;
	SetData(Data);
}

///////////////////////////////////////////////////////////////////////////////
// CResourceManager

CResourceManager::CResourceManager(int nResDataSize) :
	m_pResList(NULL),
	m_nResDataSize(nResDataSize)
{
	// nothing
}

CResourceManager::~CResourceManager()
{
	// nothing
}

//-----------------------------------------------------------------------------

RESOURCE* CResourceManager::AllocResource(const void *pResData)
{
	CAutoLocker Locker(m_Lock);

	RESOURCE *pResult = m_pResList;
	WORD nResHash = GetResHashCode(pResData, m_nResDataSize);

	while (pResult && (pResult->nHashCode != nResHash ||
		!memcmp(&pResult->Data, pResData, m_nResDataSize)))
	{
		pResult = pResult->pNext;
	}

	if (pResult == NULL)
	{
		const int RES_INFO_SIZE = offsetof(RESOURCE, Data);

		pResult = (RESOURCE*)new char[m_nResDataSize + RES_INFO_SIZE];
		pResult->pNext = m_pResList;
		pResult->nRefCount = 0;
		pResult->hHandle = ((RES_DATA*)pResData)->nHandle;
		pResult->nHashCode = nResHash;
		memmove(&pResult->Data, pResData, m_nResDataSize);

		m_pResList = pResult;
	}

	pResult->nRefCount++;
	return pResult;
}

//-----------------------------------------------------------------------------

void CResourceManager::FreeResource(RESOURCE *pResource)
{
	bool bDeleteIt = false;

	if (pResource)
	{
		CAutoLocker Locker(m_Lock);

		pResource->nRefCount--;
		bDeleteIt = (pResource->nRefCount == 0);
		if (bDeleteIt)
		{
			if (pResource == m_pResList)
				m_pResList = pResource->pNext;
			else
			{
				RESOURCE *p = m_pResList;
				while (p->pNext != pResource) p = p->pNext;
				p->pNext = pResource->pNext;
			}
		}
	}

	if (bDeleteIt)
	{
		if (pResource->hHandle != 0)
			::DeleteObject(pResource->hHandle);
		FreeObjects(pResource);
		delete[] (char*)pResource;
	}
}

//-----------------------------------------------------------------------------

void CResourceManager::ChangeResource(CGraphicsObject *pGraphicsObject, const void *pResData)
{
	CAutoLocker Locker(m_Lock);

	RESOURCE *p = pGraphicsObject->m_pResource;
	pGraphicsObject->m_pResource = AllocResource(pResData);
	if (pGraphicsObject->m_pResource != p)
		pGraphicsObject->OnChanged();
	FreeResource(p);
}

//-----------------------------------------------------------------------------

void CResourceManager::AssignResource(CGraphicsObject *pGraphicsObject, RESOURCE *pResource)
{
	CAutoLocker Locker(m_Lock);

	RESOURCE *p = pGraphicsObject->m_pResource;
	if (p != pResource)
	{
		pResource->nRefCount++;
		pGraphicsObject->m_pResource = pResource;
		pGraphicsObject->OnChanged();
		FreeResource(p);
	}
}

///////////////////////////////////////////////////////////////////////////////
// CBrushResourceManager

void CBrushResourceManager::FreeObjects(RESOURCE *pResource)
{
	if (pResource->Brush.bOwnsBitmap)
	{
		delete pResource->Brush.pBitmap;
		pResource->Brush.pBitmap = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)
