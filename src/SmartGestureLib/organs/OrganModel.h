#pragma once

class OrganModel
{
public:
	/*Create an instance of OrganModel and load the object file
	@param 
		filename path to the object file with extension .obj
				The material file with extension .mtl have to be in the same directory
	*/
	OrganModel(std::string filename);
	~OrganModel();

	/*Associate a parent string identifier to a group of string identifier, for addressing groups of meshes using a single identifier.
	E.g. Parent: "lung" associating to "lung_left" and "lung_right"
	use it with:
	>> std::vector<std::string> v;
	>> v.push_back("lung_left"); v.push_back("lung_left");
	>> linkParentToChild("lung", v);
	@param
		identifier parent identifier
		child a list of child identifier
	*/
	auto linkParentToChild(std::string identifier, std::vector<std::string> child) -> void;

	/*Select meshes by their parent identifier
	@param
		identifier parent identifier
	*/
	auto setSelectionByParent(std::string identifier) -> void;
	
	/*Select a group of meshes by their identifier
	@param 
		identifiers list of meshes, addressed by their direct identifier (not parent identifier)
	*/
	auto setSelection(std::vector<std::string> identifiers) -> void;

	/*Draw organ for ARView.

	 @param:
			lightning color rendering shader
			highlightColor shader return a single color for hightlighting
			worldPosition 3D point of the mesh, where it supposed to be in the physical world coordinate.
			rot rotation of the mesh in pitch yaw roll format
			scale scale of the mesh in x, y, z axis
			highlight enabling highlighting
			isMirror draw the mesh in mirrored view or "window" mode	
	*/
	auto draw(std::shared_ptr<ShaderProgram> lightning, std::shared_ptr<ShaderProgram> hightlightColor, glm::vec3 worldPosition, glm::vec3 rot, glm::vec3 scale, bool hightlight, bool isMirror)		-> void;

	/*Draw organ for VRView.

	@param:
	lightning color rendering shader
	highlightColor shader return a single color for hightlighting
	rot rotation of the mesh in pitch yaw roll format
	scale scale of the mesh in x, y, z axis
	highlight enabling highlighting
	*/
	auto draw(std::shared_ptr<ShaderProgram> lightning, std::shared_ptr<ShaderProgram> hightlightColor, glm::vec3 rot, glm::vec3 scale, bool hightlight)		-> void;


private:
	//Draw single organ hightlight for ARView
	auto drawHighlight(std::shared_ptr<ShaderProgram> lightning, std::shared_ptr<ShaderProgram> hightlightColor, int organID, glm::vec3 scale, glm::vec3 rot, glm::vec3 translation, bool isMirror) -> void;
	//Draw single organ highlight for VRView
	auto drawHighlight(std::shared_ptr<ShaderProgram> lightning, std::shared_ptr<ShaderProgram> hightlightColor, int organID, glm::vec3 scale, glm::vec3 rot) -> void;

	std::map<std::string, std::vector<std::string> > m_parentToChild;
	std::shared_ptr<Model> m_organ{ nullptr };
	std::map<std::string, int> m_organToID;
	std::vector<bool> selectOrgan;


};