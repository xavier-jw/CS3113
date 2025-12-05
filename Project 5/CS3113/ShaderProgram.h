#pragma once
#include "raylib.h"
#include <string>

class ShaderProgram
{
private:
    Shader mShader;
    bool mIsLoaded;

public:
    ShaderProgram();
    ~ShaderProgram();

    void load(const std::string &vertexPath, const std::string &fragmentPath);
    void unload();
    void begin();
    void end();
    void setFloat(const std::string &name, float value);
};