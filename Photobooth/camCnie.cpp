#include "stdafx.h"
#include <vfw.h>
#include "cnie.h"
#include <string>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

std::shared_ptr<cv::VideoCapture> cnie::video_capture;

void cnie::connectWebcam(HWND capWindow) {
	video_capture = std::shared_ptr<cv::VideoCapture>(new cv::VideoCapture(0));
	cv::Mat frame;
	
	std::thread camThread = std::thread(kek);
	cv::imshow();
	/*
	CAPDRIVERCAPS CapDriverCaps = {};
	CAPSTATUS     CapStatus = {};

	capDriverGetCaps(capWindow, &CapDriverCaps, sizeof(CAPDRIVERCAPS));

	if (CapDriverCaps.fHasDlgVideoSource)
	{
		capDlgVideoSource(capWindow);
	}

	if (CapDriverCaps.fHasDlgVideoFormat)
	{
		capDlgVideoFormat(capWindow);

		capGetStatus(capWindow, &CapStatus, sizeof(CAPSTATUS));
	}

	if (CapDriverCaps.fHasDlgVideoDisplay)
	{
		capDlgVideoDisplay(capWindow);
	}
	*/
}

void kek(cv::VideoCapture cap, bool& running) {
	bool running;
	cv::Mat frame;
	while (running) {

	}
}

void cnie::startCapture(HWND capWindow) {
	ShowWindow(capWindow, SW_SHOW);
	SendMessage(capWindow, WM_CAP_DRIVER_CONNECT, 0, 0);
	SendMessage(capWindow, WM_CAP_SET_SCALE, true, false);
	SendMessage(capWindow, WM_CAP_SET_PREVIEWRATE, 1, 0);
	SendMessage(capWindow, WM_CAP_SET_PREVIEW, true, 0);
}

void cnie::stopCapture(HWND capWindow) {
	ShowWindow(capWindow, SW_HIDE);
	SendMessage(capWindow, WM_CAP_DRIVER_DISCONNECT, 0, 0);
}

void cnie::getCaptureDims(HWND capWindow, int& width, int& height) {
	auto parms = CAPTUREPARMS();

	//capCaptureGetSetup(capWindow, parms, sizeof(CAPTUREPARMS));

	parms.fStepCaptureAt2x = true;
	//parms.fYield = true;
	//parms.fLimitEnabled = false;

	capCaptureSetSetup(capWindow, &parms, sizeof(CAPTUREPARMS));
	//SendMessage(capWindow, WM_CAP_SET_VIDEOFORMAT, sizeof(BITMAPINFO), (LPARAM)&set);
	//BITMAPINFO info = BITMAPINFO();
	//SendMessage(capWindow, WM_CAP_GET_VIDEOFORMAT, sizeof(BITMAPINFO), (LPARAM)&info);
	//SendMessage(capWindow, WM_CAP_SET_MCI_DEVICE, L"avivideo=mciavi.drv", (LPARAM)&set);

	OutputDebugString(std::to_wstring(parms.dwRequestMicroSecPerFrame).c_str());

	//BITMAPINFO info;
	//capGetVideoFormat(capWindow, &info, sizeof(BITMAPINFO));
	int w = 1280;
	int h = 960;
	BITMAPINFO info = BITMAPINFO {
		(DWORD)2,
		(LONG)w,
		(LONG)h,
		(WORD)1,
		(WORD)1,
		(DWORD)BI_RGB,
		(DWORD)(w * h * 2),
		(LONG)1,
		(LONG)1,
		(DWORD)0,
		(DWORD)0
	};

	bool g = capSetVideoFormat(capWindow, &info, sizeof(BITMAPINFO));

	width = info.bmiHeader.biWidth;
	height = info.bmiHeader.biHeight;
}

void cnie::captureFrame(HWND capWindow) {
	//Grab a Frame
	SendMessage(capWindow, WM_CAP_GRAB_FRAME, 0, 0);
	//Copy the frame we have just grabbed to the clipboard
	SendMessage(capWindow, WM_CAP_EDIT_COPY, 0, 0);

	//Copy the clipboard image data to a HBITMAP object called hbm
	PAINTSTRUCT ps;
	HDC camPaint = BeginPaint(capWindow, &ps);
	HDC camPaintMem = CreateCompatibleDC(camPaint);
	HBITMAP camPaintBMP = nullptr;
	if (camPaintMem != NULL)
	{
		if (OpenClipboard(capWindow))
		{
			camPaintBMP = (HBITMAP)GetClipboardData(CF_BITMAP);
			SelectObject(camPaintMem, camPaintBMP);
			RECT camPaintRect;
			GetClientRect(capWindow, &camPaintRect);
			CloseClipboard();
		}
	}
	//Save hbm to a .bmp file called Frame.bmp
	PBITMAPINFO pbi = CreateBitmapInfoStruct(base_window, camPaintBMP);
	CreateBMPFile(base_window, (LPTSTR)L"Frame.bmp", pbi, camPaintBMP, camPaintMem);

	cnie::startCapture(capWindow);
}

