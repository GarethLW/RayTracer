#include "../COMP4490/Color.h"
#include "../COMP4490/SceneObject.h"

// implement
void trace(char *input_file, void *window, int width, int height);
void pick(void *window, int x, int y);
int buildScene(char * input_file);
Color calcLighting(SceneObject * obj, Ray ray, float t);

// use
void set(void *window, int x, int y, unsigned char red, unsigned char green, unsigned char blue);
bool get(void *window, int x, int y, unsigned char &red, unsigned char &green, unsigned char &blue);
void redraw(void *window);
