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
	FileInOut fileio;
	fileio.makeImageSavingdirectory();
	fileio.initializeCV();

	NtKinect kinect;
	while (1) {
		cv::Mat colorImage, depthImage, rawDepthImage;
		
		kinect.setRGB();
		kinect.setDepth(false);

		colorImage = kinect.rgbImage.clone();
		depthImage = kinect.depthMat.clone();
		rawDepthImage = kinect.rawDepthImage.clone();

		if (!fileio.updateCameraImage(colorImage, depthImage, rawDepthImage))
		{
			break;
		}
		if (_kbhit())
		{ // Break loop
			int c = _getch() & 255;
			if (c == 27 || c == 'q' || c == 'Q') break; // ESC|q|Q for Exit
		}
	}
	cv::destroyAllWindows();
	
	
	/*try {
		doJob();
	}
	catch (exception &ex) {
		cout << ex.what() << endl;
		string s;
		cin >> s;
	}*/
	wprintf_s(L"Exiting.\n");
	fileio.deleteEmptydirectory();
	while (1)
	{
		if (_kbhit())
		{
			break;
		}
	}
	return 0;
}
