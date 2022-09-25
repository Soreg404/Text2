#pragma once

#include <glew/glew.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Windows.h>
#include <thread>
#include <iostream>
#include <fstream>

#include <vector>
#include <unordered_map>
#include <map>


#define LOG(x, ...) printf("[" __FILE__ "] %i: " x "\n", __LINE__, __VA_ARGS__);

std::vector<char> ldFile(const wchar_t *path);