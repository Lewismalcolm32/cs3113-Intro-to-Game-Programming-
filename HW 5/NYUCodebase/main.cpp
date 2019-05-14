//HW 5 
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
#define TILE_SIZE 0.125f

//#define FIXED_TIMESTEP 0.0166666f
//#define MAX_TIMESTEPS 6
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"

#endif
#define MAX_BULLETS 30
using namespace std;

SDL_Window* displayWindow;
ShaderProgram program;


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



void DrawSpriteSheetSprite(ShaderProgram &program, int index, int spriteCountX,
	int spriteCountY, int texture) {
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;
	float texCoords[] = {
	u, v + spriteHeight,
	u + spriteWidth, v,
	u, v,
	u + spriteWidth, v,
	u, v + spriteHeight,
	u + spriteWidth, v + spriteHeight
	};
	float vertices[] = { -0.5f*TILE_SIZE, -0.5f*TILE_SIZE, 0.5f*TILE_SIZE, 0.5f*TILE_SIZE, -0.5f*TILE_SIZE, 0.5f*TILE_SIZE, 0.5f*TILE_SIZE, 0.5f*TILE_SIZE, -0.5f*TILE_SIZE,
	-0.5f*TILE_SIZE, 0.5f*TILE_SIZE, -0.5f*TILE_SIZE };
	// draw this data
	glBindTexture(GL_TEXTURE_2D, texture);
	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0,6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);
}

class Entity{

public:
	Entity() {}
	Entity (float x, float y, int index) : index(index)
	{
		position.x = x;
		position.y = y;
		velocity.x = 0.5f;
		velocity.y = 0.0f;
		acceleration.x = 0.0f;
		acceleration.y = 0.0f;

		size.x = TILE_SIZE;
		size.y = TILE_SIZE;
	}
	void Draw(ShaderProgram &program) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(position.x, position.y, 0.0f)); //Moving Image (x,y,z)
		program.SetModelMatrix(modelMatrix); //Using this staring point
		DrawSpriteSheetSprite(program, index, SPRITESHEET_WIDTH, SPRITESHEET_HEIGHT, texture);
	}
	//void Update(float elapsed);
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 size;
	int texture;
	int index;

};




vector<Entity> enemies; //--------------------------Enemies

struct GameState {
	Entity player;
	Entity player2;
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




enum GameMode { STATE_MAIN_MENU, STATE_GAME_LEVEL };  ///LEVEL UPDATE
GameMode mode = STATE_MAIN_MENU;

void RenderMainMenu(ShaderProgram& program) {
	//do some hawt stuff;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.9f, -0.5f, 0.0f));
	program.SetModelMatrix(modelMatrix);
	DrawText(program, fontTexture, "FINAL", 0.13f, 0.0f);
	//DrawText(program, fontTexture, "kjb.e,nmd", 0.13f, 0.0f);

}

void RenderGameLevel(ShaderProgram& program, GameState &state) {
	//do some hawt stuff;
	
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	program.SetModelMatrix(modelMatrix);
	drawTileMap(program);

	state.player.Draw(program);
	state.player2.Draw(program);

}
void worldToTileCoordinates(float worldX, float worldY, int& gridX, int& gridY) {
	gridX = (int)(worldX / TILE_SIZE);
	gridY = (int)(worldY / -TILE_SIZE);
}

float mapValue(float value, float srcMin, float srcMax, float dstMin, float dstMax) {
	float retVal = dstMin + ((value - srcMin) / (srcMax - srcMin) * (dstMax - dstMin));
	if (retVal < dstMin) {
		retVal = dstMin;
	}
	if (retVal > dstMax) {
		retVal = dstMax;
	}
	return retVal;
}

float animationTime = 0.0f;

float lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t * v1;
}
float friction_x = 0.5f;
float friction_y = 0.5f;


