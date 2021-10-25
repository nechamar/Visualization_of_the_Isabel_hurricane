#include "Loader.h"
#include <stdio.h>
#include <iostream>
#include <limits>

std::vector<std::pair<std::string, std::vector<float>>> Loader::loadedData = std::vector<std::pair<std::string, std::vector<float>>>(10, {"", std::vector<float>()});

static bool is_big_endian(void)
{
	union {
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };

	return bint.c[0] == 1;
}

static float ReverseFloat(const float inFloat)
{
	float retVal;
	char* floatToConvert = (char*)&inFloat;
	char* returnFloat = (char*)&retVal;

	// swap the bytes into a temporary buffer
	returnFloat[0] = floatToConvert[3];
	returnFloat[1] = floatToConvert[2];
	returnFloat[2] = floatToConvert[1];
	returnFloat[3] = floatToConvert[0];

	return retVal;
}

void Loader::LoadGround(std::string filePath, std::vector<float>* vertices, std::vector<size_t>* indices)
{
	std::vector<float>* dataP;

	LoadData(filePath, dataP, XDIM * YDIM);
	std::vector<float> data = *dataP;

	(*vertices) = std::vector<float>(data.size() * 7);

	float max = std::numeric_limits<float>::min();

	for (size_t i = 0; i < data.size(); i++)
		if (max < data[i])
			max = data[i];

	for (size_t i = 0; i < data.size(); i++)
	{
		//pos X
		(*vertices)[i * 7] = static_cast<float>(i % XDIM);
		//pos Y
		(*vertices)[i * 7 + 1] = static_cast<float>(i / XDIM);
		//pos Z
		(*vertices)[i * 7 + 2] = data[i] / 150;
		//color R
		(*vertices)[i * 7 + 3] = data[i] > 0.0f ? 0.75f + (data[i] / max) * 0.25f : 0.0f;
		//color G
		(*vertices)[i * 7 + 4] = data[i] > 0.0f ? 0.9f : 0.0f;
		//color B
		(*vertices)[i * 7 + 5] = data[i] > 0.0f ? 0.1f : 1.0f;
		//color A
		(*vertices)[i * 7 + 6] = 1.0f;
	}

	//set indices for triangles
	(*indices) = std::vector<size_t>((XDIM - 1) * (YDIM - 1) * 6);

	for (size_t i = 1; i < YDIM; i++)
	{
		for (size_t j = 1; j < XDIM; j++)
		{
			size_t index = ((i - 1) * (XDIM - 1) + (j - 1)) * 6;

			(*indices)[index] = (i - 1) * XDIM + (j - 1);
			(*indices)[index + 1] = i * XDIM + j;
			(*indices)[index + 2] = (i - 1) * XDIM + j;

			(*indices)[index + 3] = (i - 1) * XDIM + (j - 1);
			(*indices)[index + 4] = i * XDIM + (j - 1);
			(*indices)[index + 5] = i * XDIM + j;
		}
	}
}

void Loader::LoadTempHorizontalCut(std::string filePath, std::vector<float>* vertices, std::vector<size_t>* indices)
{
	std::vector<float>* dataP;

	LoadData(filePath, dataP, XDIM * YDIM * ZDIM);
	std::vector<float> data = *dataP;


	(*vertices) = std::vector<float>(XDIM * YDIM * 7);

	size_t offsetZ = 30;

	for (size_t i = 0, size = XDIM * YDIM; i < size; i++)
	{
		//				   x				y					z
		size_t index = (i % XDIM) + (i / XDIM) * XDIM + offsetZ * XDIM * YDIM;
		// pos X
		(*vertices)[i * 7] = static_cast<float>(i % XDIM);
		// pos Y
		(*vertices)[i * 7 + 1] = static_cast<float>(i / XDIM);
		// pos Z
		(*vertices)[i * 7 + 2] = static_cast<float>(offsetZ);

		glm::vec4 color = tempToColor(data[index]);
		//color R
		(*vertices)[i * 7 + 3] = color.x;
		//color G
		(*vertices)[i * 7 + 4] = color.y;
		//color B
		(*vertices)[i * 7 + 5] = color.z;
		//color A
		(*vertices)[i * 7 + 6] = color.w;
	}

	//set indices for triangles
	(*indices) = std::vector<size_t>((XDIM - 1) * (YDIM - 1) * 6);

	for (size_t i = 1; i < YDIM; i++)
	{
		for (size_t j = 1; j < XDIM; j++)
		{
			size_t index = ((i - 1) * (XDIM - 1) + (j - 1)) * 6;

			(*indices)[index] = (i - 1) * XDIM + (j - 1);
			(*indices)[index + 1] = i * XDIM + j;
			(*indices)[index + 2] = (i - 1) * XDIM + j;

			(*indices)[index + 3] = (i - 1) * XDIM + (j - 1);
			(*indices)[index + 4] = i * XDIM + (j - 1);
			(*indices)[index + 5] = i * XDIM + j;
		}
	}
}

void Loader::LoadWindData(std::string filePathX, std::string filePathY, std::string filePathZ, std::vector<float>*& x, std::vector<float>*& y, std::vector<float>*& z)
{

	LoadData(filePathX, x, XDIM * YDIM * ZDIM);
	LoadData(filePathY, y, XDIM * YDIM * ZDIM);
	LoadData(filePathZ, z, XDIM * YDIM * ZDIM);
}

