#ifndef _SHADERS_H
#define _SHADERS_H

class Shader
{
    public:
        GLhandleARB LoadShaderFromArray(char * shaderName, const GLcharARB ** array, GLuint shaderType);

    private:
      
};


inline bool CheckExtension(char * ext)
{
   //printf("WTF glGetError() = %d!\n\n", glGetError());
   char *exts = (char*)glGetString(GL_EXTENSIONS);
   bool printed = false;
   while(!exts)
   {
         exts = (char*)glGetString(GL_EXTENSIONS);
         if(!printed)
         {
                printf("error: glGetError() = %d! EXT ERROR!\n", glGetError());
                printed = true;
         }
   }
   
   if(!strstr(exts, ext)) return false;
   
   return true;          
}

inline bool LoadExtensions()
{
    if(!CheckExtension("GL_ARB_shading_language_100") 
    || !CheckExtension("GL_ARB_shader_objects"))
    {
        printf("error: externsions GL_ARB_shading_language_100 or GL_ARB_shader_objects are unsupported!\n"); 
        return false;
    }
    
    glCreateProgramObjectARB           = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
    glDeleteObjectARB                  = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
    glUseProgramObjectARB              = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
    glCreateShaderObjectARB            = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
    glShaderSourceARB                  = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB"); 
    glCompileShaderARB                 = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB"); 
    glGetObjectParameterivARB          = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB"); 
    glAttachObjectARB                  = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB"); 
    glGetInfoLogARB                    = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
    glLinkProgramARB                   = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
    glValidateProgramARB	           = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgramARB");

	glGetUniformLocationARB            = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
    glUniformMatrix4fv                 = (PFNGLUNIFORMMATRIX4FVPROC)   wglGetProcAddress("glUniformMatrix4fv");   

    glUniform4fARB                     = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
    glUniform3fARB                     = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");
    glUniform1fARB                     = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
    glUniform1iARB                     = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
    
    glGetAttribLocationARB             = (PFNGLGETATTRIBLOCATIONARBPROC)wglGetProcAddress("glGetAttribLocationARB");
    glVertexAttrib3fARB                = (PFNGLVERTEXATTRIB3FARBPROC)wglGetProcAddress("glVertexAttrib3fARB");
    
    glVertexAttribPointerARB           = (PFNGLVERTEXATTRIBPOINTERARBPROC)wglGetProcAddress("glVertexAttribPointerARB");
    glEnableVertexAttribArrayARB       = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC)wglGetProcAddress("glEnableVertexAttribArrayARB");
    glDisableVertexAttribArrayARB      = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC)wglGetProcAddress("glDisableVertexAttribArrayARB");
    glActiveTextureARB                 = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
    glClientActiveTextureARB           = (PFNGLACTIVETEXTUREARBPROC)   wglGetProcAddress("glClientActiveTextureARB");
    glActiveTexture                    = (PFNGLCLIENTACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
    glMultiTexCoord2f                  = (PFNGLMULTITEXCOORD2FPROC)wglGetProcAddress("glMultiTexCoord2f");
    glClientActiveTexture              = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTexture");
  
    if(!glCreateProgramObjectARB || !glDeleteObjectARB || !glUseProgramObjectARB ||
       !glCreateShaderObjectARB || !glCreateShaderObjectARB || !glCompileShaderARB || 
       !glGetObjectParameterivARB || !glAttachObjectARB || !glGetInfoLogARB || 
       !glLinkProgramARB || !glGetUniformLocationARB || !glUniform4fARB || !glUniform1iARB || 
       !glVertexAttribPointerARB || !glEnableVertexAttribArrayARB ||
       !glDisableVertexAttribArrayARB || !glGetAttribLocationARB || 
       !glClientActiveTextureARB || !glActiveTextureARB)

    {
       printf("error: one or more GL_ARB_shader_objects functions were not found\n");
       return false;                
    }
    
    return true;
} 
#endif
