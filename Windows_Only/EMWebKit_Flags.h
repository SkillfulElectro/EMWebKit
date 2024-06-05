#pragma once
#include <string>
#include <stdio.h>

#ifndef FLAGS_CONTAINER
#define FLAGS_CONTAINER

class Flags
{
public:
	std::string url;
	std::string title_text;
	std::string className;
	std::string url_style;
	bool strictURL;
	int positionX;
	int positionY;
	bool center;
	bool full_screen;
	bool title;
	bool dynamic_title;
	int window_height;
	int window_width;
	static const wchar_t* ConvertCharToWchar(const char* c);
	Flags();
};

#endif // !flags
