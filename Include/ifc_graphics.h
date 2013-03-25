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

/// @file ifc_graphics.h
/// Defines the graphics classes.

/// @addtogroup Classes
/// @{

#pragma once

#include "ifc_options.h"
#include "ifc_global_defs.h"
#include "ifc_classes.h"
#include "ifc_sync_objs.h"

BEGIN_NAMESPACE(NS_IFC)

///////////////////////////////////////////////////////////////////////////////
// Classes

class CGraphicsObject;
class CFont;
class CPen;
class CBrush;
class CCustomCanvas;
class CCanvas;
class CGraphic;
class CPicture;
class CSharedImage;
class CBitmapImage;
class CIconImage;
class CBitmap;
class CIcon;
class CBitmapCanvas;
class CResourceManager;
class CBrushResourceManager;

///////////////////////////////////////////////////////////////////////////////
// Enum Definitions

enum FONT_PITCH
{
	FP_DEFAULT           = 0,
	FP_VARIABLE          = 1,
	FP_FIXED             = 2,
};

enum FONT_STYLE
{
	FS_BOLD              = 0x01,
	FS_ITALIC            = 0x02,
	FS_UNDERLINE         = 0x04,
	FS_STRIKE_OUT        = 0x08,
};

enum PEN_STYLE
{
	PST_SOLID            = 0,
	PST_DASH             = 1,
	PST_DOT              = 2,
	PST_DASHDOT          = 3,
	PST_DASHDOTDOT       = 4,
	PST_CLEAR            = 5,
	PST_INSIDEFRAME      = 6,
	PST_USERSTYLE        = 7,
	PST_ALTERNATE        = 8,

	PST_COUNT            = 9
};

enum PEN_MODE
{
	PM_BLACK             = 0,
	PM_WHITE             = 1,
	PM_NOP               = 2,
	PM_NOT               = 3,
	PM_COPY              = 4,
	PM_NOTCOPY           = 5,
	PM_MERGEPENNOT       = 6,
	PM_MASKPENNOT        = 7,
	PM_MERGENOTPEN       = 8,
	PM_MASKNOTPEN        = 9,
	PM_MERGE             = 10,
	PM_NOTMERGE          = 11,
	PM_MASK              = 12,
	PM_NOTMASK           = 13,
	PM_XOR               = 14,
	PM_NOTXOR            = 15,
};

enum BRUSH_STYLE
{
	BST_SOLID            = 0,
	BST_CLEAR            = 1,
	BST_HORIZONTAL       = 2,
	BST_VERTICAL         = 3,
	BST_FDIAGONAL        = 4,
	BST_BDIAGONAL        = 5,
	BST_CROSS            = 6,
	BST_DIAGCROSS        = 7,
};

enum FILL_STYLE
{
	FS_SURFACE           = 0,
	FS_BORDER            = 1,
};

enum FILL_MODE
{
	FM_ALTERNATE         = 0,
	FM_WINDING           = 1,
};

enum CANVAS_STATE
{
	CS_HANDLE_VALID      = 0x01,
	CS_FONT_VALID        = 0x02,
	CS_PEN_VALID         = 0x04,
	CS_BRUSH_VALID       = 0x08,
};

enum CANVAS_ORIENTATION
{
	CO_LEFT_TO_RIGHT     = 0,
	CO_RIGHT_TO_LEFT     = 1,
};

enum TEXT_FORMAT
{
	TF_BOTTOM            = 0x00000001,
	TF_CALC_RECT         = 0x00000002,
	TF_CENTER            = 0x00000004,
	TF_EDIT_CONTROL      = 0x00000008,
	TF_END_ELLIPSIS      = 0x00000010,
	TF_PATH_ELLIPSIS     = 0x00000020,
	TF_EXPAND_TABS       = 0x00000040,
	TF_EXTERNAL_LEADING  = 0x00000080,
	TF_LEFT              = 0x00000100,
	TF_MODIFY_STRING     = 0x00000200,
	TF_NO_CLIP           = 0x00000400,
	TF_NO_PREFIX         = 0x00000800,
	TF_RIGHT             = 0x00001000,
	TF_RTL_READING       = 0x00002000,
	TF_SINGLE_LINE       = 0x00004000,
	TF_TOP               = 0x00008000,
	TF_VERTICAL_CENTER   = 0x00010000,
	TF_WORD_BREAK        = 0x00020000,
};

