#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();
	void Timer(int time);

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:

	SDL_Texture*	Spring;
	SDL_Texture*	ball_texture;
	SDL_Texture*	blocker_texture;
	SDL_Texture*	bouncers;
	
	SDL_Rect		spring_control;

	PhysBody*		Ball;
	PhysBody*		BallSensor;
	PhysBody*		Restart;

	bool			Shoot;
	bool			restart;
	bool			getpoints;
	bool			pause;

	uint			force_counter;
	uint			score;
	uint			lives;
	uint			ticks;
	uint			flipperUp;
	uint			flipperDown;
	uint			drain;
	uint			target;
};