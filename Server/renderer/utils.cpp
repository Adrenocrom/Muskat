#include "muskat.h"

void ExitOnGLError(const char* error_message) {
	const GLenum ErrorValue = glGetError();

	if (ErrorValue != GL_NO_ERROR)   {
		fprintf(stderr, "%s: %s\n", error_message, gluErrorString(ErrorValue));
		exit(EXIT_FAILURE);
	}
}

GLuint LoadShader(const char* filename, GLenum shader_type) {
	GLuint shader_id = 0;
	FILE* file;
	long file_size = -1;
	char* glsl_source;

	if (NULL != (file = fopen(filename, "rb")) && 0 == fseek(file, 0, SEEK_END) && -1 != (file_size = ftell(file))) {
   	rewind(file);
    
		if (NULL != (glsl_source = (char*)malloc(file_size + 1))) {
			if (file_size == (long)fread(glsl_source, sizeof(char), file_size, file))       {
       		glsl_source[file_size] = '\0';

        		if (0 != (shader_id = glCreateShader(shader_type))) {
          		glShaderSource(shader_id, 1, (const char **) &glsl_source, NULL);
          		glCompileShader(shader_id);
          		ExitOnGLError("Could not compile a shader");
        		}
        		else
          		cerr<<"ERROR: Could not create a shader."<<endl;
      	}	
      	else
      		cerr<<"ERROR: Could not read file "<<filename<<endl;

            free(glsl_source);
    	}
    	else
      	cerr<<"ERROR: Could not allocate "<<file_size<<" bytes."<<endl;

    	fclose(file);
  	}
  	else {
   	if (NULL != file) fclose(file);
    	cerr<<"ERROR: Could not open file "<<filename<<endl;
	}

  	return shader_id;
}
