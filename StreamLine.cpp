#include "streamLine.h"

bool streamlinesMove = false;

float scalar[NUMBER_OF_MAPPINGS] = {
   0.0f,
   0.03125f,
   0.0625f,
   0.09375f,
   0.125f,
   0.15625f,
   0.1875f,
   0.21875f,
   0.25f,
   0.28125f,
   0.3125f,
   0.34375f,
   0.375f,
   0.40625f,
   0.4375f,
   0.46875f,
   0.5f,
   0.53125f,
   0.5625f,
   0.59375f,
   0.625f,
   0.65625f,
   0.6875f,
   0.71875f,
   0.75f,
   0.78125f,
   0.8125f,
   0.84375f,
   0.875f,
   0.90625f,
   0.9375f,
   0.96875f,
   1.0
};

unsigned char colorMap[NUMBER_OF_MAPPINGS * 3] = {
	59, 76, 192,
	68, 90, 204,
	77, 104, 215,
	87, 117, 225,
	98, 130, 234,
	108, 142, 241,
	119, 154, 247,
	130, 165, 251,
	141, 176, 254,
	152, 185, 255,
	163, 194, 255,
	174, 201, 253,
	184, 208, 249,
	194, 213, 244,
	204, 217, 238,
	213, 219, 230,
	221, 221, 221,
	229, 216, 209,
	236, 211, 197,
	241, 204, 185,
	245, 196, 173,
	247, 187, 160,
	247, 177, 148,
	247, 166, 135,
	244, 154, 123,
	241, 141, 111,
	236, 127, 99,
	229, 112, 88,
	222, 96, 77,
	213, 80, 66,
	203, 63, 56,
	192, 40, 47,
	180, 4, 38
};

glm::vec4 tempToColor(float temp)
{
	float tempRelative = temp > TEMP_MAX ? 1.0f : (temp - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);
	glm::vec3 color;
	if (tempRelative == 0.0f) {
		return glm::vec4(colorMap[0], colorMap[1], colorMap[2], 1.0f);
	}
	else {
		for (size_t i = 1; i < NUMBER_OF_MAPPINGS; i++)
		{
			if (scalar[i] >= tempRelative) {
				float first = scalar[i - 1];
				float second = scalar[i] - first;
				glm::vec3 firstVec(colorMap[3 * (i - 1)], colorMap[3 * (i - 1) + 1], colorMap[3 * (i - 1) + 2]);
				glm::vec3 secondVec(colorMap[3 * i], colorMap[3 * i + 1], colorMap[3 * i + 2]);
				float tmp = tempRelative - first;
				float k = tmp / second;
				color = linearInterpolationVec(k, firstVec, secondVec);
				break;
			}
		}
		return glm::vec4(color/255.0f, 1.0f);
	}
}

glm::vec3 linearInterpolationVec(const float& value, const glm::vec3& first, const glm::vec3& second)
{
	return second * value + first * (1 - value);
}



