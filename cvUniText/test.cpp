#include <opencv2/opencv.hpp>
#include "cvUniText.hpp"
#include <iostream>

int main(int argc, char** argv) {
    cv::Mat img = cv::Mat(200,200, CV_8UC3,cv::Scalar(255,255,255));
    uni_text::UniText uniText("microhei.ttc", 22);
    cv::Rect rect = uniText.PutText(img, "你好，世界", cv::Point(60, 100), cv::Scalar(0,0,255));
    std::cout << rect << std::endl;
    cv::imwrite("result.jpg", img);
    return 0;
}
