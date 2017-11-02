#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "Animation.h"

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

private:

	void AddBouncers();
	void AddSensors();
	void AddSceneAnimations();

public:
	p2List<PhysBody*> pinball;
	PhysBody* bouncers[3];
	
	//sensors
	PhysBody* dying_sensor;
	PhysBody* arrow_sensor_right1;
	PhysBody* arrow_sensor_right2;
	PhysBody* arrow_sensor_right3;
	PhysBody* triangle_right;
	PhysBody* triangle_left;
	PhysBody* diglett_sensor1;
	PhysBody* diglett_sensor2;
	PhysBody* ball_catcher;
	PhysBody* voltorb_sensor[3];
	
	//Animations
	Animation Dugtrio_left;
	Animation Dugtrio_right;
	Animation Pikachu;
	Animation Starmie;
	Animation Starmie2;
	Animation panel_border;
	Animation panel;
	
	bool sensed;

	//Textures
	SDL_Texture* pinball_tex;
	SDL_Texture* bouncer_tex;
	SDL_Texture* dugtrio_tex;
	SDL_Texture* pikachu_tex;
	SDL_Texture* starmie_tex;
	SDL_Texture* starmie2_tex;
	SDL_Texture* panel_bor_tex;
	SDL_Texture* panel_tex;
	SDL_Texture* arrow;
	SDL_Texture* lighted_bouncer_tex;
	
	//Sfx
	uint bonus_fx;
	uint lose_ball_fx;
	uint lose_fx;

	
	p2Point<int> ray;
	bool ray_on;

private:

};
