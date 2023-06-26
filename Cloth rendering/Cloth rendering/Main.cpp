#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#define _USE_MATH_DEFINES
#include "Cloth.cpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define PI 3.1415926
using namespace std;

GLuint gProgram[2];
int gWidth, gHeight;

GLint modelingMatrixLoc[2];
GLint viewingMatrixLoc[2];
GLint projectionMatrixLoc[2];
GLint eyePosLoc[2];

Cloth cloth;

unsigned char* tex;
unsigned int clothTexture;    
vector<Vertex> gVertices;
vector<Indice> gIndices;
vector<Texture> gTextures;
vector<Normal> gNormals;
vector<Face> gFaces;

int faceSize;
vector<Vertex> objVertices; 
vector<Texture> objTextures;
vector<Normal> objNormals;
vector<Face> objFaces;

GLfloat* vertexData;  
GLfloat* normalData;
GLuint* indexData;

int indexDataSizeInBytes;
 
glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::mat4 modelingMatrix;

float camDistance = 75;
float camAngle = 90;
float camHeight = 25;
glm::vec3 eyePos(0, camHeight, camDistance);
    
int activeProgramIndex = 0; 
glm::vec3 cubePos = glm::vec3(0,0,0);
float cubeSize = 5.0f;
glm::vec3 cubeMin = cubePos - glm::vec3(cubeSize , cubeSize , cubeSize );
glm::vec3 cubeMax = cubePos + glm::vec3(cubeSize , cubeSize , cubeSize );
glm::vec3 up(0, 1.0f, 0);
float clothHeight = 15;
float ks = 25;     
float kd = 5.0f;       
float kShear = 50   ;   
int vertexDataSizeInBytes[2];
GLuint attribBuffer, indexBuffer;
GLuint gVertexAttribBuffer, gIndexBuffer;
GLint gInVertexLoc, gInNormalLoc;
int gVertexDataSizeInBytes, gNormalDataSizeInBytes;

bool ParseObj(const string& fileName)
{ 
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            stringstream str(curLine);
            GLfloat c1, c2, c3;
            GLuint index[9];
            string tmp;

            if (curLine.length() >= 2)
            {
                if (curLine[0] == 'v')
                {  
                    if (curLine[1] == 't') // texture
                    {
                        str >> tmp; // consume "vt"
                        str >> c1 >> c2;
                        gTextures.push_back(Texture(c1, c2));
                    }
                    else if (curLine[1] == 'n') // normal
                    {
                        str >> tmp; // consume "vn"
                        str >> c1 >> c2 >> c3;
                        gNormals.push_back(Normal(c1, c2, c3));
                    }      
                    else // vertex
                    {
                        str >> tmp; // consume "v"
                        str >> c1 >> c2 >> c3;
                        gVertices.push_back(Vertex(c1, c2, c3));
                    }
                }
                else if (curLine[0] == 'f') // face
                {           
                    str >> tmp; // consume "f"
                    char c;
                    int vIndex[3], nIndex[3], tIndex[3];
                    str >> vIndex[0]; str >> c >> c; // consume "//"
                    str >> nIndex[0];
                    str >> vIndex[1]; str >> c >> c; // consume "//"
                    str >> nIndex[1];
                    str >> vIndex[2]; str >> c >> c; // consume "//"
                    str >> nIndex[2];

                    for (int c = 0; c < 3; ++c)
                    {
                        vIndex[c] -= 1;
                        nIndex[c] -= 1;
                        tIndex[c] -= 1;
                    }

                    gFaces.push_back(Face(vIndex, tIndex, nIndex));
                }
                else
                {
                    cout << "Ignoring unidentified line in obj file: " << curLine << endl;
                }
            }

            //data += curLine;
            if (!myfile.eof())
            {
                //data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }



    /*assert(gVertices.size() == gNormals.size());*/

    return true;
}

bool ReadDataFromFile(
    const string& fileName, ///< [in]  Name of the shader file
    string&       data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    return true;
}

GLuint createVS(const char* shaderName)
{
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = {0};
    glGetShaderInfoLog(vs, 1024, &length, output);
    printf("VS compile log: %s\n", output);

	return vs;
}

GLuint createFS(const char* shaderName)
{
    string shaderSource;

    string filename(shaderName);
    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = {0};
    glGetShaderInfoLog(fs, 1024, &length, output);
    printf("FS compile log: %s\n", output);

	return fs;
}

void initShaders()   
{
	// Create the programs

    gProgram[0] = glCreateProgram();
	gProgram[1] = glCreateProgram();

	// Create the shaders for both programs

    GLuint vs1 = createVS("vert.glsl");
    GLuint fs1 = createFS("frag.glsl");

	GLuint vs2 = createVS("vert2.glsl");
	GLuint fs2 = createFS("frag2.glsl");

	// Attach the shaders to the programs

	glAttachShader(gProgram[0], vs1);
	glAttachShader(gProgram[0], fs1);

	glAttachShader(gProgram[1], vs2);
	glAttachShader(gProgram[1], fs2);

	// Link the programs

    glLinkProgram(gProgram[0]);
	GLint status;
	glGetProgramiv(gProgram[0], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	glLinkProgram(gProgram[1]);
	glGetProgramiv(gProgram[1], GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		cout << "Program link failed" << endl;
		exit(-1);
	}

	// Get the locations of the uniform variables from both programs

	for (int i = 0; i < 2; ++i)
	{
		modelingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "modelingMatrix");
		viewingMatrixLoc[i] = glGetUniformLocation(gProgram[i], "viewingMatrix");
		projectionMatrixLoc[i] = glGetUniformLocation(gProgram[i], "projectionMatrix");
		eyePosLoc[i] = glGetUniformLocation(gProgram[i], "eyePos");
	}
}

void clearArrays()
{
    delete[] vertexData;
    delete[] normalData;
    delete[] indexData;
    gVertices.clear();
    gNormals.clear();
    gIndices.clear();
}
void bufferData()
{
    glBindBuffer(GL_ARRAY_BUFFER, attribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
    glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));
}


    


