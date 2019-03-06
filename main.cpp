#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

using namespace std;

SDL_Window* displayWindow;
int w, h, comp;
unsigned char* image = stbi_load("some_image.png", &w, &h, &comp, STBI_rgb_alpha);
//stbi_image_free(image); //free image



GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);

	if (image == NULL) {
		cout << "Unable to load image. Make sure the path is correct.\n";
		assert(false);
	}

	GLuint retTexture; //What does this line for? 
	glGenTextures(1, &retTexture);
	//glBindTexture(GL_TEXTURE_2D, retTexture); //(What) Confused about this line, and what she does? 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);
	return retTexture;


}
//-------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif



	glViewport(0, 0, 640, 360);
	ShaderProgram program;
	ShaderProgram program1; 
	//ShaderProgram program3; //DO I NEED THESE
	//ShaderProgram program4;
	
	program.Load(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
	//program.Load(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
	//program.Load(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");

	program1.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	

	GLuint emojiTexture = LoadTexture(RESOURCE_FOLDER"emoji.png"); //NYU Image
	GLuint bernieTexture = LoadTexture(RESOURCE_FOLDER"bernie.png"); //Bernie Image
	GLuint prideTexture = LoadTexture(RESOURCE_FOLDER"pride.png"); //Pride Image


	
	glm::mat4 projectionMatrix = glm::mat4(1.0f);//identity matrix - (View to Projection) renders scene by squishing world into 2d
	glm::mat4 modelMatrix = glm::mat4(1.0f);	 //identity matrix - (Model to World)
	glm::mat4 viewMatrix = glm::mat4(1.0f);		 //identity matrix - (World to View) Scope of Game

	glm::mat4 projectionMatrix1 = glm::mat4(1.0f);//identity matrix - (View to Projection) renders scene by squishing world into 2d
	glm::mat4 modelMatrix1 = glm::mat4(1.0f);	 //identity matrix - (Model to World)
	glm::mat4 viewMatrix1 = glm::mat4(1.0f);		 //identity matrix - (World to View) Scope of Game



	modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	modelMatrix1 = glm::scale(modelMatrix1, glm::vec3(2.0f, 2.0f, 1.0f));

	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f); //Ortho Project
	projectionMatrix1 = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f); //Ortho Project

	glUseProgram(program.programID);
	glUseProgram(program1.programID);
	//glUseProgram(program3.programID);
	//glUseProgram(program4.programID);

	float lastFrameTicks = 0.0f;


//-----------Set-Up------------------------------








//-----------Drawing----------------------
	glClearColor(0.5f, 0.7f, 0.6f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);

	/*modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 1.0f, 0.0f));

	program.SetModelMatrix(modelMatrix);
	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);*/

	modelMatrix1 = glm::mat4(1.0f);
	modelMatrix1 = glm::translate(modelMatrix1, glm::vec3(-2.0f, 0.0f, 0.0f));

	program1.SetModelMatrix(modelMatrix1);
	program1.SetProjectionMatrix(projectionMatrix1);
	program1.SetViewMatrix(viewMatrix1);



	float vertices[] = { 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f };
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 3); //Drawing Triangle

	glDisableVertexAttribArray(program.positionAttribute); //Set Position

	//SDL_GL_SwapWindow(displayWindow);




	//float angle = 0.0f;

   
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
		//------------Drawing---
		glClear(GL_COLOR_BUFFER_BIT);

		/*program.SetModelMatrix(modelMatrix);
		program.SetProjectionMatrix(projectionMatrix);
		program.SetViewMatrix(viewMatrix);*/

		//glBindTexture(GL_TEXTURE_2D, emojiTexture);
		/*glBindTexture(GL_TEXTURE_2D, bernieTexture);
		glBindTexture(GL_TEXTURE_2D, prideTexture);*/

		//float vertices[] = {				//ARE WE NOT DOING FLOATS ANYMORE?!?!
		//	0.5f, -0.5f, 
		//	0.0f, 0.5f, 
		//	-0.5f, -0.5f
		//};

		float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0}; //TextCoords? 
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute); 

		////glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 0, 6); 

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		

		SDL_GL_SwapWindow(displayWindow);


		/*float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		angle += elapsed;*/
		//------------Drawing
       /* glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(displayWindow);*/
    }
    
    SDL_Quit();
    return 0;
}
