#pragma once
#include <GL/glew.h>
GLuint LoadShader(const char* vertex_path, const char* fragment_path, const char* geometry_path = "\0");
GLuint LoadShaderFromData(const char* vertex_data, int vertex_data_size, const char* fragment_data, int fragment_data_size);