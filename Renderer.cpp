#include <iostream>
#include "Data.h"
#include "Loader.h"
#include "Renderer.h"

const char* GROUND_VERTEX_SHADER = "Ground.vert";
const char* GROUND_FRAGMENT_SHADER = "Ground.frag";

const char* GROUND_FILE_PATH = "..\\data\\HGTdata.bin";
const char* TEMPERATURE_FILE_PATH = "..\\data\\TCf10.bin";
const char* WIND_X_FILE_PATH = "..\\data\\Uf10.bin";
const char* WIND_Y_FILE_PATH = "..\\data\\Vf10.bin";
const char* WIND_Z_FILE_PATH = "..\\data\\Wf10.bin";

Renderer::Renderer()
{
	groundGeometry = NULL;
	tempCutsGeometry = NULL;
	tempHorizontalCutGeometry = NULL;
	

	initializeShaderPrograms();
	initializeModels();
}

Renderer::~Renderer()
{
	cleanupModels();
	cleanupShaderPrograms();
}

void Renderer::setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(shaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVM));

	glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
}

void Renderer::drawGround(Object* ground, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), ground->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(ground->size));

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glBindVertexArray(groundGeometry->vao);
	glDrawElements(GL_TRIANGLES, groundGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

void Renderer::drawTempCuts(Object* tempCuts, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), tempCuts->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(tempCuts->size));

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glBindVertexArray(tempCutsGeometry->vao);
	glDrawElements(GL_TRIANGLES, tempCutsGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

void Renderer::drawTempHorizontalCut(Object* tempCuts, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), tempCuts->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(tempCuts->size));

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glBindVertexArray(tempHorizontalCutGeometry->vao);
	glDrawElements(GL_TRIANGLES, tempHorizontalCutGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
}

void Renderer::drawStreamlines(Object* streamline, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	glUseProgram(shaderProgram.program);
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), streamline->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(streamline->size));

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	static float tmp = 0.0f;
	const float change = 0.0025f;
	for (size_t i = 0; i < NUMBER_OF_STREAMLINES; i++)
	{
		while (tmp > 1) tmp--;


		glBindVertexArray(streamlinesGeometry[i]->vao);
		float temp = streamlinesMove ? tmp : 1;
		glDrawArrays(GL_LINE_STRIP, 0, int(temp * (streamlinesGeometry[i]->numTriangles - 1)));
		glBindVertexArray(0);
	}
	tmp = tmp + change;
	if (!streamlinesMove)
		tmp = 0;

	glUseProgram(0);
	CHECK_GL_ERROR();
}

void Renderer::initGroundGeometry(ShaderProgram& shader, Geometry** geometry)
{
	*geometry = new Geometry;

	std::vector<float> vertices;
	std::vector<size_t> indices;

	Loader::LoadGround(GROUND_FILE_PATH, &vertices, &indices);

	glGenVertexArrays(1, &((*geometry)->vao));
	glBindVertexArray((*geometry)->vao);

	glGenBuffers(1, &((*geometry)->vbo));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	glGenBuffers(1, &((*geometry)->ebo));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(size_t) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(shader.posLocation);
	glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

	glEnableVertexAttribArray(shader.colorLocation);
	glVertexAttribPointer(shader.colorLocation, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = indices.size() / 3;
}

void Renderer::initTempCutsGeometry(ShaderProgram& shader, Geometry** geometry)
{
	*geometry = new Geometry;

	std::vector<float> vertices;
	std::vector<size_t> indices;

	Loader::LoadTempCuts(TEMPERATURE_FILE_PATH, &vertices, &indices);

	glGenVertexArrays(1, &((*geometry)->vao));
	glBindVertexArray((*geometry)->vao);

	glGenBuffers(1, &((*geometry)->vbo));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	glGenBuffers(1, &((*geometry)->ebo));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(size_t) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(shader.posLocation);
	glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

	glEnableVertexAttribArray(shader.colorLocation);
	glVertexAttribPointer(shader.colorLocation, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = indices.size() / 3;
}

void Renderer::initTempHorizontalCutGeometry(ShaderProgram& shader, Geometry** geometry)
{
	*geometry = new Geometry;

	std::vector<float> vertices;
	std::vector<size_t> indices;

	Loader::LoadTempHorizontalCut(TEMPERATURE_FILE_PATH, &vertices, &indices);

	glGenVertexArrays(1, &((*geometry)->vao));
	glBindVertexArray((*geometry)->vao);

	glGenBuffers(1, &((*geometry)->vbo));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices.front(), GL_STATIC_DRAW);

	glGenBuffers(1, &((*geometry)->ebo));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(size_t) * indices.size(), &indices.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(shader.posLocation);
	glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

	glEnableVertexAttribArray(shader.colorLocation);
	glVertexAttribPointer(shader.colorLocation, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = indices.size() / 3;
}

void Renderer::initStreamlineGeometry(ShaderProgram& shader, Geometry** geometry, glm::vec3 position)
{
	*geometry = new Geometry;
	std::vector<glm::vec3> streamline = findStreamLineNodes(position, *windX, *windY, *windZ);
	std::vector<float> data;

	int n = streamline.size();
	for (size_t i = 0; i < n; i++)
	{
		data.push_back(streamline[i].x / 100);
		data.push_back(streamline[i].y / 100);
		data.push_back(streamline[i].z / ZDIM);
	}

	std::vector<float>* temp;
	Loader::LoadData(TEMPERATURE_FILE_PATH, temp, XDIM * YDIM * ZDIM);


	for (size_t i = 0; i < n; i++)
	{
		glm::vec4 color = tempToColor(trilinearInterpolation(streamline[i], *temp));
		data.push_back(color.x);
		data.push_back(color.y);
		data.push_back(color.z);
		data.push_back(color.w);
	}
	(*geometry)->numTriangles = n;

	glGenVertexArrays(1, &((*geometry)->vao));
	glBindVertexArray((*geometry)->vao);

	glGenBuffers(1, &((*geometry)->vbo));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(shader.posLocation);
	glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glEnableVertexAttribArray(shader.colorLocation);
	glVertexAttribPointer(shader.colorLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float) * n));


}

