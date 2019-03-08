#ifndef _SHADER_H
#define _SHADER_H
#include <QString>
#include <QOpenGLExtraFunctions>
/*
 * Shader class:
 * Holds functonality to bind shader to a shaderprogram
 * and use it the current opengl Context.
 * Created: 14_02_2019
 * Author: Aditya Mattoo
*/
class _Shader: protected QOpenGLExtraFunctions
{
public:
    _Shader();
    ~_Shader();

    uint getShaderProgram();
    void setFragmentShader(QString fragmentshaderPath);
    void setVertexShader(QString vertexShaderPath);
    void attachShaders();
    void attachShaders(QString vertexInfo , QString fragmentInfo);
	uint getUniformLocation(const char* nameOfUniform);
    void useShaderProgram();
	void resetShader();
	
private:
    //Shader Program
     uint shaderProgram;
     unsigned int vertexShader;
     unsigned int fragmentShader;
     //Error checking
     int success;
     char infoLog[512];
};

#endif // _SHADER_H
