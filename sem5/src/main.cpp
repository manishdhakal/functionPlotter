#include <GL/glew.h>

#include <GLFW/glfw3.h>

//Mathematical libraries
#include "Utilities/GLM/glm/glm.hpp"
#include "Utilities/GLm/glm/gtc/matrix_transform.hpp"
#include "Utilities/GLM/glm/gtc/type_ptr.hpp"

//shader class
#include "Utilities/Shaders.h"
//camera class
#include "Utilities/Camera.h"


#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "Utilities/stb_image.h"

#define N 9126
#define PI 3.1415

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

bool firstMouse = true;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void OpenGLDebugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void*) {

	std::cout << message << std::endl;

	__debugbreak();

}

float mixval = 0.2;
float delTime = 0.0f;
float lastFrame = 0.0f;
glm::vec3 lightPos(10.0f, 10.0f, -10.0f);


//glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0f);
//glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0f);
//glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0f);

int graphSize = 20;
int sampleSize = 20;
struct points {
	float x, y, z;
public:
	points operator -(points a) {
		points temp;
		temp.x = x - a.x;
		temp.y = y - a.y;
		temp.z = z - a.z;
		return temp;

	}
	points operator +(points a) {
		points temp;
		temp.x = x + a.x;
		temp.y = y + a.y;
		temp.z = z + a.z;
		return temp;

	}
};
//{12, 13, 30, 13, 31, 30, 13, 14, 31, 14, 32, 31, 14, 15, 32, 15, 33, 32}
//normals = N/3
points axis[6], axisMarks[600], graph1[1800], graph2[400],normals[3042];
int triNum = (sampleSize*2) *6* 2*sampleSize;
//unsigned int* g1index = new unsigned int[triNum];
unsigned int g1index[N] = { 0}, wireframe[N] = { 0 };
unsigned int bufferAxis, bufferMarks, bufferGraph1, bufferGraph2, wireframeBuffer,g1Ibuffer,vertexArray,normalBuffer;

