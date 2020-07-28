#ifndef _MROPENCVUTIL_H_
#define _MROPENCVUTIL_H_
#include "opencv2/opencv.hpp"
#include "atlimage.h"
static BOOL Mat2CImage(const cv::Mat& src_img, CImage& dst_img)
{
	if (!src_img.data)
	{
		return FALSE;
	}
	int width = src_img.cols;
	int height = src_img.rows;
	int channels = src_img.channels();
	int src_type = src_img.type();

	dst_img.Destroy();

	switch (src_type)
	{
	case CV_8UC1:
	{
		dst_img.Create(width, -1 * height, 8 * channels);
		unsigned char* dst_data = static_cast<unsigned char*>(dst_img.GetBits());
		int step_size = dst_img.GetPitch();
		unsigned char* src_data = nullptr;
		for (int i = 0; i < height; i++)
		{
			src_data = const_cast<unsigned char*>(src_img.ptr<unsigned char>(i));    
			for (int j = 0; j < width; j++)
			{
				if (step_size > 0)
				{
					*(dst_data + step_size*i + j) = *src_data++;
				}  
				else
				{
					*(dst_data + step_size*i - j) = *src_data++;
				}
			}
		}
		break;
	}
	case CV_8UC3:
	{
		dst_img.Create(width, height, 8 * channels);
		unsigned char* dst_data = static_cast<unsigned char*>(dst_img.GetBits());
		int step_size = dst_img.GetPitch();
		unsigned char* src_data = nullptr;
		for (int i = 0; i < height; i++)
		{
			src_data = const_cast<unsigned char*>(src_img.ptr<unsigned char>(i));    
			for (int j = 0; j < width; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					*(dst_data + step_size*i + j * 3 + k) = src_data[3 * j + k];
				}
			}
		}
		break;
	}
	default:
		break;
	}
	return TRUE;
}


//功能：把Mat绘制到pWnd所代表的窗体上，使用方法如下所示:
//DrawMatToWnd(GetDlgItem(IDC_PIC), img);
static void DrawMat2CWnd(CWnd* pWnd, cv::Mat &img, CRect *Roi = NULL)
{
	CImage imgDst;
	Mat2CImage(img,imgDst);
	CRect drect;
	if (Roi == NULL)
		pWnd->GetClientRect(drect);
	else
		drect = *Roi;
	CDC* dc=pWnd->GetDC();
	imgDst.Draw(dc->GetSafeHdc(), drect);
	pWnd->ReleaseDC(dc);
}

static void DrawMat2Wnd(const HWND &hWnd, cv::Mat &img, CRect *Roi = NULL)
{
	CWnd*pWnd = CWnd::FromHandle(hWnd);
	DrawMat2CWnd(pWnd, img);
}

static void BitMat2Wnd(CWnd* wnd, cv::Mat &img, CRect *Roi=NULL)
{
	if (img.empty())
		return;
	CDC *cdc = wnd->GetDC();
	CDC MemDC;
	CBitmap MemBitmap;
	CRect rect, drect;
	wnd->GetClientRect(rect);
	Gdiplus::Bitmap bitmap(img.cols, img.rows, img.cols * img.channels(), PixelFormat24bppRGB, (BYTE*)img.data);
	if (Roi == NULL)
		drect = rect;
	else
		drect = *Roi;
	MemDC.CreateCompatibleDC(cdc);
	MemBitmap.CreateCompatibleBitmap(cdc, rect.Width(), rect.Height());
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
	MemDC.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(255, 255, 255));
	Gdiplus::Graphics g(MemDC.m_hDC);
	Gdiplus::Image *ii = &bitmap;
	g.DrawImage(ii, Gdiplus::Rect(0, 0, drect.Width(), drect.Height()));
	g.ReleaseHDC(MemDC.m_hDC);
	cdc->BitBlt(0, 0, drect.Width(), drect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	wnd->ReleaseDC(cdc);
}
#endif