#include "demo_camera.hpp"
#include <raymath.h>

DemoCamera::DemoCamera()
{
	camera.zoom = 1.0f;
	camera.target = Vector2{ 256, 256 };
	camera.offset = Vector2{ 256, 256 };
	camera.rotation = 0.0f;
}

void DemoCamera::Update(float deltaTime)
{
	// Handle input and update target position.
	currentMoveDirection = { 0,0 };
	Vector2 inputDirection = { 0, 0 };

	if (IsKeyDown(KEY_W)) inputDirection.y = -1;
	if (IsKeyDown(KEY_A)) inputDirection.x = -1;
	if (IsKeyDown(KEY_S)) inputDirection.y = 1;
	if (IsKeyDown(KEY_D)) inputDirection.x = 1;

	// Normalize diagonal movement.
	float length = sqrtf(inputDirection.x * inputDirection.x + inputDirection.y * inputDirection.y);

	if (length > 0)
	{
		inputDirection.x /= length;
		inputDirection.y /= length;
	}

	// Update target position based on input.
	targetPosition.x += inputDirection.x * moveSpeed * deltaTime;
	targetPosition.y += inputDirection.y * moveSpeed * deltaTime;

	// Smoothly interpolate camera to target position.
	camera.target.x = Lerp(camera.target.x, targetPosition.x, cameraSmoothing * deltaTime);
	camera.target.y = Lerp(camera.target.y, targetPosition.y, cameraSmoothing * deltaTime);

	currentMoveDirection = Vector2DivideByValue(Vector2Subtract(targetPosition, camera.target), 100);
	currentMoveDirection.y *= -1;

	// Smooth zooming logic.
	static float targetScale = camera.zoom;
	float wheelMove = GetMouseWheelMove();

	if (wheelMove != 0)
		targetScale += wheelMove * 0.1f;

	// Reset camera position and zoom.
	if (IsKeyDown(KEY_ENTER))
	{
		targetPosition = Vector2{ 256, 256 };
		camera.target = targetPosition;
		targetScale = 1;
	}

	// Clamp zoom limits.
	if (targetScale < 0.1f) targetScale = 0.1f;
	if (targetScale > 20.0f) targetScale = 20.0f;

	camera.zoom = Lerp(camera.zoom, targetScale, 2.5f * deltaTime);
}
