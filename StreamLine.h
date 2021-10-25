#pragma once
#include "pgr.h"
#include "Loader.h"

#define ERROR_CODE 1000000000

// defines grid, where streamlines start
#define X_CELLS 40
#define Y_CELLS 40
#define Z_CELLS 5

#define X_DIFF XDIM / X_CELLS
#define Y_DIFF YDIM / Y_CELLS
#define Z_DIFF ZDIM / (Z_CELLS + 1)

// controls multiplier of speed in z-dim.
#define Z_MULTIPLIER 1.0f

#define NUMBER_OF_STREAMLINES 2*( X_CELLS + Y_CELLS) * (Z_CELLS-2) + X_CELLS*Y_CELLS

extern bool streamlinesMove;

#define METERS_PER_CELL 200

constexpr float CELLS_PER_METER = 1.0f / METERS_PER_CELL;

// multiplier for the RK4 algorithm 
#define H 30.0f

#define NUMBER_OF_NODES_IN_STREAMLINE 600

// defines colormap for temperature data
// from https://www.kennethmoreland.com/color-maps/ColorMapsExpanded.pdf
#define NUMBER_OF_MAPPINGS 33
extern float scalar[NUMBER_OF_MAPPINGS];
extern unsigned char colorMap[NUMBER_OF_MAPPINGS * 3];

glm::vec4 tempToColor(float temp);

glm::vec3 linearInterpolationVec(const float& value, const glm::vec3& first, const glm::vec3& second);

float trilinearInterpolation(glm::vec3 coords, std::vector<float>& data);

glm::vec3 trilinearInterpolationVec(glm::vec3 coords, std::vector<float>& dataX, std::vector<float>& dataY, std::vector<float>& dataZ);

// returns new position, which is changed by the wind
// uses RK4 algorithm to simulate the motion
glm::vec3 RK4(glm::vec3 position, std::vector<float>& dataX, std::vector<float>& dataY, std::vector<float>& dataZ);

// finds nodes defining streamline from starting position and wind data
std::vector<glm::vec3> findStreamLineNodes(glm::vec3 startingPosition, std::vector<float>& dataX, std::vector<float>& dataY, std::vector<float>& dataZ);