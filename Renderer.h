#ifndef __RENDER_STUFF_H
#define __RENDER_STUFF_H

#include <vector>
#include "pgr.h"
#include "StreamLine.h"

// geometry is shared among all instances of the same object type
struct Geometry
{
	GLuint			vbo;			// identifier for the vertex buffer object
	GLuint			ebo;			// identifier for the element buffer object
	GLuint			vao;			// identifier for the vertex array object
	unsigned int	numTriangles;	// number of triangles in the mesh
};

// parameters of individual objects in the scene (e.g. position, size, etc.)
struct Object
{
	glm::vec3	position;
	glm::vec3	direction;
	float		size;
};

struct Camera : public Object
{
	bool freeCameraMode = false;       
	bool secondCamera = false;			
	float verticalAngle; // in degrees = initially 0.0f
	float horizontalAngle; // in degrees = initially 0.0f

	glm::vec3 lastPosition;
	glm::vec3 lastDirection;
};

struct ShaderProgram
{
	/// identifier for the program
	GLuint program;     
	/// vertex attributes locations
	GLint posLocation;      
	GLint colorLocation;    

	/// uniforms locations
	GLint PVMmatrixLocation;    
	GLint VmatrixLocation;      // view/camera matrix
	GLint MmatrixLocation;      // modeling matrix
};

class Renderer
{
private:
	Geometry* groundGeometry;
	Geometry* tempCutsGeometry;
	Geometry* tempHorizontalCutGeometry;
	std::vector<Geometry*> streamlinesGeometry;

	// wind is loaded in form of vector of 3 floats
	// each float is loaded from individual field and they are used separately
	// thats why they are stored separately
	std::vector<float>* windX;
	std::vector<float>* windY;
	std::vector<float>* windZ;

	void initializeShaderPrograms();
	void initializeModels();

	void initGroundGeometry(ShaderProgram& shader, Geometry** geometry);
	void initTempCutsGeometry(ShaderProgram& shader, Geometry** geometry);
	void initTempHorizontalCutGeometry(ShaderProgram& shader, Geometry** geometry);
	void initStreamlineGeometry(ShaderProgram& shader, Geometry** geometry, glm::vec3 position);

	void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	void cleanupGeometry(Geometry* geometry);
	void cleanupShaderPrograms();
	void cleanupModels();

public:
	///constructor
	Renderer();
	///destructor
	~Renderer();

	ShaderProgram shaderProgram;

	void drawGround(Object* ground, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void drawTempCuts(Object* cut, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void drawTempHorizontalCut(Object* tempCuts, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	void drawStreamlines(Object* streamline, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
};
#endif // __RENDER_STUFF_H