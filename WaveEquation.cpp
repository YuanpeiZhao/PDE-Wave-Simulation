#include <windows.h>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glext.h>

#include <glm/glm.hpp> //This header defines basic glm types (vec3, mat4, etc)

//These headers define matrix transformations
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <glm/gtc/type_ptr.hpp> //This header defines helper functions (e.g. value_ptr(...)) for passing vectors and matrices to shaders

#include "InitShader.h"    //Functions for loading shaders from text files
#include "imgui_impl_glut.h" //Functions for using imgui UI in glut programs

#define POINT_X 100
#define POINT_Y 100

#define WIN_X 640
#define WIN_Y 640

static const std::string vertex_shader("wave_equation_vs.glsl");
static const std::string fragment_shader("wave_equation_fs.glsl");

static const std::string height_vertex_shader("compute_height_vs.glsl");
static const std::string height_fragment_shader("compute_height_fs.glsl");

static const std::string cube_vertex_shader("cube_vs.glsl");
static const std::string cube_fragment_shader("cube_fs.glsl");

glm::vec3 points[POINT_X][POINT_Y];
glm::vec2 texureCoords[POINT_X][POINT_Y];

unsigned int indices[6 * (POINT_X - 1) * (POINT_Y - 1)];
float vertices[5 * POINT_X * POINT_Y];

float heightMapVertices[] = {
	-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
	1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
	-1.0f, 1.0f, 0.0f,		0.0f, 1.0f
};

unsigned int heightMapIndices[] = {
	0, 1, 2,
	0, 2, 3
};

float cubeVertices[] = {
	-0.4f, -0.7f, 0.2f, 0.0f, 0.0f, 1.0f,
	0.4f, -0.7f, 0.2f, 0.0f, 0.0f, 1.0f,
	-0.4f, -0.5f, 0.2f, 0.0f, 0.0f, 1.0f,
	0.4f, -0.7f, 0.2f, 0.0f, 0.0f, 1.0f,
	0.4f, -0.5f, 0.2f, 0.0f, 0.0f, 1.0f,
	-0.4f, -0.5f, 0.2f, 0.0f, 0.0f, 1.0f,

	-0.4f, -0.7f, 0.2f, 0.0f, -1.0f, 0.0f,
	0.4f, -0.7f, 0.2f, 0.0f, -1.0f, 0.0f,
	-0.4f, -0.7f, -0.2f, 0.0f, -1.0f, 0.0f,
	0.4f, -0.7f, 0.2f, 0.0f, -1.0f, 0.0f,
	-0.4f, -0.7f, -0.2f, 0.0f, -1.0f, 0.0f,
	0.4f, -0.7f, -0.2f, 0.0f, -1.0f, 0.0f,

	-0.4f, -0.7f, 0.2f, -1.0f, 0.0f, 0.0f,
	-0.4f, -0.7f, -0.2f, -1.0f, 0.0f, 0.0f,
	-0.4f, -0.5f, -0.2f, -1.0f, 0.0f, 0.0f,
	-0.4f, -0.7f, 0.2f, -1.0f, 0.0f, 0.0f,
	-0.4f, -0.5f, -0.2f, -1.0f, 0.0f, 0.0f,
	-0.4f, -0.5f, 0.2f, -1.0f, 0.0f, 0.0f,

	0.4f, -0.7f, 0.2f, 1.0f, 0.0f, 0.0f,
	0.4f, -0.5f, 0.2f, 1.0f, 0.0f, 0.0f,
	0.4f, -0.7f, -0.2f, 1.0f, 0.0f, 0.0f,
	0.4f, -0.5f, 0.2f, 1.0f, 0.0f, 0.0f,
	0.4f, -0.7f, -0.2f, 1.0f, 0.0f, 0.0f,
	0.4f, -0.5f, -0.2f, 1.0f, 0.0f, 0.0f
};

GLuint shader_program = -1;
GLuint heightmap_shader_program = -1;
GLuint cube_shader_program = -1;

GLuint texture_id = -1; //Texture map
GLuint vao;				// Vertex array object
GLuint vbuf;			// Vertex buffer
GLuint EBO;

GLuint hvao;				
GLuint hvbuf;			
GLuint hEBO;

GLuint cvao;
GLuint cvbuf;

