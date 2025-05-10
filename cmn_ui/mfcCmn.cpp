#include "pch.h"
#include "mfcCmn.h"

unsigned int MsgBox(const wchar_t* p_wszFmt, ...)
{
	CString strMsg;
	va_list vl;
	va_start(vl, p_wszFmt);

	strMsg.FormatV(p_wszFmt, vl);
	return AfxMessageBox(strMsg);
}

void LogPrint(const wchar_t* p_wszFmt, ...)
{
	CString strMsg;
	va_list vl;
	va_start(vl, p_wszFmt);

	strMsg.FormatV(p_wszFmt, vl);
	OutputDebugStringW(strMsg);
}