#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "C:\OpenglLib\freeglut\include\GL\freeglut.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

class ObjLoader
{
public:
    struct vertex
    {
        float x;
        float y;
        float z;
    };
    int vi=0, vti=0, vni=0;
    GLuint textures[1];
    cv::Mat grassImg;
    std::string texturePath;
    ObjLoader(string filename, string texturePath) {
        ifstream file(filename);
        if (!file.is_open()) {
            printf("Error opening file!\n");
        }
        string line;
        while (getline(file, line))
        {
            if(line.substr(0, 2) == "vt"){
                float x, y;
                stringstream ss(line.substr(2));
                ss >> x; ss >> y;
                vector<GLfloat> Point;
                Point.push_back(x);
                Point.push_back(y);
                vt.push_back(Point);
            }
            else if(line.substr(0, 2) == "vn"){
                float x, y, z;
                stringstream ss(line.substr(2));
                ss >> x; ss >> y; ss >> z;
                vector<GLfloat> Point;
                Point.push_back(x);
                Point.push_back(y);
                Point.push_back(z);
                vn.push_back(Point);
            }
            else if (line.substr(0, 2) == "v ")
            {
                // vi++;
                vector<GLfloat> Point;
                GLfloat x, y, z;
                istringstream s(line.substr(2));
                s >> x; s >> y; s >> z;
                Point.push_back(x);
                Point.push_back(y);
                Point.push_back(z);
                v.push_back(Point);

                if (x > maxX) {
                    maxX = x;
                }else if (x < minX) {
                    minX = x;
                }

                if (y > maxY) {
                    maxY = y;
                }else if (y < minY) {
                    minY = y;
                }

                if (z > maxZ) {
                    maxZ = z;
                }else if (z < minZ) {
                    minZ = z;
                }
            }
            else if (line.substr(0, 2) == "f ")
            {
                vector<GLint> vIndexSets;
                vector<GLint> vtIndexSets;
                vector<GLint> vnIndexSets;

                std::istringstream iss(line);
                std::string part;
                iss >> part; // Skip the initial 'f'

                while (iss >> part) {
                    std::istringstream partStream(part);
                    std::string vNumber, vtNumber, vnNumber;
                    std::getline(partStream, vNumber, '/');
                    std::getline(partStream, vtNumber, '/');
                    std::getline(partStream, vnNumber, '/');
                    vIndexSets.push_back(std::stoi(vNumber) - 1);
                    vtIndexSets.push_back(std::stoi(vtNumber) - 1);
                    vnIndexSets.push_back(std::stoi(vnNumber) - 1);
                }
                f.push_back(vIndexSets);
                fvt.push_back(vtIndexSets);
                fvn.push_back(vnIndexSets);
            }
            // else: ignore
        }
        file.close();
        srand(time(NULL));

        grassImg = cv::imread(texturePath); 
        if (grassImg.empty()) {
           std::cout << "grassImg empty\n";
        }
        else {
            cv::flip(grassImg, grassImg, 0);
            glGenTextures(1, &textures[0]);
            glBindTexture(GL_TEXTURE_2D, textures[0]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassImg.cols, grassImg.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, grassImg.ptr());
        }
    }

    void setTexture(std::string filename) {
        texturePath  = filename;
    }

    void translatedInit() {
        float cenX = (maxX + minX) / 2;
        float cenY = (maxY + minY) / 2;
        float cenZ = (maxZ + minZ) / 2;
        glTranslated(0.0-cenX, 0.0-cenY, 0);
    }
    void scaleInit() {

        float s;
        float sX = (maxX - minX);
        float sY = (maxY - minY);
        float sZ = (maxZ - minZ);
        if (sX > sY && sX > sZ)
            s = sX;
        else if (sY > sX && sY > sZ)
            s = sY;
        else    //sZ > sX && sZ > sY)
            s = sZ;

        s = 38 / s;

        glScalef(s, s, s);
    }
    void rotate() {
        glRotatef(rotateAngleX, 1, 0, 0);
        glRotatef(rotateAngleY, 0, 1, 0);
        glRotatef(rotateAngleZ, 0, 0, 1);
    }
    void rotate(int index) {
        // 0: X, 1: Y, 2: Z
        switch (index) {
        case 0:
            rotateAngleX += 30;
            break;
        case 1:
            rotateAngleY += 30;
            break;
        case 2:
            rotateAngleZ += 30;
            break;
        }
    }
    void scale() {
        glScalef(2, 2, 2);
    }
    void translate() {
        glTranslated(posX, posY, posZ);
    }
    void translate(int index) {
        // 0,1: x, 2,3: y, 4,5: z
        switch (index) {
        case 0:
            posX++;
            break;
        case 1:
            posX--;
            break;
        case 2:
            posY++;
            break;
        case 3:
            posY--;
            break;
        case 4:
            posZ++;
            break;
        case 5:
            posZ--;
            break;
        }
    }
    void init() {
        if (grassImg.empty()) {
            std::cout << "grassImg empty\n";
        }
        else {
            glBindTexture(GL_TEXTURE_2D, textures[0]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassImg.cols, grassImg.rows, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, grassImg.ptr());
        }
    }
    void resetPos() {
        rotateAngleX = 0, rotateAngleY = 0, rotateAngleZ = 0;
        posX = 0, posY = 0, posZ = 0;
        cameraAt[0] = 1.0;
        cameraAt[1] = 1.0;
        cameraAt[2] = 10.0;
        cameraLookAt[0] = 0.0;
        cameraLookAt[1] = 0.0;
        cameraLookAt[2] = 0.0;
    }
    void draw(int shadowMode) {
        switch (renderMode) {
            case 0:
                drawModePoint();
                break;
            case 1:
                drawModeLine();
                break;
            case 2:
                drawModeFace(shadowMode);
                break;
        }
    }

