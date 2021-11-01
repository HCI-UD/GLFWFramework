#include "Common.h"

#include "stdafx.h"
#include <windows.h>
#include <Commdlg.h>

#include "ConfigHandler.h"
#include "qrgen.h"


ConfigHandler::ConfigHandler(std::string filepath)
{
	cv::FileStorage iFile;
	if (!iFile.open(filepath, cv::FileStorage::READ))
	{
		std::cout << "Error when open file: " << filepath << std::endl;
		std::getchar();
		exit(0);
	}
	int screenMarker;
	int printedMarker;
	//read attributes from file
	iFile["screenName"] >> screenName;
	iFile["printedMarkerID"] >> printedMarker;
	iFile["screenMarkerID"] >> screenMarker;
	iFile["kinectOffsetX"] >> kinectOffsetX;
	iFile["kinectOffsetY"] >> kinectOffsetY;
	iFile["kinectOffsetZ"] >> kinectOffsetZ;
	iFile["kinectTilt"] >> kinectTilt;
	iFile.release();
	//converte float to string and stores them
	printedMarkerID = std::to_string(printedMarker);
	screenMarkerID = std::to_string(screenMarker);
}

ConfigHandler::ConfigHandler()
{
	//Open configfile
	//#############################################################################

	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrInitialDir = "../../resources/calibration";
	ofn.lpstrFilter = "Text files (*.txt)\0*.txt\0All files (*.*)\0*.*";			//Filter allows to display either only test files or all files
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	ofn.lpstrTitle = "Open screen configuration file";
	std::string fileNameStr;
	if (GetOpenFileName(&ofn))
		fileNameStr = fileName;

	if (fileNameStr.empty()) {
		exit(-1);
	}
	//###############################################################################

	cv::FileStorage iFile;
	if (!iFile.open(fileNameStr, cv::FileStorage::READ))
	{
		std::cout << "Error when open file: " << fileNameStr << std::endl;
		std::getchar();
		exit(0);
	}
	int screenMarker;
	int printedMarker;
	//read attributes from file
	iFile["screenName"] >> screenName;
	iFile["printedMarkerID"] >> printedMarker;
	iFile["screenMarkerID"] >> screenMarker;
	iFile["kinectOffsetX"] >> kinectOffsetX;
	iFile["kinectOffsetY"] >> kinectOffsetY;
	iFile["kinectOffsetZ"] >> kinectOffsetZ;
	iFile["kinectTilt"] >> kinectTilt;
	iFile.release();
	//converte float to string and stores them
	printedMarkerID = std::to_string(printedMarker);
	screenMarkerID = std::to_string(screenMarker);
}

ConfigHandler::~ConfigHandler()
{
}

void ConfigHandler::initScreen(GLFWwindow * window)
{
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	glfwGetWindowSize(window, &screenRes_Width, &screenRes_Height);				//get screen's resolution
	glfwGetMonitorPhysicalSize(monitor, &screenSize_Width, &screenSize_Height); //retrieve real world physical size of the primary monitor in mm
	markerSize = min(screenSize_Width, screenSize_Height) * 0.4;				//calculate markersize
	isScreenInitialized = true;
}

cv::Mat ConfigHandler::CH_GenerateQR(std::string ipv4Address)
{

	try
	{

		std::string address = ipv4Address;
		int port = 14321;									//hardcoded port, default 14321
		std::cout << "My IP: " << address << std::endl;

		//prepare the string which will be decoded into the qr code
		std::string data = screenName + " " +
			address + " " +
			std::to_string(port) + " " +
			printedMarkerID + " " +
			screenMarkerID + " " +
			std::to_string(markerSize) + " " +
			std::to_string(screenRes_Width) + " " +
			std::to_string(screenRes_Height) + " " +
			std::to_string(screenSize_Width) + " " +
			std::to_string(screenSize_Height);

		qrGenerate(data, "..\\ipqr.bmp", 8);				//generate the QR code then save it into a bitmap file
	}
	catch (std::exception& error)
	{
		std::cerr << error.what() << std::endl;
	}
	catch (const std::string& error)
	{
		std::cerr << error.c_str() << std::endl;
	}

	cv::Mat qrcode = cv::imread("..\\ipqr.bmp", CV_LOAD_IMAGE_COLOR);   // Read the qr image file

	if (!qrcode.data) {                              // Check for invalid input
		std::cout << "Could not open or find the image for qr code" << std::endl;
		exit(0);
	}

	//create a white border of 10 pixels, since the generated qr code does not have a border, which is required for the detection
	cv::copyMakeBorder(qrcode, qrcode, 10, 10, 10, 10, cv::BorderTypes::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
	return qrcode;
}

cv::Mat ConfigHandler::CH_GenerateMarker(glm::uvec2 windowDimension)
{
	//generate display marker
	cv::Mat white = cv::Mat(windowDimension.y, windowDimension.x, CV_8UC3, cv::Scalar(255, 255, 255));
	char markerpath[256];

	sprintf(markerpath, "..\\..\\resources\\calibration\\MarkerID_%s.png", screenMarkerID.c_str());

	//read in image marker file
	cv::Mat marker = cv::imread(markerpath, CV_LOAD_IMAGE_COLOR);   // Read the file
	float markerSizeRealX = screenRes_Width * markerSize / screenSize_Width;
	float markerSizeRealY = screenRes_Height * markerSize / screenSize_Height;

	//resize it to 40% of the smaller physical size of the screen
	cv::resize(marker, marker, cv::Size(markerSizeRealX, markerSizeRealY), 0, 0, 0);

	//draw onto the white background
	cv::Rect roi = cv::Rect((windowDimension.x - markerSizeRealX) / 2, (windowDimension.y - markerSizeRealY) / 2, marker.cols, marker.rows);
	cv::Mat subview = white(roi);
	marker.copyTo(subview);
	marker = white;
	if (!marker.data) {				// Check for invalid input
		std::cout << "Could not open or find the image for marker" << std::endl;
		exit(0);
	}
	return marker;
}

bool ConfigHandler::isScreenInit()
{
	return isScreenInitialized;
}
