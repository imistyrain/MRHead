#ifndef MRTESSERACT_H
#define MRTESSERACT_H
#define TESS_WITH_OPENCV 1
#include "baseapi.h"
#include "string"
#ifdef _WIN32
#if _DEBUG
#if TESSERACT_VERSION>=0x040000
#pragma comment(lib,"tesseract400d.lib")
#else
#pragma comment(lib,"libtesseract304d.lib")
#pragma comment(lib,"liblept171d.lib")
#endif
#else
#if TESSERACT_VERSION>=0x040000
#pragma comment(lib,"tesseract400.lib")
#else
#pragma comment(lib,"libtesseract304.lib")
#pragma comment(lib,"liblept171.lib")
#endif
#endif
#endif

#if TESS_WITH_OPENCV
#include "opencv2/opencv.hpp"
#else
#include "allheaders.h"
#endif
#ifdef USE_ENG_DATA
const char* langname = "eng";
#else
#define TESSERACT_PASSPORT 1
#if TESSERACT_PASSPORT
const char* langname = "eng";
#endif
#endif

class MRTessNumRecognier
{
public:
	static MRTessNumRecognier *getInstance()
	{
		static MRTessNumRecognier instance;
		return &instance;
	}
#if TESS_WITH_OPENCV
	std::string recog(const cv::Mat &img)
	{
		std::string str = "";
		if (inited==0&&img.data)
		{
			api.Clear();
			api.ClearAdaptiveClassifier();
			api.SetImage((uchar*)img.data, img.cols, img.rows, img.channels(), img.step);
			char*ocrresult = api.GetUTF8Text();
			str = ocrresult;
			delete[]ocrresult;
		}
		return str;
	}
#endif
private:
	tesseract::TessBaseAPI  api;
	int inited = -1;
	MRTessNumRecognier(){
		inited = api.Init("./", "eng");
		api.SetVariable("tessedit_char_whitelist", "0123456789");
		api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
	}
	~MRTessNumRecognier()
	{
		api.End();
	}
};

class MRTessChineseRecognier{
public:
	static MRTessChineseRecognier *getInstance()
	{
		static MRTessChineseRecognier instance;
		return &instance;
	}
#if TESS_WITH_OPENCV
	std::string recog(const cv::Mat &img)
	{
		std::string str = "";
		if (inited == 0 && img.data)
		{
			api.Clear();
			api.ClearAdaptiveClassifier();
			api.SetImage((uchar*)img.data, img.cols, img.rows, img.channels(), img.step);
			char*ocrresult = api.GetUTF8Text();
			str = ocrresult;
			delete[]ocrresult;
		}
		return str;
	}
#endif
	
private:
	tesseract::TessBaseAPI  api;
	int inited = -1;
	MRTessChineseRecognier(){
		inited = api.Init("./", "chi_sim");
		api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
	}
	~MRTessChineseRecognier()
	{
		api.End();
	}
};

class MRTesseract
{
public:
	static MRTesseract *getInstance()
	{
		static MRTesseract instance;
		return &instance;
	}
#if TESS_WITH_OPENCV
	std::string recog(const cv::Mat &img)
	{
		std::string str = "";
		if (inited == 0 && img.data)
		{
			api.Clear();
			api.ClearAdaptiveClassifier();
			api.SetImage((uchar*)img.data, img.cols, img.rows, img.channels(), img.step);
			char*ocrresult = api.GetUTF8Text();
			str = ocrresult;
			delete[]ocrresult;
		}
		return str;
	}
	std::string recog(const std::string filepath)
	{
		std::string str="";
		cv::Mat img=cv::imread(filepath);
		if(!img.data)
			return str;
		else
			return recog(img);
	}
#else
	std::string recog(const std::string filepath)
	{
		Pix*  pixs = pixRead(filepath.c_str());
		if (pixs == NULL)
		{
			std::cout << "cannot load " << filepath << std::endl;
			return "";
		}
		api.SetImage(pixs);
		std::string ret = api.GetUTF8Text();
		pixDestroy(&pixs);
		return ret;
	}
#endif
private:
	MRTesseract()
	{
#ifdef USE_ENG_DATA
		inited=api.Init("./", "eng");
		api.SetVariable("tessedit_char_whitelist", "0123456789");
#else
#if TESSERACT_PASSPORT
		inited = api.Init("./", langname);
#else	
		api.SetVariable("tessedit_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ<");
#endif
		api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
#endif
	}
	tesseract::TessBaseAPI  api;
	int inited = -1;
	~MRTesseract()
	{
		api.End();
	}
};
#endif
