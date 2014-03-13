

/* by Lakshitha T. Premathilake 
The Following code is to simulate an oil jet release from deep water.
Still this is under development. At the moment this is just working as a fountain,
like a primivitve particle system. Development is in process.
The code will be only working on openGL 4.1, since layout qualifiers have been 
used in vertex shader*/

//===================================================================================================================
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <time.h>
#include "Matrix.h"		// NEW!
//#include "cube.h"

//#define USING_INDEX_BUFFER 1
#define PARTICLE_COUNT 1000 // New definition for particle count

#ifdef USING_INDEX_BUFFER
	#define NUM_VERTICES num_vertices
	#define NUM_INDICES num_indices	
#else
	#define NUM_VERTICES num_vertices
#endif

// From http://www.opengl.org/registry/specs/EXT/pixel_buffer_object.txt 
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

GLfloat emitter_pos[] = {0.0f, -0.5f, 0.0f}; // NEW!

GLuint shaderProgramID;
GLuint vao = 0;
GLuint vbo;
GLuint emitterID, timeID;
GLuint indexBufferID; 
float current=0;
float current_seconds=0;
clock_t l;

float Timer(void);

// ==================== BEGIN NEW STUFF ============================
GLuint	perspectiveMatrixID, viewMatrixID, modelMatrixID;	// IDs of variables mP, mV and mM in the shader

GLfloat* rotXMatrix;	// Matrix for rotations about the X axis
GLfloat* rotYMatrix;	// Matrix for rotations about the Y axis
GLfloat* rotZMatrix;	// Matrix for rotations about the Z axis
GLfloat* transMatrix;	// Matrix for changing the position of the object
GLfloat* scaleMatrix;	// Duh..
GLfloat* tempMatrix1;	// A temporary matrix for holding intermediate multiplications
GLfloat* M;				// The final model matrix M to change into world coordinates

GLfloat* V;				// The camera matrix (for position/rotation) to change into camera coordinates
GLfloat* P;				// The perspective matrix for the camera (to give the scene depth); initialize this ONLY ONCE!

GLfloat  theta;			// An amount of rotation along one axis
GLfloat	 scaleAmount;	// In case the object is too big or small


void initMatrices() {

	theta = 0.0f;
	scaleAmount = 1.0f;

	// Allocate memory for the matrices and initialize them to the Identity matrix
	rotXMatrix = new GLfloat[16];	makeIdentity(rotXMatrix);
	rotYMatrix = new GLfloat[16];	makeIdentity(rotYMatrix);
	rotZMatrix = new GLfloat[16];	makeIdentity(rotZMatrix);
	transMatrix = new GLfloat[16];	makeIdentity(transMatrix);
	scaleMatrix = new GLfloat[16];	makeIdentity(scaleMatrix);
	tempMatrix1 = new GLfloat[16];	makeIdentity(tempMatrix1);
	M = new GLfloat[16];			makeIdentity(M);
	V = new GLfloat[16];			makeIdentity(V);
	P = new GLfloat[16];			makeIdentity(P);

	// Set up the (P)erspective matrix only once! Arguments are 1) the resulting matrix, 2) FoV, 3) aspect ratio, 4) near plane 5) far plane
	makePerspectiveMatrix(P, 60.0f, 1.0f, 1.0f, 1000.0f);
}
//==================== END NEW STUFF ==========================================


#pragma region SHADER_FUNCTIONS
static char* readFile(const char* filename) {
	// Open the file
	FILE* fp = fopen (filename, "r");
	// Move the file pointer to the end of the file and determing the length
	fseek(fp, 0, SEEK_END);
	long file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* contents = new char[file_length+1];
	// zero out memory
	for (int i = 0; i < file_length+1; i++) {
		contents[i] = 0;
	}
	// Here's the actual read
	fread (contents, 1, file_length, fp);
	// This is how you denote the end of a string in C
	contents[file_length+1] = '\0';
	fclose(fp);
	return contents;
}

bool compiledStatus(GLint shaderID){
	GLint compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled) {
		return true;
	}
	else {
		GLint logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char* msgBuffer = new char[logLength];
		glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer);
		printf ("%s\n", msgBuffer);
		delete (msgBuffer);
		return false;
	}
}

