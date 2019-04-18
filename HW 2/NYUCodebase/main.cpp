//HW 2
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
#include <math.h>

#include <Windows.h>


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

float time = 0.0f;


void reset(float& ballX, float& ballY, float& leftSide, float& rightSide)
{
	ballX = 0.0f;   //ball x coordinate
	ballY = 0.0f;   //ball y coordinate (-1,1)
	float angle = (float)(rand() % 180);
	float dirX = sin(angle);
	float dirY = cos(angle);
	glClearColor(0.0f, 0.0f, 0.6f, 1.0f);

	//Ball & Paddle Initial
	leftSide = 0.0f;   //paddle on the left
	rightSide = 0.0f;   //paddle on the right
	
	
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
	program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");



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

	//Ball & Paddle Initial
	float leftY = 0.0f;   //paddle on the left
	float rightY = 0.0f;   //paddle on the right
	float ballX = 0.0f;   //ball x coordinate
	float ballY = 0.0f;   //ball y coordinate (-1,1)
	
	
	float angle = (float)(rand() % 360);
	float dirX = sin(90);
	float dirY = cos(90);

	float lastFrameTicks = 0.0f; 
	float paddleHeight = 0.7f; //paddle height
	float paddleWidth = 0.1f;

	float ballHeight = 0.06f;
	float ballWidth = 0.06f;



	//End game Flag
	bool aWinFlag = false; 
	
	

	
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
			/*else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
				{
					angle += 1.0;
				}
			}*/
		}

		

		glClear(GL_COLOR_BUFFER_BIT);


		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		time += elapsed; //actual seconds
		lastFrameTicks = ticks;
		float speed = 1.0f;
		

		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		
		ballX += speed*elapsed*dirX;
		ballY += speed*elapsed*dirY;

		if (keys[SDL_SCANCODE_W]){leftY += speed * elapsed;}//Left Paddle																	//leftY = elapsed*4;//cout << leftY << endl;
		if (keys[SDL_SCANCODE_S]){leftY -= speed*elapsed;}
		//----------------------------------------------------------------------
		if (keys[SDL_SCANCODE_UP]) { rightY += speed * elapsed; }//Right Paddle
		if (keys[SDL_SCANCODE_DOWN]) { rightY -= speed * elapsed; }

		



	// pong ball--------------------------------------------------------------------------------------------------------------------------------------------------------------O
		modelMatrix = glm::mat4(1.0f); //Setting starting Point	
		
		//-------------------------------------------------Boarder of the Game
		if (ballX >= (1.7777777f - ballWidth / 2.0f))  //-------------------------------------------------Left Side Wins
		{
		
			/*time = 0.0f;
			
			cout << time << endl;
			time++;
			*/
			//cout << time << endl;
			//aWinFlag = true;
			//cout << "test" << endl;
			glClearColor(0.0f, 255.0f, 0.0f, 1.0f); 
			
			//while (time < 2000){
			//	//glClearColor(255.0f, 255.0f, 255.0f, 1.0f);
			//	time++;
			//	cout << time << endl;
			//}
			cout << "Left Player Wins. Screen is now Green." << endl;
			//reset(ballX, ballY, leftY, rightY);

		}

		if (ballX <= -(1.7777777f - ballWidth / 2.0f)) //-------------------------------------------------Right Side Wins
		{
			//aWinFlag = true;
			glClearColor(255.0f, 0.0f, 255.0f, 1.0f); //Set Color
			//reset(ballX, ballY, leftY, rightY);

			cout << "Right Player Wins. Screen is now Pink." << endl;
		}

		//====================================================

		if (ballY >= (1.0f - ballHeight / 2.0f))
		{
			//ballY = 1/(1.0f - ballHeight / 2.0f);
			dirY = -dirY;
		}

		if (ballY <= -(1.0f - ballHeight / 2.0f))
		{
			//ballY = -1/(1.0f - ballHeight / 2.0f);
			dirY = -dirY; 
		}												
		//-------------------------------------------------Boarder of the Game


		float leftX = -1.67f;
		float rightX = 1.67f;

		float LeftCollisionY = (abs(leftY - ballY)) - ((ballHeight + paddleHeight) / 2); // 1, 2, 3, 4 quadrants to check using collY and collX
		float LeftCollisionX = (abs(leftX - ballX)) - ((ballWidth + paddleWidth) / 2);

		float RightCollisionY = (abs(rightY - ballY)) - ((ballHeight + paddleHeight) / 2);
		float RightCollisionX = (abs(rightX - ballX)) - ((ballHeight + paddleWidth) / 2);

		if ((LeftCollisionY <= 0) && (LeftCollisionX <= 0)) {dirX = -dirX;} 
		// indicate left side wins

		if ((RightCollisionY <= 0) && (RightCollisionX <= 0)) {dirX = -dirX;}
		//indicate right side wins





		//float collisionY = (ballHeight - height)

		modelMatrix = glm::translate(modelMatrix, glm::vec3(ballX,ballY, 0.0f)); //Moving Image (x,y,z)
		program.SetModelMatrix(modelMatrix); //Using this staring point
		//draw untextured
		float vertices[] = { 
			-0.03, -0.03, //Lower Left -x,-y
			 0.03, -0.03, //Lower Right x,-y
			 0.03, 0.03, //Upper Right x,y
			 -0.03, -0.03, //Upper Left -x,-y
			 0.03, 0.03, // x,y
			 -0.03, 0.03 // -x,y
		}; 
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices); //Vertex Atrribute = use vertices
		glEnableVertexAttribArray(program.positionAttribute); //Enable Vertex Atrribute = enable vertices	
		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw Previously Setup Object
		glDisableVertexAttribArray(program.positionAttribute); //Undraw Coordinates










