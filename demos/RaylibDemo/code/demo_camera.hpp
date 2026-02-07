#pragma once
#include <raylib.h>
class DemoCamera
{
public:
	DemoCamera();
	void Update(float deltaTime);

public:
    Camera2D camera;
    Vector2 currentMoveDirection = { 0,0 };
    bool isMoving = false;

private:
    Vector2 targetPosition = {};
    float cameraSmoothing = 10.0f;
    float moveSpeed = 1050;
};