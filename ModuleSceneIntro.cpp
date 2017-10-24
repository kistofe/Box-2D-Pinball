#include "Globals.h"
#include "Application.h"
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
	pinball_tex = bouncer_tex = NULL;
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

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	
	/*if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}
	*/

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

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

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(bonus_fx);

	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
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

