#ifndef MRTESSERACT_H
#define MRTESSERACT_H
#define TESS_WITH_OPENCV 1
#include "baseapi.h"
#include "string"
#ifdef _WIN32
	#if _DEBUG
		#pragma comment(lib,"tesseract40d.lib")
	#else
		#pragma comment(lib,"tesseract40.lib")
	#endif
#endif

#if TESS_WITH_OPENCV
#include "opencv2/opencv.hpp"
#else
#include "allheaders.h"
#endif

class MRTesseract
{
public:
	static MRTesseract *getInstance()
	{
		static MRTesseract instance;
		return &instance;
	}
#if TESS_WITH_OPENCV
	std::string recog(const cv::Mat &img){
		std::string str = "";
		if (inited == 0 && img.data)
		{
			api.Clear();
			api.ClearAdaptiveClassifier();
			api.SetImage((uchar*)img.data, img.cols, img.rows, img.channels(), img.step);
			char * ocrresult = api.GetUTF8Text();
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
		inited=api.Init("./", "eng");
		api.SetVariable("tessedit_char_whitelist", "0123456789");
		api.SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
	}
	tesseract::TessBaseAPI  api;
	int inited = -1;
	~MRTesseract(){api.End();}
};
#endif
