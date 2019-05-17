
#include "Group.hpp"


    // default triangle
Group::Group(){
        init();
    
    };
Group::~Group(){
        
        
    };
    
void Group::addMesh(Mesh* mesh){
        meshes.push_back(mesh);
    
    }
void Group::addMaterial(Material* mat){
        materials.push_back(mat);
        
    }
    
Mesh* Group::getMesh(int index){
        return meshes[index];
    
    }
Material* Group::getMaterial(int index){
        return materials[index];
    }
    
int Group::getNumMeshes(){
    return meshes.size();

}
void Group::setRenderMode(float rendermode){
    for (int i=0;i<materials.size();i++)
        {
            MTLShader* shader = static_cast<MTLShader*>(materials[i]->getShader());
            if(shader!=NULL)
                shader->setRenderMode(rendermode);
        }
    
}

void Group::init(){
    setupShaders();
    
}
void Group::render(Camera* camera){
   //TODO: Make sure all meshes are rendered - at the moment only the first one is rendered
    for(int i = 0; i < meshes.size(); i++){
		meshes[i]->bindShaders();
        meshes[i]->render(camera);
        
    }
    
}
//Changed indexing from 0 to a for loop
void Group::setupShaders(){
	//TODO: Do this for all meshes
	for(int i = 0; i < meshes.size();i++){
		int matIndex = meshes[i]->getMatIndex(); // TODO: get the correct material index from obj file
        Material* mat = getMaterial(matIndex);
        Shader* shader = NULL;
        if(!mat->shaderIsInitialized()){
            MTLShader* mtlshader = new MTLShader( "mtlShader");
            mtlshader->setDiffuse(mat->getDiffuseColour());
            mtlshader->setAmbient(mat->getAmientColour());
            mtlshader->setSpecular(mat->getSpecularColour());
            mtlshader->setOpacity(mat->getOpacity());
            mat->setShader(mtlshader);
            if(mat->getTextureName()!=""){
                Texture* texture = new Texture(mat->getTextureName());
                mtlshader->setTexture(texture);
            }

            shader = mtlshader;
        }
        else {
            shader = mat->getShader();
        }
        meshes[i]->setShader(shader);
        
    }
}
