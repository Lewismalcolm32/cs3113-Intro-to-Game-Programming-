//HW 3
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <cstdlib>
#include <SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#define STB_IMAGE_IMPLEMENTATION
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include <vector>

#include <Windows.h>


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define MAX_BULLETS 30
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

float time = 0.0f;


void reset(float& ballX, float& ballY, float& leftSide, float& rightSide)
{
	ballX = 0.0f;   //ball x coordinate
	ballY = 0.0f;   //ball y coordinate (-1,1)
	float angle = (float)(rand() % 90);
	float dirX = sin(angle);
	float dirY = cos(angle);
	glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

	//Ball & Paddle Initial
	leftSide = 0.0f;   //paddle on the left
	rightSide = 0.0f;   //paddle on the right
	
	
}

//class Skateboard {
//	Skateboard() {} //momo instructor
//};

class SheetSprite {

public:
	SheetSprite() {}
	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float
		size): textureID(textureID), u(u), v(v), width(width), height(height), size(size) {}


	void Draw(ShaderProgram &program);

	
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	float size;
};

void SheetSprite::Draw(ShaderProgram &program) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLfloat texCoords[] = {
	u, v + height,
	u + width, v,
	u, v,
	u + width, v,
	u, v + height,
	u + width, v + height
	};
	float aspect = width / height;
	float vertices[] = {
	-0.5f * size * aspect, -0.5f * size,
	0.5f * size * aspect, 0.5f * size,
	-0.5f * size * aspect, 0.5f * size,
	0.5f * size * aspect, 0.5f * size,
	-0.5f * size * aspect, -0.5f * size ,
	0.5f * size * aspect, -0.5f * size };
	// draw our arrays

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}










class Entity{

public:
	Entity() {}
	Entity ( float x, float y, float h) 
	{
		position.x = x;
		position.y = y;
		velocity.x = h;
	}
	void Draw(ShaderProgram &program);
	//void Update(float elapsed);
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 size;
	/*float rotation;*/
	SheetSprite sprite;
	//float health;
	//float somethingElse;


};

void Entity::Draw(ShaderProgram &program) {
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, position.y, 0.0f)); //Moving Image (x,y,z)
	program.SetModelMatrix(modelMatrix); //Using this staring point
	sprite.Draw(program);

}



Entity player;

void Render(ShaderProgram &program) 
{
	player.Draw(program);
}










int main(int argc, char *argv[])
{
	//setup------------------------------------------------------
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong (by Malcolm Lewis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	//setup---------------------------------------------------------

	glViewport(0, 0, 640, 360);
	ShaderProgram program;
	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glm::mat4 projectionMatrix = glm::mat4(1.0f);//Project matrix -[Defining Bounds of Window of Projection]:  (View to Projection) renders scene by squishing world into 2d 
	glm::mat4 modelMatrix = glm::mat4(1.0f);	 //Model matrix - [TRS per object]: (Model to World) 
	glm::mat4 viewMatrix = glm::mat4(1.0f);		 //View matrix - [Where in ithe world am I looking]: (World to View) Scope of Game 
	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f); //Ortho Project ----------------- ACTUALLY DEFINING VIEW (x,x y,y z,z)
	glClearColor(0.0f, 0.0f, 0.6f, 1.0f); //Set Color

	program.SetProjectionMatrix(projectionMatrix); //Setting Projection Matrix
	program.SetViewMatrix(viewMatrix); //Setting View Matrix

	 /* float ticks = (float)SDL_GetTicks() / 1000.0f;
	  float lastFrameTicks = ticks;
	  float elapsed = ticks - lastFrameTicks;
	  angle += elapsed;*/





	GLuint spriteSheetTexture = LoadTexture("sheet.png");
	SheetSprite mySprite = SheetSprite(spriteSheetTexture, 425.0f / 1024.0f, 468.0f / 1024.0f, 93.0f / 1024.0f, 84.0f /	1024.0f, 0.2f);

	Entity Josh(0.0f, -0.9f, 1.0f);
	Josh.sprite = mySprite;
	Josh.size.x = mySprite.size * (mySprite.width / mySprite.height);
	Josh.size.y = mySprite.size;
	

	float lastFrameTicks = 0.0f; 



	
	   	  	

	
	SDL_Event event;
	bool done = false;
	while (!done)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
			{
				done = true;
			}
			
		}

		

		glClear(GL_COLOR_BUFFER_BIT);


		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		time += elapsed; //actual seconds
		lastFrameTicks = ticks;
		float speed = 1.0f;
		

		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		
		
		if (keys[SDL_SCANCODE_LEFT]) 
			{
				Josh.position.x += -(Josh.velocity.x * elapsed);
			}//
		if (keys[SDL_SCANCODE_RIGHT]) 
			{
				Josh.position.x += (Josh.velocity.x * elapsed);
			}

		Josh.Draw(program);






		

		SDL_GL_SwapWindow(displayWindow);

	}


		SDL_Quit();
		return 0;
	
}