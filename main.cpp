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

SDL_Window* displayWindow;
int w, h, comp;
unsigned char* image = stbi_load("some_image.png", &w, &h, &comp, STBI_rgb_alpha);
//stbi_image_free(image); //free image


int main(int argc, char *argv[])
{

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	/*

		SDL_Init(SDL_INIT_VIDEO);
		displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
		SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
		SDL_GL_MakeCurrent(displayWindow, context);

	*/


	//------GLUint Program----------------------------------
	GLuint LoadTexture(const char *filePath)
	{
		int w, h, comp;
		unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);

		if (image == NULL) {
			std::count << "Unable to load image. Make sure th epath is correct.\n";
			assert(false);
		}

		GLuint retTexture;
		glGenTextures(1, &retTexture);
		glBindTexture(GL_TEXTURE_2D, retTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINER);

		stbi_image_free(image);
		return retTexture;


	}

	GLuint LoadTexture(const char *filePath) {
		int w, h, comp;
		unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
		if (image == NULL) {
			std::cout << "Unable to load image. Make sure the path is correct\n";
			assert(false);
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
	

//------GLuint Program----------------------------------
//-----------Set-Up---------------------------
	glViewport(0, 0, 640, 360);
	ShaderProgram program;
	program.Load(RESOURCE_FOLDER "vertex_textured.glsl", RESOURCE_FOLDER "fragment_textured.glsl");

	GLuint emojiTexture = LoadTexture(RESOURCE_FOLDER"emoji.png"); //GLuint and LoadTexture are giving me undefinded errors
	
	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);

	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);


	glUseProgram(program.programID);
//-----------Set-Up------------------------------








//-----------Drawing
	//glClear(GL_COLOR_BUFFER_BIT);

	//program.SetModelMatrix(modelMatrix);
	//program.SetProjectionMatrix(projectionMatrix);
	//program.SetViewMatrix(viewMatrix);

	//float vertices[] = { 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f };
	//glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	//glEnableVertexAttribArray(program.positionAttribute);

	//glDrawArrays(GL_TRIANGLES, 0, 3);

	//glDisableVertexAttribArray(program.positionAttribute);

	//SDL_GL_SwapWindow(displayWindow);





   
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
		//------------Drawing
		glClear(GL_COLOR_BUFFER_BIT);

		program.SetModelMatrix(modelMatrix);
		program.SetProjectionMatrix(projectionMatrix);
		program.SetViewMatrix(viewMatrix);

		glBindTexture(GL_TEXTURE_2D, emojiTexture);

		/*float vertices[] = {				ARE WE NOT DOING FLOATS ANYMORE?!?!
			0.5f, -0.5f, 
			0.0f, 0.5f, 
			-0.5f, -0.5f
		};*/

		float vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float textCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, textCoords);
		glEnableVertexAttribArray(program.texCoordAttribute); 

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawArrays(GL_TRIANGLES, 0, 6); 

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		

		SDL_GL_SwapWindow(displayWindow);
		//------------Drawing
       /* glClear(GL_COLOR_BUFFER_BIT);
        SDL_GL_SwapWindow(displayWindow);*/
    }
    
    SDL_Quit();
    return 0;
}
