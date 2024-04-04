/*
//experimental drawing stuff
#pragma once

#include <blend2d.h>
#ifdef WIN32
#include <windows.h>
#endif

class DrawRenderBufferPool
{
public:
	DrawRenderBufferPool();
	~DrawRenderBufferPool();

	void DrawImage(BLImage* image);
	BLImage* GrabImage();
	void ReleaseImage(BLImage* image);

	void OnScreenResized(int newWidth, int newHeight);
private:
	int GetImageIndex(BLImage* image);

	BLImage** images = nullptr;
	uint32_t imageCount;
	uint32_t usedImagesMask;
};

// functions for interacting with blend2d
class Draw2D
{
public:
	static DrawRenderBufferPool pool;
};*/