///////////////////////////////////////////////////////////////////////////////
// Type Definitions

typedef INT32 TCOLOR;
typedef TCOLOR* PCOLOR;

typedef BYTE FONT_CHARSET;
typedef BYTE FONT_STYLES;

typedef TCHAR FONT_DATA_NAME[LF_FACESIZE];

typedef INT32 COPY_MODE;

typedef BYTE CANVAS_STATES;
typedef DWORD TEXT_FORMATS;

///////////////////////////////////////////////////////////////////////////////
// Structure Definitions

#pragma pack(1)

struct CURSOR_OR_ICON
{
	WORD nReserved;
	WORD nType;
	WORD nCount;
};

struct ICON_REC
{
	BYTE nWidth;
	BYTE nHeight;
	WORD nColors;
	WORD nReserved1;
	WORD nReserved2;
	INT32 nDibSize;
	INT32 nDibOffset;
};

#pragma pack()

struct RES_DATA
{
	HANDLE nHandle;
};

struct FONT_DATA
{
	HFONT hHandle;
	int nHeight;
	int nOrientation;
	FONT_PITCH nPitch;
	FONT_STYLES nStyle;
	FONT_CHARSET nCharset;
	FONT_DATA_NAME szName;
};

struct PEN_DATA
{
	HPEN hHandle;
	TCOLOR nColor;
	int nWidth;
	PEN_STYLE nStyle;
};

struct BRUSH_DATA
{
	HBRUSH hHandle;
	TCOLOR nColor;
	CBitmap *pBitmap;
	BRUSH_STYLE nStyle;
	bool bOwnsBitmap;
};

struct RESOURCE
{
	RESOURCE *pNext;
	int nRefCount;
	HANDLE hHandle;
	WORD nHashCode;

	union
	{
		RES_DATA Data;
		FONT_DATA Font;
		PEN_DATA Pen;
		BRUSH_DATA Brush;
	};
};

///////////////////////////////////////////////////////////////////////////////
// Constant Definitions

const TCOLOR CL_BLACK   = 0x000000;
const TCOLOR CL_MAROON  = 0x000080;
const TCOLOR CL_GREEN   = 0x008000;
const TCOLOR CL_OLIVE   = 0x008080;
const TCOLOR CL_NAVY    = 0x800000;
const TCOLOR CL_PURPLE  = 0x800080;
const TCOLOR CL_TEAL    = 0x808000;
const TCOLOR CL_GRAY    = 0x808080;
const TCOLOR CL_SILVER  = 0xC0C0C0;
const TCOLOR CL_RED     = 0x0000FF;
const TCOLOR CL_LIME    = 0x00FF00;
const TCOLOR CL_YELLOW  = 0x00FFFF;
const TCOLOR CL_BLUE    = 0xFF0000;
const TCOLOR CL_FUCHSIA = 0xFF00FF;
const TCOLOR CL_AQUA    = 0xFFFF00;
const TCOLOR CL_LTGRAY  = 0xC0C0C0;
const TCOLOR CL_DKGRAY  = 0x808080;
const TCOLOR CL_WHITE   = 0xFFFFFF;

///////////////////////////////////////////////////////////////////////////////
// CGraphicsObject

class CGraphicsObject
{
public:
	friend class CResourceManager;
protected:
	RESOURCE *m_pResource;
	CCriticalSection *m_pOwnerLock;
protected:
	virtual void OnChanged() {}
public:
	CGraphicsObject() : m_pResource(NULL), m_pOwnerLock(NULL) {}
	virtual ~CGraphicsObject() {}

	bool IsHandleAllocated() { return m_pResource && m_pResource->hHandle != 0; }
	void SetOwnerLock(CCriticalSection *pLock) { m_pOwnerLock = pLock; }
};

///////////////////////////////////////////////////////////////////////////////
// CFont

class CFont : public CGraphicsObject
{
private:
	TCOLOR m_nColor;
	int m_nPixelsPerInch;
private:
	void Init();
	void GetData(FONT_DATA& Data);
	void SetData(const FONT_DATA& Data);
protected:
	virtual void OnChanged() {}
public:
	CFont();
	CFont(const CFont& src);
	virtual ~CFont();