HBITMAP cnie::captureBitmap(HWND capWindow) {

	HBITMAP ret = nullptr;

	//Grab a Frame
	SendMessage(capWindow, WM_CAP_GRAB_FRAME, 0, 0);
	//Copy the frame we have just grabbed to the clipboard
	SendMessage(capWindow, WM_CAP_EDIT_COPY, 0, 0);

	//Copy the clipboard image data to a HBITMAP object called hbm
	PAINTSTRUCT ps;
	HDC paint = BeginPaint(capWindow, &ps);
	HDC paintMemory = CreateCompatibleDC(paint);
	if (paintMemory != NULL)
	{
		if (OpenClipboard(capWindow))
		{
			ret = (HBITMAP)CopyImage((HBITMAP)GetClipboardData(CF_BITMAP), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);
			CloseClipboard();
		}
	}

	//resume device preview
	cnie::startCapture(capWindow);

	ReleaseDC(capWindow, paint);
	return ret;
}

HWND cnie::createCaptureWindow(int x, int y, int width, int height) {
	return capCreateCaptureWindowW(L"camera window", WS_CHILD, x, y, width, height, base_window, 0);
}

void cnie::CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC) {
		HANDLE hf;                  // file handle
		BITMAPFILEHEADER hdr;       // bitmap file-header
		PBITMAPINFOHEADER pbih;     // bitmap info-header
		LPBYTE lpBits;              // memory pointer
		DWORD dwTotal;              // total count of bytes
		DWORD cb;                   // incremental count of bytes
		BYTE *hp;                   // byte pointer
		DWORD dwTmp;

		pbih = (PBITMAPINFOHEADER)pbi;
		lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

		if (!lpBits)
		{
				MessageBox(hwnd, L"GlobalAlloc", L"Error", MB_OK);
		}
		// Retrieve the color table (RGBQUAD array) and the bits
		// (array of palette indices) from the DIB.
		if (!GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS))
		{
				MessageBox(hwnd, L"GetDIBits", L"Error", MB_OK);
		}
		// Create the .BMP file.
		hf = CreateFile(pszFile, GENERIC_READ | GENERIC_WRITE, (DWORD)0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
		if (hf == INVALID_HANDLE_VALUE)
		{
				MessageBox(hwnd, L"CreateFile", L"Error", MB_OK);
		}

		hdr.bfType = 0x4d42;  // File type designator "BM" 0x42 = "B" 0x4d = "M"
		// Compute the size of the entire file.
		hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage);
		hdr.bfReserved1 = 0;
		hdr.bfReserved2 = 0;
		// Compute the offset to the array of color indices.
		hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD);
		// Copy the BITMAPFILEHEADER into the .BMP file.
		if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER), (LPDWORD)&dwTmp, NULL))
		{
				MessageBox(hwnd, L"WriteFileHeader", L"Error", MB_OK);
		}
		// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
		if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof(RGBQUAD), (LPDWORD)&dwTmp, NULL))
		{
				MessageBox(hwnd, L"WriteInfoHeader", L"Error", MB_OK);
		}
		// Copy the array of color indices into the .BMP file.
		dwTotal = cb = pbih->biSizeImage;
		hp = lpBits;
		if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL))
		{
				MessageBox(hwnd, L"WriteFile", L"Error", MB_OK);
		}
		// Close the .BMP file.
		if (!CloseHandle(hf))
		{
				MessageBox(hwnd, L"CloseHandle", L"Error", MB_OK);
		}

		// Free memory.
		GlobalFree((HGLOBAL)lpBits);
}

PBITMAPINFO cnie::CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{
		BITMAP bmp;
		PBITMAPINFO pbmi;
		WORD cClrBits;
		// Retrieve the bitmap color format, width, and height.
		if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp))
		{
				MessageBox(hwnd, L"GetObject", L"Error", MB_OK);
		}
		// Convert the color format to a count of bits.
		cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
		if (cClrBits == 1)
			cClrBits = 1;
		else if (cClrBits <= 4)
			cClrBits = 4;
		else if (cClrBits <= 8)
			cClrBits = 8;
		else if (cClrBits <= 16)
			cClrBits = 16;
		else if (cClrBits <= 24)
			cClrBits = 24;
		else cClrBits = 32;
		// Allocate memory for the BITMAPINFO structure. (This structure
		// contains a BITMAPINFOHEADER structure and an array of RGBQUAD
		// data structures.)
		if (cClrBits != 24)
		{
				pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits));
		}
		// There is no RGBQUAD array for the 24-bit-per-pixel format.
		else
			pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

		// Initialize the fields in the BITMAPINFO structure.
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbmi->bmiHeader.biWidth = bmp.bmWidth;
		pbmi->bmiHeader.biHeight = bmp.bmHeight;
		pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
		pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
		if (cClrBits < 24)
		{
				pbmi->bmiHeader.biClrUsed = (1 << cClrBits);
		}
		// If the bitmap is not compressed, set the BI_RGB flag.
		pbmi->bmiHeader.biCompression = BI_RGB;

		// Compute the number of bytes in the array of color
		// indices and store the result in biSizeImage.
		// For Windows NT, the width must be DWORD aligned unless
		// the bitmap is RLE compressed. This example shows this.
		// For Windows 95/98/Me, the width must be WORD aligned unless the
		// bitmap is RLE compressed.
		pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8 * pbmi->bmiHeader.biHeight;
		// Set biClrImportant to 0, indicating that all of the
		// device colors are important.
		pbmi->bmiHeader.biClrImportant = 0;

		return pbmi; //return BITMAPINFO
}