void initVBO2(int index)
{
    GLuint vao;
    glGenVertexArrays(1, &vao);
    assert(vao > 0);
    glBindVertexArray(vao);
    cout << "vao = " << vao << endl;

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    //assert(glGetError() == GL_NONE);

    glGenBuffers(1, &attribBuffer);
    glGenBuffers(1, &indexBuffer);

    //assert(attribBuffer > 0 && indexBuffer > 0);

    glBindBuffer(GL_ARRAY_BUFFER, attribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    gVertexDataSizeInBytes = gVertices.size() * 3 * sizeof(GLfloat);
    gNormalDataSizeInBytes = gNormals.size() * 3 * sizeof(GLfloat);

    int indexDataSizeInBytes = gFaces.size() * 3 * sizeof(GLuint);
    vertexDataSizeInBytes[index] = gVertexDataSizeInBytes;
    faceSize = gFaces.size();
    GLfloat* vertexData = new GLfloat[gVertices.size() * 3];
    GLfloat* normalData = new GLfloat[gNormals.size() * 3];
    GLuint* indexData = new GLuint[gFaces.size() * 3];

    float minX = 1e6, maxX = -1e6;
    float minY = 1e6, maxY = -1e6;
    float minZ = 1e6, maxZ = -1e6;

    for (int i = 0; i < gVertices.size(); ++i)
    {
        vertexData[3 * i] = gVertices[i].pos.x;
        vertexData[3 * i + 1] = gVertices[i].pos.y;
        vertexData[3 * i + 2] = gVertices[i].pos.z;

        minX = std::min(minX, gVertices[i].pos.x);
        maxX = std::max(maxX, gVertices[i].pos.x);
        minY = std::min(minY, gVertices[i].pos.y);
        maxY = std::max(maxY, gVertices[i].pos.y);
        minZ = std::min(minZ, gVertices[i].pos.z);
        maxZ = std::max(maxZ, gVertices[i].pos.z);
    }
    for (int i = 0; i < gNormals.size(); ++i)
    {
        normalData[3 * i] = gNormals[i].x;
        normalData[3 * i + 1] = gNormals[i].y;
        normalData[3 * i + 2] = gNormals[i].z;
    }

    for (int i = 0; i < gFaces.size(); ++i)
    {
        indexData[3 * i] = gFaces[i].vIndex[0];
        indexData[3 * i + 1] = gFaces[i].vIndex[1];
        indexData[3 * i + 2] = gFaces[i].vIndex[2];
    }        

     
    glBufferData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes + gNormalDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, gVertexDataSizeInBytes, vertexData);
    glBufferSubData(GL_ARRAY_BUFFER, gVertexDataSizeInBytes, gNormalDataSizeInBytes, normalData);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indexData, GL_STATIC_DRAW);
    delete[] vertexData;
    delete[] normalData;
    delete[] indexData;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(gVertexDataSizeInBytes));        
}

void loadTexture()
{
    int width, height, nrChannels;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    tex = stbi_load("cloth.jpg", &width, &height, &nrChannels, 0);
    
    glGenTextures(1, &clothTexture);
    glBindTexture(GL_TEXTURE_2D, clothTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);
    glGenerateMipmap(GL_TEXTURE_2D);

}
   
void generateCloth() {
    cloth = Cloth(15, 15, 25);
    cloth.initVBO();
    cloth.bufferData();
    cloth.SetFixedVertices();
    cloth.bind();
}
void init()
{ 
         
     
    glEnable(GL_DEPTH_TEST); 

    loadTexture();    
    //ParseObj("cube.obj");        
    ParseObj("cube.obj");    
    cout << gVertices.size() << endl;
    cout << gNormals.size() << endl;
    cout << gFaces.size() << endl;
    initVBO2(0);
    clearArrays(); 

    // You can change the size of the cloth here, the 3rd parameter is resolution.
    generateCloth();
      

    initShaders();
    
}
 


