/*#include <gl/glew.h>
#include <GL/gl.h>

#include "draw.h"
#include "gucci.h"


DrawRenderBufferPool Draw2D::pool;

DrawRenderBufferPool::DrawRenderBufferPool()
{
	imageCount = 32;
	images = new BLImage*[imageCount];
	for (int i = 0; i < imageCount; i++)
	{
		images[i] = nullptr;
	}

	usedImagesMask = 0;
}

DrawRenderBufferPool::~DrawRenderBufferPool()
{
	if (images != nullptr)
	{
		for (int i = 0; i < imageCount; i++)
		{
			if (images[i] != nullptr)
			{
				delete images[i];
				images[i] = nullptr;
			}
		}

		images = nullptr;
		usedImagesMask = 0;
	}
}

void DrawRenderBufferPool::ReleaseImage(BLImage* image)
{
	int idx = GetImageIndex(image);
	if (idx == -1)
	{
		throw new std::exception("returned image but can't find in pool");
	}

	usedImagesMask &= (0xffffffff ^ (0x01 << idx));
}

int DrawRenderBufferPool::GetImageIndex(BLImage* image)
{
	for (int i = 0; i < imageCount; i++)
	{
		if (images[i] == image)
		{
			return i;
		}
	}

	return -1;
}

BLImage* DrawRenderBufferPool::GrabImage()
{
	URect ScreenRect = GciGetScreenRect();

	for (int i = 0; i < imageCount; i++)
	{
		int test = (0x01 << i);
		if ((usedImagesMask & test) == 0)
		{
			usedImagesMask &= test;
			if (images[i] != nullptr)
			{
				if (
					images[i]->width() == ScreenRect.BottomRight.X &&
					images[i]->height() == ScreenRect.BottomRight.Y)
				{
					return images[i];
				}

				delete images[i];
			}

			images[i] = new BLImage(
				ScreenRect.BottomRight.X, 
				ScreenRect.BottomRight.Y, 
				BL_FORMAT_PRGB32);

			return images[i];
		}
	}

	throw new std::exception("can't find any unused image!");
	return nullptr;
}

void DrawRenderBufferPool::DrawImage(BLImage* image)
{
	int idx = GetImageIndex(image);
	BLImageData data;
	image->getData(&data);

	assert(data.format == BL_FORMAT_PRGB32);

	// use reversed pixel format to load ARGB32

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glRasterPos2i(0, 0);
	glDrawPixels(image->width(), image->height(), GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, data.pixelData);

	glPopAttrib();
	glDisable(GL_BLEND);
}

void DrawRenderBufferPool::OnScreenResized(int newWidth, int newHeight)
{
	for (int i = 0; i < imageCount; i++)
	{
		int test = (0x01 << i);
		if ((usedImagesMask & test) == test)
		{
			BLImage* oldImage = images[i];
			images[i] = new BLImage(newWidth, newWidth, BL_FORMAT_PRGB32);
			delete oldImage;
		}
	}
}*/