void Renderer::cleanupShaderPrograms()
{
	pgr::deleteProgramAndShaders(shaderProgram.program);
}

void Renderer::initializeShaderPrograms()
{
	std::vector<GLuint> shaderList;
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, GROUND_VERTEX_SHADER));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, GROUND_FRAGMENT_SHADER));

	shaderProgram.program = pgr::createProgram(shaderList);

	shaderProgram.posLocation = glGetAttribLocation(shaderProgram.program, "position");
	shaderProgram.colorLocation = glGetAttribLocation(shaderProgram.program, "color");

	shaderProgram.PVMmatrixLocation = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
	shaderProgram.VmatrixLocation = glGetUniformLocation(shaderProgram.program, "Vmatrix");
	shaderProgram.MmatrixLocation = glGetUniformLocation(shaderProgram.program, "Mmatrix");

	CHECK_GL_ERROR();
}

void Renderer::initializeModels()
{
	streamlinesGeometry = std::vector<Geometry*>(NUMBER_OF_STREAMLINES);
	Loader::LoadWindData(WIND_X_FILE_PATH, WIND_Y_FILE_PATH, WIND_Z_FILE_PATH, windX, windY, windZ);
	int index = 0;
	for (size_t i = 0; i < X_CELLS; i++)
	{
		for (size_t j = 2; j < Z_CELLS; j++)
		{
			initStreamlineGeometry(shaderProgram, &streamlinesGeometry[index++], glm::vec3(i * X_DIFF, 20, j * Z_DIFF));
			initStreamlineGeometry(shaderProgram, &streamlinesGeometry[index++], glm::vec3(i * X_DIFF, 480, j * Z_DIFF));
		}
	}

	for (size_t i = 0; i < Y_CELLS; i++)
	{
		for (size_t j = 2; j < Z_CELLS; j++)
		{
			initStreamlineGeometry(shaderProgram, &streamlinesGeometry[index++], glm::vec3(20, i * Y_DIFF, j * Z_DIFF));
			initStreamlineGeometry(shaderProgram, &streamlinesGeometry[index++] , glm::vec3(480, i * Y_DIFF, j * Z_DIFF));
		}
	}

	for (size_t i = 0; i < X_CELLS; i++)
	{
		for (size_t j = 0; j < Y_CELLS; j++)
		{
			initStreamlineGeometry(shaderProgram, &streamlinesGeometry[index++], glm::vec3(i*X_DIFF, j * Y_DIFF, Z_DIFF));

		}
	}



	initGroundGeometry(shaderProgram, &groundGeometry);
	initTempCutsGeometry(shaderProgram, &tempCutsGeometry);
	initTempHorizontalCutGeometry(shaderProgram, &tempHorizontalCutGeometry);
	CHECK_GL_ERROR();
}

void Renderer::cleanupGeometry(Geometry* geometry)
{
	glDeleteVertexArrays(1, &(geometry->vao));
	glDeleteBuffers(1, &(geometry->ebo));
	glDeleteBuffers(1, &(geometry->vbo));
}

void Renderer::cleanupModels()
{
	cleanupGeometry(groundGeometry);
	cleanupGeometry(tempCutsGeometry);
	cleanupGeometry(tempHorizontalCutGeometry);

	

	for (size_t i = 0; i < NUMBER_OF_STREAMLINES; i++)
	{
		cleanupGeometry(streamlinesGeometry[i]);
	}
}