GLuint makeVertexShader(const char* shaderSource) {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource (vertexShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(vertexShaderID);
	bool compiledCorrectly = compiledStatus(vertexShaderID);
	if (compiledCorrectly) {
		return vertexShaderID;
	}
	return -1;
}

GLuint makeFragmentShader(const char* shaderSource) {
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, (const GLchar**)&shaderSource, NULL);
	glCompileShader(fragmentShaderID);
	bool compiledCorrectly = compiledStatus(fragmentShaderID);
	if (compiledCorrectly) {
		return fragmentShaderID;
	}
	return -1;
}

GLuint makeShaderProgram (GLuint vertexShaderID, GLuint fragmentShaderID) {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	glLinkProgram(shaderID);
	return shaderID;
}
#pragma endregion SHADER_FUNCTIONS

//===============================================================================

// Any time the window is resized, this function gets called.  It's setup to the 
// "glutReshapeFunc" in main.
void changeViewport(int w, int h){
	glViewport(0, 0, w, h);
}




// Here is the function that gets called each time the window needs to be redrawn.
// It is the "paint" method for our program, and is set up from the glutDisplayFunc in main
void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// New!
	glUseProgram(shaderProgramID);
	//theta = 0.01f;
	scaleAmount = sin(theta);

	// Fill the matrices with valid data
	makeScale(scaleMatrix, 1.0f, 1.0f, 1.0f);			// Fill the scaleMatrix variable
	makeRotateY(rotYMatrix, theta);						// Fill the rotYMatrix variable

	//makeTranslate(transMatrix, -0.25f, -0.25f, -0.25f);		// Fill the transMatrix to push the model back 1 "unit" into the scene
	 makeTranslate(V, 0.0f, 0.0f, -1.0f);			// Fill the (V)iew matrix to push the world up 1 unit
	
	// Multiply them together 
	//matrixMult4x4(M, transMatrix, rotYMatrix);	// Order is important!
	//matrixMult4x4(M, rotYMatrix, transMatrix);	// This is backwards, unless you want orbiting!

	matrixMult4x4(tempMatrix1, transMatrix, scaleMatrix);	// Scale, then rotate...
	matrixMult4x4(M, rotYMatrix, tempMatrix1);	// ... then multiply THAT by the translate
		
	// Important! Pass that data to the shader variables
	//glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, M);
	glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, V);
	glUniformMatrix4fv(perspectiveMatrixID, 1, GL_TRUE, P);
	glUniform3fv(emitterID, 1, emitter_pos);
	
	/* Render Particles. Enabling point re-sizing in vertex shader */
	glEnable (GL_PROGRAM_POINT_SIZE);
	glEnable (GL_BLEND);
	//glActiveTexture (GL_TEXTURE0);
	//glBindTexture (GL_TEXTURE_2D, tex);
	glUseProgram (shaderProgramID);
	current_seconds=Timer();
	/* update time in shaders */
	glUniform1f (timeID, (GLfloat)current_seconds);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#ifdef USING_INDEX_BUFFER
	glDrawElements (GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, NULL);
#else
	//glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);

	glBindVertexArray (vao);
	// draw points 0-3 from the currently bound VAO with current in-use shader
	glDrawArrays (GL_POINTS, 0, PARTICLE_COUNT);
	glDisable (GL_BLEND);
	glDisable (GL_PROGRAM_POINT_SIZE);
	l=clock();

#endif
	glutSwapBuffers();
	glutPostRedisplay();		// NEW! This calls "render" again, allowing for animation!
}

void keyboard(unsigned char key,int x,int y){
	switch(key){
		case'S':
		theta-=0.01f; break;
		case'D':
		theta+=0.01f; break;

}

}

//======================PARTICLE GENERATION=================================================