	CFont& operator = (const CFont& rhs);

	HFONT GetHandle();
	int GetPixelsPerInch() const { return m_nPixelsPerInch; }
	FONT_CHARSET GetCharset() const;
	TCOLOR GetColor() const { return m_nColor; }
	int GetHeight() const;
	CString GetName() const;
	int GetOrientation() const;
	FONT_PITCH GetPitch() const;
	int GetSize() const;
	FONT_STYLES GetStyle() const;

	void SetHandle(HFONT hValue);
	void SetPixelsPerInch(int nValue) { m_nPixelsPerInch = nValue; }
	void SetCharset(FONT_CHARSET nValue);
	void SetColor(TCOLOR nValue);
	void SetHeight(int nValue);
	void SetName(LPCTSTR lpszValue);
	void SetOrientation(int nValue);
	void SetPitch(FONT_PITCH nValue);
	void SetSize(int nValue);
	void SetStyle(FONT_STYLES nValue);
};

///////////////////////////////////////////////////////////////////////////////
// CPen

class CPen : public CGraphicsObject
{
private:
	PEN_MODE m_nMode;
private:
	void Init();
	void GetData(PEN_DATA& Data);
	void SetData(const PEN_DATA& Data);
public:
	CPen();
	CPen(const CPen& src);
	virtual ~CPen();

	CPen& operator = (const CPen& rhs);

	HPEN GetHandle();
	TCOLOR GetColor() const;
	PEN_MODE GetMode() const { return m_nMode; }
	PEN_STYLE GetStyle() const;
	int GetWidth() const;

	void SetHandle(HPEN hValue);
	void SetColor(TCOLOR nValue);
	void SetMode(PEN_MODE nValue);
	void SetStyle(PEN_STYLE nValue);
	void SetWidth(int nValue);
};

///////////////////////////////////////////////////////////////////////////////
// CBrush

class CBrush : public CGraphicsObject
{
private:
	void Init();
	void GetData(BRUSH_DATA& Data);
	void SetData(const BRUSH_DATA& Data);
public:
	CBrush();
	CBrush(const CBrush& src);
	virtual ~CBrush();

	CBrush& operator = (const CBrush& rhs);

	HBRUSH GetHandle();
	TCOLOR GetColor() const;
	BRUSH_STYLE GetStyle() const;
	CBitmap* GetBitmap() const;

	void SetHandle(HBRUSH hValue);
	void SetColor(TCOLOR nValue);
	void SetStyle(BRUSH_STYLE nValue);
	void SetBitmap(CBitmap *pValue);
};

///////////////////////////////////////////////////////////////////////////////
// CCustomCanvas

class CCustomCanvas
{
protected:
	CCriticalSection m_Lock;
	int m_nLockCount;
	DWORD m_nTextFlags;
protected:
	virtual void RequiredState(CANVAS_STATES nReqStates) = 0;
public:
	CCustomCanvas() {}
	virtual ~CCustomCanvas() {}

	virtual void MoveTo(int x, int y) = 0;
	virtual void LineTo(int x, int y) = 0;
	virtual void Rectangle(int x1, int y1, int x2, int y2) = 0;
	void Rectangle(const CRect& Rect) { Rectangle(Rect.left, Rect.top, Rect.right, Rect.bottom); }
	virtual void RoundRect(int x1, int y1, int x2, int y2, int x3, int y3) = 0;
	void RoundRect(const CRect& Rect, int cx, int cy) { RoundRect(Rect.left, Rect.top, Rect.right, Rect.bottom, cx, cy); }
	virtual void FillRect(int x1, int y1, int x2, int y2) = 0;
	void FillRect(const CRect& Rect) { FillRect(Rect.left, Rect.top, Rect.right, Rect.bottom); }
	virtual void FloodFill(int x, int y, TCOLOR nColor, FILL_STYLE nFillStyle) = 0;
	virtual void Ellipse(int x1, int y1, int x2, int y2) = 0;
	void Ellipse(const CRect& Rect) { Ellipse(Rect.left, Rect.top, Rect.right, Rect.bottom); }
	virtual void Arc(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) = 0;
	virtual void Chord(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) = 0;
	virtual void Pie(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) = 0;
	virtual void Polygon(CPoint *pPoints, int nCount) = 0;
	virtual void Polyline(CPoint *pPoints, int nCount) = 0;
	virtual void PolyBezier(CPoint *pPoints, int nCount) = 0;
	virtual void PolyBezierTo(CPoint *pPoints, int nCount) = 0;
	virtual void DrawFocusRect(const CRect& Rect) = 0;
	virtual void FrameRect(const CRect& Rect) = 0;

