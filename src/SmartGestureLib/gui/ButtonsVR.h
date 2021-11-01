#pragma once
#include "Common.h"
#include <functional>
#include "Button3D.h"

class ButtonsVR
{
public:
	/*Initialize button handler

	@param
		screenResX, screenResY screen resolution of the monitor.
	*/
	ButtonsVR(int screenResX, int screenResY);

	/*Adding a button to the container.
	Use this function only, if the mesh contains one material with one texture assigned to it.

	@param:
	filepath path to the button mesh.
	texturepath path to the texture file which replaces the default one.
	identifier a unique string, which can be used to address the button
	position a coordinate ranging from x = [-5, 5] where 0 is the vertical mid and y = [0, 4]
	callback function which will be called if button is clicked. call function isInside to select it and clickSelected to trigger the function.
	drawScreen define, on which splitscreen the buttons belongs to
	*/
	auto addButton(std::string filepath, std::string texturepath, std::string identifier, glm::vec2 position, std::function<void()> callback, int drawScreen = B3D_FULLSPLITBOTH) -> void;

	/*Adding a button to the container.

	@param:
	filepath path to the button mesh.
	identifier a unique string, which can be used to address the button
	position a coordinate ranging from x = [-5, 5] where 0 is the vertical mid and y = [0, 4]
	callback function which will be called if button is clicked. call function isInside to select it and clickSelected to trigger the function.
	drawScreen define, on which splitscreen the buttons belongs to
	*/
	auto addButton(std::string filepath, std::string identifier, glm::vec2 position, std::function<void()> callback, int drawScreen = B3D_FULLSPLITBOTH) -> void;


	/*Set highlight of a specific button.

	 @param:
			identifier button, which shall be set selection to
			selection -
	*/
	auto setSelection(std::string identifier, bool selection) -> void;

	/*Draw the button into screen buffer. If buttons are positioned wrongly after switching screenscreen, remember to call function switchSplitscreen(bool b) upon switching viewing mode.
	 Prelims: Opengl must be initialized

	 @param:
			lightning color rendering shader
			highlightColor shader return a single color for hightlighting
			ubo_matrix Uniform Buffer Object, which was bind to the device before
			whichScreen defines on which side of the splitscreen the method is called on.
	*/
	auto draw(std::shared_ptr<ShaderProgram> lightning, std::shared_ptr<ShaderProgram> hightlightColor, GLuint *ubo_matrix, int whichScreen = B3D_FROMFULL) -> void;

	/*Reposition the buttons based on whether splitscreen is enabled or disabled

	 @param
			splitscreenEnabled the state after switching screen mode
	*/
	auto switchSplitscreen(bool splitscreenEnabled) -> void;

	/*Initialize the buttons and assign them to their screen coordinate	*/
	auto initScreenPosition() -> void;

	/*Returns the identifier of the button, in which the mouse is inside. If the mouse is in none, the return value is an empty string

	@param
		x, y screen coordinate of the mouse
		whichScreen defines, on which side of the splitscreen the mouse is

	@return identifier of the button. If mouse is not in the buttons, the return value is an empty string.
	*/
	auto isInside(int x, int y, int whichScreen) -> std::string;

	/*Perform callback functions of buttons, which are hightlighted*/
	auto clickSelected() -> void;
private:

	int screenResX;
	int screenResY;

	std::vector<std::shared_ptr<Button3D>> container;
	size_t containerSize{ 0 };

	bool splitscreenEnabled{ false };
	std::shared_ptr<Camera> camera;

	std::map<std::string, int> m_button3DToID;



	static int idCounter;
	static float VERTSPACE;
	static float HORISPACE;
	static float SPLITHORISPACE;


};