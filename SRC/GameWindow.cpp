#include "GameUtil.h"
#include "GameWorld.h"
#include "IKeyboardListener.h"
#include "GameDisplay.h"
#include "GameWindow.h"

const int GameWindow::ZOOM_LEVEL = 3;

GameWindow::GameWindow(int w, int h, int x, int y, char *t)
	: GlutWindow(w, h, x, y, t),
	  mWorld(NULL),
	  mDisplay(NULL)
{
}

GameWindow::~GameWindow()
{
}

void GameWindow::OnDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (mWorld) { mWorld->Render(); }
	if (mDisplay) { mDisplay->Render(); }
	glutSwapBuffers();
}

void GameWindow::OnIdle(void)
{
	GlutWindow::OnIdle();
	static int lasttime;
	int dt=glutGet(GLUT_ELAPSED_TIME)-lasttime;
	lasttime=glutGet(GLUT_ELAPSED_TIME);
	if (mWorld) { mWorld->Update(dt); }
	if (mDisplay) { mDisplay->Update(dt); }
	glutPostRedisplay();
}

void  GameWindow::OnWindowReshaped(int w, int h)
{
	GlutWindow::OnWindowReshaped(w, h);
	glViewport(0, 0, w, h);
	UpdateWorldSize();
	UpdateDisplaySize();
}
   
void GameWindow::SetWorld(GameWorld* w) { mWorld = w; UpdateWorldSize(); }
GameWorld* GameWindow::GetWorld() { return mWorld; }

void GameWindow::SetDisplay(GameDisplay* d) { mDisplay = d; UpdateDisplaySize(); }
GameDisplay* GameWindow::GetDisplay() { return mDisplay; }

void GameWindow::UpdateWorldSize()
{
	if (mWorld) {
		mWorld->SetWidth(glutGet(GLUT_WINDOW_WIDTH)/ZOOM_LEVEL);
		mWorld->SetHeight(glutGet(GLUT_WINDOW_HEIGHT)/ZOOM_LEVEL);
	}
}

void GameWindow::UpdateDisplaySize()
{
	if (mDisplay) {
		mDisplay->Reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	}
}