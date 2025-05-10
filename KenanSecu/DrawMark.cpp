#include "pch.h"
#include "DrawMark.h"
#include "ClaSecuPolicy.h"
#include "Global.h"

struct handle_data {
	unsigned long process_id;
	HWND window_handle;
};

BOOL is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.process_id != process_id || !is_main_window(handle))
		return TRUE;
	data.window_handle = handle;
	return FALSE;
}
HWND find_main_window(unsigned long process_id)
{
	handle_data data;
	data.process_id = process_id;
	data.window_handle = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&data);
	return data.window_handle;
}


RECT previousRect = { 0 };
#define BORDER_WIDTH 4
#define BORDER_COLOR RGB(0, 160, 250)

void DrawRectangleWithLines(HDC hdc, int left, int top, int right, int bottom) {
	// Create a pen for drawing lines
	HPEN hPen = CreatePen(PS_SOLID, BORDER_WIDTH, BORDER_COLOR);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

	// Draw lines to form a rectangle
	MoveToEx(hdc, left, top, NULL);
	LineTo(hdc, right, top);
	LineTo(hdc, right, bottom);
	LineTo(hdc, left, bottom);
	LineTo(hdc, left, top);

	// Cleanup
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
}

// Function to draw the border
void DrawBorderEx(HWND hwndTarget, RECT& rect, bool bErase) {
	if (hwndTarget) {
		HWND hwndDesktop = ::GetDesktopWindow();
		//if (bErase) {
		RECT rcDesktop;
		GetWindowRect(hwndDesktop, &rcDesktop);
		InvalidateRect(hwndDesktop, &previousRect, TRUE);
		UpdateWindow(hwndDesktop);
		//}

		HDC hdc = GetWindowDC(hwndDesktop);

		DrawRectangleWithLines(hdc, rect.left + 8, rect.top + 2, rect.right - 8, rect.bottom - 8);
		ReleaseDC(hwndTarget, hdc);

		previousRect = rect;
	}
}

void DrawBorder(HWND hWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);

	if (rect.right < 0) {
		return;
	}

	DrawBorderEx(hWnd, rect, !EqualRect(&rect, &previousRect));
}

unsigned long TF_DRAW_MARK(void*) {

	LogPrintW(GD_LOG_LEVEL_NORMAL, GD_LOG_AREA_S_INIT, L"Capture Flag = [%d]", ClaSecuPolicy::GetDlpCapture());

	while (TRUE)
	{
		Sleep(300);
		HWND hWnd = find_main_window(GetCurrentProcessId());

		if (hWnd == NULL) {
			continue;
		}

		if (hWnd != GetForegroundWindow()) {
			continue;
		}

		if (ClaSecuPolicy::GetDlpCapture()) {
			SetWindowDisplayAffinity(hWnd, WDA_EXCLUDEFROMCAPTURE);
		}

		DrawBorder(hWnd);
	}
	return 0;
}

void startDrawMark()
{
	DWORD dwTID = 0;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TF_DRAW_MARK, NULL, 0, &dwTID);
}