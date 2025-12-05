#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : mIsLoaded(false) {}

ShaderProgram::~ShaderProgram()
{
    unload();
}

void ShaderProgram::load(const std::string &vertexPath, const std::string &fragmentPath)
{
    mShader = LoadShader(vertexPath.c_str(), fragmentPath.c_str());
    mIsLoaded = true;
}

void ShaderProgram::unload()
{
    if (mIsLoaded)
    {
        UnloadShader(mShader);
        mIsLoaded = false;
    }
}

void ShaderProgram::begin()
{
    if (mIsLoaded)
        BeginShaderMode(mShader);
}

void ShaderProgram::end()
{
    if (mIsLoaded)
        EndShaderMode();
}

void ShaderProgram::setFloat(const std::string &name, float value)
{
    if (mIsLoaded)
    {
        int loc = GetShaderLocation(mShader, name.c_str());
        SetShaderValue(mShader, loc, &value, SHADER_UNIFORM_FLOAT);
    }
}