#include "Common.h"
#include "OrganModel.h"


OrganModel::OrganModel(std::string filename)
{
	std::cout << "Loading organs..."; //######################################################################
	m_organ = std::make_shared<Model>(filename);

	//get meshes inside the object file.
	std::vector<std::shared_ptr<Mesh>> meshes = m_organ->getMeshes();

	//init highlight selection to false
	for (int i = 0; i < meshes.size(); i++) {
		selectOrgan.push_back(false);
	}

	//get mesh names, based on the name given to them (either already built in or rename in blender)
	std::vector<std::string> meshName = m_organ->getMeshNames();

	//assign each mesh name with an enumerated ID, matching the inner order of the object file
	for (int i = 0; i < meshName.size(); i++) {
		m_organToID[meshName[i]] = i;
	}
	std::cout << "finished" << std::endl; //##################################################################
}

OrganModel::~OrganModel()
{
}

auto OrganModel::linkParentToChild(std::string identifier, std::vector<std::string> child) -> void
{
	m_parentToChild[identifier] = child;
}

auto OrganModel::setSelectionByParent(std::string identifier) -> void
{
	if (!identifier.empty()) {
		if (!m_parentToChild[identifier].empty()) {			//check whether the parent identifier matches children
			setSelection(m_parentToChild[identifier]);
		}

	}
	else {	//identifier is empty, unselect all organs
		for (int i = 0; i < selectOrgan.size(); i++) {
			selectOrgan[i] = false;
		}
	}
}

auto OrganModel::setSelection(std::vector<std::string> identifier) -> void
{
	//unselect all meshes
	for (int i = 0; i < selectOrgan.size(); i++) {
		selectOrgan[i] = false;
	}

	//select the meshes based on the list of identifier
	for (int i = 0; i < identifier.size(); i++) {
		selectOrgan[m_organToID[identifier[i]]] = true;
	}
}

auto OrganModel::draw(std::shared_ptr<ShaderProgram> lightning, std::shared_ptr<ShaderProgram> hightlightColor, glm::vec3 worldPosition, glm::vec3 rot, glm::vec3 scale, bool hightlight, bool isMirror) -> void
{
	//enable Depth test for organ rendering
	glEnable(GL_DEPTH_TEST);

	std::vector<std::shared_ptr<Mesh>> meshes = m_organ->getMeshes();
	
	lightning->UseProgram();
	{
		for (GLuint i = 0; i < meshes.size(); i++)
		{
			if (!selectOrgan[i]) {		//directly draw organ, if it is not selected

				//prepare matrix for window mode
				glm::mat4 matreflect = glm::transpose(glm::mat4(
					-1.0, 0.0, 0.0, 0.0,
					0.0, 1.0, 0.0, 0.0,
					0.0, 0.0, 1.0, 0.0,
					0.0, 0.0, 0.0, 1.0));

				meshes[i]->ResetModelMatrix();
				meshes[i]->ScaleModel(110.f * scale);
				//rotating by reversing order
				meshes[i]->RotateModel(static_cast<float>(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));							//roll rotation
				meshes[i]->RotateModel(static_cast<float>(rot.y) + 3.1415926535f, glm::vec3(0.0f, 1.0f, 0.0f));			//yaw rotation 
				meshes[i]->RotateModel(static_cast<float>(-rot.x) + 3.1415926535f / 2, glm::vec3(1.0f, 0.0f, 0.0f));	//pitch rotation
				meshes[i]->TranslateModel(1000.0f * glm::vec3(worldPosition.x, worldPosition.y, worldPosition.z));		//translate to world position

				//mirroring, if necessary
				if (isMirror) {
					lightning->SetUniform("modelMatrix", meshes[i]->GetModelMatrix());
				}
				else {
					lightning->SetUniform("modelMatrix", meshes[i]->GetModelMatrix() * matreflect);
				}

				meshes[i]->Render(GL_TRIANGLES);	//render meshes
			}
		}
	}
	lightning->UnUseProgram();

	if (hightlight) {
		for (GLuint i = 0; i < meshes.size(); i++)
		{
			if (selectOrgan[i])			//draw highlighted version of mesh
				drawHighlight(lightning, hightlightColor, i, 110.0f * scale, rot, glm::vec3(worldPosition.x, worldPosition.y, worldPosition.z), isMirror);
		}
	}

	glDisable(GL_DEPTH_TEST);			//Disable depth test, retrieving original state
}

auto OrganModel::draw(std::shared_ptr<ShaderProgram> lightning, std::shared_ptr<ShaderProgram> hightlightColor, glm::vec3 rot, glm::vec3 scale, bool hightlight) -> void
{
	//enable Depth test for organ rendering
	glEnable(GL_DEPTH_TEST);

	std::vector<std::shared_ptr<Mesh>> meshes = m_organ->getMeshes();
	lightning->UseProgram();
	{
		for (GLuint i = 0; i < meshes.size(); i++)
		{

			if (!selectOrgan[i]) {		//directly draw organ, if it is not selected

				meshes[i]->ResetModelMatrix();
				meshes[i]->ScaleModel(scale);

				//rotating by reversing order
				meshes[i]->RotateModel(static_cast<float>(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));						//roll rotation
				meshes[i]->RotateModel(static_cast<float>(rot.y) + 3.1415926535f, glm::vec3(0.0f, 1.0f, 0.0f));		//yaw rotation 
				meshes[i]->RotateModel(static_cast<float>(-rot.x) + 3.1415926535f / 2, glm::vec3(1.0f, 0.0f, 0.0f));//pitch rotation

				lightning->SetUniform("modelMatrix", meshes[i]->GetModelMatrix());
				//render mesh
				meshes[i]->Render(GL_TRIANGLES);

			}
		}
	}
	lightning->UnUseProgram();

	if (hightlight) {
		for (GLuint i = 0; i < meshes.size(); i++)
		{
			if (selectOrgan[i])		//draw highlighted version of mesh
				drawHighlight(lightning, hightlightColor, i, scale, glm::vec3(0, rot.y, 0));
		}
	}
}



