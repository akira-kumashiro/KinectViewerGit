#include "FileIO.h"

FileInOut::FileInOut()
{
	initializeCV();
	std::cout << "Q/Esc:\t終了" << std::endl;
	std::cout << "S:\t保存" << std::endl;
	std::cout << "D:\t数値データの保存，選択範囲の画像の保存(選択されていれば)" << std::endl;


	//makeImageSavingDirectory();
}

FileInOut::~FileInOut()
{
	cv::destroyAllWindows();
	//deleteEmptydirectory();
}

void FileInOut::initializeCV(void)//各Windowの名前登録
{
	cv::namedWindow(windownName[0], CV_WINDOW_AUTOSIZE);
	cv::namedWindow(windownName[1], CV_WINDOW_AUTOSIZE);
	cv::namedWindow(windownName[2], CV_WINDOW_AUTOSIZE);
	cv::namedWindow(windownName[3], CV_WINDOW_AUTOSIZE);
	cvConsole = cv::Mat::zeros(cv::Size(640, 480), CV_8UC4);

	cv::setMouseCallback(windownName[1], onMouse, this);
}

bool FileInOut::makeImageSavingDirectory(void)//画像を保存するためのフォルダを作る
{
	SYSTEMTIME t;

	GetLocalTime(&t);
	swprintf_s(directoryName, 20, L"%04d%02d%02d%02d%02d%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
	sprintf_s(nallowDirectoryName, "%04d%02d%02d%02d%02d%02d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	if (!_wmkdir(directoryName))
	{
		wprintf_s(L"directory \"%ws\" has been created.\n", directoryName);
		return true;
	}
	return false;
}

void FileInOut::onMouse(int event, int x, int y, int flags, void * param)//ウィンドウクリック時の動作の登録
{
	FileInOut* pThis = (FileInOut*)param;
	pThis->onMouse(event, x, y, flags);
}

void FileInOut::onMouse(int event, int x, int y, int flags)//クリック座標の距離の表示
{
	if (select_object)
	{
		selection.x = CV_IMIN(x, origin.x);
		selection.y = CV_IMIN(y, origin.y);
		selection.width = selection.x + CV_IABS(x - origin.x);
		selection.height = selection.y + CV_IABS(y - origin.y);

		selection.x = CV_IMAX(selection.x, 0);
		selection.y = CV_IMAX(selection.y, 0);
		selection.width = CV_IMIN(selection.width, rawDepthImage.cols);
		selection.height = CV_IMIN(selection.height, rawDepthImage.rows);
		selection.width -= selection.x;
		selection.height -= selection.y;
		if (selection.width > 10 && selection.height > 10)
		{
			selectionMeas.x = selection.x + 5;
			selectionMeas.y = selection.y + 5;
			selectionMeas.width = selection.width - 10;
			selectionMeas.height = selection.height - 10;
		}
	}

	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		point.x = x;
		point.y = y;
		origin = cv::Point(x, y);
		selection = cv::Rect(x, y, 0, 0);
		select_object = true;
		isAllSelected = false;
		break;

	case CV_EVENT_LBUTTONUP:
		select_object = false;
		break;

	default:
		break;
	}
}

void FileInOut::saveCameraImage(bool isData)//カメラの画像の保存
{
	char imageName[4][40];

	if (imageNum == 0)
		makeImageSavingDirectory();

	if (!isData)
	{
		sprintf_s(imageName[1], "%ws\\color_%04d.tiff", directoryName, imageNum);
		imwrite(imageName[1], colorImage);
	}

	sprintf_s(imageName[0], "%ws\\depth_%04d.tiff", directoryName, imageNum);
	sprintf_s(imageName[2], "%ws\\rdepth_%04d.tiff", directoryName, imageNum);

	imwrite(imageName[0], depthImage);
	imwrite(imageName[2], rawDepthImage);

	imageNum++;

	if (!isData)
		wprintf_s(L"\nSaved as %hs and %hs and %hs and %hs.\n", imageName[0], imageName[1], imageName[2], imageName[3]);
}

void FileInOut::convertGrayToColor(const cv::Mat grayImage)//選択領域に着色
{
	rawDepthImageTemp = cv::Mat::zeros(cv::Size(rawDepthImage.cols, rawDepthImage.rows), CV_8UC3);
	cv::Mat grayTemp;
	grayImage.convertTo(grayTemp, CV_8U);

	for (int y = 0; y < rawDepthImageTemp.rows; y++)
	{
		cv::Vec3b *rawDepthImageTempPtr = rawDepthImageTemp.ptr<cv::Vec3b>(y);

		for (int x = 0; x < rawDepthImageTemp.cols; x++)
		{
			if (matMax != matMin && (!isHolden || matMinHol != matMaxHol) && x >= selection.x && x < selection.x + selection.width && y >= selection.y && y < selection.y + selection.height)
			{
				if (x == selection.x&&y == selection.y)
					wprintf_s(L"%lf,%lf,%lf,%lf\n", matMax, matMaxHol, matMin, matMinHol);
					//wprintf_s(L"%d,%d,%d,%d\n", matMax, matMaxHol, matMin, matMinHol);
				double xbrt = (isHolden ? ((double)(rawDepthImage.at<UINT16>(y, x) - matMinHol) / ((double)matMaxHol - matMinHol)) : ((double)(rawDepthImage.at<UINT16>(y, x) - matMin) / ((double)matMax - matMin)));
				//カラー変換
				if (xbrt < 0)
				{
					rawDepthImageTempPtr[x] = cv::Vec3b((matMinHol == matMin) ? 255 : (uchar)(rawDepthImage.at<UINT16>(y, x) - matMin) / (matMinHol - matMin) * 255, 0, 0);
					/*rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 0] = (matMinHol == matMin) ? 255 : (double)(rawDepthImage.at<UINT16>(y, x) - matMin) / (matMinHol - matMin) * 255;      //B
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 1] = 0;        //G
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 2] = 0;        //R*/
				}
				else if (xbrt > 1)
				{
					rawDepthImageTempPtr[x] = cv::Vec3b(0, 0, (matMaxHol == matMax) ? 255 : (uchar)(matMin - rawDepthImage.at<UINT16>(y, x)) / (matMaxHol - matMax) * 255);
					/*rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 0] = 0;      //B
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 1] = 0;        //G
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 2] = (matMaxHol == matMax) ? 255 : (double)(matMin - rawDepthImage.at<UINT16>(y, x)) / (matMaxHol - matMax) * 255;        //R*/
				}
				else if (xbrt >= 0 && xbrt <= 0.25)
				{
					rawDepthImageTempPtr[x] = cv::Vec3b(255, (int)((double)255 * sin(xbrt * 2 * M_PI)), 0);
					/*rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 0] = 255;      //B
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 1] = (int)((double)255 * sin(xbrt * 2 * M_PI));        //G
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 2] = 0;        //R*/
				}
				else if (xbrt > 0.25 && xbrt <= 0.5)
				{
					rawDepthImageTempPtr[x] = cv::Vec3b((int)((double)255 * sin(xbrt * 2 * M_PI)), 255, 0);
					/*rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 0] = (int)((double)255 * sin(xbrt * 2 * M_PI));  //B
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 1] = 255;      //G
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 2] = 0;        //R*/
				}
				else if (xbrt > 0.5 && xbrt <= 0.75)
				{
					rawDepthImageTempPtr[x] = cv::Vec3b(0, 255, (int)(-(double)255 * sin(xbrt * 2 * M_PI)));
					/*rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 0] = 0;        //B
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 1] = 255;      //G
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 2] = (int)(-(double)255 * sin(xbrt * 2 * M_PI));  //R*/
				}
				else
				{
					rawDepthImageTempPtr[x] = cv::Vec3b(0, (int)(-(double)255 * sin(xbrt * 2 * M_PI)), 255);
					/*rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 0] = 0;        //B
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 1] = (int)(-(double)255 * sin(xbrt * 2 * M_PI));  //G
					rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 2] = 255;      //R*/
				}
			}
			else
			{
				int xbrt = (grayTemp.at<UINT8>(y, x));
				if (isHolden&&x == selection.x&&y == selection.y)
					wprintf_s(L"%lf,%lf,%lf,%lf\n", matMax, matMaxHol, matMin, matMinHol);
					//wprintf_s(L"%d,%d,%d,%d\n", matMax, matMaxHol, matMin, matMinHol);
				rawDepthImageTempPtr[x] = cv::Vec3b(xbrt, xbrt, xbrt);
				/*rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 0] = xbrt;        //B
				rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 1] = xbrt;	 //G
				rawDepthImageTemp.data[y * rawDepthImageTemp.step + x * 3 + 2] = xbrt;      //R*/
			}

		}
	}
}

bool FileInOut::updateCameraImage(cv::Mat colIm, cv::Mat depIm, cv::Mat rDpIm)//画像取得系関数
{
	colorImage = colIm.clone();
	depthImage = depIm.clone();
	rawDepthImage = rDpIm.clone();

	cv::minMaxLoc(rawDepthImageSelected, &matMin, &matMax, &minLoc, &maxLoc);

	convertGrayToColor(rawDepthImage);

	if (!isHolden)
	{
		matMaxHol = matMax;
		matMinHol = matMin;
	}

	showSelectedDepth();

	cv::imshow(windownName[0], depthImage);
	cv::imshow(windownName[1], rawDepthImageTemp);
	if (point.x > -1 && point.y > -1)
	{
		cv::imshow(windownName[3], cvConsole);
	}
	cv::imshow(windownName[2], colorImage);
	// 表示する

	int c = cv::waitKey(10);
	if ((c == 27) || (c == 'q') || (c == 'Q'))
	{
		// ESC|q|Q for Exit
		return false;
	}
	else if ((c == 's') || (c == 'S'))
	{
		saveCameraImage(false);
	}
	else if (c == 'd' || c == 'D')
	{
		if (dataNum == 0)
		{
			dataFileName = std::string(nallowDirectoryName) + "\\dadaFile.csv";
			dataFile.open(dataFileName, std::ios::out);
		}

		dataFile << dataNum << ',' << selection.height << ',' << selection.width << ',' << mean.val[0] << ',' << stdDev.val[0] << ',' << selectionMeas.height << ',' << selectionMeas.width << ',' << meanAve << ',' << stdDevAve << ',' << matMin << ',' << matMax << std::endl;

		if (selection.width > 0 && selection.height > 0)
		{
			saveSelectedCameraImage();
			saveCameraImage(true);
		}
		wprintf_s(L"the data saved.\n");
		dataNum++;
	}
	//↓うまくいってない
	/*else if (c == 'h' || c == 'H')
	{
		if (isHolden)
			isHolden = FALSE;
		else
			isHolden = TRUE;
	}
	else if (c == 'a' || c == 'A')
	{
		if (isAllSelected)
		{
			isAllSelected = FALSE;
			selection.width = 0;
			selection.height = 0;
		}
		else
		{
			isAllSelected = TRUE;
			selection.x = 0;
			selection.y = 0;
			selection.width = rawDepthImage.cols;
			selection.height = rawDepthImage.rows;
			select_object = FALSE;
		}
	}*/

	return true;
}

cv::Mat FileInOut::cutImage(cv::Mat originalImage, cv::Rect cutRect)//矩形選択
{
	cv::Mat cutImage(originalImage, cutRect);

	return cutImage;
}

void FileInOut::showSelectedDepth(void)
{
	if (selection.width > 0 && selection.height > 0)
	{
		rawDepthImageSelected = cutImage(rawDepthImage, selection);
		cv::Mat rawDepthImageFix = cutImage(rawDepthImage, selectionMeas);
		cv::meanStdDev(rawDepthImageSelected, mean, stdDev);

		cv::meanStdDev(rawDepthImageFix, meanFix, stdDevFix);

		meanAve = 0;
		stdDevAve = 0;

		for (int i = 0; i < AVERAGE_FRAME_NUM - 1; i++)
		{
			float meanTemp, stdDevTemp;

			meanAve += meanFixHis[i] / AVERAGE_FRAME_NUM;
			stdDevAve += stdDevFixHis[i] / AVERAGE_FRAME_NUM;

			meanTemp = meanFixHis[i];
			meanFixHis[i + 1] = meanTemp;
			stdDevTemp = stdDevFixHis[i];
			stdDevFixHis[i + 1] = stdDevTemp;
		}

		meanFixHis[0] = (float)meanFix.val[0];
		stdDevFixHis[0] = (float)stdDevFix.val[0];

		meanAve += meanFixHis[0] / AVERAGE_FRAME_NUM;
		stdDevAve += stdDevFixHis[0] / AVERAGE_FRAME_NUM;
	}

	if (point.x >= 0 && point.y >= 0)
	{
		cvConsole = cv::Mat::zeros(cv::Size(640, 480), CV_8UC4);
		cv::circle(rawDepthImageTemp, point, 1, cv::Scalar(255 * 255 * 255), 3);
		float depth = rawDepthImage.at<UINT16>(point.y, point.x);

		char distance[14];
		char distanceP[24];
		std::string str[SHOW_DEPTH_ROW];

		sprintf_s(distance, "%09.4f[mm]", depth);

		sprintf_s(distanceP, "(%03d,%03d)=%09.4f[mm]", point.x, point.y, depth);
		str[0] = distanceP;

		if (selection.width > 0 && selection.height > 0)
		{
			str[1] = "width*height=" + std::to_string(selection.width) + "*" + std::to_string(selection.height);
			str[2] = "mean=" + std::to_string(mean.val[0]) + "[mm]";
			str[3] = "standard deviation=" + std::to_string(stdDev.val[0]);
			str[4] = "mean(fixed,average)=" + std::to_string(meanAve) + "[mm]";
			str[5] = "standard deviation(fixed,average)=" + std::to_string(stdDevAve);
		}
		cv::putText(rawDepthImageTemp, std::string(distance), point, cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255 * 255), 1, 8);
		for (int i = 0; i < SHOW_DEPTH_ROW; i++)
		{
			cv::putText(cvConsole, str[i], { 0,17 * (i + 1) }, cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(255, 255, 255, 0), 1, 8);
		}
	}
}

