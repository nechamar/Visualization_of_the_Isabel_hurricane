#include <time.h>
#include <iostream>
#include "Data.h"
#include "Renderer.h"

using namespace std;

Renderer* renderStaff;

struct SceneObjects
{
	Object* ground;
	Object* tempCuts;
	Object* tempHorCut;
	Object* streamLine;
	Camera* camera;

} sceneObjects;

struct State
{
	int windowWidth;
	int windowHeight;

	bool keyMap[KEYS_COUNT];

	float elapsedTime;

	bool horizontalCut;
	bool drawCuts;
	bool drawStreamlines;

} state;

/*debug*/
float moveX = 0.0f;
float moveY = 0.0f;
float moveZ = 0.0f;
/*debug*/

//set first static camera
void setCam1()
{
	sceneObjects.camera->horizontalAngle = 0.0f;
	sceneObjects.camera->verticalAngle = 89.99f;
	sceneObjects.camera->position = glm::vec3(0.0f, 0.0f, 20.0f);
	sceneObjects.camera->direction = glm::vec3(0.0f, 0.0f, -1.0f);
	return;
}

//set second static camera
void setCam2()
{
	sceneObjects.camera->horizontalAngle = 340.0f;
	sceneObjects.camera->verticalAngle = 40.49f;
	sceneObjects.camera->position = glm::vec3(-1.7446, -0.515631, 1.29848);
	sceneObjects.camera->direction = glm::vec3(0.714654, 0.260113, -0.649315);
	return;
}

void drawWindowContents()
{
	/*debug*/
	if (state.keyMap[KEY_UP_ARROW])
		sceneObjects.ground->size += 1;
	if (state.keyMap[KEY_DOWN_ARROW])
		sceneObjects.ground->size -= 1;
	//std::cout << "Ground size: " << sceneObjects.ground->size << std::endl;
	/*
	std::cout << "Camera pos: " << sceneObjects.camera->position.x << ", " << sceneObjects.camera->position.y << ", " << sceneObjects.camera->position.z << std::endl;
	std::cout << "Camera dir: " << sceneObjects.camera->direction.x << ", " << sceneObjects.camera->direction.y << ", " << sceneObjects.camera->direction.z << std::endl;
	std::cout << "Camera hor: " << sceneObjects.camera->horizontalAngle << std::endl;
	std::cout << "Camera ver: " << sceneObjects.camera->verticalAngle << std::endl;
	*/
	//std::cout << "Horizontal cut: " << state.horizontalCut << std::endl;
	/*debug*/

	glm::mat4 orthoProjectionMatrix = glm::ortho(
		-SCENE_WIDTH, SCENE_WIDTH,
		-SCENE_HEIGHT, SCENE_HEIGHT,
		-10.0f * SCENE_DEPTH, 10.0f * SCENE_DEPTH
	);
	glm::mat4 orthoViewMatrix = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(70.0f), state.windowWidth / (float)state.windowHeight, 0.1f, 100.0f);

	glm::vec3 cameraPosition = sceneObjects.camera->position;
	glm::vec3 cameraCenter = sceneObjects.camera->direction + cameraPosition;
	glm::vec3 cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);

	if (sceneObjects.camera->freeCameraMode)
	{
		cameraPosition = sceneObjects.camera->position;
		cameraCenter = sceneObjects.camera->direction + cameraPosition;
		cameraUpVector = glm::cross(glm::cross(sceneObjects.camera->direction, cameraUpVector), sceneObjects.camera->direction);
	}
	else if (!sceneObjects.camera->secondCamera)
	{
		setCam1();
		cameraPosition = sceneObjects.camera->position;
		cameraCenter = sceneObjects.camera->direction + cameraPosition;
		cameraUpVector = glm::cross(glm::cross(sceneObjects.camera->direction, glm::vec3(1, 0, 0)), sceneObjects.camera->direction);
	}
	else
	{
		setCam2();
		cameraPosition = sceneObjects.camera->position;
		cameraCenter = sceneObjects.camera->direction + cameraPosition;
		cameraUpVector = glm::cross(glm::cross(sceneObjects.camera->direction, glm::vec3(0, 0, 1)), sceneObjects.camera->direction);

	}

	glm::mat4 viewMatrix = glm::lookAt(
		cameraPosition,
		cameraCenter,
		cameraUpVector
	);

	glUseProgram(renderStaff->shaderProgram.program);
	glUseProgram(0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderStaff->drawGround(sceneObjects.ground, viewMatrix, projectionMatrix);
	if(state.drawStreamlines)
	renderStaff->drawStreamlines(sceneObjects.streamLine, viewMatrix, projectionMatrix);

	if (state.drawCuts) {
		renderStaff->drawTempCuts(sceneObjects.tempCuts, viewMatrix, projectionMatrix);
		if (state.horizontalCut)
			renderStaff->drawTempHorizontalCut(sceneObjects.tempHorCut, viewMatrix, projectionMatrix);
	}
	CHECK_GL_ERROR();

}

