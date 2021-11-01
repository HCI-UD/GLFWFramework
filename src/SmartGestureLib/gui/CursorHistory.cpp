#include "Common.h"
#include <ctime>
#include "screen\ConfigHandler.h"
#include "CursorHistory.h"

CursorHistory::CursorHistory(int historyLength, std::shared_ptr<ConfigHandler> cfg):
	m_historyLength(historyLength)
{
	//Only allow further operations, if configuration file existent and initialized.
	if (cfg == nullptr) {
		std::cout << "ConfigurationHandler is null!" << std::endl;
		exit(-1);
	}
	if (!cfg->isScreenInit()) {
		std::cout << "ConfigurationHandler is unititialized!" << std::endl;
		exit(-1);
	}
	m_config = cfg;

	//start click delay monitor
	clickDelay = std::clock();
}

auto CursorHistory::addEntry(int x, int y, int z) -> void
{
	addEntry(glm::vec3(x, y, z));
}

auto CursorHistory::addEntry(glm::vec3 pos) -> void
{
	if (m_hist.size() < m_historyLength) {
		//Creates new entries, if the size of the list did not reach the maximum historylenght
		m_posPointer++;
		m_hist.push_back(pos);
	}
	else {
		//Overwrite oldest entries with the newest position and increase the pointing counter
		m_posPointer = (m_posPointer + 1) % m_historyLength;
		m_hist[m_posPointer] = pos;
	}
}

auto CursorHistory::getSmoothedPosition() -> glm::vec3
{
	//Compute average coordinate from all entries inside the historylist
	glm::vec3 ret(0.0f, 0.0f, 0.0f);

	if (m_hist.size() < m_historyLength) {
		for (int i = 0; i <= m_posPointer; i++) {
			ret += m_hist[i];
		}
		ret /= m_posPointer;
	}
	else {
		for (int i = 0; i < m_historyLength; i++) {
			ret += m_hist[i];
		}
		ret /= m_historyLength;
	}
	return ret;
}

auto CursorHistory::clickVertical(glm::vec3 ray) -> bool
{
	double lastClick = (std::clock() - clickDelay) / (double)CLOCKS_PER_SEC;
	if (lastClick > 0.5) {				//Allow 1 click per 0.5 seconds, in order to prevent to fast clicking or double clicking
		glm::vec3 smoothed = getSmoothedPosition();
		glm::vec3 dif = smoothed - ray;

		//Confirm a click, if ray is an outlier in y-axis inside a range of 2.5cm and 10cm
		if (dif.y > 25.0f && dif.y < 100.0f) {
			clickDelay = std::clock();
			return true;
		}
	}

	return	false;
}

auto CursorHistory::smoothedRayPointing(glm::vec3 startPoint, glm::vec3 passingPoint) -> glm::vec3
{
	glm::vec3 kinectOffset(m_config->kinectOffsetX, m_config->kinectOffsetY, m_config->kinectOffsetZ);				//in millimeter

	glm::vec3 rayDirection = passingPoint - startPoint;

	//Computer the tilt of the screen plane, based on the coordinate system of the kinect sensor
	glm::mat4 rotationMatrix(1);
	rotationMatrix = glm::rotate(rotationMatrix, m_config->kinectTilt, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::vec3 screenNormal = glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	//compute intersection between ray and screen plane in world coordinate
	float t = glm::dot(screenNormal, -startPoint) / glm::dot(screenNormal, rayDirection);

	//extrapolate pointing ray to computer absolute intersection point in world coordinate
	glm::vec3 pWorld = startPoint + t * rayDirection;

	//convert intersection point into 2D screen position
	glm::vec3 pScreen = pWorld *1000.0f - kinectOffset;
	pScreen.x = (pScreen.x / m_config->screenSize_Width + 0.5) * m_config->screenRes_Width;
	pScreen.y = (-pScreen.y / m_config->screenSize_Height) * m_config->screenRes_Height;

	return pScreen;
}

auto CursorHistory::clear() -> void
{
	//Reset history
	m_hist = std::vector<glm::vec3>();
	m_posPointer = -1;
}

auto CursorHistory::setAll(glm::vec3 pos) -> void
{
	//Fill history with all same values
	clear();
	for (int i = 0; i < m_historyLength; i++) {
		m_hist.push_back(pos);
	}
	m_posPointer = 0;
}
