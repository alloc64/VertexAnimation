#ifndef _XMVERTEXANIMATION_H
#define _XMVERTEXANIMATION_H

#include "main.h"

struct xmaFrame
{
    vec3 * vertex;
    vec3 * normal;
    vec3 * tangent;
    GLuint uNormalVBO;
    GLuint uTangentVBO;
    vec3 center;
};

struct xmaPerVertexObject
{
    xmFace *face;
    xmaFrame *frame;
    vec2 *texcoord;
    vec3 *animVertices;
    
    vec3 min;
    vec3 max;
    
    float radius;
    short int iNumFaces;
    short int iNumVertices;  
    short int textureFilter;    
    
    char name[256]; 
    char texturePath[1024]; 
    
    GLuint uDiffuseMap;
    GLuint uNormalMap;
    GLuint uSpecularMap;
    
    GLuint uVertexVBO;
    GLuint uTexCoordVBO;
    GLuint uFaceVBO;
    
};

class xmVertexAnimation
{
    
    public:
        xmVertexAnimation();
        ~xmVertexAnimation();
        
        bool Load(char * path);
        void Animate(int o);
        bool SetSequence(short int start, short int end, bool animate);
        bool SetDefaultSequence(bool animate);
        int GetHealth(void)
        {
            return (int) iHealth;
        }
        void Draw(void);
        
        xmaPerVertexObject *object;
        
        short int iNumObjects;
        short int iLastFrame; 
        short int iFirstFrame;
        short int iFramerate;
        short int iHealth;
        short int iCurrFrame;
        vec3 BoxVertices[36];
        vec3 min;
        vec3 max;
        vec3 center;
        float radius;
        float dt;
        bool bAnim;
        
        
    private:
        char headerTitle[255];
        char header[7];

    };

#endif
