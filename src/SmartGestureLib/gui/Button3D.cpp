#include "Common.h"
#include "Button3D.h"

Button3D::Button3D(std::shared_ptr<Model> buttonModel, glm::vec2 position, std::string identifier, std::function<void()> callback, int drawScreen = B3D_FULLSPLITBOTH):
	buttonMesh(buttonModel->getMesh(0)),
	position(position),
	identifier(identifier),
	callback(callback),
	selected(false),
	splitScreenSide(drawScreen)
{
}

auto Button3D::render() -> void
{
	buttonMesh->Render(GL_TRIANGLES);
}

auto Button3D::setSelected(bool isSelected) -> void
{
	this->selected = isSelected;
}


auto Button3D::isSelected() -> bool
{
	return selected;
}

auto Button3D::getPosition() -> glm::vec2
{
	return position;
}

auto Button3D::getMesh() -> std::shared_ptr<Mesh>
{
	return buttonMesh;
}

auto Button3D::getIdentifier() -> std::string
{
	return identifier;
}

auto Button3D::doClick() -> void
{
	callback();
}

auto Button3D::getScreen() -> int
{
	return splitScreenSide;
}
