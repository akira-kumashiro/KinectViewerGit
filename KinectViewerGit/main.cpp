#ifdef _DEBUG
//Debugモードの場合
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_core2411d.lib")
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_imgproc2411d.lib")
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_highgui2411d.lib")
#else
//Releaseモードの場合
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_core2411.lib")
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_imgproc2411.lib")
#pragma comment(lib,"C:\\opencv\\opencv-2.4.11\\build\\x64\\vc12\\lib\\opencv_highgui2411.lib")
#endif

#include <iostream>
#include <sstream>

#include "NtKinect.h"
#include "opencv/highgui.h"
#include <direct.h>
#include <conio.h>
#include "FileIO.h"

using namespace std;

int main(int argc, char** argv) 
{
	try
	{
		FileInOut *fileio;
		fileio = new FileInOut();

		NtKinect kinect;
		while (1)
		{
			cv::Mat colorImage, depthImage, rawDepthImage;

			kinect.setRGB();
			kinect.setDepth(false);

			colorImage = kinect.rgbImage.clone();
			depthImage = kinect.depthMat.clone();
			rawDepthImage = kinect.rawDepthImage.clone();

			if (!fileio->updateCameraImage(colorImage, depthImage, rawDepthImage))
			{
				break;
			}
		}
		wprintf_s(L"Exiting.\n");
		delete fileio;
		return 0;
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}
