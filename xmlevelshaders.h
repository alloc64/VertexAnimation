#ifndef _XMLSHADERS_H
#define _XMLSHADERS_H

#include "main.h"

class xmLevelShaders
{
    public:
        bool CreateLevelLighting();   
        
        void EnableShader(GLuint shadowMapID, int numVisibleLights);
        void BindTextures(GLuint tex, GLuint normalTex, GLuint specularTex);
        void DisableShader(void); 
        
        GLuint iUniformTangent;
    
        GLhandleARB LightingPH;
        GLuint iUniformShadowMap;
        GLuint iUniformDiffuseMap;
        GLuint iUniformNormalMap;
        GLuint iUniformSpecularMap;
        GLuint iUniformModelViewMat;
        GLuint iUniformNumVisibleLights;
        GLuint iUniformFlashLightMap;
        GLuint iUniformIsFlashlightActive;
    private:
        
};

#endif
