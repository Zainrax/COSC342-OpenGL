/*
 * renderApp.cpp
 *
 * by Stefanie Zollmann
 *
 * Basic model loading with assimp library
 *
 */


// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

// Include GLEW
#include <GL/glew.h>


// Include GLFW
#include <glfw3.h>
GLFWwindow* window;


// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/Shader.hpp>
#include <common/Texture.hpp>
#include <common/Object.hpp>
#include <common/ColorShader.hpp>
#include <common/Scene.hpp>
#include <common/Triangle.hpp>
#include <common/BasicMaterialShader.hpp>
#include <common/Mesh.hpp>
#include <common/Controls.hpp>
#include <common/Group.hpp>
#include <common/Objloader.hpp>



bool initWindow(std::string windowName){

    // Initialise GLFW
    if( !glfwInit() ){
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, windowName.c_str(), NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
        getchar();
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    return true;

}




int main( int argc, char *argv[] )
{

    initWindow("renderEngine Skeleton");
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    //create a Vertex Array Object and set it as the current one
    //we will not go into detail here. but this can be used to optimise the performance by storing all of the state needed to supply vertex data
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    /*
     * Create Scene
     * With a object and texture loaded from loader
     */

    Scene* myScene = new Scene();
    std::vector<Group*> groupObjects;
    // Read our .obj files - this is hard coded for testing - you can pass obj file names as arguments instead to make the code more flexible

    if(argc==1){
//     if nothing specified we load default objects
//     person obj
        Group* person = new Group();
        bool res = loadOBJMTL("person.obj", person);
        person->init();

        //earth obj
        Group* sphere = new Group();
        res = loadOBJMTL("earthobj.obj", sphere);
        sphere->init();

        //add objects to the scene
        myScene->addObject(person);
        myScene->addObject(sphere);
        groupObjects.resize(2);
        groupObjects[0] = person;
        groupObjects[1] = sphere;
    }
    else{
        groupObjects.resize(argc + 1);
        for (int a = 1; a < argc; ++a) {
            Group* objGroup = new Group();
            bool res = loadOBJMTL(argv[a], objGroup);
            if(res){
                objGroup->init();
                //add objects to the scene
                myScene->addObject(objGroup);
                groupObjects[a-1] = objGroup;
            }
        }
    }


    Camera* myCamera = new Camera();
    myCamera->setPosition(glm::vec3(0,100,200)); //set camera to show the models
    Controls* myControls = new Controls(myCamera);
    myControls->setSpeed(30);

    //Render loop
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 ) {// Clear the screen
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Also clear the depth buffer!!!
        for(int i = 0; i < groupObjects.size(); i++) {
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
                groupObjects[i]->setRenderMode(0);
            } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
                //Black & White
                groupObjects[i]->setRenderMode(1);
            } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
                //Toon
                groupObjects[i]->setRenderMode(2);
            }
        }
        // update camera controls with mouse input
        myControls->update();
        // takes care of all the rendering automatically
        myScene->render(myCamera);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }


    glDeleteVertexArrays(1, &VertexArrayID);
    //delete texture;
    delete myScene;
    delete myCamera;
    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

