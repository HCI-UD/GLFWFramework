#pragma once
#include <string>
#include <functional>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

class Button {

public:

	Button() {}

	Button(std::string label, double posX, double posY, double width, double height, cv::Point2i windowsSize, bool visible = true, bool active = true, int style = 0) :
		label(label),
		posX(posX),
		posY(posY),
		width(width),
		height(height),
		windowsSize(windowsSize),
		visible(visible),
		active(active),
		style(style)
	{ 
		changeStyle(style);
	}

	Button(std::string label, double posX, double posY, double width, double height, cv::Point2i windowsSize, std::function<void(Button *b)> callback, bool visible = true) :
		Button(label, posX, posY, width, height, windowsSize, visible)
	{
		this->callback = callback;
	}


	bool isInside(int x, int y);
	void hide();
	void switchOnOff();
	void setActive(bool active);
	void setVisible(bool visible);
	void changeStyle(int style);
	void resizeWindow(double posX, double posY, int width, int height);
	void resize(double width, double height);
	void resize(double posX, double posY, double width, double height);
	void doClick();
	void setCallback(std::function<void(Button *b)> cb);
	void addLabel(cv::Mat &img, std::string text, cv::Scalar color, int fontFace = cv::FONT_HERSHEY_DUPLEX, double fontScale = 2);
	cv::Rect getRoi();

	cv::Mat getMat();

	std::function<void(Button *b)> callback;
	cv::Mat skinActive;
	cv::Mat skinInactive;
	cv::Mat skinSelected;
	double posX, posY;
	double width, height;
	cv::Point2i windowsSize;
	bool active;
	bool visible;
	bool mouseOntop = false;
	std::string label;
	int style;
	
	


};