	virtual CSize TextExtent(LPCTSTR lpszText) = 0;
	int TextHeight(LPCTSTR lpszText) { return TextExtent(lpszText).cy; }
	int TextWidth(LPCTSTR lpszText) { return TextExtent(lpszText).cx; }
	virtual void TextOut(int x, int y, LPCTSTR lpszText) = 0;
	virtual void TextRect(CRect& Rect, CString& strText, TEXT_FORMATS nTextFormats = 0) = 0;
	virtual void TextRect(const CRect& Rect, int x, int y, LPCTSTR lpszText) = 0;

	virtual void Draw(int x, int y, CGraphic *pGraphic) = 0;
	virtual void Draw(int x, int y, CGraphic *pGraphic, BYTE nOpacity) = 0;
	virtual void StretchDraw(const CRect& Rect, CGraphic *pGraphic) = 0;
	virtual void BrushCopy(const CRect& DestRect, CBitmap *pBitmap, const CRect& SrcRect, TCOLOR nColor) = 0;
	virtual void Refresh() = 0;

	virtual CRect GetClipRect() = 0;
	virtual CANVAS_ORIENTATION GetCanvasOrientation() = 0;
	virtual CPoint GetPenPos() = 0;
	virtual TCOLOR GetPixel(int x, int y) = 0;
	DWORD GetTextFlags() { return m_nTextFlags; }

	virtual void SetPenPos(CPoint Pos) = 0;
	virtual void SetPixel(int x, int y, TCOLOR nColor) = 0;
	void SetTextFlags(DWORD nValue) { m_nTextFlags = nValue; }
};

///////////////////////////////////////////////////////////////////////////////
// CCanvas

class CCanvas : public CCustomCanvas
{
public:
};

///////////////////////////////////////////////////////////////////////////////
// CGraphic

class CGraphic
{
public:
};

///////////////////////////////////////////////////////////////////////////////
// CPicture

class CPicture
{
public:
};

///////////////////////////////////////////////////////////////////////////////
// CSharedImage

class CSharedImage
{
public:
};

///////////////////////////////////////////////////////////////////////////////
// CBitmapImage

class CBitmapImage : public CSharedImage
{
public:
};

///////////////////////////////////////////////////////////////////////////////
// CIconImage

class CIconImage : public CSharedImage
{
public:
};

///////////////////////////////////////////////////////////////////////////////
// CBitmap

class CBitmap : public CGraphic
{
public:
};

///////////////////////////////////////////////////////////////////////////////
// CIcon

class CIcon : public CGraphic
{
public:
};

///////////////////////////////////////////////////////////////////////////////
// CBitmapCanvas

class CBitmapCanvas : public CCanvas
{
public:
};

///////////////////////////////////////////////////////////////////////////////
// CResourceManager

class CResourceManager
{
public:
	RESOURCE *m_pResList;
	int m_nResDataSize;
	CCriticalSection m_Lock;
protected:
	virtual void FreeObjects(RESOURCE *pResource) {}
public:
	explicit CResourceManager(int nResDataSize);
	virtual ~CResourceManager();

	RESOURCE* AllocResource(const void *pResData);
	void FreeResource(RESOURCE *pResource);
	void ChangeResource(CGraphicsObject *pGraphicsObject, const void *pResData);
	void AssignResource(CGraphicsObject *pGraphicsObject, RESOURCE *pResource);
};

///////////////////////////////////////////////////////////////////////////////
// CBrushResourceManager

class CBrushResourceManager : public CResourceManager
{
protected:
	virtual void FreeObjects(RESOURCE *pResource);
public:
	explicit CBrushResourceManager(int nResDataSize) : CResourceManager(nResDataSize) {}
};

///////////////////////////////////////////////////////////////////////////////
// Misc Routines

COLORREF ColorToRGB(TCOLOR nColor);
FONT_CHARSET GetDefFontCharSet();

///////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(NS_IFC)

/// @}
