
#ifndef _included_openglinterface_h
#define _included_openglinterface_h

#include <list>

#include "eclipse.h"
#include "gucci.h"

/*

	Generic OpenGL functions for rendering the interface
	(defined by Eclipse)

  */

void clear_draw(int x, int y, int w, int h);

void button_draw(Button* button, bool highlighted, bool clicked);
void button_highlight(Button* button);
void button_click(Button* button);

void imagebutton_draw(Button* button, bool highlighted, bool clicked);
void imagebutton_draw(Button* button,
					  bool highlighted,
					  bool clicked,
					  Image* standard_i_ref,
					  Image* highlighted_i_ref,
					  Image* clicked_i_ref);
void imagebutton_drawtextured(Button* button, bool highlighted, bool clicked);
void imagebutton_draw_blend(Button* button, bool highlighted, bool clicked);

void border_draw(Button* button);
void textbutton_draw(Button* button, bool highlighted, bool clicked);
void textbutton_keypress(Button* button, char key);

std::list<std::string_view> wordwraptext(const std::string& string, int linesize);

void text_draw(int x, int y, const std::string string, int linesize);
void text_draw(Button* button, bool highlighted, bool clicked);
void textbox_draw(Button* button, bool highlighted, bool clicked);

void buttonborder_draw(Button* button, bool highlighted, bool clicked);

void superhighlight_draw(Button* button, bool highlighted, bool clicked);

void button_assignbitmap(std::string name, std::string standard_f);
void button_assignbitmap(std::string name, Image* standard_i_ref);
void button_assignbitmaps(std::string name,
						  std::string standard_f,
						  std::string highlighted_f,
						  std::string clicked_f);
void button_assignbitmaps(std::string name,
						  Image* standard_i_ref,
						  Image* highlighted_i_ref,
						  Image* clicked_i_ref);

void button_assignbitmap_blend(std::string name, std::string standard_f);
void button_assignbitmaps_blend(std::string name,
								std::string standard_f,
								std::string highlighted_f,
								std::string clicked_f);

Image* get_assignbitmap(std::string filename);

void tooltip_update(std::string newtooltip);

Button* getcurrentbutton(); // Returns button under mouse or NULL

//
// This set of functions can create and manage a scrollable text box
//

void create_stextbox(int x, int y, int width, int height, std::string caption, std::string name);
void remove_stextbox(std::string name);

void draw_stextbox(Button* button, bool highlighted, bool clicked);
void draw_scrollbox(Button* button, bool highlighted, bool clicked);
void stextbox_scroll(const char* name, int newValue);

//
// Functions for modal message boxes
//

void create_msgbox(std::string title, std::string message, void (*closeclick)(Button*) = NULL);
void create_yesnomsgbox(std::string title,
						std::string message,
						void (*yesclick)(Button*) = NULL,
						void (*noclick)(Button*) = NULL);
void remove_msgbox();
bool isvisible_msgbox();

//
// Functions to handle screen resize / button repositioning

void SetWindowScaleFactor(float scaleX, float scaleY);
int GetScaledXPosition(int xPos);
int GetScaledYPosition(int yPos);

#define SX(x) GetScaledXPosition(x)
#define SY(y) GetScaledYPosition(y)

#endif
