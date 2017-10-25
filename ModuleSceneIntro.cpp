#include "Globals.h"
#include "Application.h"
#include "Animation.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"
#include "ModuleWindow.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	pinball_tex = dugtrio_tex = NULL;
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	//Loading music
	App->audio->PlayMusic("pinball/audio/Themes/Field_Theme.ogg");

	//Loading Sfx
	bonus_fx = App->audio->LoadFx("pinball/audio/Sfx/bonus.wav");

	App->renderer->camera.x = App->renderer->camera.y = 0;
	
	//Loading Textures
	pinball_tex = App->textures->Load("pinball/images/Pinball.png");
	dugtrio_tex = App->textures->Load("pinball/images/dugtrio.png");
	
	//Adding Animations
	AddAnimations();

	//Sensor Rectangle to detect when the player loses
	dying_sensor = App->physics->CreateRectangleSensor(0 , SCREEN_HEIGHT +  50, SCREEN_WIDTH, 50);
	
	//Create every scenario part
	App->physics->AddPinballParts();
	AddBouncers();

	//Calling function to create sensors
	AddSensors();

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	App->textures->Unload(dugtrio_tex);
	App->textures->Unload(pinball_tex);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	
	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = pinball.getFirst();
	
	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(pinball_tex, x, y, NULL);
		break;
	}

	/*c = bouncers.getFirst();

	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		if (c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
			App->renderer->Blit(bouncer_tex, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}*/

	
	// Blit Animations -------------------------------------------
	//Right dugtrio animation
	App->renderer->Blit(dugtrio_tex, 411, 500, &(Dugtrio_right.GetCurrentFrame()));

	//Left dugtrio animation
	App->renderer->Blit(dugtrio_tex, 0, 500, &(Dugtrio_left.GetCurrentFrame()));

	// ----------------------------------------------------------

	// RESPAWN BALL	--------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->player->score = 0;
		App->player->tries = 3;
	}

	// -------------------------------------------------------

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::AddBouncers()
{
	bouncers.add(App->physics->CreateCircle(198, 257, 27, b2_staticBody, 1.5f, 20));
	bouncers.add(App->physics->CreateCircle(274, 227, 27, b2_staticBody, 1.5f, 20));
	bouncers.add(App->physics->CreateCircle(255, 312, 27, b2_staticBody, 1.5f, 20));
}

void ModuleSceneIntro::AddSensors()
{
	// Creating Animation Sensors

	arrow_sensor_right1 = App->physics->CreateRectangleSensor(367, 480, 120, 45, 90.075);

	// ---------------------------------------------------------------------------------


	//Creating other Sensors

	//Diglet Sensors
	diglett_sensor1 = App->physics->CreateCircleSensor(85, 550, 25, 30);
	diglett_sensor2 = App->physics->CreateCircleSensor(395, 550, 25, 30);
	// ---------------------------------------------------------------------------------

	//Ball catcher sensor
	ball_catcher = App->physics->CreateCircleSensor(50, 57, 25, 70);
	// --------------------------------------------------------------------------------
}

void ModuleSceneIntro::AddAnimations()
{
	//Right Dugtrio Animation
	Dugtrio_right.PushBack({   0, 0, 69, 90 });
	Dugtrio_right.PushBack({  75, 0, 69, 90 });
	Dugtrio_right.PushBack({ 150, 0, 69, 90 });
	Dugtrio_right.PushBack({ 225, 0, 69, 90 });
	Dugtrio_right.loop = true;
	Dugtrio_right.speed = 0.05f;

	//Left Dugtrio Animation
	Dugtrio_left.PushBack({   0, 120, 69, 90 });
	Dugtrio_left.PushBack({  75, 120, 69, 90 });
	Dugtrio_left.PushBack({ 150, 120, 69, 90 });
	Dugtrio_left.PushBack({ 225, 120, 69, 90 });
	Dugtrio_left.loop = true;
	Dugtrio_left.speed = 0.05f;
}

