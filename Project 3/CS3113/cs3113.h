#ifndef CS3113_H
#define CS3113_H

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>

enum AppStatus
{
    TERMINATED,
    RUNNING
};
enum TextureType
{
    SINGLE,
    ATLAS
};

Color ColorFromHex(const char *hex);
Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols);

#endif // CS3113_H