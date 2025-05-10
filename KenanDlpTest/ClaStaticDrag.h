#pragma once

#include <afxcmn.h>

class ClaStaticDrag : public CStatic
{
	DECLARE_DYNAMIC(ClaStaticDrag)

public:
	ClaStaticDrag();

protected:
	int _nColCnt;

protected:
	DECLARE_MESSAGE_MAP()

public:
	BOOL _bDrag;
	CWnd* _pWndParent;
	UINT _nMsgID;

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

public:
	void setCallBack(CWnd* p_pWnd, UINT p_nMsg) {
		_pWndParent = p_pWnd;
		_nMsgID = p_nMsg;
	};
};

