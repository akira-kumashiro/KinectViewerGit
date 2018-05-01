#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core/core_c.h>
#include <opencv/highgui.h>
#include <opencv2\highgui\highgui.hpp>

#include<iostream>
#include<fstream>
#include <windows.h>
#include <direct.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define SHOW_DEPTH_ROW 6
#define AVERAGE_FRAME_NUM 20

class FileInOut
{
public:
	FileInOut();
	~FileInOut();
	bool updateCameraImage(cv::Mat colIm, cv::Mat depIm, cv::Mat rDpIm);
private:
	void initializeCV(void);
	bool makeImageSavingDirectory(void);
	static void onMouse(int event, int x, int y, int flags, void* param);
	void onMouse(int event, int x, int y, int flags);
	void saveCameraImage(bool isData);
	void convertGrayToColor(const cv::Mat grayImage);
	cv::Mat cutImage(cv::Mat originalImage, cv::Rect cutRect);
	void showSelectedDepth(void);
	void saveSelectedCameraImage(void);
	bool deleteEmptydirectory(void);

	//画像表示の記述
	std::vector<unsigned short> depthBuffer;
	cv::Point point = { -1,-1 };
	cv::Point origin;

	const std::string windownName[4] = { "Depth Image","Raw Depth Image","Color Image","CV Console" };
	cv::Mat colorImage;
	cv::Mat depthImage;
	cv::Mat rawDepthImage;
	cv::Mat rawDepthImageTemp;
	cv::Mat rawDepthImageSelected;
	cv::Mat depthImageSelected;
	cv::Mat cvConsole;
	cv::Mat rawDepthImageLevel;


	//クラス内変数
	int colorImageNum = 0;
	int depthImageNum = 0;
	int imageNum = 0;
	int dataNum = 0;

	bool select_object = FALSE;
	bool isAllSelected = FALSE;
	bool isHolden = FALSE;
	cv::Rect selection;
	cv::Rect selectionMeas;

	wchar_t directoryName[20];
	char nallowDirectoryName[20];
	std::string dataFileName;
	std::ofstream dataFile;
	char windowTitle[20];

	//cv::Mat handImage1;
	//cv::Mat handImage2;

	cv::Scalar mean, stdDev;
	cv::Scalar meanFix, stdDevFix;

	float meanFixHis[10] = { 0 }, stdDevFixHis[10] = { 0 };
	float meanAve, stdDevAve;
	double matMax, matMin, matMaxHol, matMinHol;
	cv::Point minLoc, maxLoc;
};