void generate()
{
	//axis lines
   //x
	axis[0].x = -graphSize;
	axis[0].y = 0;
	axis[0].z = 0;
	axis[1].x = graphSize;
	axis[1].y = 0;
	axis[1].z = 0;
	//y
	axis[2].x = 0;
	axis[2].y = -graphSize;
	axis[2].z = 0;
	axis[3].x = 0;
	axis[3].y = graphSize;
	axis[3].z = 0;
	//z
	axis[4].x = 0;
	axis[4].y = 0;
	axis[4].z = graphSize;
	axis[5].x = 0;
	axis[5].y = 0;
	axis[5].z = -graphSize;

	//axis marks
	int i;
	int index = 0;
	//x
	for (i = -graphSize; i < graphSize; i++) {
		axisMarks[index].x = i;
		axisMarks[index].y = -0.2;
		axisMarks[index].z = 0;
		index++;
		axisMarks[index].x = i;
		axisMarks[index].y = 0.2;
		axisMarks[index].z = 0;
		index++;
	}
	//y
	for (i = -graphSize; i < graphSize; i++) {
		axisMarks[index].x = -0.2;
		axisMarks[index].y = i;
		axisMarks[index].z = 0;
		index++;
		axisMarks[index].x = 0.2;
		axisMarks[index].y = i;
		axisMarks[index].z = 0;
		index++;
	}
	//z
	for (i = -graphSize; i < graphSize; i++) {
		axisMarks[index].x = 0;
		axisMarks[index].y = -0.2;
		axisMarks[index].z = i;
		index++;
		axisMarks[index].x = 0;
		axisMarks[index].y = 0.2;
		axisMarks[index].z = i;
		index++;
	}
	//for axis
	glGenBuffers(1, &bufferAxis);
	glBindBuffer(GL_ARRAY_BUFFER, bufferAxis);
	//assign axis data
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_STATIC_DRAW);

	//for axis marks
	glGenBuffers(1, &bufferMarks);
	glBindBuffer(GL_ARRAY_BUFFER, bufferMarks);
	//assign axis_marks data
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisMarks), axisMarks, GL_STATIC_DRAW);

	index = 0;
	float x, y;
	for (int i = -sampleSize,x=-PI; i < sampleSize; i++, x += 0.209) {
		for (int j = -sampleSize,y= -PI; j < sampleSize; j++,y+= 0.1047) {
			/*graph1[index].x = 4*cos(x)*sin(y);
			graph1[index].z = 4*sin(x)*sin(y);*/
			//graph1[index].x = 4 * cos(x);
			//graph1[index].z = 4 * sin(x);
			//graph1[index].y = sin(i*i +  j*j);

			graph1[index].x = i*0.3;
			graph1[index].z =j*0.3;
			float d = sqrt  (i*i + j * j);
			if (d == 0.0)
				d = 1;

			//graph1[index].y = (sin(d) / d)*10;
			//graph1[index].y = sin(5*i)*cos(5*j)/3;
			float tem = (i*i + j * j);
			/*if (tem == 0) {
				tem = 1;
			}*/
			//graph1[index].y = 10 - abs(j);
			//graph1[index].y = 1.75 / exp((i * 5)*(i * 5) * (j * 5)*(j * 5));

			//graph1[index].y = (i + j)*0.2;
			//graph1[index].y = tem*0.05;
			//graph1[index].y = pow(tem , 0.5);
			graph1[index].y = sqrt(36 - (10- sqrt(tem))*(10- sqrt(tem)));

			index++;
		}
	}

	for (int i = -sampleSize, x = -6.0; i < sampleSize; i++, x += 0.4) {
		for (int j = -sampleSize, y = -6.0; j < sampleSize; j++, y += 0.4) {
			graph1[index].x = -i*0.3;
			graph1[index].z = j*0.3;
			float d = sqrt(i*i + j * j);
			if (d == 0.0)
				d = 1;
			//graph1[index].y = (sin(d) / d)*10;
			//graph1[index].y = sin(5 * i)*cos(5 * j)/3;
			float tem =  (i*i + j * j);
			/*if (tem == 0) {
				tem = 1;
			}*/
			//graph1[index].y = 10 - abs(j);
			//graph1[index].y = 1.75 / exp((i * 5)*(i * 5) * (j * 5)*(j * 5));
			//graph1[index].y = (i + j)*0.2;
			//graph1[index].y = tem * 0.05;
			graph1[index].y = sqrt(36- (10 - sqrt(tem))*(10 - sqrt(tem)));
			index++;
		}
	}

	//for triangles
	int landscape = 2 * sampleSize;
	index = 0;
	int j = 0,ni=0;
	int t = 0;

	for (int k = 0; k < landscape*(landscape - 1); k += landscape) {
		for (int i = k; i < k + landscape - 1; i++) {
			j = i + 1;
			t = i + landscape;
			g1index[index] = i;
			index++;
			g1index[index] = j;
			index++;
			g1index[index] = t;

			points v1 = graph1[j] - graph1[i], v2 = graph1[t] - graph1[i];
			points n = { v2.z*v1.y - v2.y*v1.z, v2.x*v1.z - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x };
			normals[ni] = n;
			ni++;
			index++;
			g1index[index] = j;
			index++;
			g1index[index] = t + 1;
			index++;
			g1index[index] = t;
			index++;

			v1 = graph1[t + 1] - graph1[j]; v2 = graph1[t] - graph1[j];
			n = { v2.z*v1.y - v2.y*v1.z, v2.x*v1.z - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x };
			normals[ni] = n;
			ni++;
		}
		

	}
	
	/*for (int i = 0; i < N / 3; i++) {
		std::cout<< normals[i].x <<", "<<normals[i].y << ", " << normals[i].z << "\t";
	}*/
	
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	//assign graph data
	glGenBuffers(1, &bufferGraph1);
	glBindBuffer(GL_ARRAY_BUFFER, bufferGraph1);
	//assign axis_marks data
	glBufferData(GL_ARRAY_BUFFER, sizeof(graph1), graph1, GL_STATIC_DRAW);

	glGenBuffers(1, &g1Ibuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g1Ibuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g1index), g1index, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	//assign axis_marks data
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	/*glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback((GLDEBUGPROC)OpenGLDebugCallback, 0);*/

}



