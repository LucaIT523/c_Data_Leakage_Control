#pragma once

#include <afxcmn.h>

class ClaListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(ClaListCtrl)

public:
	ClaListCtrl();

	void setHead(const wchar_t* headers);
	int addRecord(const wchar_t* firstItem, ...);
	int addRecordWithIcon(int p_nIconIndex, const wchar_t* firstItem, ...);
	void autoFitWidth();
	int getSelectedIndex();

protected:
	int _nColCnt;

protected:
	DECLARE_MESSAGE_MAP()
};

