#pragma once

struct UPoint {
public:
	float X;
	float Y;

	UPoint(float x, float y) :
		X(x),
		Y(y)
	{
	}
};

struct URect {
public:
	UPoint TopLeft;
	UPoint BottomRight;

	URect(UPoint tl, float width, float height) :
		TopLeft(tl),
		BottomRight(UPoint(tl.X + width, tl.Y + height))
	{
	}

	URect(UPoint tl, UPoint br) :
		TopLeft(tl),
		BottomRight(br)
	{
	}

	static const URect Identity;
};

struct UColor {
public:
	float R;
	float G;
	float B;
	float A;

	UColor(float r, float g, float b, float a = 1.0f) :
		R(r),
		G(g),
		B(b),
		A(a)
	{
	}
};