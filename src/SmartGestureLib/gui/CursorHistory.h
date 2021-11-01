#pragma once
#include <ctime>

class CursorHistory
{
public:
	/*Creates an instance of CursorHistory.
	prelim: Confighandler object must be initialized before calling this function
	@param
		historyLength the amount of entries that should be stored.
		cfg configuration file of the screen
	*/
	CursorHistory(int historyLength, std::shared_ptr<ConfigHandler> cfg);

	/*Adds an entry into the interal coordinate list*/
	auto addEntry(int x, int y, int z) -> void;

	/*Adds an entry into the internal coordinate list*/
	auto addEntry(glm::vec3 pos) -> void;

	/*Retrieves the average position, calculated from all entries of the internal coordinate list*/
	auto getSmoothedPosition() -> glm::vec3;

	/*Test whether a click is performed by detecting whether the ray is a downward outlier compared to the smoothed coordinate in y-axis
	The threshold for a positive return value is between 2,5cm and 10cm in worldcoordinate system*/
	auto clickVertical(glm::vec3 ray) -> bool;

	auto smoothedRayPointing(glm::vec3 startPoint, glm::vec3 passingPoint) -> glm::vec3;

	/*Resets the history list*/
	auto clear() -> void;

	/*Sets all history entries to the same value. Helpfull for setting the smoothed value to a specific value.
	Use with caution, deletes old history entries*/
	auto setAll(glm::vec3 pos) -> void;

private:

	std::clock_t clickDelay;
	int m_historyLength;
	std::shared_ptr<ConfigHandler> m_config;
	std::vector<glm::vec3> m_hist;
	int m_posPointer{ -1 };
};