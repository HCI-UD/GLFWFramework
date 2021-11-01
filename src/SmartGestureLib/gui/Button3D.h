#pragma once
#include "Common.h"
#include <functional>

//Draw in fullscreen and both screens while splitscreen
#define B3D_FULLSPLITBOTH 96
//Draw in only in fullscreen
#define B3D_FULLSPLITNONE 97
//Draw in fullscreen and right screen while splitscreen
#define B3D_FULLSPLITRIGHT 98
//Draw in fullscreen and left screen while splitscreen
#define B3D_FULLSPLITLEFT 99

//Draw only on both screens while splitscreen
#define B3D_SPLITBOTH 100
//Draw only on left screen while splitscreen
#define B3D_SPLITLEFT 101
//Draw only on right screen while splitscreen
#define B3D_SPLITRIGHT 102

//Method call origins from fullscreen drawing/testing routine
#define B3D_FROMFULL 0
//Method call origins from left splitscreen drawing/testing routine
#define B3D_FROMLEFT 1
//Method call origins from right splitscreen drawing/testing routine
#define B3D_FROMRIGHT 2


class Button3D
{
public:
	/*Creates a new 3 dimensional button.
	@param
		buttonModel filepath to the object model
		position 2D coordinate of the button on the screen, ranging from x = [-5, 5] and y = [0, 4]
	*/
	Button3D(std::shared_ptr<Model> buttonModel, glm::vec2 position, std::string identifier, std::function<void()> callback, int drawScreen);

	/*Render the button*/
	auto render() -> void;

	/*Set the selection. If selection is true, buttons will be highlighted*/
	auto setSelected(bool isSelected) -> void;

	/*Return whether the button is hightlighted*/
	auto isSelected() -> bool;

	/*Get 2D position of the button*/
	auto getPosition()->glm::vec2;

	/*Get mesh of the button*/
	auto getMesh()->std::shared_ptr<Mesh>;

	/*Get the unique identifier for this button*/
	auto getIdentifier()->std::string;

	/*Perform callback, which was assiged to this button*/
	auto doClick() -> void;

	/*Get on which side of the splitscreen this buttons belongs to*/
	auto getScreen() -> int;
private:

	int splitScreenSide{ B3D_FULLSPLITBOTH };
	glm::vec2 position;
	std::function<void()> callback;
	bool selected{ false };
	std::string identifier;
	std::shared_ptr<Mesh> buttonMesh;


};
