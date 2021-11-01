#include "Common.h"
#include "screen\ConfigHandler.h"
#include "Button3D.h"
#include "ButtonsVR.h"


int ButtonsVR::idCounter = 0;
float ButtonsVR::VERTSPACE = 1.25f;
float ButtonsVR::HORISPACE = 1.0f;
float ButtonsVR::SPLITHORISPACE = 0.448f;

ButtonsVR::ButtonsVR(int screenResX, int screenResY) :
	screenResX(screenResX),
	screenResY(screenResY)
{
	camera = std::make_shared<Camera>();
	camera->SetPosition(glm::vec3(0.0f, 0, 6.65));
}
auto ButtonsVR::addButton(std::string filepath, std::string identifier, glm::vec2 position, std::function<void()> callback, int drawScreen) -> void
{
	addButton(filepath, "", identifier, position, callback, drawScreen);
}
auto ButtonsVR::addButton(std::string filepath, std::string texturepath, std::string identifier, glm::vec2 position, std::function<void()> callback, int drawScreen) -> void
{
	//Loading button mesh and creates a new Button3D object
	std::shared_ptr<Model> buttonMesh;
	if (texturepath.empty()) {
		buttonMesh = std::make_shared<Model>(filepath);
	}
	else {
		buttonMesh = std::make_shared<Model>(filepath, texturepath);
	}

	std::shared_ptr<Button3D> button = std::make_shared<Button3D>(buttonMesh, position, identifier, callback, drawScreen);

	m_button3DToID[identifier] = idCounter;		//Maps identifier to the new button

	//add new button to container
	container.push_back(button);
	containerSize = container.size();

	idCounter++;		//increase class variable for counting how many buttons were created
}


auto ButtonsVR::setSelection(std::string identifier, bool selection) -> void
{
	container[m_button3DToID[identifier]]->setSelected(selection);
}

