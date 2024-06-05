#include "Flags.h"

const wchar_t* Flags::ConvertCharToWchar(const char* c) {
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];

	// Use mbstowcs_s for safer conversion
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wc, cSize, c, cSize - 1);

	return wc;
}

Flags::Flags() {
	this->url = "";
	this->center = true;
	this->dynamic_title = false;
	this->full_screen = false;
	this->window_height = 480;
	this->window_width = 800;
	this->title = true;
	this->title_text = "Mutexis Sdk";
	this->positionX = 0;
	this->positionY = 0;
	this->className = "myAppClassName";
	this->strictURL = false;
	this->url_style = "";
}