// Called whenever the window is resized. The new window size is given in pixels.
void reshapeCallback(int newWidth, int newHeight)
{

	state.windowWidth = newWidth;
	state.windowHeight = newHeight;

	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
}

// The special keyboard callback is triggered when keyboard function or directional
// keys are released.
void specialKeyboardUpCallback(int specKeyReleased, int mouseX, int mouseY)
{
	switch (specKeyReleased)
	{
	case GLUT_KEY_RIGHT:
		state.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		state.keyMap[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_UP:
		state.keyMap[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		state.keyMap[KEY_DOWN_ARROW] = false;
		break;
	default:
		break;
		// printf("Unrecognized special key released\n");
	}
}

// The special keyboard callback is triggered when keyboard function or directional
// keys are pressed.
void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY)
{
	switch (specKeyPressed)
	{
	case GLUT_KEY_RIGHT:
		state.keyMap[KEY_RIGHT_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		state.keyMap[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_UP:
		state.keyMap[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		state.keyMap[KEY_DOWN_ARROW] = true;
		break;
	case GLUT_KEY_F1:
		state.horizontalCut = !state.horizontalCut;
		break;
	case GLUT_KEY_F2:
		state.drawCuts = !state.drawCuts;
		break;
	case GLUT_KEY_F3:
		state.drawStreamlines = !state.drawStreamlines;
		break;
	case GLUT_KEY_F4:
			streamlinesMove = !streamlinesMove;
		break;
	default:
		break;
		// printf("Unrecognized special key pressed\n");
	}
}

// Called when mouse is moving while no mouse buttons are pressed.
void passiveMouseMotionCallback(int mouseX, int mouseY)
{
	if (sceneObjects.camera->freeCameraMode)
	{
		if (mouseY != state.windowHeight / 2)
		{
			float cameraUpAngleDelta = 0.5f * (mouseY - state.windowHeight / 2);

			if (fabs(sceneObjects.camera->verticalAngle + cameraUpAngleDelta) < CAMERA_VERTICAL_MAX)
				sceneObjects.camera->verticalAngle += cameraUpAngleDelta;

			glutWarpPointer(state.windowWidth / 2, state.windowHeight / 2);

			glutPostRedisplay();
		}
		if (mouseX != state.windowWidth / 2)
		{
			float cameraSideAngleDelta = 0.5f * (mouseX - state.windowWidth / 2);


			sceneObjects.camera->horizontalAngle += cameraSideAngleDelta;
			if (sceneObjects.camera->horizontalAngle > 360)
			{
				sceneObjects.camera->horizontalAngle -= 360;
			}
			if (sceneObjects.camera->horizontalAngle < 0)
			{
				sceneObjects.camera->horizontalAngle += 360;
			}
		}
		sceneObjects.camera->direction = glm::vec3(cos(glm::radians(-sceneObjects.camera->horizontalAngle)), sin(glm::radians(-sceneObjects.camera->horizontalAngle)), 0);

		glm::vec3 norm = glm::cross(sceneObjects.camera->direction, glm::vec3(0, 0, 1));

		glm::mat4 cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(-sceneObjects.camera->verticalAngle), norm);

		sceneObjects.camera->direction = glm::vec3(cameraTransform * glm::vec4(sceneObjects.camera->direction, 0.0f));

		glutWarpPointer(state.windowWidth / 2, state.windowHeight / 2);

		glutPostRedisplay();
	}
	glutWarpPointer(state.windowWidth / 2, state.windowHeight / 2);
}


// Called whenever a key on the keyboard was released. The key is given by
// the "keyReleased" parameter, which is in ASCII. 
void keyboardUpCallback(unsigned char keyReleased, int mouseX, int mouseY)
{
	switch (keyReleased)
	{
	case 'w': // stop moving camera forward
		state.keyMap[KEY_W] = false;
		break;
	case 's': // stop moving camera backward
		state.keyMap[KEY_S] = false;
		break;
	case 'a': // stop moving camera to left
		state.keyMap[KEY_A] = false;
		break;
	case 'd': // stop moving camera to right
		state.keyMap[KEY_D] = false;
		break;
	default:
		break; // printf("Unrecognized key released\n");
	}
}


void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY)
{
	switch (keyPressed)
	{
	case 27: // escape
		glutLeaveMainLoop();
		break;
	case 'f': //turn on/off free camera mode
		sceneObjects.camera->freeCameraMode = !sceneObjects.camera->freeCameraMode;
		if (sceneObjects.camera->freeCameraMode) {
			glutPassiveMotionFunc(passiveMouseMotionCallback);
			glutWarpPointer(state.windowWidth / 2, state.windowHeight / 2);
		}
		else
		{
			sceneObjects.camera->lastPosition = sceneObjects.camera->position;
			sceneObjects.camera->lastDirection = sceneObjects.camera->direction;
			glutPassiveMotionFunc(NULL);
		}
		break;
	case '1': // first camera
		if (sceneObjects.camera->freeCameraMode)
		{
			sceneObjects.camera->freeCameraMode = false;
			sceneObjects.camera->lastPosition = sceneObjects.camera->position;
			sceneObjects.camera->lastDirection = sceneObjects.camera->direction;
			glutPassiveMotionFunc(NULL);
		}
		sceneObjects.camera->secondCamera = false;
		break;
	case '2': // second camera
		if (sceneObjects.camera->freeCameraMode)
		{
			sceneObjects.camera->freeCameraMode = false;
			sceneObjects.camera->lastPosition = sceneObjects.camera->position;
			sceneObjects.camera->lastDirection = sceneObjects.camera->direction;
			glutPassiveMotionFunc(NULL);
		}
		sceneObjects.camera->secondCamera = true;
		break;
	case '8': // move camera (debug)
		moveX += 0.01f;
		break;
	case '5': // move camera (debug)
		moveX -= 0.01f;
		break;
	case '4': // move camera (debug)
		moveY += 0.01f;
		break;
	case '6': // move camera (debug)
		moveY -= 0.01f;
		break;
	case '7': // move camera (debug)
		moveZ += 0.01f;
		break;
	case '9': // move camera (debug)
		moveZ -= 0.01f;
		break;
	case 'w': // move camera forward
		state.keyMap[KEY_W] = true;
		break;
	case 's': // move camera backward
		state.keyMap[KEY_S] = true;
		break;
	case 'a': // move camera to left
		state.keyMap[KEY_A] = true;
		break;
	case 'd': // move camera to right
		state.keyMap[KEY_D] = true;
		break;
	default:
		break; // printf("Unrecognized key pressed\n");
	}
}

// Called to update the display. You should call glutSwapBuffers after all of your
// rendering to display what you rendered.
void displayCallback() {
	GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

	glClear(mask);

	drawWindowContents();

	glutSwapBuffers();
}

void updateObjects(float elapsedTime) {

	float timeDelta = elapsedTime - state.elapsedTime;
	state.elapsedTime = elapsedTime;


	//update camera
	if (sceneObjects.camera->freeCameraMode) {
		if (state.keyMap[KEY_W]) {
			sceneObjects.camera->position = sceneObjects.camera->position + timeDelta * 2.85f * sceneObjects.camera->direction;
		}
		if (state.keyMap[KEY_S]) {
			sceneObjects.camera->position = sceneObjects.camera->position - timeDelta * 1.85f * sceneObjects.camera->direction;
		}
		if (state.keyMap[KEY_A]) {
			sceneObjects.camera->position = sceneObjects.camera->position - timeDelta * 1.85f * glm::cross(sceneObjects.camera->direction, glm::vec3(0, 0, 1));
		}
		if (state.keyMap[KEY_D]) {
			sceneObjects.camera->position = sceneObjects.camera->position + timeDelta * 1.85f * glm::cross(sceneObjects.camera->direction, glm::vec3(0, 0, 1));
		}
	}
}

// Callback responsible for the scene update
void timerCallback(int) {

	// update scene time
	float elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

	// update objects in the scene
	updateObjects(elapsedTime);

	// set timeCallback next invocation
	glutTimerFunc(33, timerCallback, 0);

	glutPostRedisplay();
}

void restartScene()
{
	state.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

	// initialize
	if (sceneObjects.camera == NULL)
		sceneObjects.camera = new Camera;
	if (sceneObjects.ground == NULL)
		sceneObjects.ground = new Object;
	if (sceneObjects.tempCuts == NULL)
		sceneObjects.tempCuts = new Object;
	if (sceneObjects.tempHorCut == NULL)
		sceneObjects.tempHorCut = new Object;

	if (sceneObjects.streamLine == NULL)
		sceneObjects.streamLine = new Object;
	// set up camera
	setCam1();

	if (sceneObjects.camera->freeCameraMode) {
		glutPassiveMotionFunc(NULL);
	}

	sceneObjects.camera->verticalAngle = 0.0f;
	sceneObjects.camera->horizontalAngle = 0.0f;
	sceneObjects.camera->freeCameraMode = false;
	sceneObjects.camera->secondCamera = false;

	// set up ground
	sceneObjects.ground->position = glm::vec3(0.0f, 0.0f, 0.0f);
	sceneObjects.ground->size = GROUND_SIZE;

	// set up temperature cuts
	sceneObjects.tempCuts->position = glm::vec3(0.0f, 0.0f, 0.0f);
	sceneObjects.tempCuts->size = TEMP_CUTS_SIZE;

	sceneObjects.tempHorCut->position = glm::vec3(0.0f, 0.0f, 0.0f);
	sceneObjects.tempHorCut->size = TEMP_CUTS_SIZE;

	// set up streamlines
	sceneObjects.streamLine->position = glm::vec3(0.0f, 0.0f, 0.0f);
	sceneObjects.streamLine->size = 1;

	// reset key map
	for (int i = 0; i < KEYS_COUNT; i++)
		state.keyMap[i] = false;

	state.horizontalCut = true;
	state.drawCuts = true;
	state.drawStreamlines = true;
}

// Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void initializeApplication()
{
	// initialize random seed
	srand((unsigned int)time(NULL));

	// initialize OpenGL
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// initialize shaders
	renderStaff = new Renderer();

	restartScene();
}

void finalizeApplication(void)
{
	if (sceneObjects.camera != NULL)
		delete sceneObjects.camera;
	if (sceneObjects.ground != NULL)
		delete sceneObjects.ground;
	if (sceneObjects.tempCuts != NULL)
		delete sceneObjects.tempCuts;
	if (sceneObjects.tempHorCut != NULL)
		delete sceneObjects.tempHorCut;
	if (sceneObjects.streamLine != NULL)
		delete sceneObjects.streamLine;

	delete renderStaff;
}

int main(int argc, char** argv)
{
	// initialize windowing system
	glutInit(&argc, argv);

	glutInitContextVersion(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);

	// initial window size
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow(WINDOW_TITLE);

	glutDisplayFunc(displayCallback);
	glutReshapeFunc(reshapeCallback);
	glutKeyboardFunc(keyboardCallback);
	glutKeyboardUpFunc(keyboardUpCallback);
	glutSpecialFunc(specialKeyboardCallback);     // key pressed
	glutSpecialUpFunc(specialKeyboardUpCallback); // key released

	glutTimerFunc(33, timerCallback, 0);

	// initialize PGR framework (GL, DevIl, etc.)
	if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");

	initializeApplication();

	glutCloseFunc(finalizeApplication);

	glutMainLoop();

	return 0;
}