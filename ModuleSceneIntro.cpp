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

#include "SDL\include\SDL.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	pinball_tex = dugtrio_tex = pikachu_tex = starmie_tex = panel_bor_tex = panel_tex = arrow = NULL;
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
	bonus_fx		= App->audio->LoadFx("pinball/audio/Sfx/bonus.wav");
	lose_ball_fx	= App->audio->LoadFx("pinball/audio/Sfx/Lose ball.wav");
	lose_fx			= App->audio->LoadFx("pinball/audio/Sfx/Lose.wav");

	App->renderer->camera.x = App->renderer->camera.y = 0;
	
	//Loading Textures
	pinball_tex		= App->textures->Load("pinball/images/pinball2.png");
	dugtrio_tex		= App->textures->Load("pinball/images/dugtrio.png");
	pikachu_tex		= App->textures->Load("pinball/images/pikachu.png");
	starmie_tex		= App->textures->Load("pinball/images/starmie.png");
	panel_bor_tex	= App->textures->Load("pinball/images/border.png");
	panel_tex		= App->textures->Load("pinball/images/Panel.png");
	arrow			= App->textures->Load("pinball/images/diagonal_arrow.png");
	
	
	//Adding Animations
	AddSceneAnimations();
		
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
	
	//Unloading Textures
	App->textures->Unload(pinball_tex);
	App->textures->Unload(dugtrio_tex);
	App->textures->Unload(pikachu_tex);
	App->textures->Unload(starmie_tex);
	App->textures->Unload(panel_bor_tex);
	App->textures->Unload(panel_tex);
	App->textures->Unload(arrow);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	
	// All draw functions ------------------------------------------------------
	/*p2List_item<PhysBody*>* c = pinball.getFirst();
	
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


	App->renderer->Blit(pinball_tex, 0, 0, NULL);

	// Blit Animations -------------------------------------------
	//Right dugtrio animation
	App->renderer->Blit(dugtrio_tex, 411, 500, &(Dugtrio_right.GetCurrentFrame()));

	//Left dugtrio animation
	App->renderer->Blit(dugtrio_tex, 0, 500, &(Dugtrio_left.GetCurrentFrame()));

	//Pikachu animation
	App->renderer->Blit(pikachu_tex, 20, 780, &(Pikachu.GetCurrentFrame()));
	App->renderer->Blit(pikachu_tex, 420, 780, &(Pikachu.GetCurrentFrame()));

	//Starmie animation
	App->renderer->Blit(starmie_tex, 0, 450, &(Starmie.GetCurrentFrame()));

	//Panel Border Animation
	App->renderer->Blit(panel_bor_tex, 168, 498, &(panel_border.GetCurrentFrame()));

	//Panel Animation
	App->renderer->Blit(panel_tex, 171, 501, &(panel.GetCurrentFrame()));

	// --------------------------------------------------------------

	//Check Ball collision to animate scenario ----------------------

	//Red arrows 
	if (App->player->light_r_arrow)
		App->renderer->Blit(arrow, 360, 470, NULL);
	


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
	// Creating Dying Sensor
	
	dying_sensor = App->physics->CreateRectangleSensor(0, SCREEN_HEIGHT + 50, SCREEN_WIDTH, 50);
	// -------------------------------------------------------------------------------
	
	// Creating Animation Sensors

	//Three sensors at the right
	arrow_sensor_right1 = App->physics->CreateRectangleSensor(350, 515, 40, 43, 90.075);
	arrow_sensor_right2 = App->physics->CreateRectangleSensor(367, 485, 40, 43, 90.075);
	arrow_sensor_right3 = App->physics->CreateRectangleSensor(384, 450, 40, 43, 90.075);

	//Triangle sensors
	triangle_left = App->physics->CreateRectangleSensor(130, 662, 5, 85, 100.0f);//Left triangle
	triangle_right = App->physics->CreateRectangleSensor(350, 662, 5, 85, -100.0f);//right triangle
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

void ModuleSceneIntro::AddSceneAnimations()
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

	//Pikachu Animation
	Pikachu.PushBack({   0, 0, 48, 48 });
	Pikachu.PushBack({  54, 0, 42, 48 });
	Pikachu.PushBack({ 102, 6, 45, 42 });
	Pikachu.loop = true;
	Pikachu.speed = 0.05f;

	//Starmie Animation
	Starmie.PushBack({ 0, 0, 54, 48 });
	Starmie.PushBack({ 60, 0, 54, 48 });
	Starmie.loop = true;
	Starmie.speed = 0.01f;

	//Panel Border Animation
	panel_border.PushBack({  0,  0, 144, 96 });
	panel_border.PushBack({ 154, 0, 144, 96 });
	panel_border.PushBack({ 304, 0, 144, 96 });
	panel_border.loop = true;
	panel_border.speed = 0.015f;

	//Panel Animation
	panel.PushBack({   0,   0, 138, 90 }); 
	panel.PushBack({   0, 100, 138, 90 });
	panel.PushBack({   0, 200, 138, 90 });
	panel.PushBack({   0, 300, 138, 90 });
	panel.PushBack({   0, 400, 138, 90 });
	panel.PushBack({   0, 500, 138, 90 });
	panel.PushBack({ 148,   0, 138, 90 });
	panel.PushBack({ 148, 100, 138, 90 });
	panel.PushBack({ 148, 200, 138, 90 });
	panel.PushBack({ 148, 300, 138, 90 });
	panel.PushBack({ 148, 400, 138, 90 });
	panel.PushBack({ 148, 500, 138, 90 });
	panel.PushBack({ 296,   0, 138, 90 });
	panel.PushBack({ 296, 100, 138, 90 });
	panel.PushBack({ 296, 200, 138, 90 });
	panel.PushBack({ 296, 300, 138, 90 });
	panel.PushBack({ 296, 400, 138, 90 });
	panel.PushBack({ 296, 500, 138, 90 });
	panel.PushBack({ 444,   0, 138, 90 });
	panel.PushBack({ 444, 100, 138, 90 });
	panel.PushBack({ 444, 200, 138, 90 });
	panel.PushBack({ 444, 300, 138, 90 });
	panel.PushBack({ 444, 400, 138, 90 });
	panel.PushBack({ 444, 500, 138, 90 });
	panel.loop = true;
	panel.speed = 0.0025f; 

	//Launcher animation

}

