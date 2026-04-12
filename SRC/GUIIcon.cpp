#include "Image.h"
#include "GUIIcon.h"

GUIIcon::GUIIcon()
{
	SetImage(NULL);
}

GUIIcon::GUIIcon(Image* image)
{
	SetImage(image);
}

GUIIcon::~GUIIcon()
{
}

void GUIIcon::Draw()
{
	if (!mVisible) return;
	if (mImage == NULL) return;
	glAlphaFunc(GL_GEQUAL, 0.5);
	glEnable(GL_ALPHA_TEST);
	glDrawBuffer(GL_BACK);
	glRasterPos2i(mPosition.x + mBorder.x, mPosition.y + mBorder.y);
	glDrawPixels(mImage->GetWidth(), mImage->GetHeight(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, mImage->GetPixelData());
	glDisable(GL_ALPHA_TEST);
}

void GUIIcon::SetImage(Image* image)
{
	mImage = image;
}