// paddle left--------------------------------------------------------------------------------------------------------------------------------------------------------------<
		if (leftY >= (1.0f - paddleHeight / 2.0f))
		{
			leftY = (1.0f - paddleHeight / 2.0f);
		}
		
		if (leftY <= -(1.0f - paddleHeight / 2.0f))
		{
			leftY = -(1.0f - paddleHeight / 2.0f);
		}

		modelMatrix = glm::mat4(1.0f); //Setting starting Point	
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.67f, leftY, 0.0f)); //Moving Image (x,y,z)
		program.SetModelMatrix(modelMatrix); //Using this staring point
		//draw untextured
		float vertices1[] = {
			-0.05, -0.35, //			Bottom Middle Side [1] == [4]
			 0.05, -0.35, //			Lower Right x,-y -----Lower Right Corner [2]
			 0.05, 0.35, //			Right middle side [3] = [5]

			 -0.05, -0.35, //			Left Middle Side [4] == [1]
			 0.05, 0.35, //			Top Middle Side [5] = [3]
			 -0.05, 0.35 //			-x,y ---------------Upper Left Corner [6]
		};
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1); //Vertex Atrribute = use vertices
		glEnableVertexAttribArray(program.positionAttribute); //Enable Vertex Atrribute = enable vertices	
		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw Previously Setup Object
		glDisableVertexAttribArray(program.positionAttribute); //Undraw Coordinates







	// left flag ---------------------------------
		//modelMatrix = glm::mat4(1.0f); //Setting starting Point	
		//modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.7777777f, 0.0f, 0.0f)); //Moving Image (x,y,z)
		//program.SetModelMatrix(modelMatrix); //Using this staring point
		////draw untextured
		//float vertices3[] = {
		//	-0.05, -0.35, //			Bottom Middle Side [1] == [4]
		//	 0.05, -0.35, //			Lower Right x,-y -----Lower Right Corner [2]
		//	 0.05, 0.35, //			Right middle side [3] = [5]

		//	 -0.05, -0.35, //			Left Middle Side [4] == [1]
		//	 0.05, 0.35, //			Top Middle Side [5] = [3]
		//	 -0.05, 0.35 //			-x,y ---------------Upper Left Corner [6]
		//};
		//glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3); //Vertex Atrribute = use vertices
		//glEnableVertexAttribArray(program.positionAttribute); //Enable Vertex Atrribute = enable vertices	
		//glDrawArrays(GL_TRIANGLES, 0, 6); //Draw Previously Setup Object
		//glDisableVertexAttribArray(program.positionAttribute); //Undraw Coordinates









// paddle right---------------------------------------------------------------------------------------------------------------------------------------------------------->
		
		if (rightY >= (1.0f - paddleHeight / 2.0f))
		{
			rightY = (1.0f - paddleHeight / 2.0f);
		}

		if (rightY <= -(1.0f - paddleHeight / 2.0f))
		{
			rightY = -(1.0f - paddleHeight / 2.0f);
		}

		modelMatrix = glm::mat4(1.0f); //Setting starting Point	
		modelMatrix = glm::translate(modelMatrix, glm::vec3(1.67f, rightY, 0.0f)); //Moving Image (x,y,z)
		program.SetModelMatrix(modelMatrix); //Using this staring point
		//draw untextured
		float vertices2[] = {
			-0.05, -0.35, //			Bottom Middle Side [1] == [4]
			 0.05, -0.35, //			Lower Right x,-y -----Lower Right Corner [2]
			 0.05, 0.35, //			Right middle side [3] = [5]

			 -0.05, -0.35, //			Left Middle Side [4] == [1]
			 0.05, 0.35, //			Top Middle Side [5] = [3]
			 -0.05, 0.35 //			-x,y ---------------Upper Left Corner [6]
		};
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2); //Vertex Atrribute = use vertices
		glEnableVertexAttribArray(program.positionAttribute); //Enable Vertex Atrribute = enable vertices	
		glDrawArrays(GL_TRIANGLES, 0, 6); //Draw Previously Setup Object
		glDisableVertexAttribArray(program.positionAttribute); //Undraw Coordinates

	// right flag ----------------------------------------------------------------------------------------------------
		//modelMatrix = glm::mat4(1.0f); //Setting starting Point	
		//modelMatrix = glm::translate(modelMatrix, glm::vec3(1.777777777f, 1.0f, 0.0f)); //Moving Image (x,y,z)
		//program.SetModelMatrix(modelMatrix); //Using this staring point
		////draw untextured
		//float vertices4[] = {
		//	-0.05, -0.35, //			Bottom Middle Side [1] == [4]
		//	 0.05, -0.35, //			Lower Right x,-y -----Lower Right Corner [2]
		//	 0.05, 0.35, //			Right middle side [3] = [5]

		//	 -0.05, -0.35, //			Left Middle Side [4] == [1]
		//	 0.05, 0.35, //			Top Middle Side [5] = [3]
		//	 -0.05, 0.35 //			-x,y ---------------Upper Left Corner [6]
		//};
		//glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices4); //Vertex Atrribute = use vertices
		//glEnableVertexAttribArray(program.positionAttribute); //Enable Vertex Atrribute = enable vertices	
		//glDrawArrays(GL_TRIANGLES, 0, 6); //Draw Previously Setup Object
		//glDisableVertexAttribArray(program.positionAttribute); //Undraw Coordinates




























		

		SDL_GL_SwapWindow(displayWindow);
		glUseProgram(program.programID);

	}


		SDL_Quit();
		return 0;
	
}