auto ButtonsVR::draw(std::shared_ptr<ShaderProgram> lightning, std::shared_ptr<ShaderProgram> hightlightColor, GLuint *ubo_matrices, int whichScreen) -> void
{
	/*Bind ubo_matrices to the device*/
	glBindBuffer(GL_UNIFORM_BUFFER, *ubo_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->GetProjectionMatrix()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	for (int i = 0; i < containerSize; i++) {
		std::shared_ptr<Button3D> button = container[i];
		int mode = button->getScreen();
		//Draw the correct buttons, dependant from which screen this method is called
		if (!splitscreenEnabled && (mode < B3D_SPLITBOTH) ||
			splitscreenEnabled && (whichScreen == B3D_FROMLEFT && (mode == B3D_FULLSPLITBOTH || mode == B3D_FULLSPLITLEFT || mode == B3D_SPLITBOTH || mode == B3D_SPLITLEFT)
								|| whichScreen == B3D_FROMRIGHT && (mode == B3D_FULLSPLITBOTH || mode == B3D_FULLSPLITRIGHT || mode == B3D_SPLITBOTH || mode == B3D_SPLITRIGHT))) {
			//if the button should be highlighted, then prepare stencil drawing
			if (button->isSelected()) {
				glEnable(GL_STENCIL_TEST);
				glStencilFunc(GL_ALWAYS, 0, ~0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			}
			//Draw colored button mesh
			glEnable(GL_DEPTH_TEST);
			std::shared_ptr<Mesh> buttonMesh = button->getMesh();
			lightning->UseProgram();
			{
				lightning->SetUniform("modelMatrix", buttonMesh->GetModelMatrix());
				button->render();
			}
			lightning->UnUseProgram();

			//Draw same mesh again, this time only the vertices with big points and a single color.
			if (button->isSelected()) {
				glStencilFunc(GL_EQUAL, 0, ~0);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

				//Draw the mesh in wireframe mode. GL_LINE is deprecated and doesn't work for many graphic cards anymore
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
				glPointSize(10.0f);			
				// ###################################################################
				hightlightColor->UseProgram();
				{
					hightlightColor->SetUniform("modelMatrix", buttonMesh->GetModelMatrix());
					button->render();
				}
				hightlightColor->UnUseProgram();
				// ###################################################################

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}
}

auto ButtonsVR::switchSplitscreen(bool splitscreenEnabled) -> void
{
	this->splitscreenEnabled = splitscreenEnabled;
	
	for (int i = 0; i < containerSize; i++) {
		std::shared_ptr<Mesh> buttonMesh = container[i]->getMesh();
		glm::vec2 pos = container[i]->getPosition();
		buttonMesh->ResetModelMatrix();

		if (splitscreenEnabled) {
			camera->SetAspectRatio(1.0f * screenResX * 0.5 / screenResY);				//adjust camera aspectratio, since screen is halved upon activating splitscreen.
			buttonMesh->TranslateModel(glm::vec3(pos.x * SPLITHORISPACE, 2.5f - pos.y * VERTSPACE, 0.0f));	//translate button to the matching position on screen
		}
		else {
			camera->SetAspectRatio(1.0f * screenResX / screenResY);						//reset aspect ration to default settings.
			buttonMesh->TranslateModel(glm::vec3(pos.x * HORISPACE, 2.5f - pos.y * VERTSPACE, 0.0f));		//translate button to the matching position on screen
		}

	}

}

auto ButtonsVR::initScreenPosition() -> void
{
	camera->SetAspectRatio(1.0f * screenResX / screenResY);
	for (int i = 0; i < containerSize; i++) {
		std::shared_ptr<Mesh> buttonMesh = container[i]->getMesh();
		glm::vec2 pos = container[i]->getPosition();
		buttonMesh->ResetModelMatrix();
		buttonMesh->TranslateModel(glm::vec3(pos.x * HORISPACE, 2.5f - pos.y * VERTSPACE, 0.0f));
	}
}

auto ButtonsVR::isInside(int x, int y, int whichScreen) -> std::string
{
	//Picking is using OBB's. The mouse is "shooting" bullets/ray orthogonal into the screen
	glm::vec3 ray_origin;
	glm::vec3 ray_direction;

	//adjust offset and aspect ratio depending on where the mouse is.
	if (splitscreenEnabled) {
		if (x > screenResX / 2) {
			camera->SetAspectRatio(1.0f * screenResX * 0.5 / screenResY);
			glm::mat4 ProjectionMatrix = camera->GetProjectionMatrix();
			glm::mat4 ViewMatrix = camera->GetViewMatrix();
			Picking::ScreenPosToWorldRay(x - screenResX / 2, screenResY - y, screenResX / 2, screenResY, ViewMatrix, ProjectionMatrix, ray_origin, ray_direction);
		}
		else {
			camera->SetAspectRatio(1.0f * screenResX * 0.25 / screenResY);
			glm::mat4 ProjectionMatrix = camera->GetProjectionMatrix();
			glm::mat4 ViewMatrix = camera->GetViewMatrix();
			Picking::ScreenPosToWorldRay(x + (x - screenResX / 4), screenResY - y, screenResX / 2, screenResY, ViewMatrix, ProjectionMatrix, ray_origin, ray_direction);
			camera->SetAspectRatio(1.0f * screenResX * 0.5 / screenResY);
		}
		
	}
	else if (!splitscreenEnabled) {
		camera->SetAspectRatio(1.0f * screenResX / screenResY);
		glm::mat4 ProjectionMatrix = camera->GetProjectionMatrix();
		glm::mat4 ViewMatrix = camera->GetViewMatrix();
		Picking::ScreenPosToWorldRay(x, screenResY - y, screenResX, screenResY, ViewMatrix, ProjectionMatrix, ray_origin, ray_direction);
	}

	float intersection_distance; // Output of TestRayOBBIntersection()

	//Reset all selection
	for (int i = 0; i < containerSize; i++) {
		container[i]->setSelected(false);
	}

	bool selection = false;
	std::vector<std::string> toSelect;
	std::string returnID = "";

	//Returns the identifier of the first object, the ray is colliding with
	for (int i = 0; i < containerSize && !selection; i++) {
		std::shared_ptr<Button3D> button = container[i];
		int mode = button->getScreen();
		if(!splitscreenEnabled && (mode < B3D_SPLITBOTH) ||
			splitscreenEnabled && (whichScreen == B3D_FROMLEFT && (mode == B3D_FULLSPLITBOTH || mode == B3D_FULLSPLITLEFT || mode == B3D_SPLITBOTH || mode == B3D_SPLITLEFT)
				|| whichScreen == B3D_FROMRIGHT && (mode == B3D_FULLSPLITBOTH || mode == B3D_FULLSPLITRIGHT || mode == B3D_SPLITBOTH || mode == B3D_SPLITRIGHT))) {
			std::shared_ptr<Mesh> buttonMesh = button->getMesh();
			if (selection = Picking::TestRayOBBIntersection(ray_origin, ray_direction, buttonMesh->getCostumAABB()[0], buttonMesh->getCostumAABB()[1], buttonMesh->GetModelMatrix(), intersection_distance)) {
				button->setSelected(true);

				returnID = button->getIdentifier();

				button->setSelected(selection);
			}
		}

	}
	return returnID;
}

auto ButtonsVR::clickSelected() -> void
{
	int i = 0;
	
	while (i < containerSize && !container[i]->isSelected()) {
		i++;
	}
	if (i < containerSize) {
		container[i]->doClick();
	}
}
