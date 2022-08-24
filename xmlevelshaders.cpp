#include "main.h"

Shader shader;
extern xmTexture texture;

void xmLevelShaders::EnableShader(GLuint shadowMapID, int numVisibleLights)
{
    glUseProgramObjectARB(LightingPH);
    glUniform1iARB(iUniformNumVisibleLights, numVisibleLights);

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glBindTexture(GL_TEXTURE_2D, shadowMapID);
    glUniform1iARB(iUniformShadowMap, 0);
    glActiveTextureARB(GL_TEXTURE0_ARB);
}

void xmLevelShaders::BindTextures(GLuint tex, GLuint normalTex, GLuint specularTex)
{
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1iARB(iUniformDiffuseMap, 1); 

    glActiveTextureARB(GL_TEXTURE2_ARB);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    glUniform1iARB(iUniformNormalMap, 2);

    glActiveTextureARB(GL_TEXTURE3_ARB);
    glBindTexture(GL_TEXTURE_2D, specularTex);
    glUniform1iARB(iUniformSpecularMap, 3);
}

void xmLevelShaders::DisableShader(void)
{
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glUseProgramObjectARB(0);
}

bool xmLevelShaders::CreateLevelLighting()
{
    const char *vertexShader = "\n\
       \n\
       varying vec4 position;\n\
       varying vec4 normal;\n\
       varying vec3 t;\n\
       varying vec3 b;\n\
       attribute vec3 tangent;\n\
       \n\
       %s\n\
       %s\n\
       \n\
       \n\
       void main(void)\n\
       {\n\
            gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
            gl_TexCoord[0] = gl_MultiTexCoord0;\n\
            \n\
            position = gl_ModelViewMatrix *gl_Vertex;\n\
            normal = vec4(normalize(gl_NormalMatrix * gl_Normal.xyz), -position.z);\n\
            \n\
            #ifdef _BUMPMAPPING\n\
                t = normalize(vec3(gl_NormalMatrix * tangent));\n\
                b = cross(normal.xyz, t);\n\
            #endif\n\
            \n\
            #ifdef _SHADOWS\n\
                gl_TexCoord[7] = gl_TextureMatrix[7] * gl_Vertex;\n\
            #endif\n\
            gl_TexCoord[1] = gl_TextureMatrix[1] * gl_Vertex;\n\
        }\n\
    ";    
     
    const char *fragmentShader = "\n\
    \n\
    varying vec4 position;\n\
    varying vec4 normal;\n\
    varying vec3 t;\n\
    varying vec3 b;\n\
    \n\
    uniform sampler2D DiffuseMap;\n\
    uniform sampler2D NormalMap;\n\
    uniform sampler2D SpecularMap;\n\
    uniform sampler2D FlashLightMap;\n\
    uniform sampler2DShadow ShadowMap;\n\
    uniform int NumVisibleLights;\n\
    uniform int isFlashlightActive;\n\
    \n\
    #define SPOT_LIGHT 0\n\
    #define POINT_LIGHT 1\n\
    %s\n\
    %s\n\
    %s\n\
    \n\
    vec4 ApplyFlashlight(in vec4 texCoord)\n\
    {\n\
        if(texCoord.q > 0.0 && isFlashlightActive == 1)\n\
        {\n\
            return texture2DProj(FlashLightMap, texCoord.xyz);\n\
        }else{\n\
            return vec4(vec3(0.0), 1.0);\n\
        }\n\
    }\n\
    \n\
    vec4 ApplyMainLight(in vec3 vertexPosition, in vec4 normal, in vec4 color)\n\
    {\n\
        vec4 diffuse, specular;\n\
        float gloss = 1.0;\n\
        vec3 lightPosition = gl_LightSource[0].position.xyz;\n\
        vec4 lightAmbient = vec4(0.2, 0.2, 0.2, 1.0);\n\
        vec4 lightDiffuse = vec4(%f, %f, %f, 1.0);\n\
        \n\
        #ifdef _BUMPMAPPING\n\
            vec3 p = -position.xyz;\n\
            vec3 l = lightPosition.xyz - vertexPosition.xyz;\n\
            vec3 lightDir = normalize(vec3(dot(l, t), dot(l, b), dot(l, normal.xyz)));\n\
            vec3 eyeDir   = normalize(vec3(dot(p, t), dot(p, b), dot(p, normal.xyz)));\n\
            vec3 halfDir  = normalize(lightDir+eyeDir);\n\
            normal.xyz = texture2D(NormalMap, gl_TexCoord[0].xy).xyz * 2.0 - 1.0;\n\
            gloss = texture2D(SpecularMap, gl_TexCoord[0].xy).a;\n\
        #else\n\
            vec3 lightDir = normalize(lightPosition.xyz - vertexPosition.xyz);\n\
            vec3 eyeDir   = vec3(0.0, 0.0, 1.0);\n\
            vec3 halfDir  = gl_LightSource[0].halfVector.xyz;\n\
        #endif\n\
        \n\
        float NdotL = max(dot(lightDir, normal.xyz), 0.0);\n\
        vec4 ambient = lightAmbient * color;\n\
        diffuse = lightDiffuse * color * NdotL;\n\
        specular = lightDiffuse * gloss * pow(max(dot(reflect(-lightDir, normal.xyz), lightDir), 0.0), 32.0);\n\
        \n\
        return vec4(clamp(ambient + diffuse + specular, 0.0, 1.0).rgb, 1.0);\n\
    }\n\
    \n\
    vec4 ApplyPointLight(in vec3 vertexPosition, in vec4 normal, int isFrontFacing, int i)\n\
    {\n\
        float radius = gl_LightSource[i].diffuse.w;\n\
        vec3 lightDir = (gl_LightSource[i].position.xyz - position.xyz) / radius;\n\
        vec3 viewDir = vec3(0.0, 0.0, 1.0);\n\
        \n\
        float atten = max(0.0, 1.0 - dot(lightDir, lightDir));\n\
        if(atten <= 0.01) return vec4(vec3(0.0), 1.0);\n\
        lightDir = normalize(lightDir);\n\
        \n\
        float NdotL = max(dot(lightDir, normal.xyz*isFrontFacing), 0.0);\n\
        vec3 diffuse = gl_LightSource[i].diffuse.xyz * vec3(NdotL);\n\
        vec3 specular = diffuse * pow(max(dot(normalize(lightDir+viewDir).xyz, normal.xyz*isFrontFacing), 0.0), 128.0);\n\
        return vec4((diffuse + specular) * atten, 1.0);\n\
    }\n\
    \n\
    vec4 ApplySpotLight(in vec3 vertexPosition, in vec4 normal, int isFrontFacing, int i)\n\
    {\n\
        float falloff = gl_LightSource[i].diffuse.g;\n\
        float hotspot = gl_LightSource[i].ambient.b;\n\
        vec3 lightDir = normalize(gl_LightSource[i].position.xyz - position.xyz);\n\
	    vec3 spotDir = normalize(gl_LightSource[i].spotDirection.xyz);\n\
        vec3 viewDir = vec3(0.0, 0.0, 1.0);\n\
        float spot = 1.0 - clamp((dot(-lightDir, spotDir) - falloff) / (hotspot - falloff), 0.0, 1.0);\n\
        \n\
        if(spot <= 0.01) return vec4(vec3(0.0), 1.0);\n\
        \n\
        float NdotL = max(dot(lightDir, normal.xyz*isFrontFacing), 0.0);\n\
        vec3 diffuse = gl_LightSource[i].diffuse.xyz * vec3(NdotL);\n\
        vec3 specular = diffuse * pow(max(dot(normalize(lightDir+viewDir).xyz, normal.xyz*isFrontFacing), 0.0), 128.0);\n\
        return vec4((diffuse + specular) * spot, 1.0);\n\
    }\n\
    \n\
    void main(void)\n\
    {\n\
        vec4 lightColor = vec4(0.0);\n\
        vec4 color = texture2D(DiffuseMap, gl_TexCoord[0].st);\n\
        if(color.a < 0.1) discard;\n\
        vec4 mainLight = ApplyMainLight(position.xyz, normal, color);\n\
        vec4 flashLight = ApplyFlashlight(gl_TexCoord[1]);\n\
        float shadow = 1.0;\n\
        #ifdef _SHADOWS\n\
            shadow = clamp(shadow2DProj(ShadowMap, gl_TexCoord[7]).x+0.3, 0.0, 1.0);\n\
        #endif\n\
        int isFrontFacing = -(1 - int(gl_FrontFacing)*2);\n\
        for(int i=1; i < 8; i++)\n\
        {\n\
            if(i >= NumVisibleLights+1) break;\n\
            int type = int(gl_LightSource[i].ambient.r);\n\
            //if(type == SPOT_LIGHT)\n\
            //{\n\
            //    lightColor += ApplySpotLight(position.xyz, normal, isFrontFacing, i);\n\
            //    continue;\n\
            //}\n\
            \n\
            if(type == POINT_LIGHT)\n\
            {\n\
                lightColor += ApplyPointLight(position.xyz, normal, isFrontFacing, i);\n\
                continue;\n\
            }\n\
        }\n\
        gl_FragData[0] = color;//((mainLight * shadow) + flashLight) + lightColor * color;\n\
        gl_FragData[1] = vec4(-normal.xyz, normal.w);\n\
    }\n\
    ";
     char * fVS = new char[strlen(vertexShader)*10];
     char * fFS = new char[strlen(fragmentShader)*10];

    sprintf(fVS, vertexShader, "#define _BUMPMAPPING", "#define _SHADOWS");
     sprintf(fFS, fragmentShader,
             "#define _BUMPMAPPING",
             "#define _SHADOWS",
                               "#define _AO",
                               1.0,
                               1.0,
                               1.0);
     
     GLhandleARB LightingVS = shader.LoadShaderFromArray("xmLevel lighting vertex shader", (const GLcharARB**)&fVS, GL_VERTEX_SHADER_ARB);
     GLhandleARB LightingFS = shader.LoadShaderFromArray("xmLevel lighting fragment shader", (const GLcharARB**)&fFS, GL_FRAGMENT_SHADER_ARB);
     
     delete fVS;
     delete fFS;
     
     if(!LightingVS || !LightingFS)
     {
         printf("error: couldn't create vertex || fragment shader handles!\n");   
         return false;
     }
     
     if(!(LightingPH = glCreateProgramObjectARB()))
     {
         printf("error: cannot create shader handle!\n");
         return false;            
     }
     
     glAttachObjectARB(LightingPH, LightingVS);
     glAttachObjectARB(LightingPH, LightingFS);
     glLinkProgramARB(LightingPH);  
     glValidateProgramARB(LightingPH);
     
     GLint ok = false;
     glGetObjectParameterivARB(LightingPH, GL_OBJECT_VALIDATE_STATUS_ARB, &ok);
     if (!ok)
     {
         int maxLength=4096;
         char *infoLog = new char[maxLength];
         glGetInfoLogARB(LightingPH, maxLength, &maxLength, infoLog);
         printf("error: shader validation failed!\n%s\n", infoLog);
         delete []infoLog;
     } 
     
     iUniformShadowMap          = glGetUniformLocationARB(LightingPH, "ShadowMap");
     iUniformDiffuseMap         = glGetUniformLocationARB(LightingPH, "DiffuseMap");
     iUniformNormalMap          = glGetUniformLocationARB(LightingPH, "NormalMap");
     iUniformSpecularMap        = glGetUniformLocationARB(LightingPH, "SpecularMap");
     iUniformNumVisibleLights   = glGetUniformLocationARB(LightingPH, "NumVisibleLights");
     iUniformFlashLightMap      = glGetUniformLocationARB(LightingPH, "FlashLightMap");
     iUniformIsFlashlightActive = glGetUniformLocationARB(LightingPH, "isFlashlightActive");
     iUniformTangent           = glGetAttribLocationARB(LightingPH, "tangent");

     
     return true;
        
}    

