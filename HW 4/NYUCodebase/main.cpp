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







//Entity player;
vector<Entity> enemies; //--------------------------Enemies

struct GameState {
	Entity player;
	vector<Entity> enemies;
	Entity bullets[MAX_BULLETS];
};
GameState state;

int bulletIndex = 0;

void shootBullet(Entity &ship) {

	state.bullets[bulletIndex].position.x = ship.position.x;
	state.bullets[bulletIndex].position.y = ship.position.y;
	bulletIndex++;

	if (bulletIndex > MAX_BULLETS - 1)
	{
		bulletIndex = 0;
	}


}

GLuint fontTexture;
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






enum GameMode { STATE_MAIN_MENU, STATE_GAME_LEVEL };
GameMode mode = STATE_MAIN_MENU;

void RenderMainMenu(ShaderProgram& program) {
	//do some hawt stuff;
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.5f, 0.0f, 0.0f));
	program.SetModelMatrix(modelMatrix);
	DrawText(program, fontTexture, "HI THERE, PRESS SPACEBAR!", 0.13f, 0.0f);

}

void RenderGameLevel(ShaderProgram& program, GameState &state) {
	//do some hawt stuff;
	state.player.Draw(program);

	for (Entity &enemy : state.enemies)
	{
		enemy.Draw(program);
	}


	for (Entity &bullet : state.bullets)
	{
		bullet.Draw(program);

	}




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

	for (Entity &bullet : state.bullets)
	{
		bullet.position.y += elapsed * bullet.velocity.y;

	}


	//-------------------------------------Collision-------------------------------------------

	for (int i = 0; i < state.enemies.size(); ++i)
	{

		for (int j = 0; j < MAX_BULLETS; ++j)
		{
			float bulletEnemyCollisionY = (abs(state.enemies[i].position.y - state.bullets[j].position.y)) - ((state.bullets[j].size.y + state.enemies[i].size.y) / 2);
			float bulletEnemyCollisionX = (abs(state.enemies[i].position.x - state.bullets[j].position.x)) - ((state.bullets[j].size.x + state.enemies[i].size.x) / 2);


			if ((bulletEnemyCollisionY <= 0) && (bulletEnemyCollisionX <= 0)) {
				state.enemies[i].position.y = -1000.0f;
				state.bullets[j].position.y = 1000.0f;
			}


		}
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
	fontTexture = LoadTexture("font1.png");
	SheetSprite mySprite = SheetSprite(spriteSheetTexture, 425.0f / 1024.0f, 468.0f / 1024.0f, 93.0f / 1024.0f, 84.0f /	1024.0f, 0.2f);

	Entity Josh(0.0f, -0.9f, 1.0f);
	Josh.sprite = mySprite;
	Josh.size.x = mySprite.size * (mySprite.width / mySprite.height);
	Josh.size.y = mySprite.size;

	state.player = Josh;
	//<SubTexture name="ufoYellow.png" x="505" y="898" width="91" height="91"/>
	
	SheetSprite myEnemySprite = SheetSprite(spriteSheetTexture, 505.0f / 1024.0f, 898.0f / 1024.0f, 91.0f / 1024.0f, 91.0f / 1024.0f, 0.3f);
	/*Entity Momo(0.0f, 0.8f, 1.0f);
	Momo.sprite = myEnemySprite;
	Momo.size.x = myEnemySprite.size * (myEnemySprite.width / myEnemySprite.height);
	Momo.size.y = myEnemySprite.size;*/

	float x = -1.0f;
	float y = 0.8f;
	int count = 0;

	for (int i = 0; i < 28; ++i)
	{
		Entity Momo(x, y, 1.0f);
		Momo.sprite = myEnemySprite;
		Momo.size.x = myEnemySprite.size * (myEnemySprite.width / myEnemySprite.height);
		Momo.size.y = myEnemySprite.size;
		enemies.push_back(Momo);
		x += Momo.size.x;
		count++;

		if (count==7)
		{
			x = -1.0f;
			y -= myEnemySprite.size;
			count = 0;
		}
		
		
	}

	state.enemies = enemies; 

	//Laser
	//<SubTexture name="laserBlue10.png" x="740" y="724" width="37" height="37"/>

	SheetSprite myPlayerBullet = SheetSprite(spriteSheetTexture, 740.0f / 1024.0f, 724.0f / 1024.0f, 37.0f / 1024.0f, 37.0f / 1024.0f, 0.05f);

	for (int i = 0; i < MAX_BULLETS; ++i) { 
		
		state.bullets[i].position.x = -2000.0f;
		state.bullets[i].sprite = myPlayerBullet;
		state.bullets[i].velocity.y = 1.0f;

	}


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
					shootBullet(state.player);
				}
			}
			
		}

		

		glClear(GL_COLOR_BUFFER_BIT);


		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		time += elapsed; //actual seconds
		lastFrameTicks = ticks;
		float speed = 1.0f;
		


		//
		////Momo.Draw(program);






		
		//indicate right side wins

		//float leftX = -1.67f;
		//float rightX = 1.67f;

		//float LeftCollisionY = (abs(leftY - ballY)) - ((ballHeight + paddleHeight) / 2); // 1, 2, 3, 4 quadrants to check using collY and collX
		//float LeftCollisionX = (abs(leftX - ballX)) - ((ballWidth + paddleWidth) / 2);

		//float RightCollisionY = (abs(rightY - ballY)) - ((ballHeight + paddleHeight) / 2);
		//float RightCollisionX = (abs(rightX - ballX)) - ((ballHeight + paddleWidth) / 2);

		//if ((LeftCollisionY <= 0) && (LeftCollisionX <= 0)) { dirX = -dirX; }
		//// indicate left side wins

		//if ((RightCollisionY <= 0) && (RightCollisionX <= 0)) { dirX = -dirX; }
		////indicate right side wins

		Render(program);
		Update(elapsed);

		SDL_GL_SwapWindow(displayWindow);

	}


		SDL_Quit();
		return 0;
	
}