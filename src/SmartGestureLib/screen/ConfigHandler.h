#pragma once

class ConfigHandler
{
public:
	/*Read in configuration file and stores the screenname, ID of printed, screen marker and the offset of the Kinect sensor to the top midpoint of the screen
	@param:
		filepath path to the configuration file
		The file contains information in style of:
		############################################
		
		%YAML:1.0
		screenName: <Name>
		printedMarkerID: <ID of printed Marker e.g. 1 >
		screenMarkerID: <ID of screen marker e.g. 101 >
		kinectOffsetX: <Offset in x-axis (left in kinect view)>
		kinectOffsetY: <Offset in y-axis (up in kinect view)>
		kinectOffsetZ: <Offset in z-axis (forward in kinect view)>
		kinectTilt: <Tilt of Kinect camera in degree>

		############################################
	*/
	ConfigHandler(std::string filepath);

	/*Read in configuration file and stores the screenname,  ID of printed, screen marker and the offset of the Kinect sensor to the top midpoint of the screen

	The file contains information in style of:
	############################################

	%YAML:1.0
	screenName: <Name>
	printedMarkerID: <ID of printed Marker e.g. 1 >
	screenMarkerID: <ID of screen marker e.g. 101 >
	kinectOffsetX: <Offset in x-axis (left in kinect view)>
	kinectOffsetY: <Offset in y-axis (up in kinect view)>
	kinectOffsetZ: <Offset in z-axis (forward in kinect view)>
	kinectTilt: <Tilt of Kinect camera in degree>

	############################################
	*/
	ConfigHandler();
	~ConfigHandler();

	/*Initialize screen and retrieves size and resolution of primary monitor
	Prelim: GLFW & glew are initialized
	@param:
		window GLFWwindow of application

	*/
	void initScreen(GLFWwindow *window);

	/*Generate QR code
	@param
		ipv4Address destination address of local area network. The port is hardcoded to 14321
	@return OpenCV Mat containing the qrcode.
	*/
	cv::Mat CH_GenerateQR(std::string ipv4Address);

	/*Generate the printed marker ontop of a white background. The generated image is sized to the screen's dimension.
	The size of the marker is 40% of the smaller side of the screen.
	@param
		windowDimension resolution of rendered background texture. Not necessary the screen resolution.
	@return OpenCV Mat containing the printed marker ontop of a white background.
	*/
	cv::Mat CH_GenerateMarker(glm::uvec2 windowDimension);

	/*Returns the flag whether initScreen function was called already*/
	bool isScreenInit();

	std::string configFile;

	std::string screenName;
	std::string printedMarkerID;
	std::string screenMarkerID;
	float kinectOffsetX;
	float kinectOffsetY;
	float kinectOffsetZ;
	float kinectTilt;
	float markerSize;
	int screenRes_Width;
	int screenRes_Height;
	int screenSize_Width;
	int screenSize_Height;

private:
	bool isScreenInitialized{ false };

};