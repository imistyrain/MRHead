#include "iostream"
#include "mropencv.h"

void testImage(const std::string imgpath = "opencv.png")
{
	cv::Mat img = cv::imread(imgpath);
	if(img.data == NULL){
		std::cout<<imgpath<<" doesn't exists"<<std::endl;
		return;
	}
	cv::namedWindow("img", 0);
	cv::imshow("img", img);
	cv::waitKey();
}
void testProfile(const int n = 10000){
	int sum = 0;
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
		sum += j;
	}
	}
	std::cout<<sum<<std::endl;
}
void testCamera(const int device = 0)
{
	cv::VideoCapture capture(device);
	cv::Mat frame;
	while(true)
	{
		capture >> frame;
		if (!frame.data)
			break;
		cv::imshow("img", frame);
		cv::waitKey(1);
	}
}

void testVideo(const std::string videoPath = "test.mov")
{
	cv::VideoCapture capture(videoPath);
	cv::Mat frame;
	int frame_count = 0;
	while(true)
	{
		capture >> frame;
		if (!frame.data)
			break;
		std::cout << frame_count << std::endl;
		cv::imshow("img", frame);
		cv::imwrite("test.jpg", frame);
		cv::waitKey(1);
		frame_count++;
	}
}

#if WITH_LOG
#define IOS_LOG 1
#include "mrlog.h"
void test_log(){
	MRTIMER_START(test_log);
	int sum = 0;
	for(int i=0;i<100000;i++){
		sum+=i;
	}
	LOGI("read time: %fms\n", MRTIMER_END(test_log));
}
#endif

int main()
{
	testProfile();
//	testImage();
//	testCamera();
//	testVideo();
	return 0;
}