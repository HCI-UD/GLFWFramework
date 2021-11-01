#include "Button.h"
#include <opencv2/highgui/highgui.hpp>


bool Button::isInside( int x, int y)
{

	return mouseOntop = visible && active && x >= posX * windowsSize.x && x <= (posX + width) * windowsSize.x && y >= posY * windowsSize.y && y <= (posY + height) * windowsSize.y;
}

void Button::hide()
{
	visible = true;
	active = false;
}

void Button::switchOnOff()
{
	active = !active;
}

void Button::setActive(bool active)
{
	this->active = active;
}

void Button::setVisible(bool visible)
{
	this->visible = visible;
}

void Button::changeStyle(int style)
{
	
	std::string color;
	switch (style) {
	case 0:
		color = "White";
		break;
	case 1:
		color = "Red";
		break;
	case 2:
		color = "Blue";
		break;
	case 3:
		color = "Green";
		break;
	default:
		std::cout << "Not supported style " << style << std::endl;
		return;
	}
	this->style = style;
	std::string imgPath = "../../resources/skins/simpleButton";
	std::string imgExtension = ".png";

	skinActive = cv::imread(imgPath + "Light" + color + imgExtension, CV_LOAD_IMAGE_COLOR);
	skinInactive = cv::imread(imgPath + "Dark" + color + imgExtension, CV_LOAD_IMAGE_COLOR);
	if (!skinActive.data || !skinInactive.data)                              // Check for invalid input
	{
		std::cout << "Could not open or find the image for button" << std::endl;
		return;
	}
	cv::resize(skinActive, skinActive, cv::Size(width * windowsSize.x, height * windowsSize.y));
	cv::resize(skinInactive, skinInactive, cv::Size(width * windowsSize.x, height * windowsSize.y));
	addLabel(skinActive, label, cv::Scalar(255, 0, 0), cv::FONT_HERSHEY_DUPLEX);
	addLabel(skinInactive, label, cv::Scalar(255, 0, 0), cv::FONT_HERSHEY_DUPLEX);
	skinSelected = cv::Mat(height * windowsSize.y, width * windowsSize.x, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::addWeighted(skinActive, 0.8, skinSelected, 0.2, 0.0, skinSelected);
}
void Button::resizeWindow(double posX, double posY, int width, int height) {
	this->posX = posX;
	this->posY = posY;
	this->windowsSize = cv::Point2i(width, height);
	changeStyle(style);
}

void Button::resize(double width, double height)
{
	this->width = width;
	this->height = height;
}

void Button::resize(double posX, double posY, double width, double height)
{
	this->posX = posX;
	this->posY = posY;
	this->height = height;
	this->width = width;
	changeStyle(style);
}

void Button::doClick()
{
	callback(this);
}


void Button::setCallback(std::function<void(Button *b)> cb)
{
	this->callback = cb;
}

cv::Mat Button::getMat()
{
	if (visible) {
		if (active) 
		{
			if (mouseOntop) {
				return skinSelected;
			}
			return skinActive;
		}
		else 
		{
			return skinInactive;
		}
			
	}
	return cv::Mat();
}

void Button::addLabel(cv::Mat &img, std::string text, cv::Scalar color, int fontFace, double fontScale)
{
	CV_Assert(!img.empty() && (img.type() == CV_8UC3 || img.type() == CV_8UC1 /*|| img.type() == CV_8UC4*/));
	cv::Rect roi(width * windowsSize.x/4, height * windowsSize.y /4, img.cols - width * windowsSize.x / 4, img.rows - height * windowsSize.y / 4);
	CV_Assert(roi.area() > 0);
	if (text.empty())
		text = " ";
	
	int baseline = 0;

	double scale = fontScale;

	cv::Size textSize = cv::getTextSize(text, fontFace, scale, (int)scale + 1, &baseline);

	while(textSize.width > roi.width || textSize.height > roi.height){
		scale -= 0.2;
		textSize = cv::getTextSize(text, fontFace, scale, (int)scale + 1, &baseline);
	}
	baseline += (int)scale;
	cv::Point textOrg((img.cols - textSize.width) / 2, (img.rows + textSize.height - baseline) / 1.75);
	cv::putText(img, text, textOrg, fontFace, scale, color, scale + 1);

}

cv::Rect Button::getRoi() {
	return cv::Rect(posX * windowsSize.x, posY * windowsSize.y, width * windowsSize.x, height * windowsSize.y);
}