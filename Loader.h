#ifndef __LOADER_H_
#define __LOADER_H_

#define XDIM 500
#define YDIM 500
#define ZDIM 100

#define TEMP_MIN -83.00402f
#define TEMP_MAX 31.51576f

#include <string>
#include <vector>
#include "pgr.h"
#include "streamLine.h"

class Loader
{
public:
	static void LoadGround(std::string filePath, std::vector<float>* vertices, std::vector<size_t>* indices);
	static void LoadTempCuts(std::string filePath, std::vector<float>* vertices, std::vector<size_t>* indices);
	static void LoadTempHorizontalCut(std::string filePath, std::vector<float>* vertices, std::vector<size_t>* indices);
	static void LoadWindData(std::string filePathX, std::string filePathY, std::string filePathZ, std::vector<float>*& x, std::vector<float>*& y, std::vector<float>*& z);
	static void LoadData(std::string filePath, std::vector<float>*& data, size_t size);
private:
	static std::vector<std::pair<std::string, std::vector<float>>> loadedData;
};

#endif // !__LOADER_H_