float trilinearInterpolation(glm::vec3 coords, std::vector<float>& data)
{
	float x = coords.x;
	float y = coords.y;
	float z = coords.z;

	int x0 = x;
	int y0 = y;
	int z0 = z;
	
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	int z1 = z0 + 1;

	if (x0 < 0 || y0 < 0 || z0 < 0 ||
		x1 >= XDIM || y1 >= YDIM || z1 >= ZDIM) {
		return ERROR_CODE;
	}


	
	float xd = (x - x0) / (x1 - x0);
	float yd = (y - y0) / (y1 - y0);
	float zd = (z - z0) / (z1 - z0);

	float c000 = data[x0 + XDIM * y0 + YDIM * XDIM * z0];
	float c111 = data[x1 + XDIM * y1 + YDIM * XDIM * z1];
	float c100 = data[x1 + XDIM * y0 + YDIM * XDIM * z0];
	float c010 = data[x0 + XDIM * y1 + YDIM * XDIM * z0];
	float c001 = data[x0 + XDIM * y0 + YDIM * XDIM * z1];
	float c110 = data[x1 + XDIM * y1 + YDIM * XDIM * z0];
	float c011 = data[x0 + XDIM * y1 + YDIM * XDIM * z1];
	float c101 = data[x1 + XDIM * y0 + YDIM * XDIM * z1];
	if (c000 == ERROR_CODE ||
		c111 == ERROR_CODE ||
		c100 == ERROR_CODE ||
		c010 == ERROR_CODE ||
		c001 == ERROR_CODE ||
		c110 == ERROR_CODE ||
		c011 == ERROR_CODE ||
		c101 == ERROR_CODE)
		return ERROR_CODE;

	float c00 = c000 * (1 - xd) + c100 * xd;
	float c01 = c001 * (1 - xd) + c101 * xd;
	float c10 = c010 * (1 - xd) + c110 * xd;
	float c11 = c011 * (1 - xd) + c111 * xd;

	float c0 = c00 * (1 - yd) + c10 * yd;
	float c1 = c01 * (1 - yd) + c11 * yd;

	float c = c0 * (1 - zd) + c1 * zd;

	return c;
}

glm::vec3 trilinearInterpolationVec(glm::vec3 coords, std::vector<float>& dataX, std::vector<float>& dataY, std::vector<float>& dataZ)
{
	float x = trilinearInterpolation(coords, dataX);
	float y = trilinearInterpolation(coords, dataY);
	float z = Z_MULTIPLIER * trilinearInterpolation(coords, dataZ);

	return glm::vec3(x, y, z);
}

glm::vec3 RK4(glm::vec3 position, std::vector<float>& dataX, std::vector<float>& dataY, std::vector<float>& dataZ)
{
	glm::vec3 k1 = trilinearInterpolationVec(position, dataX, dataY, dataZ);
	if (k1.x == ERROR_CODE || k1.y == ERROR_CODE || k1.z == ERROR_CODE)
		return glm::vec3(-1, -1, -1);
	glm::vec3 k2 = trilinearInterpolationVec(position + H * (k1 * 0.5f) * CELLS_PER_METER, dataX, dataY, dataZ);
	if (k2.x == ERROR_CODE || k2.y == ERROR_CODE || k2.z == ERROR_CODE)
		return glm::vec3(-1, -1, -1);
	glm::vec3 k3 = trilinearInterpolationVec(position + H * (k2 * 0.5f) * CELLS_PER_METER, dataX, dataY, dataZ);
	if (k3.x == ERROR_CODE || k3.y == ERROR_CODE || k3.z == ERROR_CODE)
		return glm::vec3(-1, -1, -1);
	glm::vec3 k4 = trilinearInterpolationVec(position + H * (k3) * CELLS_PER_METER, dataX, dataY, dataZ);
	if (k4.x == ERROR_CODE || k4.y == ERROR_CODE || k4.z == ERROR_CODE)
		return glm::vec3(-1, -1, -1);
	glm::vec3 unscaledChange  = (k1 + 2.0f * k2 + 2.0f * k3 + k4);
	glm::vec3 change = CELLS_PER_METER * (1.0f / 6) * H * unscaledChange;
	return position + change;
}

std::vector<glm::vec3> findStreamLineNodes(glm::vec3 startingPosition, std::vector<float>& dataX, std::vector<float>& dataY, std::vector<float>& dataZ)
{
	std::vector<glm::vec3> nodePositions;
	nodePositions.push_back(startingPosition);
	for (size_t i = 1; i < NUMBER_OF_NODES_IN_STREAMLINE; i++)
	{
		nodePositions.push_back(RK4(nodePositions[i - 1], dataX, dataY, dataZ));
		if (nodePositions[i].x > XDIM || nodePositions[i].y > YDIM || nodePositions[i].z > ZDIM || 
			nodePositions[i].x < 0 || nodePositions[i].y < 0 || nodePositions[i].z < 0) {
			nodePositions.pop_back();
			break;
		}
	}
	return nodePositions;

}

