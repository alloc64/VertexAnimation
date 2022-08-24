#include "main.h"

GLhandleARB Shader::LoadShaderFromArray(char * shaderName, const GLcharARB ** array, GLuint shaderType)
{
        GLhandleARB shaderHandle = glCreateShaderObjectARB(shaderType);
    	if (!shaderHandle)
    	{
    		printf("error: failed creating shader object from %s array!\n", shaderName);
    		shaderHandle = 0;
    		return false;		
    	}
        glShaderSourceARB(shaderHandle, 1, array, NULL);

    	int result;	
       	glCompileShaderARB(shaderHandle);
       	glGetObjectParameterivARB(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &result);
    
    	if (!result)
    	{
        	GLsizei errorLoglength;    
        	int actualErrorLogLength;
    		printf("error: shader %s failed compilation\n", shaderName);
    		glGetObjectParameterivARB(shaderHandle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);
    		char* errorLogText = new char[errorLoglength+1];
    		glGetInfoLogARB(shaderHandle, errorLoglength, &actualErrorLogLength, errorLogText);
    		printf("%s\n", errorLogText);
    		printf("%s\n",array[0]);
    		delete [] errorLogText;
    	    shaderHandle = 0;     
    	    system("pause");
            return NULL;
    	}else{
    		printf("ok: shader %s compiled successfully\n", shaderName);          
        }
        
	return shaderHandle;
}