GLuint framebuffer;
GLuint texColorBuffer;

float step_distance = 1.0f;
int initialized = 0;
int cur_frame = 0;
float c = 0.05f;
float d = step_distance / (POINT_X - 1);
float miu = 0.01f;
glm::vec2 peak0_pos = glm::vec2(0.5f, 0.5f);

float curr_time;
float prev_time;
float delta_t;

int peakPattern = 1;

void initPoints() {

	// Init all vertices
	for (int y = 0; y < POINT_X; y++)
	{
		for (int x = 0; x < POINT_Y; x++)
		{
			points[y][x] = glm::vec3(-1.0 + 2.0 * x / (POINT_X - 1), 1.0 - 2.0 * y / (POINT_Y - 1), 0.0);
		}
	}

	// Init the indeces of triangles
	for (int i = 0; i < 2 * (POINT_X - 1) * (POINT_Y - 1); i++)
	{
		int NY = (i / 2) % (POINT_X - 1);
		int NX = (i / 2) / (POINT_X - 1);
		if (i % 2 == 0)
		{
			indices[3 * i] = POINT_X * NX + NY;
			indices[3 * i + 1] = POINT_X * NX + NY + 1;
			indices[3 * i + 2] = POINT_X * (NX + 1) + NY;
		}
		else
		{
			indices[3 * i] = POINT_X * NX + NY + 1;
			indices[3 * i + 1] = POINT_X * (NX + 1) + NY + 1;
			indices[3 * i + 2] = POINT_X * (NX + 1) + NY;
		}
	}

	// Init Texture Coords
	for (int y = 0; y < POINT_X; y++)
	{
		for (int x = 0; x < POINT_Y; x++)
		{
			texureCoords[y][x] = glm::vec2(1.0 * x / POINT_X, 1.0 - 1.0 * y / POINT_Y);
		}
	}
}

void initVertices() {
	for (int i = 0; i < POINT_X * POINT_Y; i++)
	{
		vertices[5 * i] = points[i / POINT_X][i % POINT_X].x;
		vertices[5 * i + 1] = points[i / POINT_X][i % POINT_X].y;
		vertices[5 * i + 2] = points[i / POINT_X][i % POINT_X].z;
		vertices[5 * i + 3] = texureCoords[i / POINT_X][i % POINT_X].x;
		vertices[5 * i + 4] = texureCoords[i / POINT_X][i % POINT_X].y;
	}
}

