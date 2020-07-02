// COMP4490.cpp : Defines the entry point for the console application.

#define ASSIGNMENT "1A"
#define AUTHOR "Gareth"
#define WINDOW_NAME "COMP 4490 Assignment " ASSIGNMENT " by " AUTHOR

#define NOMINMAX
#ifdef _WIN32
#include "stdafx.h"
#endif /* _WIN32 */

#include <string.h>
#include <stdio.h>
#include <vector>
#include <regex>
#include <GL/glew.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>

#include "a1main.h"

class COMP4490Window;

std::vector<char *> inputfiles;
COMP4490Window *gl_window;

class COMP4490Window : public Fl_Gl_Window {
	void draw() {
		glEnable(GL_TEXTURE_RECTANGLE);

		if (resize_texture) {
			init_texture(w(), h());
			resize_texture = false;
			retrace = true;
		}

		if (retrace) {
			if (inputfiles.size() == 0) {
				trace(NULL, this, width, height);
			} else {
				for (auto &file: inputfiles) {
					trace(file, this, width, height);
				}
			}
			retrace = false;
			dirty = true;
		}
		
		if (dirty) {
			glBindTexture(GL_TEXTURE_RECTANGLE, tex);
			glTexSubImage2D(GL_TEXTURE_RECTANGLE, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
			dirty = false;
		}

		glLoadIdentity();
		glViewport(0, 0, w(), h());
		glOrtho(0, w(), 0, h(), -1, 1);

		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(1, 1, 1);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex2i(0, 0);
		glTexCoord2i(width, 0);
		glVertex2i(w()-1, 0);
		glTexCoord2i(width - 1, height - 1);
		glVertex2i(w()-1, h()-1);
		glTexCoord2i(0, height-1);
		glVertex2i(0, h()-1);
		glEnd();

		glDisable(GL_TEXTURE_RECTANGLE);

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) {
			fprintf(stderr, "OpenGL error %d\n", err);
		}
	}

	int handle(int event) {
		switch (event) {
		case FL_PUSH:
			pick(this, Fl::event_x(), h() - Fl::event_y() - 1);
			break;
		case FL_KEYUP:
			if (Fl::event_length() > 0 && (Fl::event_text()[0] == 'r' || Fl::event_text()[0] == 'R')) {
				retrace = true;
				redraw();
			}
			break;
		default:
			return Fl_Gl_Window::handle(event);
		}
		return 1;
	}

	void resize(int X, int Y, int W, int H) {
		resize_texture = true;
		printf("resize to (%d, %d)\n", W, H);
		Fl_Gl_Window::resize(X, Y, W, H);
	}

	void init_texture(GLuint new_width, GLuint new_height) {
		GLubyte *new_buffer = (GLubyte *)calloc(new_width * new_height, 3);
		if (NULL == new_buffer) {
			perror("Unable to initialize output texture.");
			exit(0);
		}

		if (NULL != buffer) {
			for (GLuint y = 0; y < std::min(height, new_height); y++)
				for (GLuint x = 0; x < std::min(width, new_width); x++) {
					new_buffer[(x + y * new_width) * 3] = buffer[(x + y * width) * 3];
					new_buffer[(x + y * new_width) * 3 + 1] = buffer[(x + y * width) * 3 + 1];
					new_buffer[(x + y * new_width) * 3 + 2] = buffer[(x + y * width) * 3 + 2];
				}
		}

		if (NULL == buffer) {
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glGenTextures(1, &tex);
		}
		glBindTexture(GL_TEXTURE_RECTANGLE, tex);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, new_width, new_height, 0, GL_RGB, GL_UNSIGNED_BYTE, new_buffer);

		if (NULL != buffer) {
			free(buffer);
		}
		width = new_width;
		height = new_height;
		buffer = new_buffer;
	}

	bool dirty = true;
	bool retrace = true;
	bool resize_texture = true;
	GLubyte *buffer = NULL;
	GLuint tex, width, height;

public:
	COMP4490Window(int X, int Y, int W, int H, const char*L = 0) : Fl_Gl_Window(X, Y, W, H, L) {
	}

	void set(int x, int y, GLubyte red, GLubyte green, GLubyte blue) {
		if (x < 0 || y < 0 || x >= (int)width || y >= (int)height) {
			fprintf(stderr, "Invalid set coordinate (%d, %d).\n", x, y);
		} else {
			buffer[(x + y * width) * 3] = red;
			buffer[(x + y * width) * 3 + 1] = green;
			buffer[(x + y * width) * 3 + 2] = blue;
		}
	}
	
	bool get(int x, int y, unsigned char &red, unsigned char &green, unsigned char &blue) {
		if (x < 0 || y < 0 || x >= (int)width || y >= (int)height) {
			fprintf(stderr, "Invalid get coordinate (%d, %d).\n", x, y);
			return false;
		} else {
			red = buffer[(x + y * width) * 3];
			green = buffer[(x + y * width) * 3 + 1];
			blue = buffer[(x + y * width) * 3 + 2];
		}
		return true;
	}
	
	void dirty_redraw() {
		dirty = true;
		redraw();
	}
};

void set(void *window, int x, int y, unsigned char red, unsigned char green, unsigned char blue) {
	((COMP4490Window *)window)->set(x, y, red, green, blue);
}

bool get(void *window, int x, int y, unsigned char &red, unsigned char &green, unsigned char &blue) {
	return ((COMP4490Window *)window)->get(x, y, red, green, blue);
}

void redraw(void *window) {
	((COMP4490Window *)window)->dirty_redraw();
}

int main(int argc, char *argv[])
{
	int w = 640, h = 480;
	std::regex wh("([0-9]+)[xX]([0-9]+)");

	for (int i = 1; i < argc; i++) {
		if (0 == strncmp(argv[i], "-h", 2)) {
			printf("Usage: %s [WIDTHxHEIGHT] [inputfile]...\n", argv[0]);
			exit(0);
		} else {
			std::string arg(argv[1]);
			std::smatch matches;
			if (std::regex_search(arg, matches, wh) && 3 == matches.size()) {
				w = std::stoi(matches[1]);
				h = std::stoi(matches[2]);
			} else {
				inputfiles.push_back(argv[i]);
			}
		}
	}

	Fl_Window win(w, h, WINDOW_NAME);
	gl_window = new COMP4490Window(0, 0, win.w(), win.h());
	win.end();
	win.resizable(gl_window);
	win.show();
	return(Fl::run());
}

