//HW 4
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

#include "FlareMap.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "stb_image.h"
#include <vector>

#include <Windows.h>

#define SPRITESHEET_HEIGHT 8
#define SPRITESHEET_WIDTH 16
#define TILE_SIZE 0.05f


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



//class Skateboard {
//	Skateboard() {} //momo instructor
//};

class Sprite {

public:
	Sprite() {}
	Sprite(unsigned int textureID, float u, float v, float width, float height, float
		size): textureID(textureID), u(u), v(v), width(width), height(height), size(size) {}


	void Draw(ShaderProgram &program);

	
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	float size;
};

void Sprite::Draw(ShaderProgram &program) {
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
	Entity ( float x, float y, float h, float yBullets = 0.0f) 
	{
		position.x = x;
		position.y = y;
		velocity.x = h;
		velocity.y = yBullets;
	}
	void Draw(ShaderProgram &program);
	//void Update(float elapsed);
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 size;
	/*float rotation;*/
	Sprite sprite;
	//float health;
	//float somethingElse;


};

void Entity::Draw(ShaderProgram &program) {
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, position.y, 0.0f)); //Moving Image (x,y,z)
	program.SetModelMatrix(modelMatrix); //Using this staring point
	sprite.Draw(program);

}







//Entity player;
vector<Entity> enemies; //--------------------------Enemies

struct GameState {
	Entity player;
};
GameState state;


GLuint fontTexture;
GLuint mapTexture;
void DrawText(ShaderProgram &program, int fontTexture, std::string text, float size, float spacing) {
	float character_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		int spriteIndex = (int)text[i];
		float texture_x = (float)(spriteIndex % 16) / 16.0f;
		float texture_y = (float)(spriteIndex / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
		((size + spacing) * i) + (-0.5f * size), 0.5f * size,
		((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		((size + spacing) * i) + (0.5f * size), 0.5f * size,
		((size + spacing) * i) + (0.5f * size), -0.5f * size,
		((size + spacing) * i) + (0.5f * size), 0.5f * size,
		((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			});
		texCoordData.insert(texCoordData.end(), {
		texture_x, texture_y,
		texture_x, texture_y + character_size,
		texture_x + character_size, texture_y,
		texture_x + character_size, texture_y + character_size,
		texture_x + character_size, texture_y,
		texture_x, texture_y + character_size,
			});
	}
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	// draw this data (use the .data() method of std::vector to get pointer to data)
	// draw this yourself, use text.size() * 6 or vertexData.size()/2 to get number of vertices    

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program.positionAttribute);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

}
//void Render(ShaderProgram &program) 
//{
//	player.Draw(program);
//}

FlareMap fm; 
void drawTileMap(ShaderProgram &program)
{
	
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int y = 0; y < fm.mapHeight; y++) {
		for (int x = 0; x < fm.mapWidth; x++) {
			if (fm.mapData[y][x] != 0) {
				// add vertices

				float u = (float)(((int)fm.mapData[y][x]) % SPRITESHEET_WIDTH) / (float)SPRITESHEET_WIDTH;
				float v = (float)(((int)fm.mapData[y][x]) / SPRITESHEET_WIDTH) / (float)SPRITESHEET_HEIGHT;
				float spriteWidth = 1.0f / (float)SPRITESHEET_WIDTH;
				float spriteHeight = 1.0f / (float)SPRITESHEET_HEIGHT;

				vertexData.insert(vertexData.end(), {
				TILE_SIZE * x, -TILE_SIZE * y,
				TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
				TILE_SIZE * x, -TILE_SIZE * y,
				(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
				(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
					});
				texCoordData.insert(texCoordData.end(), {
				u, v,
				u, v + (spriteHeight),
				u + spriteWidth, v + (spriteHeight),
				u, v,
				u + spriteWidth, v + (spriteHeight),
				u + spriteWidth, v
					});
			}
		}
	}
	glBindTexture(GL_TEXTURE_2D, mapTexture);
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program.positionAttribute);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, vertexData.size()/2);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}




enum GameMode { STATE_MAIN_MENU, STATE_GAME_LEVEL };
GameMode mode = STATE_MAIN_MENU;

void RenderMainMenu(ShaderProgram& program) {
	//do some hawt stuff;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.9f, -0.5f, 0.0f));
	program.SetModelMatrix(modelMatrix);
	DrawText(program, fontTexture, "HW 4", 0.13f, 0.0f);
	//DrawText(program, fontTexture, "kjb.e,nmd", 0.13f, 0.0f);

}

void RenderGameLevel(ShaderProgram& program, GameState &state) {
	//do some hawt stuff;
	//state.player.Draw(program);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	program.SetModelMatrix(modelMatrix);
	drawTileMap(program);

}

void UpdateGameLevel(GameState& state, float elapsed) {
	
	const Uint8 *keys = SDL_GetKeyboardState(NULL);


	if (keys[SDL_SCANCODE_LEFT])
	{
		state.player.position.x += -(state.player.velocity.x * elapsed);
	}

	if (keys[SDL_SCANCODE_RIGHT])
	{
		state.player.position.x += (state.player.velocity.x * elapsed);
	}


}

void Render(ShaderProgram& program) {
	switch (mode) {
	case STATE_MAIN_MENU:
		RenderMainMenu(program);
		break;
	case STATE_GAME_LEVEL:
		RenderGameLevel(program, state);
		break;
	}
}

void Update(float elapsed) {
	switch (mode) {
	case STATE_MAIN_MENU:
		break;
	case STATE_GAME_LEVEL:
		UpdateGameLevel(state, elapsed);
		break;
	}
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
	fm.Load("hw4example.txt");
	glViewport(0, 0, 640, 360);
	ShaderProgram program;
	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glm::mat4 projectionMatrix = glm::mat4(1.0f);//Project matrix -[Defining Bounds of Window of Projection]:  (View to Projection) renders scene by squishing world into 2d 
	glm::mat4 modelMatrix = glm::mat4(1.0f);	 //Model matrix - [TRS per object]: (Model to World) 
	glm::mat4 viewMatrix = glm::mat4(1.0f);		 //View matrix - [Where in ithe world am I looking]: (World to View) Scope of Game 
	viewMatrix = glm::translate(viewMatrix, glm::vec3(-1.5f, 0.5f, 0.0f));
	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f); //Ortho Project ----------------- ACTUALLY DEFINING VIEW (x,x y,y z,z)
	glClearColor(0.0f, 0.0f, 0.6f, 1.0f); //Set Color

	program.SetProjectionMatrix(projectionMatrix); //Setting Projection Matrix
	program.SetViewMatrix(viewMatrix); //Setting View Matrix

	 /* float ticks = (float)SDL_GetTicks() / 1000.0f;
	  float lastFrameTicks = ticks;
	  float elapsed = ticks - lastFrameTicks;
	  angle += elapsed;*/





	GLuint spriteSheetTexture = LoadTexture("sheet.png");
	fontTexture = LoadTexture("font1.png");
	mapTexture = LoadTexture("arne_sprites.png");
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
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
				{
					if (mode == STATE_MAIN_MENU)
					{
						mode = STATE_GAME_LEVEL;
					}
				}
			}
			
		}

		

		glClear(GL_COLOR_BUFFER_BIT);


		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		time += elapsed; //actual seconds
		lastFrameTicks = ticks;
		float speed = 1.0f;
		


		Render(program);
		Update(elapsed);

		SDL_GL_SwapWindow(displayWindow);

	}


		SDL_Quit();
		return 0;
	
}