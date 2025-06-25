/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include <stdio.h>
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

typedef enum GameScreen { START_SCREEN, GAMEPLAY, GAME_OVER } GameScreen;
GameScreen currentScreen = START_SCREEN;

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	int gameHeight = 800;
	int gameWidth = 600;
	InitWindow(gameWidth, gameHeight, "Hello Raylib");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");
	SetTargetFPS(60);
	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");

	Rectangle player = { 100, 750, 50, 50 };
	float velocityY = 0.0f;
	float speed = 400.0f;
	float jumpforce = -600.0f;
	float gravity = (jumpforce * jumpforce) / 800;
	float platformSpeed = 100.0f;
	bool isOnGround = true;
	bool gameStart = false;
	float survivalTime = 0.0f;

	int platform_height = 20;
	int num_levels = 4;
	int num_platforms_arr[num_levels];
	int num_platforms;
	int total = 0;
	for (int i = 0; i < num_levels; i++) 
	{
		num_platforms = GetRandomValue(1,5);
		num_platforms_arr[i] = num_platforms;
		total += num_platforms;
	}
	int count = 0;
	Rectangle all_platforms[total];
	int *platforms_idx;
	for (int i = 0; i < num_levels; i++) { 
		platforms_idx = LoadRandomSequence(num_platforms_arr[i], 0, num_platforms_arr[i]);
		for (int j = 0; j < num_platforms_arr[i]; j++)
		{
			int width = GetRandomValue(100,150);
			all_platforms[count] = {(float)platforms_idx[j] * 200, (float)(200*i) + platform_height, (float)width, (float)platform_height};
			count++;
		}
		MemFree(platforms_idx);
	}
	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		float dt = GetFrameTime();
		BeginDrawing();
		ClearBackground(BLACK);

		if (currentScreen == START_SCREEN)
		{
			DrawText("MY AWESOME GAME", (gameWidth - MeasureText("MY AWESOME GAME", 40)) / 2, 200, 40, WHITE);
			DrawText("Press [SPACE] to Start", (gameWidth - MeasureText("Press [SPACE] to Start", 20)) / 2, 300, 20, GRAY);

			if (IsKeyPressed(KEY_SPACE))
			{
				currentScreen = GAMEPLAY;
				// Reset game state here if needed
			}
		}  else if (currentScreen == GAMEPLAY) {
			if (gameStart)
			{
				for (int i = 0; i < total; i++) {
					all_platforms[i].y += platformSpeed * dt;
					if (all_platforms[i].y > 800) {
						// Reset this platform to the top
						all_platforms[i].y = 10; // Slight offset above screen
						all_platforms[i].x = GetRandomValue(0, 5) * 200;
						all_platforms[i].width = GetRandomValue(100, 150);
					}
				}
				platformSpeed += 0.1f;
				jumpforce -= 0.7f;
				gravity = (jumpforce * jumpforce) / 800;
				speed += 0.5f;
				survivalTime += dt;
			}
			bool collided = false;
			count = 0;
			for (int i = 0; i < num_levels; i++) {
				for (int j = 0; j < num_platforms_arr[i]; j++) 
				{
					Rectangle platform = all_platforms[count];
					count++;
					if (player.y + player.height <= platform.y && // was above platform last frame
					player.y + player.height + velocityY * dt >= platform.y){
						if (player.x + player.width > platform.x && 
						player.x < platform.x + platform.width)
						{
							player.y = platform.y - player.height;
							velocityY = platformSpeed;
							isOnGround = true;
							collided = true;
							break;
						}
						else {
							isOnGround = false;
						}
					}
				}
				if (collided) break;
			}
			if (player.y != 750 && !collided) isOnGround = false;
			if (IsKeyPressed(KEY_SPACE) && isOnGround){
				velocityY = jumpforce;
				isOnGround = false;
				gameStart = true;
			}
			if (!isOnGround)
			{
				velocityY += gravity * dt;
				player.y += velocityY * dt;
				if (player.y >= 750)
				{
					currentScreen = GAME_OVER;
					gameStart = false;
					survivalTime = 0.0f;
				}
				
			}
			if (IsKeyDown(KEY_RIGHT))
			{
				player.x += speed * dt;
			} 
			if (IsKeyDown(KEY_LEFT))
			{
				player.x -= speed * dt;
			} 

			// draw some text using the default font
			BeginDrawing();
			ClearBackground(BLACK);
			char scoreText[64];
			sprintf(scoreText, "Time: %.2f", survivalTime);
			DrawText(scoreText, 20, 20, 20, WHITE);
			count = 0;
			for (int i = 0; i < num_levels; i++) {
				for (int j = 0; j < num_platforms_arr[i]; j++) {
					DrawRectangleRec(all_platforms[count], DARKGREEN);
					count++;
				}
			}
			DrawRectangleRec(player, WHITE);
			// draw our texture to the screen
		} else if (currentScreen == GAME_OVER)
		{
			DrawText("GAME OVER", (gameWidth - MeasureText("GAME OVER", 40)) / 2, 200, 40, WHITE);
			DrawText("Press [SPACE] to Retry", (gameWidth - MeasureText("Press [SPACE] to Retry", 20)) / 2, 300, 20, GRAY);

			if (IsKeyPressed(KEY_SPACE))
			{
				currentScreen = GAMEPLAY;
				isOnGround = true;
				player.y = 750;
				velocityY = 0;
				// Reset game state here if needed
			}
		}
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

bool check_platform(Rectangle player, Rectangle **all_platforms, int num_levels, float velocityY) 
{
	int dt = GetFrameTime();
	for (int i = 0; i < num_levels; i++)
	{
		for (int j = 0; j < sizeof(all_platforms[i]); i++) {
			Rectangle platform = all_platforms[j][i];

			// Only check if falling
			if (velocityY >= 0 &&
				player.y + player.height <= platform.y && // was above platform last frame
				player.y + player.height + velocityY * dt >= platform.y)
			{
				if (player.x + player.width > platform.x && 
				player.x < platform.x + platform.width)
				{
					player.y = platform.y - player.height;
					velocityY = 0;
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	return false;
}