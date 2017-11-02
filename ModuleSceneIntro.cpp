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
	pinball_tex = dugtrio_tex = pikachu_tex = starmie_tex = panel_bor_tex = panel_tex = starmie2_tex = lit_bouncer_tex = blue_light_tex = lit_triangle_tex = again_tex = NULL;

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
	bonus_fx				= App->audio->LoadFx("pinball/audio/Sfx/bonus.wav");
	lose_ball_fx			= App->audio->LoadFx("pinball/audio/Sfx/Lose ball.wav");
	lose_fx					= App->audio->LoadFx("pinball/audio/Sfx/Lose.wav");

	App->renderer->camera.x = App->renderer->camera.y = 0;
	
	//Loading Textures
	pinball_tex				= App->textures->Load("pinball/images/Pinball.png");
	dugtrio_tex				= App->textures->Load("pinball/images/dugtrio.png");
	pikachu_tex				= App->textures->Load("pinball/images/pikachu.png");
	starmie_tex				= App->textures->Load("pinball/images/starmie.png");
	starmie2_tex			= App->textures->Load("pinball/images/starmie2.png");
	panel_bor_tex			= App->textures->Load("pinball/images/border.png");
	panel_tex				= App->textures->Load("pinball/images/Panel.png");
	blue_light_tex			= App->textures->Load("pinball/images/blue_light.png");
	red_light_tex			= App->textures->Load("pinball/images/red_light.png");
	light_off_tex			= App->textures->Load("pinball/images/light_off.png");
	lit_bouncer_tex			= App->textures->Load("pinball/images/lit_bouncer.png");
	lit_triangle_tex		= App->textures->Load("pinball/images/lit_triangle.png");
	again_tex				= App->textures->Load("pinball/images/again_tex.png");

	//Adding Animations
	AddSceneAnimations();
		
	//Create every scenario part
	AddPinballParts();
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
	App->textures->Unload(starmie2_tex);
	App->textures->Unload(panel_bor_tex);
	App->textures->Unload(panel_tex);
	App->textures->Unload(lit_bouncer_tex);
	App->textures->Unload(lit_triangle_tex);
	App->textures->Unload(again_tex);
	
	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	// Update the bouncer timer
	for (int i = 0; i < 3; i++)
	{
		if (is_bouncer_hit[i])
			bouncer_timer[i]++;

		if (bouncer_timer[i] > 10)
		{
			bouncer_timer[i] = 0;
			is_bouncer_hit[i] = false;
		}
	}

	//Update the triangle timer
	for (int i = 0; i < 2; i++)
	{
		if (is_triangle_hit[i])
			triangle_timer[i]++;

		if (triangle_timer[i] > 10)
		{
			triangle_timer[i] = 0;
			is_triangle_hit[i] = false;
		}
	}

	
	// All draw functions ------------------------------------------------------
	
	//BLIT TEXTURES --------------------------------------------

	//Pinball Textures
	App->renderer->Blit(pinball_tex, 0, 0, NULL);

	//Lit Bouncer Textures
	if (is_bouncer_hit[0] == true)
		App->renderer->Blit(lit_bouncer_tex, 174, 234, NULL);
	
	if (is_bouncer_hit[1] == true)
		App->renderer->Blit(lit_bouncer_tex, 249, 204, NULL);
	
	if (is_bouncer_hit[2] == true)
		App->renderer->Blit(lit_bouncer_tex, 231, 288, NULL);
	
	//Lit Triangle Textures
	if (is_triangle_hit[0] == true)//Left triangle
		App->renderer->Blit(lit_triangle_tex, 106, 619, NULL, 0, 0, 0, 0, SDL_FLIP_HORIZONTAL);
	
	if (is_triangle_hit[1] == true)//Right triangle
		App->renderer->Blit(lit_triangle_tex, 325, 619, NULL);

	//Again texture
	if (App->player->tries <= 0)
		App->renderer->Blit(again_tex, 192, 741, NULL);

	
		

	//----------------------------------------------------------

	// BLIT ANIMATIONS -------------------------------------------

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

	//Second Starmie animation
	App->renderer->Blit(starmie2_tex, 133, 315, &(Starmie2.GetCurrentFrame()));

	// --------------------------------------------------------------

	//Check Ball collision to animate scenario ----------------------

	// Blue lights beside the flippers

	if (round_lights_on[0])
		App->renderer->Blit(blue_light_tex, 30, 618, NULL);
	else if (!round_lights_on[0])
		App->renderer->Blit(light_off_tex, 30, 618, NULL);
	if (round_lights_on[1])
		App->renderer->Blit(blue_light_tex, 78, 618, NULL);
	else if (!round_lights_on[1])
		App->renderer->Blit(light_off_tex, 78, 618, NULL);
	if (round_lights_on[2])
		App->renderer->Blit(blue_light_tex, 381, 618, NULL);
	else if (!round_lights_on[2])
		App->renderer->Blit(light_off_tex,381, 618, NULL);
	if (round_lights_on[3])
		App->renderer->Blit(blue_light_tex, 429, 618, NULL);
	else if (!round_lights_on[3])
		App->renderer->Blit(light_off_tex, 429, 618, NULL);
	if (round_lights_on[4])
		App->renderer->Blit(red_light_tex, 153, 339, NULL);
	else if (!round_lights_on[4])
		App->renderer->Blit(light_off_tex, 153, 339, NULL);

	// Blit bouncers 
	
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::AddPinballParts()
{
	//Main pinball layer
	//Part 1
	int pinball_shape[84] = {
		165, 832,
		24, 740,
		23, 620,
		36, 599,
		53, 589,
		73, 589,
		73, 570,
		95, 567,
		104, 555,
		103, 540,
		94, 527,
		75, 527,
		79, 513,
		87, 503,
		63, 462,
		50, 414,
		38, 362,
		30, 303,
		29, 47,
		40, 30,
		64, 30,
		76, 46,
		78, 101,
		114, 73,
		150, 51,
		186, 40,
		218, 34,
		298, 32,
		345, 42,
		385, 60,
		432, 92,
		458, 115,
		489, 160,
		505, 196,
		515, 244,
		519, 263,
		518, 833,
		576, 833,
		575, 0,
		0, 0,
		0, 833,
		158, 833
	};

	pinball.add(App->physics->CreateChain(0, 0, pinball_shape, 84));
	// ---------------------------------------------------------------

	//Part 2
	int pinball_shape2[58] = {
		474, 835,
		311, 835,
		454, 737,
		455, 622,
		441, 597,
		424, 589,
		407, 587,
		406, 570,
		384, 566,
		376, 556,
		375, 538,
		382, 528,
		406, 526,
		399, 511,
		391, 500,
		411, 473,
		423, 442,
		431, 411,
		439, 377,
		446, 331,
		450, 288,
		450, 219,
		435, 163,
		415, 122,
		439, 144,
		460, 182,
		473, 217,
		481, 250,
		480, 832
	};

	pinball.add(App->physics->CreateChain(0, 0, pinball_shape2, 58));
	//--------------------------------------------------------------------

	// Scenario parts
	//Part 1 
	int scenario_shape1[32] = {
		118, 130,
		182, 102,
		212, 94,
		269, 93,
		307, 102,
		333, 112,
		352, 120,
		345, 109,
		308, 85,
		280, 72,
		262, 66,
		229, 66,
		196, 72,
		161, 88,
		131, 107,
		115, 123
	};

	pinball.add(App->physics->CreateChain(0, 0, scenario_shape1, 32));
	//-------------------------------------------------------------------

	//Part 2 
	int scenario_shape2[18] = {
		420, 621,
		409, 621,
		409, 686,
		404, 697,
		323, 754,
		361, 762,
		413, 728,
		419, 719,
		419, 627
	};

	pinball.add(App->physics->CreateChain(0, 0, scenario_shape2, 16));
	//-------------------------------------------------------------------

	//Part 3 
	int scenario_shape3[24] = {
		140, 776,
		60, 718,
		60, 623,
		63, 618,
		68, 618,
		70, 623,
		70, 689,
		73, 695,
		156, 751,
		140, 750,
		131, 760,
		145, 778
	};
	pinball.add(App->physics->CreateChain(0, 0, scenario_shape3, 24));
	//-------------------------------------------------------------------

	//Part 4 
	int scenario_shape4[34] = {
		119, 456,
		135, 439,
		134, 429,
		113, 401,
		102, 376,
		93, 349,
		87, 322,
		87, 298,
		89, 273,
		91, 257,
		78, 268,
		73, 296,
		72, 343,
		81, 378,
		90, 404,
		101, 433,
		116, 452
	};

	pinball.add(App->physics->CreateChain(0, 0, scenario_shape4, 34));
	//--------------------------------------------------------------------

	//Part 5
	int scenario_shape5[28] = {
		181, 389,
		201, 371,
		198, 341,
		157, 299,
		142, 272,
		140, 245,
		148, 229,
		149, 199,
		132, 241,
		129, 325,
		133, 339,
		139, 361,
		154, 382,
		175, 388
	};

	pinball.add(App->physics->CreateChain(0, 0, scenario_shape5, 28));
	//-------------------------------------------------------------------

	//Part 6
	int scenario_shape6[48] = {
		315, 349,
		329, 250,
		331, 148,
		360, 167,
		384, 193,
		399, 222,
		406, 243,
		406, 343,
		397, 384,
		385, 417,
		363, 453,
		346, 441,
		345, 429,
		367, 402,
		380, 369,
		386, 346,
		393, 315,
		393, 292,
		386, 298,
		361, 298,
		347, 295,
		347, 317,
		329, 361,
		319, 353
	};

	pinball.add(App->physics->CreateChain(0, 0, scenario_shape6, 48));
	//-------------------------------------------------------------------

	//Part 7 & 8
	pinball.add(App->physics->CreateRectangle(207, 167, 18, 55, 0, b2_staticBody));
	pinball.add(App->physics->CreateRectangle(273, 152, 18, 45, 0, b2_staticBody));
	//-------------------------------------------------------------------

	//Part 9
	int scenario_shape9[8] = {
		336, 709,
		370, 681,
		372, 631,
		338, 703
	};

	//Main part
	pinball.add(App->physics->CreateChain(0, 0, scenario_shape9, 8));
	//External part with restitution
	int secondary_shape[8] = {
		363, 626,
		326, 700,
		332, 700,
		367, 629
	};

	pinball.add(App->physics->CreateChain(0, 0, secondary_shape, 8, 2.0f, 20));
	// ------------------------------------------------------------------

	//Part 10
	int scenario_shape10[8] = {
		106, 632,
		106, 680,
		144, 709,
		110, 637
	};

	//Main part
	pinball.add(App->physics->CreateChain(0, 0, scenario_shape10, 8));
	//External part with restitution
	int secondary_shape2[8] = {
		114, 626,
		152, 701,
		144, 697,
		110, 629
	};

	pinball.add(App->physics->CreateChain(0, 0, secondary_shape2, 8, 2.0f, 20));

	// Circle between flippers
	pinball.add(App->physics->CreateCircle(240, 835, 5, b2_staticBody));
	// -----------------------------------------------------------------


}