void UpdateGameLevel(GameState& state, float elapsed) {

	animationTime = animationTime + elapsed;
	float animationValue = mapValue(animationTime, 0.0f, 9.0f, 0.0f, 1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	std::cout << state.player.position.y << std::endl;
	float xPos = lerp(0.5f, 8.0f, animationValue);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(xPos, -1.3f, 0.0));
	program.SetModelMatrix(modelMatrix);
	DrawText(program, fontTexture, "LEVEL ONE: GET THIS BREAD!!!", 0.13f, 0.0f);

	state.player.acceleration.x = 0.0f;
	state.player.acceleration.y = -1.0f;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_LEFT])
	{
		state.player.velocity.x = -1.0f;
	}

	if (keys[SDL_SCANCODE_RIGHT])
	{
		state.player.velocity.x = 1.0f;
	}
	state.player2.position.y += -0.5f*elapsed; //gravity but for my twin

	state.player.velocity.x = lerp(state.player.velocity.x, 0.0f, elapsed * friction_x);
	state.player.velocity.y = lerp(state.player.velocity.y, 0.0f, elapsed * friction_y);
	state.player.velocity.x += state.player.acceleration.x * elapsed;
	state.player.velocity.y += state.player.acceleration.y * elapsed;
	state.player.position.y += state.player.velocity.y * elapsed;

	float bottomOfPlayer = state.player.position.y - (TILE_SIZE / 2.0f);
	float bottomOfPlayer2 = state.player2.position.y - (TILE_SIZE / 2.0f);

	int gridX;
	int gridY;

	//player bottom collision
	worldToTileCoordinates(state.player.position.x, bottomOfPlayer, gridX, gridY);
	if (gridY < fm.mapHeight && gridX < fm.mapWidth && gridY >= 0 && gridX >= 0 && fm.mapData[gridY][gridX] != 0) {
		float penetration = fabs((-TILE_SIZE * gridY) - (bottomOfPlayer));
		state.player.position.y += penetration;

	}

	//top collision
	float topOfPlayer = state.player.position.y + (TILE_SIZE / 2.0f);
	worldToTileCoordinates(state.player.position.x, topOfPlayer, gridX, gridY);
	if (gridY < fm.mapHeight && gridX < fm.mapWidth && gridY >= 0 && gridX >= 0 && fm.mapData[gridY][gridX] != 0) {
		float penetration = fabs(topOfPlayer - ((-TILE_SIZE * gridY) - TILE_SIZE));
		state.player.position.y -= penetration;

	}
	state.player.position.x += state.player.velocity.x * elapsed;
	//Left Collision
	float leftOfPlayer = state.player.position.x - (TILE_SIZE / 2.0f);
	worldToTileCoordinates(leftOfPlayer, state.player.position.y, gridX, gridY);
	if (gridY < fm.mapHeight && gridX < fm.mapWidth && gridY >= 0 && gridX >= 0 && fm.mapData[gridY][gridX] != 0) {
		float penetration = fabs(leftOfPlayer - ((TILE_SIZE * gridX) + TILE_SIZE));
		state.player.position.x += penetration;

	}

	//Right Collision
	float rightOfPlayer = state.player.position.x + (TILE_SIZE / 2.0f);
	worldToTileCoordinates(rightOfPlayer, state.player.position.y, gridX, gridY);
	if (gridY < fm.mapHeight && gridX < fm.mapWidth && gridY >= 0 && gridX >= 0 && fm.mapData[gridY][gridX] != 0) {
		float penetration = fabs((TILE_SIZE * gridX) - rightOfPlayer);
		state.player.position.x -= penetration;

	}


	//player 2 bottom collision
	worldToTileCoordinates(state.player2.position.x, bottomOfPlayer2, gridX, gridY);
	if (gridY< fm.mapHeight && gridX < fm.mapWidth && gridY >= 0 && gridX >= 0 && fm.mapData[gridY][gridX] != 0) {
		float penetration = fabs((-TILE_SIZE * gridY) - (bottomOfPlayer2));
		state.player2.position.y += penetration;

	}
	
	//if (ma facking tile smacks my bottom) {
	//		get the strap and make sure I'm supported }



	/*state.player2.position.x += -(state.player2.velocity.x * elapsed);
	state.player2.position.x += (state.player2.velocity.x * elapsed);*/

	float p1 = (abs(state.player.position.y - state.player2.position.y)) - ((state.player2.size.y + state.player.size.y) / 2);
	float p2 = (abs(state.player.position.x - state.player2.position.x)) - ((state.player2.size.x + state.player.size.x) / 2);

	if ((p1 <= 0) && (p2 <= 0)) {
		state.player2.position.y = -1000.0f;
	}

	
	//collision




	glm::mat4 viewMatrix = glm::mat4(1.0f);
	viewMatrix = glm::translate(viewMatrix, glm::vec3(-state.player.position.x, -state.player.position.y, 0.0f));
	program.SetViewMatrix(viewMatrix);
}




