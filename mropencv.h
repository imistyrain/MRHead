#ifndef _MROPENCV_H_
#define _MROPENCV_H_

#include "opencv2/opencv.hpp"

#ifndef NOT_USE_OPENCV_NAMESPACE
	using namespace cv;
#endif

#ifdef _WIN32

	#define CV_VERSION_ID CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)
	
	#ifdef _DEBUG
		#define cvLIB(name) "opencv_" name CV_VERSION_ID "d"
	#else
		#define cvLIB(name) "opencv_" name CV_VERSION_ID
	#endif

	#ifdef OPENCV_STATIC
		#pragma comment( lib,"vfw32.lib")
		#pragma comment( lib,"comctl32.lib")
		#pragma comment( lib,"zlib.lib")
		#pragma comment( lib,"libjasper.lib")
		#pragma comment( lib,"libjpeg.lib")
		#pragma comment( lib,"libpng.lib")
		#pragma comment( lib,"libtiff.lib")
		#pragma comment( lib,"IlmImf.lib")
		#pragma comment( lib,"COMDLG32.lib")
	#endif

	#if CV_MAJOR_VERSION == 2
		#include <opencv2/nonfree/nonfree.hpp>
		#pragma comment( lib, cvLIB("core"))
		#pragma comment( lib, cvLIB("imgproc"))
		#pragma comment( lib, cvLIB("highgui"))
		#pragma comment( lib, cvLIB("contrib"))
		#if CV_USE_ALL_MODULES
			#pragma comment( lib, cvLIB("flann"))
			#pragma comment( lib, cvLIB("features2d"))
			#pragma comment( lib, cvLIB("calib3d"))
			#pragma comment( lib, cvLIB("ml"))
			#pragma comment( lib, cvLIB("legacy"))
			#pragma comment( lib, cvLIB("objdetect"))
			#pragma comment( lib, cvLIB("video"))
			#pragma comment( lib, cvLIB("nonfree"))
		#endif
	#else
		#if CV_MINOR_VERSION >= 3
			#include <opencv2/dnn.hpp>
		#endif

		#if CV_MINOR_VERSION == 1
			#include "tickmeter.hpp"
            #pragma comment( lib, cvLIB("core"))
            #pragma comment( lib, cvLIB("highgui"))
            #pragma comment( lib, cvLIB("imgproc"))
            #pragma comment( lib, cvLIB("imgcodecs"))
			#pragma comment( lib, cvLIB("video"))
			#pragma comment( lib, cvLIB("videoio"))
			#pragma comment( lib, cvLIB("calib3d"))
			#pragma comment( lib, cvLIB("ml"))
			#pragma comment( lib, cvLIB("objdetect"))
		#endif

		#ifdef OPENCV_STATIC
			#pragma comment( lib, "ippicvmt.lib")
			#pragma comment( lib, "libwebp.lib")
		#endif

		#if CV_USE_ALL_MODULES
			#pragma comment( lib, cvLIB("dnn"))		
			#pragma comment( lib, cvLIB("video"))		
			#pragma comment( lib, cvLIB("calib3d"))
			#pragma comment( lib, cvLIB("ml"))
			#pragma comment( lib, cvLIB("flann"))
			#pragma comment( lib, cvLIB("features2d"))
			#pragma comment( lib, cvLIB("objdetect"))
			#pragma comment( lib, cvLIB("tracking"))
        #endif

		#if CV_MAJOR_VERSION>=4 || CV_MINOR_VERSION >=4
			#pragma comment( lib, cvLIB("world") )	
		#endif
	#endif
#endif

#endif