void ModuleSceneIntro::AddBouncers()
{
	bouncers[0] = App->physics->CreateCircle(198, 257, 23, b2_staticBody, 1.0f, 20);
	bouncers[1] = App->physics->CreateCircle(274, 227, 23, b2_staticBody, 1.0f, 20);
	bouncers[2] = App->physics->CreateCircle(255, 312, 23, b2_staticBody, 1.0f, 20);
}

void ModuleSceneIntro::AddSensors()
{
	// Creating Dying Sensor
	dying_sensor			= App->physics->CreateRectangleSensor(0, SCREEN_HEIGHT + 50, SCREEN_WIDTH, 50);

	// -------------------------------------------------------------------------------
	
	// Creating Animation Sensors
	
	//Voltorb sensors
	voltorb_sensor[0]		= App->physics->CreateCircleSensor(198, 256, 29);
	voltorb_sensor[1]		= App->physics->CreateCircleSensor(274, 227, 29);
	voltorb_sensor[2]		= App->physics->CreateCircleSensor(255, 312, 29);

	//Blue lights
	roundlights[0]			= App->physics->CreateCircleSensor(30, 618, PIXEL_TO_METERS(21), 50);
	roundlights[1]			= App->physics->CreateCircleSensor(78, 618, PIXEL_TO_METERS(21), 50);
	roundlights[2]			= App->physics->CreateCircleSensor(381, 618, PIXEL_TO_METERS(21), 50);
	roundlights[3]			= App->physics->CreateCircleSensor(429, 618, PIXEL_TO_METERS(21), 50);
	roundlights[4]			= App->physics->CreateCircleSensor(105, 332, PIXEL_TO_METERS(21), 30);

	//Triangle sensors
	triangle_sensors[0]		= App->physics->CreateRectangleSensor(135, 665, 5, 90, -27 * DEGTORAD);
	triangle_sensors[1]		= App->physics->CreateRectangleSensor(343, 665, 5, 90, 27 * DEGTORAD);

	// ---------------------------------------------------------------------------------


	//Creating other Sensors

	//Diglett Sensors
	diglett_sensor1			= App->physics->CreateCircleSensor(85, 550, 25, 30);
	diglett_sensor2			= App->physics->CreateCircleSensor(397, 550, 25, 30);

	// ---------------------------------------------------------------------------------

	//Ball catcher sensor
	ball_catcher			= App->physics->CreateCircleSensor(50, 57, 25, 70);

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

	//Second Starmie animation
	Starmie2.PushBack({   0, 0, 63, 69 });
	Starmie2.PushBack({  69, 0, 63, 69 });
	Starmie2.loop = true;
	Starmie2.speed = 0.05f;
	
}

