#include "pch.h"
#include "ClaStaticDrag.h"

#include <sstream>

IMPLEMENT_DYNAMIC(ClaStaticDrag, CStatic)

BEGIN_MESSAGE_MAP(ClaStaticDrag, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

ClaStaticDrag::ClaStaticDrag()
 : CStatic()
 , _bDrag(FALSE)
 , _nMsgID(0)
 , _pWndParent(NULL)
{

}


void ClaStaticDrag::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetCapture();
	_bDrag = TRUE;
	CStatic::OnLButtonDown(nFlags, point);
}

void ClaStaticDrag::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	_bDrag = FALSE;
	ReleaseCapture();

	CStatic::OnLButtonUp(nFlags, point);
}

void ClaStaticDrag::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (_bDrag && _pWndParent && _nMsgID) {

		CPoint pt = point;

		::ClientToScreen(GetSafeHwnd(), &pt);

		HWND hWnd = ::WindowFromPoint(pt);
		
		_pWndParent->SendMessage(_nMsgID, (WPARAM)hWnd);
	}

	CStatic::OnMouseMove(nFlags, point);
}
