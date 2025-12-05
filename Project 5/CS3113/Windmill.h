#pragma once
#include "Entity.h"

class Windmill : public Entity
{
public:
    Windmill(Vector2 position);
    ~Windmill() override;
};