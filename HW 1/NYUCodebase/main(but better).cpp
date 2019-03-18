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




GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);

	if (image == NULL) {
		cout << "Unable to load image. Make sure the path is correct.\n";
		//assert(false);
	}

	GLuint retTexture; 
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture); 
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


	program.Load(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");
	program1.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");


	//GLuint emojiTexture = LoadTexture(RESOURCE_FOLDER"emoji.png"); //NYU Image
	//GLuint bernieTexture = LoadTexture(RESOURCE_FOLDER"bernie.png"); //Bernie Image
	//GLuint prideTexture = LoadTexture(RESOURCE_FOLDER"pride.png"); //Pride Image



	glm::mat4 projectionMatrix = glm::mat4(1.0f);//Project matrix -[Defining Bounds of Window of Projection]:  (View to Projection) renders scene by squishing world into 2d 
	glm::mat4 modelMatrix = glm::mat4(1.0f);	 //Model matrix - [TRS per object]: (Model to World) 
	glm::mat4 viewMatrix = glm::mat4(1.0f);		 //View matrix - [Where in ithe world am I looking]: (World to View) Scope of Game 


	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f); //Ortho Project ----------------- ACTUALLY DEFINING VIEW


//-----------Set-Up------------------------------








//-----------Drawing----------------------
	glClearColor(0.5f, 0.7f, 0.6f, 1.0f); //Set Color
	

	program.SetProjectionMatrix(projectionMatrix); //Setting Projection Matrix
	program.SetViewMatrix(viewMatrix); //Setting View Matrix

	program1.SetProjectionMatrix(projectionMatrix);  //Setting Projection Matrix
	program1.SetViewMatrix(viewMatrix); //Setting View Matrix



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

		
		//---------------------------------Untextured---------------------------------------------------------------------
		modelMatrix = glm::mat4(1.0f); //Setting starting Point	
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.6f, 0.0f)); //Moving Image
		program1.SetModelMatrix(modelMatrix); //Using this staring point
		//draw untextured
		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 }; // Defining Square (X,Y)
		glVertexAttribPointer(program1.positionAttribute, 2, GL_FLOAT, false, 0, vertices); //Vertex Atrribute = use vertices
		glEnableVertexAttribArray(program1.positionAttribute); //Enable Vertex Atrribute = enable vertices	
		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw Previously Setup Object
		glDisableVertexAttribArray(program1.positionAttribute); //Undraw Coordinates




		//==================================================================================================================================>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>IMAGE 1
		modelMatrix = glm::mat4(1.0f); //Setting starting Point																			
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.5f, 0.0f, 0.0f)); //Moving Image											
		program.SetModelMatrix(modelMatrix); //Using this staring point																	

		//Coordinates System (Untextured) 																				
		//float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 }; // Defining Square (X,Y)				
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices); //Vertex Atrribute = use vertices			
		glEnableVertexAttribArray(program.positionAttribute); //Enable Vertex Atrribute = enable vertices								

		//Textured Coordinates System																									
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 }; //U,V (class05, 22/40)? 					
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		//glBindTexture(GL_TEXTURE_2D, emojiTexture); //Use This Picture - How does this function know I'm talking about my textured object																																											

		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw Previously Setup Object																																														
		glDisableVertexAttribArray(program.positionAttribute); //Undraw Coordinates														
		glDisableVertexAttribArray(program.texCoordAttribute); //Undraw Textured Coordinates	




		//=================================================================================================================================>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>IMAGE 2
		modelMatrix = glm::mat4(1.0f); //Setting starting Point	
		modelMatrix = glm::translate(modelMatrix, glm::vec3(1.5f, 0.0f, 0.0f)); //Moving Image	
		program.SetModelMatrix(modelMatrix); //Using this staring point	
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices); //Vertex Atrribute = use vertices			
		glEnableVertexAttribArray(program.positionAttribute); //Enable Vertex Atrribute = enable vertices
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		//glBindTexture(GL_TEXTURE_2D, bernieTexture); //Use This Picture
		
		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw Previously Setup Object																
		glDisableVertexAttribArray(program.positionAttribute); //Undraw Coordinates														
		glDisableVertexAttribArray(program.texCoordAttribute); //Undraw Textured Coordinates	


		//=================================================================================================================================>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>IMAGE 3
		modelMatrix = glm::mat4(1.0f); //Setting starting Point	
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.5f, 0.0f)); //Moving Image	
		program.SetModelMatrix(modelMatrix); //Using this staring point	
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices); //Vertex Atrribute = use vertices			
		glEnableVertexAttribArray(program.positionAttribute); //Enable Vertex Atrribute = enable vertices
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		//glBindTexture(GL_TEXTURE_2D, prideTexture); //Use This Picture

		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw Previously Setup Object																
		glDisableVertexAttribArray(program.positionAttribute); //Undraw Coordinates														
		glDisableVertexAttribArray(program.texCoordAttribute); //Undraw Textured Coordinates	















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