void Render(ShaderProgram& program) {
	switch (mode) {
	case STATE_MAIN_MENU:				///LEVEL UPDATE
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
	case STATE_GAME_LEVEL:  ///LEVEL UPDATE
		UpdateGameLevel(state, elapsed);
		break;
	}
}




float accumulator = 0.0f;




int main(int argc, char *argv[])
{
	//setup------------------------------------------------------
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Pong (y Malcolm Lewis", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	mapTexture = LoadTexture("arne_sprites.png");
	//setup---------------------------------------------------------
	fm.Load("hw4example.txt");
	for (int i = 0; i < fm.entities.size(); ++i) {
		float x = fm.entities[i].x * TILE_SIZE;
		float y = fm.entities[i].y * -TILE_SIZE;
		
		if (fm.entities[i].type == "Player") {
			Entity temp(x, y, 98);
			temp.texture = mapTexture;
			temp.size.x = TILE_SIZE;
			temp.size.y = TILE_SIZE;
			state.player = temp;
		}

		if (fm.entities[i].type == "Player2") {
			Entity temp(x, y, 99);
			temp.texture = mapTexture;
			temp.size.x = TILE_SIZE;
			temp.size.y = TILE_SIZE;

			state.player2 = temp;
		}

	}
	glViewport(0, 0, 640, 360);
	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glm::mat4 projectionMatrix = glm::mat4(1.0f);//Project matrix -[Defining Bounds of Window of Projection]:  (View to Projection) renders scene by squishing world into 2d 
	glm::mat4 modelMatrix = glm::mat4(1.0f);	 //Model matrix - [TRS per object]: (Model to World) 
	glm::mat4 viewMatrix = glm::mat4(1.0f);		 //View matrix - [Where in ithe world am I looking]: (World to View) Scope of Game 
	viewMatrix = glm::translate(viewMatrix, glm::vec3(-1.777f, 1.0f, 0.0f));
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


				else if (event.key.keysym.scancode == SDL_SCANCODE_UP)
				{
					state.player.velocity.y += 1.0f;
				}

				else if (event.key.keysym.scancode == SDL_SCANCODE_Q)
				{
					//SDL_Quit();
					done = true;
				}

				
				
			}
			
		}

		

		glClear(GL_COLOR_BUFFER_BIT);


		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		time += elapsed; //actual seconds
		lastFrameTicks = ticks;


		//elapsed += accumulator;
		//if (elapsed < FIXED_TIMESTEP) {
		//	accumulator = elapsed;
		//	continue;
		//}
		//while (elapsed >= FIXED_TIMESTEP) {
		//	Update(FIXED_TIMESTEP);
		//	elapsed -= FIXED_TIMESTEP;
		//}
		//accumulator = elapsed;

		float speed = 1.0f;
		


		Render(program);
		Update(elapsed); //Because my computer was about to facking break!
		SDL_GL_SwapWindow(displayWindow);

	}


		SDL_Quit();
		return 0;
	
}