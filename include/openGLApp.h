#ifndef __OPENGLAPP_H__
#define __OPENGLAPP_H__

#include "GL/freeglut.h"
#include "GL/glui.h"
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

#include "obstacleRemoval.h"
#include "camera.h"

class OpenGLApp {
public:
	int mFlgShowGrid;
	int mFFDisplayType;
	int mAgentVisualizationType;
	float mExecutionSpeed;

	OpenGLApp();
	void initOpenGL( int argc, char *argv[] );
	void runOpenGL();

	static void display();
	static void idle();
	static void reshape( int width, int height );
	static void mouse( int button, int state, int x, int y );
	static void motion( int x, int y );
	static void passiveMotion( int x, int y );
	static void keyboardCallback( unsigned char key, int x, int y );

private:
	static OpenGLApp *mOpenGLApp;
	bool mFlgRunApp;
	bool mFlgEditAgents;
	bool mFlgEditExits;
	bool mFlgEditMovableObstacles;
	bool mFlgEditImmovableObstacles;
	bool mFlgDragCamera;
	int mMainWindowId;
	int mFrameStartTime;
	int mTimer;

	ObstacleRemovalModel mModel;
	Camera mCamera;

	/*
	 * The definitions are in openGLApp_gui.cpp.
	 */
	void createGUI();
	static void gluiCallback( int id );
};

/*
 * Global callbacks for OpenGL.
 */
void displayCallback();
void idleCallback();
void reshapeCallback( int width, int height );
void mouseCallback( int button, int state, int x, int y );
void motionCallback( int x, int y );
void passiveMotionCallback( int x, int y );
void keyboardCallback( unsigned char key, int x, int y );

#endif