/* create initial attribute values for particles. return a VAO */
GLuint gen_particles () {
  float vv[PARTICLE_COUNT * 3]; // start velocities vec3
  float vt[PARTICLE_COUNT]; // start times
  float t_accum = 0.0f; // start time
  int j = 0;
  for (int i = 0; i < PARTICLE_COUNT; i++) {
    // start times
    vt[i] = t_accum;
    t_accum += 0.01f; // spacing for start times is 0.01 seconds
    // start velocities. randomly vary x and z components
    float randx = ((float)rand() / (float)RAND_MAX) * 1.0f - 0.5f;
    float randz = ((float)rand() / (float)RAND_MAX) * 1.0f - 0.5f;
    vv[j] = randx; // x
    vv[j + 1] = 2.0f; // y
    vv[j + 2] = randz; // z
    j+= 3;
}

  GLuint velocity_vbo;
  glGenBuffers (1, &velocity_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, velocity_vbo);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vv), vv, GL_STATIC_DRAW);
  
  GLuint time_vbo;
  glGenBuffers (1, &time_vbo);
  glBindBuffer (GL_ARRAY_BUFFER, time_vbo);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vt), vt, GL_STATIC_DRAW);
  
  GLuint vao;
  glGenVertexArrays (1, &vao);
  glBindVertexArray (vao);
  glBindBuffer (GL_ARRAY_BUFFER, velocity_vbo);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glBindBuffer (GL_ARRAY_BUFFER, time_vbo);
  glVertexAttribPointer (1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray (0);
  glEnableVertexAttribArray (1);
  
  return vao;
}

//============================================================================================

float Timer(){

	static clock_t t1=clock();
	clock_t  t2 = l;
	float time_elapsed = (1.005*(float)t2-(float)t1)/CLOCKS_PER_SEC;
	current = current + time_elapsed;
	t1=t2;
	return current;
}


int main (int argc, char** argv) {
	// Standard stuff...
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Index Buffers");
	glutReshapeFunc(changeViewport);
	
	glewInit();

	initMatrices(); // New <========================================
	vao=gen_particles();

/*#ifdef USING_INDEX_BUFFER


	
#endif*/

	// Make a shader
	char* vertexShaderSourceCode = readFile("vertexShader1.vsh");
	char* fragmentShaderSourceCode = readFile("fragmentShader1.fsh");
	GLuint vertShaderID = makeVertexShader(vertexShaderSourceCode);
	GLuint fragShaderID = makeFragmentShader(fragmentShaderSourceCode);
	shaderProgramID = makeShaderProgram(vertShaderID, fragShaderID);

	/*/ / Create the "remember all"
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Create the buffer, but don't load anything yet
	//glBufferData(GL_ARRAY_BUFFER, 7*NUM_VERTICES*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 6*NUM_VERTICES*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	
	// Load the vertex points
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3*NUM_VERTICES*sizeof(GLfloat), vertices);
	// Load the colors right after that
	//glBufferSubData(GL_ARRAY_BUFFER, 3*NUM_VERTICES*sizeof(GLfloat),4*NUM_VERTICES*sizeof(GLfloat), colors);
	glBufferSubData(GL_ARRAY_BUFFER, 3*NUM_VERTICES*sizeof(GLfloat),3*NUM_VERTICES*sizeof(GLfloat), normals);*/
	
	
/*#ifdef USING_INDEX_BUFFER
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_INDICES*sizeof(GLuint), indices, GL_STATIC_DRAW);
#endif*/
	

	// Find the position of the variables in the shader
	//ositionID = glGetAttribLocation(shaderProgramID, "v_i");
	//colorID = glGetAttribLocation(shaderProgramID, "elapsed_system_time");
	
	// ============ New! glUniformLocation is how you pull IDs for uniform variables===============
	perspectiveMatrixID = glGetUniformLocation(shaderProgramID, "P");
	viewMatrixID = glGetUniformLocation(shaderProgramID, "V");
	emitterID=glGetUniformLocation(shaderProgramID, "emitter_pos_wor");
	timeID=glGetUniformLocation(shaderProgramID, "elapsed_system_time");
	
	//=============================================================================================

	/*glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));
	glVertexAttribPointer(colorID, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));
	printf ("BUFFER_OFFSET is: %d\n", sizeof(vertices));*/

	glUseProgram(shaderProgramID);
	//glEnableVertexAttribArray(positionID);
	//glEnableVertexAttribArray(colorID);
	//glEnable(GL_DEPTH_TEST);
    //glClearColor( 0.0, 0.0, 0.0, 0.2 );

	//current_seconds=Timer();
	glutDisplayFunc(render);

	glutKeyboardFunc( keyboard );
	glutMainLoop();
	
	return 0;
}