#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

#include "Box2D/Box2D/Box2D.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

private:

	void AddBouncers();
	void AddSensors();

public:
	p2List<PhysBody*> pinball;
	p2List<PhysBody*> bouncers;
	
	//sensors
	PhysBody* dying_sensor;
	PhysBody* arrow_sensor_right1;
	PhysBody* diglett_sensor1;
	PhysBody* diglett_sensor2;
	PhysBody* ball_catcher;
	
	bool sensed;

	SDL_Texture* pinball_tex;
	SDL_Texture* bouncer_tex;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;

private:

};