    void setRenderMode(int mode) {
        renderMode = mode;
    }

    void setColorMode(int mode) {
        colorMode = mode;
        if (mode != 0) {
            RandomColor[0] = (double)rand() / (RAND_MAX);
            RandomColor[1] = (double)rand() / (RAND_MAX);
            RandomColor[2] = (double)rand() / (RAND_MAX);
        }
    }

    void setCameraAt(int index, int count) {
        // index: x: 0, y: 1, z: 2
        // count: +1/-1
        cameraAt[index]+=count;
    }

    void setCameraLookAt(int index, int count) {
        // index: x: 0, y: 1, z: 2
        // count: +1/-1
        cameraLookAt[index]+=count;
    }

private:
    vector<vector<GLfloat>> v;
    vector<vector<GLint>> f;
    vector<vector<GLfloat>> vt;
    vector<vector<GLint>> fvt;
    vector<vector<GLfloat>> vn;
    vector<vector<GLint>> fvn; 
    float maxX=0, maxY=0, maxZ=0;
    float minX=0, minY=0, minZ=0;
    int renderMode = 2;     //0: point, 1: line, 2: face
    int rotateAngleX = -90, rotateAngleY = 0, rotateAngleZ = 0;
    float posX = -1.0, posY = 0.0, posZ = -0.03;
    int colorMode = 0;      //0: default, 1: random
    double defaultColorPoint[3] = { 0.3, 0.3, 0.6 },
        defaultColorLine[3] = { 0.6, 0.3, 0.0 },
        defaultColorFace[3] = { 1.0, 1.0, 1.0 },
        RandomColor[3];
    double cameraAt[3] = { 1.0, 1.0, 10.0 }, 
        cameraLookAt[3] = { 0.0, 0.0, 0.0 };