void Loader::LoadTempCuts(std::string filePath, std::vector<float>* vertices, std::vector<size_t>* indices)
{
	std::vector<float>* dataP;

	LoadData(filePath, dataP, XDIM * YDIM * ZDIM);
	std::vector<float> data = *dataP;

	(*vertices) = std::vector<float>(XDIM * ZDIM * 7 * 2);

	/*X cut*/
	size_t offsetY = 275;
	for (size_t i = 0, size = XDIM * ZDIM; i < size; i++)
	{
		//				   x				y					z
		size_t index = (i % XDIM) + offsetY * XDIM + (i / XDIM) * XDIM * YDIM;
		//pos X
		(*vertices)[i * 7] = static_cast<float>(i % XDIM);
		//pos Y
		(*vertices)[i * 7 + 1] = static_cast<float>(offsetY);
		//pos Z
		(*vertices)[i * 7 + 2] = static_cast<float>(i / XDIM);

		glm::vec4 color = tempToColor(data[index]);
		//color R
		(*vertices)[i * 7 + 3] = color.x;
		//color G
		(*vertices)[i * 7 + 4] = color.y;
		//color B
		(*vertices)[i * 7 + 5] = color.z;
		//color A
		(*vertices)[i * 7 + 6] = color.w;
	}

	/*Y cut*/
	size_t offsetX = 340;
	for (size_t i = XDIM * ZDIM, size = XDIM * ZDIM * 2; i < size; i++)
	{
		//				   x				y							z
		size_t index = offsetX + (i % YDIM) * XDIM + ((i - XDIM * ZDIM) / YDIM) * XDIM * YDIM;
		//pos X
		(*vertices)[i * 7] = static_cast<float>(offsetX);
		//pos Y
		(*vertices)[i * 7 + 1] = static_cast<float>(i % YDIM);
		//pos Z
		(*vertices)[i * 7 + 2] = static_cast<float>((i - XDIM * ZDIM) / YDIM);

		glm::vec4 color = tempToColor(data[index]);
		//color R
		(*vertices)[i * 7 + 3] = color.x;//data[index] > TEMP_MAX ? 1.0f : (data[index] - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);
		//color G
		(*vertices)[i * 7 + 4] = color.y;//data[index] > TEMP_MAX ? 0.0f : (data[index] - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);
		//color B
		(*vertices)[i * 7 + 5] = color.z;//data[index] > TEMP_MAX ? 0.0f : (data[index] - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);
		//color A
		(*vertices)[i * 7 + 6] = color.w;
	}

	//set indices for triangles
	(*indices) = std::vector<size_t>((XDIM - 1) * (ZDIM - 1) * 6 * 2);

	/*X cut*/
	for (size_t i = 1; i < ZDIM; i++)
	{
		for (size_t j = 1; j < XDIM; j++)
		{
			size_t index = ((i - 1) * (XDIM - 1) + (j - 1)) * 6;

			(*indices)[index] = (i - 1) * XDIM + (j - 1);
			(*indices)[index + 1] = i * XDIM + j;
			(*indices)[index + 2] = (i - 1) * XDIM + j;

			(*indices)[index + 3] = (i - 1) * XDIM + (j - 1);
			(*indices)[index + 4] = i * XDIM + (j - 1);
			(*indices)[index + 5] = i * XDIM + j;
		}
	}

	/*Y cut*/
	for (size_t i = 1, offset = XDIM * ZDIM; i < ZDIM; i++)
	{
		for (size_t j = 1; j < XDIM; j++)
		{
			size_t index = ((i - 1) * (XDIM - 1) + (j - 1)) * 6 + (XDIM - 1) * (ZDIM - 1) * 6;

			(*indices)[index] = offset + (i - 1) * XDIM + (j - 1);
			(*indices)[index + 1] = offset + i * XDIM + j;
			(*indices)[index + 2] = offset + (i - 1) * XDIM + j;

			(*indices)[index + 3] = offset + (i - 1) * XDIM + (j - 1);
			(*indices)[index + 4] = offset + i * XDIM + (j - 1);
			(*indices)[index + 5] = offset + i * XDIM + j;
		}
	}
}

void Loader::LoadData(std::string filePath, std::vector<float>*& data, size_t size)
{
	int lastIndex = 0;
	for (size_t i = 0; i < loadedData.size(); i++)
	{
		if (loadedData[i].first == "") {
			lastIndex = i;
			break;
		}

		if (filePath == loadedData[i].first) {
			data = &loadedData[i].second;
			return;
		}
	}

	FILE* fp;


	std::vector<float> newData = std::vector<float>(size);

	fopen_s(&fp, filePath.c_str(), "r+");

	fseek(fp, 0, SEEK_SET);

	fread((float*)&(newData).front(), (newData).size() * sizeof(float), 1, fp);

	fclose(fp);

	if (!is_big_endian())
	{
		for (size_t i = 0; i < (newData).size(); i++)
		{
			(newData)[i] = ReverseFloat((newData)[i]);
		}
	}

	loadedData[lastIndex] = { filePath, newData };
	data = &loadedData[lastIndex].second;
}