void initViewPlane() {

	const int time_ms = glutGet(GLUT_ELAPSED_TIME);
	curr_time = 0.001f * time_ms;

	initPoints();
	initVertices();

	// generate FBO
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Generate texture
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIN_X, WIN_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbuf);
	glGenBuffers(1, &EBO);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &cvao);
	glGenBuffers(1, &cvbuf);

	glBindVertexArray(cvao);

	glBindBuffer(GL_ARRAY_BUFFER, cvbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/// do heightmap buffers
	glGenVertexArrays(1, &hvao);
	glGenBuffers(1, &hvbuf);
	glGenBuffers(1, &hEBO);

	glBindVertexArray(hvao);

	glBindBuffer(GL_ARRAY_BUFFER, hvbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(heightMapVertices), heightMapVertices, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(heightMapIndices), heightMapIndices, GL_DYNAMIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

//Draw the ImGui user interface
void draw_gui()
{
   ImGui_ImplGlut_NewFrame();

   ImGui::Begin("Menu (Double Click to Activate/Deactivate)");

   if (initialized == 0)
   {
		if (ImGui::Button("Start")) {
		   initialized = 1;
		}
   }
   else
   {
		if (ImGui::Button("Stop")) {
		   initialized = 0;
		}
   }

   ImGui::RadioButton("square", &peakPattern, 0);
   ImGui::RadioButton("circle", &peakPattern, 1);

   ImGui::SliderFloat("Water Resistance", &miu, 0.01, 3.0);
   ImGui::SliderFloat("Step Distance", &step_distance, 1.0, 3.0);
   d = step_distance / (POINT_X - 1);

   ImGui::End();

   ImGui::Render();
 }

// glut display callback function.
// This function gets called every time the scene gets redisplayed 
void display()
{

	// first pass
   //glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

   glClearColor(0.0, 0.0, 0.0, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the back buffer
   glDisable(GL_DEPTH_TEST);

   glUseProgram(heightmap_shader_program);

   const int time_ms = glutGet(GLUT_ELAPSED_TIME);
   float time_sec = 0.001f * time_ms;

   prev_time = curr_time;
   curr_time = time_sec;
   delta_t = curr_time - prev_time;

   int h_time_loc = glGetUniformLocation(heightmap_shader_program, "time");
   if (h_time_loc != -1)
   {
	   glUniform1f(h_time_loc, delta_t);
   }

   int peak_pattern_loc = glGetUniformLocation(heightmap_shader_program, "peak_pattern");
   if (peak_pattern_loc != -1)
   {
	   glUniform1i(peak_pattern_loc, peakPattern);
   }

   int peak0_pos_loc = glGetUniformLocation(heightmap_shader_program, "peak0_pos");
   if (peak0_pos_loc != -1)
   {
	   glUniform2fv(peak0_pos_loc, 1, glm::value_ptr(peak0_pos));
   }

   int initialized_loc = glGetUniformLocation(heightmap_shader_program, "initialized");
   if (initialized_loc != -1)
   {
	   glUniform1i(initialized_loc, initialized);
   }

   int cur_frame_loc = glGetUniformLocation(heightmap_shader_program, "cur_frame");
   if (cur_frame_loc != -1)
   {
	   glUniform1i(cur_frame_loc, cur_frame);
   }

   c = d / 2.0f / delta_t * sqrt(miu * delta_t + 2) / 1.5f;
   int c_loc = glGetUniformLocation(heightmap_shader_program, "c");
   if (c_loc != -1)
   {
	   glUniform1f(c_loc, c);
   }

   int d_loc = glGetUniformLocation(heightmap_shader_program, "d");
   if (d_loc != -1)
   {
	   glUniform1f(d_loc, d);
   }

   int miu_loc = glGetUniformLocation(heightmap_shader_program, "miu");
   if (miu_loc != -1)
   {
	   glUniform1f(miu_loc, miu);
   }

   glBindVertexArray(hvao);
   glBindTexture(GL_TEXTURE_2D, texColorBuffer);
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

   glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, WIN_X, WIN_Y);
   //glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
   //glBindTexture(GL_TEXTURE_2D, 0);

   // second pass
   //glClearColor(0.0, 0.0, 0.0, 1.0);
   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear the back buffer

   glEnable(GL_DEPTH_TEST);

   glm::mat4 M = glm::scale(glm::vec3(1.0));
   glm::mat4 V = glm::lookAt(glm::vec3(0.0f, -1.5f, 1.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
   glm::mat4 P = glm::perspective(90.0f, 1.0f, 0.1f, 100.0f);

   glUseProgram(cube_shader_program);

   int cube_M_loc = glGetUniformLocation(cube_shader_program, "M");
   if (cube_M_loc != -1)
   {
	   glUniformMatrix4fv(cube_M_loc, 1, false, glm::value_ptr(M));
   }

   int cube_P_loc = glGetUniformLocation(cube_shader_program, "P");
   if (cube_P_loc != -1)
   {
	   glUniformMatrix4fv(cube_P_loc, 1, false, glm::value_ptr(P));
   }

   int cube_V_loc = glGetUniformLocation(cube_shader_program, "V");
   if (cube_V_loc != -1)
   {
	   glUniformMatrix4fv(cube_V_loc, 1, false, glm::value_ptr(V));
   }

   glBindVertexArray(cvao);
   glDrawArrays(GL_TRIANGLES, 0, 24);

   glUseProgram(shader_program);

   int PVM_loc = glGetUniformLocation(shader_program, "PVM");
   if (PVM_loc != -1)
   {
      glm::mat4 PVM = P*V*M;
      glUniformMatrix4fv(PVM_loc, 1, false, glm::value_ptr(PVM));
   }

   int P_loc = glGetUniformLocation(shader_program, "P");
   if (P_loc != -1)
   {
      glUniformMatrix4fv(P_loc, 1, false, glm::value_ptr(P));
   }

   int VM_loc = glGetUniformLocation(shader_program, "VM");
   if (VM_loc != -1)
   {
      glm::mat4 VM = V*M;
      glUniformMatrix4fv(VM_loc, 1, false, glm::value_ptr(VM));
   }

   int frame_loc = glGetUniformLocation(shader_program, "cur_frame");
   if (frame_loc != -1)
   {
	   glUniform1i(frame_loc, cur_frame);
   }
   cur_frame = (cur_frame + 1) % 3;

   int dd_loc = glGetUniformLocation(shader_program, "d");
   if (dd_loc != -1)
   {
	   glUniform1f(dd_loc, d);
   }

   int tex_loc = glGetUniformLocation(shader_program, "diffuse_tex");
   if (tex_loc != -1)
   {
      glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
   }

   glBindVertexArray(vao);
   glBindTexture(GL_TEXTURE_2D, texColorBuffer);
   glDrawElements(GL_TRIANGLES, 6 * (POINT_X - 1) * (POINT_Y - 1), GL_UNSIGNED_INT, 0);
   glBindTexture(GL_TEXTURE_2D, 0);

   draw_gui();

   glutSwapBuffers();
}

void idle()
{
	glutPostRedisplay();

   const int time_ms = glutGet(GLUT_ELAPSED_TIME);
   float time_sec = 0.001f*time_ms;

   //Pass time_sec value to the shaders
   int time_loc = glGetUniformLocation(shader_program, "time");
   if (time_loc != -1)
   {
	   glUniform1f(time_loc, time_sec);
   }
}

void reload_shader()
{
   GLuint new_shader = InitShader(vertex_shader.c_str(), fragment_shader.c_str());

   if (new_shader == -1) // loading failed
   {
      glClearColor(1.0f, 0.0f, 1.0f, 0.0f); //change clear color if shader can't be compiled
   }
   else
   {
      glClearColor(0.35f, 0.35f, 0.35f, 0.0f);

      if (shader_program != -1)
      {
         glDeleteProgram(shader_program);
      }
      shader_program = new_shader;
   }
}

void printGlInfo()
{
   std::cout << "Vendor: "       << glGetString(GL_VENDOR)                    << std::endl;
   std::cout << "Renderer: "     << glGetString(GL_RENDERER)                  << std::endl;
   std::cout << "Version: "      << glGetString(GL_VERSION)                   << std::endl;
   std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)  << std::endl;
}

void initOpenGl()
{
   glewInit();

   glEnable(GL_DEPTH_TEST);

   reload_shader();

   heightmap_shader_program = InitShader(height_vertex_shader.c_str(), height_fragment_shader.c_str());
   cube_shader_program = InitShader(cube_vertex_shader.c_str(), cube_fragment_shader.c_str());

   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

// glut callbacks need to send keyboard and mouse events to imgui
void keyboard(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyCallback(key);

   switch(key)
   {
      case 'r':
      case 'R':
         reload_shader();     
      break;
   }
}

void keyboard_up(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyUpCallback(key);
}

void special_up(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialUpCallback(key);
}

void passive(int x, int y)
{
   ImGui_ImplGlut_PassiveMouseMotionCallback(x,y);
}

void special(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialCallback(key);
}

void motion(int x, int y)
{
   ImGui_ImplGlut_MouseMotionCallback(x, y);
}

void mouse(int button, int state, int x, int y)
{
   ImGui_ImplGlut_MouseButtonCallback(button, state);
}


int main (int argc, char **argv)
{
   //Configure initial window state
   glutInit(&argc, argv); 
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowPosition (5, 5);
   glutInitWindowSize (WIN_X, WIN_Y);
   int win = glutCreateWindow ("Wave Equation - Yuanpei Zhao");

   //printGlInfo();

   //Register callback functions with glut. 
   glutDisplayFunc(display); 
   glutKeyboardFunc(keyboard);
   glutSpecialFunc(special);
   glutKeyboardUpFunc(keyboard_up);
   glutSpecialUpFunc(special_up);
   glutMouseFunc(mouse);
   glutMotionFunc(motion);
   glutPassiveMotionFunc(motion);
   glutIdleFunc(idle);

   initOpenGl();
   ImGui_ImplGlut_Init(); // initialize the imgui system

   initViewPlane();

   std::cout << glGetString(GL_VERSION);

   //Enter the glut event loop.
   glutMainLoop();
   glutDestroyWindow(win);
   return 0;		
}