    void drawCube() {
        glLineWidth(1.0f);
        glBegin(GL_LINES);

        glColor3f(0.8, 0.5, 0.5);
        glVertex3f(maxX, maxY, maxZ);// 1
        glVertex3f(maxX, minY, maxZ);// 2

        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(maxX, minY, maxZ);// 2
        glVertex3f(minX, minY, maxZ);// 3

        glVertex3f(minX, minY, maxZ);// 3
        glVertex3f(minX, maxY, maxZ);// 4

        glVertex3f(minX, maxY, maxZ);// 4
        glVertex3f(maxX, maxY, maxZ);// 1

        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(maxX, maxY, maxZ);// 1
        glVertex3f(maxX, maxY, minZ);// 5

        glVertex3f(maxX, maxY, minZ);// 5
        glVertex3f(maxX, minY, minZ);// 6

        glVertex3f(maxX, minY, minZ);// 6
        glVertex3f(maxX, minY, maxZ);// 2

        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(maxX, maxY, minZ);// 5
        glVertex3f(minX, maxY, minZ);// 7

        glVertex3f(minX, maxY, minZ);// 7
        glVertex3f(minX, minY, minZ);// 8

        glVertex3f(minX, minY, minZ);// 8
        glVertex3f(maxX, minY, minZ);// 6

        glColor3f(0.8, 0.8, 0.0);
        glVertex3f(minX, maxY, minZ);// 7
        glVertex3f(minX, maxY, maxZ);// 4

        glVertex3f(minX, minY, minZ);// 8
        glVertex3f(minX, minY, maxZ);// 3

        glEnd();
    }
    void drawModePoint() {
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        if (colorMode == 0) {
            glColor3f(defaultColorPoint[0], defaultColorPoint[1], defaultColorPoint[2]);
        }
        else {
            glColor3f(RandomColor[0], RandomColor[1], RandomColor[2]);
        }
        
        for (int i = 0; i < v.size(); i++) {
            glVertex3f(v[i][0], v[i][1], v[i][2]);
        }
        glEnd();
    }
    void drawModeLine() {
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        if (colorMode == 0) {
            glColor3f(defaultColorLine[0], defaultColorLine[1], defaultColorLine[2]);
        }
        else {
            glColor3f(RandomColor[0], RandomColor[1], RandomColor[2]);
        }
        for (int i = 0; i < f.size(); i++) {
            //�T���I
            vertex a, b, c, normal;

            if ((f[i]).size() != 3) {
                cout << "ERRER::THE SIZE OF f IS NOT 3!" << endl;
            }
            else {
                GLint firstVertexIndex = (f[i])[0];
                GLint secondVertexIndex = (f[i])[1];
                GLint thirdVertexIndex = (f[i])[2];

                a.x = (v[firstVertexIndex])[0]; // 1
                a.y = (v[firstVertexIndex])[1];
                a.z = (v[firstVertexIndex])[2];

                b.x = (v[secondVertexIndex])[0]; // 2
                b.y = (v[secondVertexIndex])[1];
                b.z = (v[secondVertexIndex])[2];

                c.x = (v[thirdVertexIndex])[0]; // 3
                c.y = (v[thirdVertexIndex])[1];
                c.z = (v[thirdVertexIndex])[2];

                glVertex3f(a.x, a.y, a.z); 
                glVertex3f(b.x, b.y, b.z);

                glVertex3f(b.x, b.y, b.z);
                glVertex3f(c.x, c.y, c.z);

                glVertex3f(a.x, a.y, a.z);
                glVertex3f(c.x, c.y, c.z);
            }
        }

        glEnd();
    }
    void drawModeFace(int shadowMode) {
        glBegin(GL_TRIANGLES);
        if (shadowMode == 0) {
            glColor3f(defaultColorFace[0], defaultColorFace[1], defaultColorFace[2]);
        }
        else { // is shadow
            glColor4d(0.0, 0.0, 0.0, 0.6);
        }
        for (int i = 0; i < f.size(); i++) {
            vertex a, b, c, normala, normalb, normalc, normal;

            if ((f[i]).size() != 3) {
                cout << "ERRER::THE SIZE OF f IS NOT 3!" << endl;
            }
            else {
                GLint firstVertexIndex = (f[i])[0];
                GLint secondVertexIndex = (f[i])[1];
                GLint thirdVertexIndex = (f[i])[2];

                a.x = (v[firstVertexIndex])[0];
                a.y = (v[firstVertexIndex])[1];
                a.z = (v[firstVertexIndex])[2];

                b.x = (v[secondVertexIndex])[0];
                b.y = (v[secondVertexIndex])[1];
                b.z = (v[secondVertexIndex])[2];

                c.x = (v[thirdVertexIndex])[0];
                c.y = (v[thirdVertexIndex])[1];
                c.z = (v[thirdVertexIndex])[2];

                GLint firstNormalIndex = (fvn[i])[0];
                GLint secondNormalIndex = (fvn[i])[1];
                GLint thirdNormalIndex = (fvn[i])[2];

                normala.x = (vn[firstNormalIndex])[0];
                normala.y = (vn[firstNormalIndex])[1];
                normala.z = (vn[firstNormalIndex])[2];

                normalb.x = (vn[secondNormalIndex])[0];
                normalb.y = (vn[secondNormalIndex])[1];
                normalb.z = (vn[secondNormalIndex])[2];

                normalc.x = (vn[thirdNormalIndex])[0];
                normalc.y = (vn[thirdNormalIndex])[1];
                normalc.z = (vn[thirdNormalIndex])[2];

                normal.x = (normala.x + normalb.x + normalc.x) / 3;
                normal.y = (normala.y + normalb.y + normalc.y) / 3;
                normal.z = (normala.z + normalb.z + normalc.z) / 3;

                GLint firstTextureIndex = (fvt[i])[0];
                GLint secondTextureIndex = (fvt[i])[1];
                GLint thirdTextureIndex = (fvt[i])[2];

                glNormal3f(normal.x, normal.y, normal.z);
                glBindTexture(GL_TEXTURE_2D, textures[0]);
                glTexCoord2f( (vt[firstTextureIndex])[0], (vt[firstTextureIndex])[1]);
                glVertex3f(a.x, a.y, a.z);
                glTexCoord2f( (vt[secondTextureIndex])[0], (vt[secondTextureIndex])[1]);
                glVertex3f(b.x, b.y, b.z);
                glTexCoord2f( (vt[thirdTextureIndex])[0], (vt[thirdTextureIndex])[1]);
                glVertex3f(c.x, c.y, c.z);
                
            }
        }

        glEnd();
    }
};
