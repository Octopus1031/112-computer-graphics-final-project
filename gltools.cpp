#include "C:\OpenglLib\freeglut\include\GL\freeglut.h"
#include "gltools.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include "glee.h"
#include <cmath>

using namespace std;

typedef struct{
    GLubyte Header[12];     // TGA File Header
    GLint  width;           // Width of image
    GLint  height;          // Height of image
    GLint  bits;            // Bits per pixel
} TGAHEADER;

// Function to load a TGA file
GLbyte* gltLoadTGA(const char* szFileName, GLint* iWidth, GLint* iHeight, GLint* iComponents, GLenum* eFormat) {
    FILE* pFile = NULL;            // File pointer
    TGAHEADER tgaHeader;    // TGA file header
    unsigned long lImageSize;   // Size in bytes of image
    short sDepth;           // Pixel depth;
    GLbyte* pBits = NULL;    // Pointer to bits

    // Default return values
    *iWidth = 0;
    *iHeight = 0;
    *iComponents = GL_RGB;
    *eFormat = GL_RGB;

    // Attempt to open the file
     fopen_s(&pFile, szFileName, "rb");
    if(pFile == NULL) {
        return NULL;
    }

    // Read in header (binary)
    fread(&tgaHeader, 18/* sizeof(TGAHEADER)*/, 1, pFile);

    // Get width, height, and depth
    *iWidth = tgaHeader.width;
    *iHeight = tgaHeader.height;
    sDepth = tgaHeader.bits / 8;

    // Put some validity checks here. Very simply, I only understand
    // or care about 8, 24, or 32 bit targa's.
    if(tgaHeader.bits != 8 && tgaHeader.bits != 24 && tgaHeader.bits != 32)
        return NULL;

    // Calculate size of image buffer
    lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

    // Allocate memory and check for success
    pBits = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
    if(pBits == NULL)
        return NULL;

    // Read in the bits
    // Check for read error. This should catch RLE or other 
    // weird formats that I don't want to recognize
    if(fread(pBits, lImageSize, 1, pFile) != 1) {
        free(pBits);
        return NULL;
    }

    // Set OpenGL format expected
    switch(sDepth) {
        case 3:     // Most likely case
            *eFormat = GL_BGR;
            *iComponents = GL_RGB;
            break;
        case 4:
            *eFormat = GL_BGRA;
            *iComponents = GL_RGBA;
            break;
        case 1:
            *eFormat = GL_LUMINANCE;
            *iComponents = GL_LUMINANCE;
            break;
    }

    // Done with File
    fclose(pFile);

    // Return pointer to image data
    return pBits;
}

// Function to write a TGA file
// GLint gltWriteTGA(const char* szFileName) {
//     return 0; // Implement as needed
// }

// Function to draw a torus
// void gltDrawTorus(GLfloat majorRadius, GLfloat minorRadius, GLint numMajor, GLint numMinor) {
    
// }

// Function to draw a sphere
void gltDrawSphere(GLfloat fRadius, GLint iSlices, GLint iStacks) {
    GLUquadric* quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricTexture(quadric, GL_TRUE);
    gluSphere(quadric, fRadius, iSlices, iStacks);
    gluDeleteQuadric(quadric);
}

// Function to draw unit axes
void gltDrawUnitAxes(void) {
    glBegin(GL_LINES);
    // X Axis - Red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    // Y Axis - Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 1.0f, 0.0f);
    // Z Axis - Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
}

// Function to load a shader file
bool bLoadShaderFile(const char* szFile, GLhandleARB shader) {
    ifstream file(szFile);
    if (!file.is_open()) return false;

    stringstream shaderData;
    shaderData << file.rdbuf();
    string str = shaderData.str();
    const char* szShader = str.c_str();

    glShaderSourceARB(shader, 1, &szShader, NULL);
    return true;
}

// Function to load shader pair
GLhandleARB gltLoadShaderPair(const char* szVertexProg, const char* szFragmentProg) {
    GLhandleARB hVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    GLhandleARB hFragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    if (bLoadShaderFile(szVertexProg, hVertexShader) == false ||
        bLoadShaderFile(szFragmentProg, hFragmentShader) == false) {
        return 0;
    }

    glCompileShaderARB(hVertexShader);
    glCompileShaderARB(hFragmentShader);

    GLhandleARB hProgram = glCreateProgramObjectARB();
    glAttachObjectARB(hProgram, hVertexShader);
    glAttachObjectARB(hProgram, hFragmentShader);
    glLinkProgramARB(hProgram);

    return hProgram;
}

// Function to get OpenGL version
bool gltGetOpenGLVersion(int& nMajor, int& nMinor) {
    const char* version = (const char*)glGetString(GL_VERSION);
    if (sscanf_s(version, "%d.%d", &nMajor, &nMinor) != 2)
        return false;
    return true;
}

// Function to check if an extension is supported
int gltIsExtSupported(const char* szExtension) {
    const GLubyte* extensions = glGetString(GL_EXTENSIONS);
    if (strstr((const char*)extensions, szExtension) != NULL)
        return 1;
    return 0;
}

// Function to get extension pointer
void* gltGetExtensionPointer(const char* szFunctionName) {
    void* p = (void*)wglGetProcAddress(szFunctionName);
    if (p == NULL || (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) || (p == (void*)-1)) {
        static HMODULE hModule = NULL;
        if (hModule == NULL) {
            hModule = LoadLibraryA("opengl32.dll");
        }
        p = (void*)GetProcAddress(hModule, szFunctionName);
    }
    return p;
}

// Win32 specific function to check if WGL extension is supported
#ifdef WIN32
int gltIsWGLExtSupported(HDC hDC, const char* szExtension) {
    const char* extensions = (const char*)wglGetExtensionsStringARB(hDC);
    if (strstr(extensions, szExtension) != NULL)
        return 1;
    return 0;
}
#endif