auto OrganModel::drawHighlight(std::shared_ptr<ShaderProgram> lightning, std::shared_ptr<ShaderProgram> hightlightColor, int organID, glm::vec3 scale, glm::vec3 rot, glm::vec3 translation, bool isMirror) -> void
{
	std::vector<std::shared_ptr<Mesh>> meshes = m_organ->getMeshes();
	std::shared_ptr<Mesh> organSingle = meshes[organID];

	//prepare matrix for window mode
	glm::mat4 matreflect = glm::transpose(glm::mat4(
		-1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0));

	//configure state machine of opengl by enabling depth test and stencil test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, ~0);					//always draw mesh -> stencil test always passes
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);			//draw front and back faces, all filled

	// ###################################################################
	lightning->UseProgram();
	{
		// Draw organs
		organSingle->ResetModelMatrix();
		organSingle->ScaleModel(scale);
		organSingle->RotateModel(static_cast<float>(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));						//roll
		organSingle->RotateModel(static_cast<float>(rot.y) + 3.1415926535f, glm::vec3(0.0f, 1.0f, 0.0f));		//yaw
		organSingle->RotateModel(static_cast<float>(-rot.x) + 3.1415926535f / 2, glm::vec3(1.0f, 0.0f, 0.0f));	//pitch

		//translate
		organSingle->TranslateModel(1000.0f * translation);

		//mirroring, if necessary
		if (isMirror) {
			lightning->SetUniform("modelMatrix", organSingle->GetModelMatrix());
		}
		else {
			lightning->SetUniform("modelMatrix", organSingle->GetModelMatrix() * matreflect);
		}

		organSingle->Render(GL_TRIANGLES);		//Draw the single organ
	}
	lightning->UnUseProgram();
	// ####################################################################

	glStencilFunc(GL_EQUAL, 0, ~0);				//draw where routine above did not draw
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); //draw outline by drawing big circles at vertices

	GLfloat outlineScale = 10.0f - std::max(1.0f, std::min(5.0f, translation.z));	//draw smaller outline, if mesh is further away
	glPointSize(outlineScale);

	// ###################################################################
	hightlightColor->UseProgram();
	{
		//mirroring, if necessary
		if (isMirror) {
			hightlightColor->SetUniform("modelMatrix", organSingle->GetModelMatrix());
		}
		else {
			hightlightColor->SetUniform("modelMatrix", organSingle->GetModelMatrix() * matreflect);
		}
	}
	//render the mesh again, this time for the outline
	organSingle->Render(GL_TRIANGLES);
	hightlightColor->UnUseProgram();
	// ###################################################################

	//set state of opengl to filled faces again
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

auto OrganModel::drawHighlight(std::shared_ptr<ShaderProgram> lightning, std::shared_ptr<ShaderProgram> hightlightColor, int organID, glm::vec3 scale, glm::vec3 rot) -> void
{
	std::vector<std::shared_ptr<Mesh>> meshes = m_organ->getMeshes();
	std::shared_ptr<Mesh> organSingle = meshes[organID];

	//prepare matrix for window mode
	glm::mat4 matreflect = glm::transpose(glm::mat4(
		-1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0));

	//configure state machine of opengl by enabling depth test and stencil test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, ~0);							//always draw mesh -> stencil test always passes
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);					//draw front and back faces, all filled

	// ###################################################################
	lightning->UseProgram();
	{
		// Draw organs
		organSingle->ResetModelMatrix();
		organSingle->ScaleModel(scale);
		organSingle->RotateModel(static_cast<float>(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));						//roll
		organSingle->RotateModel(static_cast<float>(rot.y) + 3.1415926535f, glm::vec3(0.0f, 1.0f, 0.0f));		//yaw
		organSingle->RotateModel(static_cast<float>(-rot.x) + 3.1415926535f / 2, glm::vec3(1.0f, 0.0f, 0.0f));	//pitch

		lightning->SetUniform("modelMatrix", organSingle->GetModelMatrix());

		organSingle->Render(GL_TRIANGLES);
	}
	lightning->UnUseProgram();
	// ####################################################################

	glStencilFunc(GL_EQUAL, 0, ~0);				//draw where routine above did not draw
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); //draw outline by drawing big circles at vertices

	glPointSize(20.0f);

	// ###################################################################
	hightlightColor->UseProgram();
	{
		hightlightColor->SetUniform("modelMatrix", organSingle->GetModelMatrix());
		organSingle->Render(GL_TRIANGLES);
	}
	hightlightColor->UnUseProgram();
	// ###################################################################

	//set state of opengl to filled faces again
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