int main(void)
{
	GLFWwindow *window;
	/* Initialize the library */

	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(800, 600, "SEM5", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);



	if (glewInit() != GLEW_OK)
		std::cout << "Error occurred" << std::endl;


	Shader MyShader("vertexShader.vert", "fragmentShader.frag");

	generate();


	Shader lightSourceShader("lightVertexShader.vert", "lightFragmentShader.frag");


	float positionswithTexture[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};


	float positions[] = {
	 -0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,  
	 0.5f,  0.5f, -0.5f,  
	 0.5f,  0.5f, -0.5f,  
	-0.5f,  0.5f, -0.5f,  
	-0.5f, -0.5f, -0.5f,  

	-0.5f, -0.5f,  0.5f,  
	 0.5f, -0.5f,  0.5f,  
	 0.5f,  0.5f,  0.5f,  
	 0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f,  
	-0.5f, -0.5f,  0.5f,  

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,  
	 0.5f,  0.5f, -0.5f,  
	 0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f,  0.5f,  
	 0.5f,  0.5f,  0.5f,  

	-0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f,  0.5f,  
	 0.5f, -0.5f,  0.5f,  
	-0.5f, -0.5f,  0.5f,  
	-0.5f, -0.5f, -0.5f,  

	-0.5f,  0.5f, -0.5f,  
	 0.5f,  0.5f, -0.5f,  
	 0.5f,  0.5f,  0.5f,  
	 0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f, -0.5f 
	};
	float vertices[] = {
	 0.5f,  0.5f, 0.0f,  // top right
	 0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
	};
	int indices[] = {
		0,1,3,
		1,2,3
	};

	glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);


	//configuring for cube
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
*/
	glBindVertexArray(0);
	

	//configuring for lightcube
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO, the container's VBO's data already contains the data.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// set the vertex attributes (only position data for our lamp)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);




	const unsigned char* vendor = glGetString(GL_VENDOR);
	const unsigned char* render = glGetString(GL_RENDERER);

	/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);*/

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);

	int width, height, nrChannels;
	//texture1
	stbi_set_flip_vertically_on_load(true);
	glBindTexture(GL_TEXTURE_2D, texture1);
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "COULD NOT LOAD TEXTURE" << std::endl;
	}
	stbi_image_free(data);


	//texture2
	glGenTextures(1, &texture2);
	data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "COULD NOT LOAD TEXTURE" << std::endl;
	}
	stbi_image_free(data);



	/*glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0));
*/

	glfwSwapInterval(1);

	int move = 2, by = 10;
	MyShader.use();
	/*MyShader.setInt("texture1", 0);
	MyShader.setInt("texture2", 1);*/
	
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	MyShader.setVec3("lightColor", lightColor);






	/*unsigned int viewloc = glGetUniformLocation(MyShader.ID, "view");
	glUniformMatrix4fv(viewloc, 1, GL_FALSE, glm::value_ptr(view));
	unsigned int projloc = glGetUniformLocation(MyShader.ID, "proj");*/

	glEnable(GL_DEPTH_TEST);
	int angle = 0;
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback((GLDEBUGPROC)OpenGLDebugCallback, 0);
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		processInput(window);
		float currentFrame = glfwGetTime();
		delTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		MyShader.use();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glm::vec3 objColor = glm::vec3(0.0f, 0.8f, 0.5f);
		MyShader.setVec3("objectColor", objColor);
		glBindVertexArray(vertexArray);
		glDrawElements(GL_TRIANGLES, N, GL_UNSIGNED_INT, 0);
		objColor = glm::vec3(1.0f, 0.8f, 1.0f);
		MyShader.setVec3("objectColor", objColor);
		//glDrawElements(GL_LINES, N, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		objColor = glm::vec3(1.0f, 0.0f, 0.0f);
		MyShader.setVec3("objectColor", objColor);
		//binding and drawing axis
		glBindBuffer(GL_ARRAY_BUFFER, bufferAxis);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_LINES, 0, 9);
		glDisableVertexAttribArray(0);

		//binding and drawing axis marks
		glBindBuffer(GL_ARRAY_BUFFER, bufferMarks);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_LINES, 0, 600);
		glDisableVertexAttribArray(0);



		//binding and drawing graph

	//	glBindBuffer(GL_ARRAY_BUFFER, bufferGraph1);

		

	/*	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g1Ibuffer);*/
		objColor = glm::vec3(0.0f, 0.0f, 0.0f);
		MyShader.setVec3("objectColor", objColor);
		//glDrawArrays(GL_POINTS, 0, 1);
	//	glDisableVertexAttribArray(0);

		objColor = glm::vec3(1.0f, 1.0f, 1.0f);
		MyShader.setVec3("objectColor", objColor);
		glBindBuffer(GL_ARRAY_BUFFER, bufferGraph1);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		for (int i = 0; i <2*sampleSize; i++) {
			glDrawArrays(GL_LINE_STRIP, i * sampleSize * 2, sampleSize * 2);
		}
		glDisableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, bufferGraph2);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		/*for (int i = 0; i < sampleSize; i++) {
			glDrawArrays(GL_LINE_STRIP, i * sampleSize * 2, sampleSize * 2);

		}*/
		glDisableVertexAttribArray(0);

		
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		int width, height;
		glfwGetWindowSize(window, &width, &height);

		
		glBindVertexArray(VAO);
		const float radius = 10.0f;
		float camX = radius * sin(glfwGetTime());
		float camZ = radius * cos(glfwGetTime());
		glm::mat4 view;
		view = camera.GetViewMatrix();
		MyShader.setMat4("view", view);

		glm::mat4 proj = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		MyShader.setMat4("proj", proj);

		MyShader.setVec3("lightPos", lightPos);
		MyShader.setVec3("cameraPos", camera.cameraPos);
		glm::mat4 model = glm::mat4(1.0f);
		MyShader.setMat4("model", model);
		objColor = glm::vec3(0.0f, 1.0f, 1.0f);
		MyShader.setVec3("objectColor", objColor);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(VAO);
		lightSourceShader.use();
		view = camera.GetViewMatrix();
		lightSourceShader.setMat4("view", view);
		 proj = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		MyShader.setMat4("proj", proj);
		lightSourceShader.setMat4("proj", proj);
		model = glm::mat4(1.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightSourceShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//glBindVertexArray(VAO);
		

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
//	delete[] g1index;
	return 0;
}

// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.KeyboardProcess(FORWARD, delTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.KeyboardProcess(BACKWARD, delTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.KeyboardProcess(LEFT, delTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.KeyboardProcess(RIGHT, delTime);




}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
	glfwSetCursor(window, cursor);

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.MouseProcess(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.MouseScrollProcess(yoffset);
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