void FileInOut::saveSelectedCameraImage(void)//部分画像の保存
{
	char imageName[3][40];
	sprintf_s(imageName[0], "%ws\\rdepthSel_%04d.tiff", directoryName, dataNum);
	sprintf_s(imageName[1], "%ws\\depthSel_%04d.tiff", directoryName, dataNum);
	sprintf_s(imageName[2], "%ws\\rdepthLev_%04d.tiff", directoryName, dataNum);

	depthImageSelected = cutImage(depthImage, selection);

	rawDepthImageLevel = cv::Mat::zeros(cv::Size(selection.width, selection.height), CV_8UC3);

	for (int y = 0; y < rawDepthImageLevel.rows; y++)
	{
		cv::Vec3b *rawDepthImageLevelPtr = rawDepthImageLevel.ptr<cv::Vec3b>(y);
		for (int x = 0; x < rawDepthImageLevel.cols; x++)
		{
			if (matMax != matMin)
			{
				double xbrt = (double)(rawDepthImageSelected.at<UINT16>(y, x) - matMin) / ((double)matMax - matMin);
				//カラー変換
				if (xbrt >= 0 && xbrt <= 0.25)
				{
					rawDepthImageLevelPtr[x] = cv::Vec3b(255, (int)((double)255 * sin(xbrt * 2 * M_PI)), 0);
					/*rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 0] = 255;      //B
					rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 1] = (int)((double)255 * sin(xbrt * 2 * M_PI));        //G
					rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 2] = 0;        //R*/
				}
				else if (xbrt > 0.25 && xbrt <= 0.5)
				{
					rawDepthImageLevelPtr[x] = cv::Vec3b((int)((double)255 * sin(xbrt * 2 * M_PI)), 255, 0);
					/*rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 0] = (int)((double)255 * sin(xbrt * 2 * M_PI));  //B
					rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 1] = 255;      //G
					rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 2] = 0;        //R*/
				}
				else if (xbrt > 0.5 && xbrt <= 0.75)
				{
					rawDepthImageLevelPtr[x] = cv::Vec3b(0, 255, (int)(-(double)255 * sin(xbrt * 2 * M_PI)));
					/*rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 0] = 0;        //B
					rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 1] = 255;      //G
					rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 2] = (int)(-(double)255 * sin(xbrt * 2 * M_PI));  //R*/
				}
				else
				{
					rawDepthImageLevelPtr[x] = cv::Vec3b(0, (int)(-(double)255 * sin(xbrt * 2 * M_PI)), 255);
					/*rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 0] = 0;        //B
					rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 1] = (int)(-(double)255 * sin(xbrt * 2 * M_PI));  //G
					rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 2] = 255;      //R*/
				}
			}
			else
			{
				int xbrt = (int)(matMax / 255);
				rawDepthImageLevelPtr[x] = cv::Vec3b(xbrt, xbrt, xbrt);
				/*rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 0] = xbrt;        //B
				rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 1] = xbrt;	 //G
				rawDepthImageLevel.data[y * rawDepthImageLevel.step + x * 3 + 2] = xbrt;      //R*/
			}

		}
	}
	imwrite(imageName[0], rawDepthImageSelected);
	imwrite(imageName[1], depthImageSelected);
	imwrite(imageName[2], rawDepthImageLevel);
}

bool FileInOut::deleteEmptydirectory(void)//何も画像を保存しなかったときに生じる空のディレクトリの削除
{
	if (!imageNum && !dataNum)
	{
		if (_wrmdir(directoryName) == 0)
		{
			wprintf_s(L"directory \"%ws\" has been removed.\n", directoryName);
			return true;
		}
		else
		{
			wprintf_s(L"directory removing error\n");
		}
	}
	return false;
}