void drawModel2(int size)
{
    glBindBuffer(GL_ARRAY_BUFFER, attribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexDataSizeInBytes[0]));

    glDrawElements(GL_TRIANGLES, faceSize * 3, GL_UNSIGNED_INT, 0);
}

static float angle = 0;     
glm::mat4 scaleMat = glm::scale(glm::mat4(1), cubeSize*glm::vec3(1, 1, 1));


int k1, k2;
void display() 
{
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClearDepth(1.0f);  
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	

	float angleRad = (float) (angle / 180.0) * M_PI;
	
    glm::mat4 matT = glm::translate(glm::mat4(1.0), cubePos);  
    glm::mat4 matRz = glm::rotate<float>(glm::mat4(1.0), (angle / 180.) * M_PI, glm::vec3(1, 0, 0));
    
    activeProgramIndex = 1; 
    angle += 0.1f;
    modelingMatrix = matT *scaleMat;
	glUseProgram(gProgram[activeProgramIndex]);
	glUniformMatrix4fv(projectionMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(viewingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
	glUniformMatrix4fv(modelingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
	glUniform3fv(eyePosLoc[activeProgramIndex], 1, glm::value_ptr(eyePos));
    drawModel2(0);   

    matT = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0.0f));
    glm::mat4 matRx = glm::rotate<float>(glm::mat4(1.0), (0. / 180.) * M_PI, glm::vec3(1.0, 0.0, 0.0));
    modelingMatrix = matT * matRx;
    activeProgramIndex = 0;
    glUseProgram(gProgram[activeProgramIndex]);
    glUniformMatrix4fv(projectionMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(viewingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(viewingMatrix));
    glUniformMatrix4fv(modelingMatrixLoc[activeProgramIndex], 1, GL_FALSE, glm::value_ptr(modelingMatrix));
    glUniform3fv(eyePosLoc[activeProgramIndex], 1, glm::value_ptr(eyePos));
    

    
    cloth.draw();
    
    


}                

void reshape(GLFWwindow* window, int w, int h)
{
    w = w < 1 ? 1 : w;
    h = h < 1 ? 1 : h;

    gWidth = w;
    gHeight = h;

    glViewport(0, 0, w, h);
       
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glOrtho(-10, 10, -10, 10, -10, 10);
    //gluPerspective(45, 1, 1, 100);

	// Use perspective projection

	float fovyRad = (float) (45.0 / 180.0) * M_PI;
	projectionMatrix = glm::perspective(fovyRad, 1.0f, 1.0f, 500.0f);

	// Assume default camera position and orientation (camera is at
	// (0, 0, 0) with looking at -z direction and its up vector pointing
	// at +y direction)

    viewingMatrix = glm::lookAt(eyePos, glm::vec3(0,0,0),up);

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
}

bool paused = false ;   
bool lineMode = false;
float angleX = 0;
void lookAt() 
{ 
    viewingMatrix = glm::lookAt(glm::vec3(camDistance * cos(PI / 180 * camAngle), eyePos.y, camDistance * sin(PI / 180 * camAngle)), cubePos, up);
}
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) 
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    else if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        lineMode = !lineMode;   
        if(lineMode)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        paused = !paused;
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        camAngle += 10;
        lookAt();
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        camAngle -= 10;
        lookAt();
    }
    else if (key == GLFW_KEY_J && action == GLFW_PRESS)
    {
        cloth.SwitchType(FourCorners);
    } 
    else if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        cloth.SwitchType(TwoCorners);
    }
    else if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        cloth.SwitchType(Drop); 
        paused = false;
    }
    else if (key == GLFW_KEY_R)
    {
        cubePos += glm::vec3(0, 0, 0.1f);
        cubeMin += glm::vec3(0, 0, 0.1f);
        cubeMax += glm::vec3(0, 0, 0.1f);
    }
    else if (key == GLFW_KEY_F)
    {
        cubePos -= glm::vec3(0, 0, 0.1f);
        cubeMin -= glm::vec3(0, 0, 0.1f);
        cubeMax -= glm::vec3(0, 0, 0.1f);
    }

}

void mainLoop(GLFWwindow* window)
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        if (paused) continue; 
        display();
        glfwSwapBuffers(window);
        cloth.ApplyAllForces();                          
        cloth.bufferData();
    }    
}

int main(int argc, char** argv)   // Create Main Function For Bringing It All Together
{
    GLFWwindow* window;
    if (!glfwInit())
    {
        exit(-1);
    }                

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int width = 640, height = 480;
    window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

    

    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }
      
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
     
    char rendererInfo[512] = {0};
    strcpy_s(rendererInfo, (const char*) glGetString(GL_RENDERER));
    strcat_s(rendererInfo, " - ");
    strcat_s(rendererInfo, (const char*) glGetString(GL_VERSION));
    glfwSetWindowTitle(window, rendererInfo);

    init();

    glfwSetKeyCallback(window, keyboard);
    glfwSetWindowSizeCallback(window, reshape); 

    
    reshape(window, width, height); // need to call this once ourselves
    mainLoop(window); // this does not return unless the window is closed

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
