#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleWindow.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	ball_tex = flipper_tex = launcher_tex = NULL;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	//Loading textures
	ball_tex		= App->textures->Load("pinball/images/ball.png");
	flipper_tex		= App->textures->Load("pinball/images/flipper.png");
	launcher_tex	= App->textures->Load("pinball/images/launcher.png");

	flipper_fx = App->audio->LoadFx("pinball/audio/Sfx/Flipper.wav");

	// Calling player functions
	CreateBall();
	CreateFlippers();
	CreateLauncher();

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	App->textures->Unload(ball_tex);
	App->textures->Unload(flipper_tex);
	App->textures->Unload(launcher_tex);

	return true;
}

void ModulePlayer::OnCollision(PhysBody * bodyA, PhysBody * bodyB)
{

	if (bodyB == App->scene_intro->dying_sensor)
	{
		must_destroy_ball = true;
		tries -= 1;

		if (tries > 0)
			App->audio->PlayFx(App->scene_intro->lose_ball_fx);
		else
		{
			App->audio->PlayFx(App->scene_intro->lose_fx);
			App->audio->PlayMusic("pinball/audio/Themes/Game Over.ogg");
		}

	}
	
	if (bodyB->score != 0)
	{
		App->audio->PlayFx(App->scene_intro->bonus_fx);
		score += bodyB->score;
	}

	if (bodyB == App->scene_intro->arrow_sensor_right1 || bodyB == App->scene_intro->arrow_sensor_right2 || bodyB == App->scene_intro->arrow_sensor_right3)//primitive code to blit light arrow->needs to be revised
	{
		light_r_arrow = true;
	}

/*1	while (bodyB == App->scene_intro->triangle_left)
		triangle_left_is_hit = true;

	triangle_left_is_hit = false;

	while (bodyB == App->scene_intro->triangle_right)
		triangle_right_is_hit = true;

	triangle_right_is_hit = false;*/
}

void ModulePlayer::CreateFlippers()
{
	// Left flipper			------------------------------------------------------------

	b2RevoluteJointDef revoluteJointDef;
	

	flipperL								= App->physics->CreateRectangle(159, 767, 70, 20, 32 * DEGTORAD);
	pivotL									= App->physics->CreateCircle(159, 767, 10, b2_staticBody);
	flipperL->body->SetGravityScale(30.0f);

	revoluteJointDef.bodyA = flipperL->body;
	revoluteJointDef.bodyB = pivotL->body;

	revoluteJointDef.localAnchorA.Set(PIXEL_TO_METERS(-30), 0);		// Set the pivot point of the rectangle where the center of the circle is
	revoluteJointDef.localAnchorB.Set(0, 0);						// Set the pivot point of the circle on its center
	revoluteJointDef.collideConnected = false;


	revoluteJointDef.upperAngle = 32 * DEGTORAD;			// Angle limits
	revoluteJointDef.lowerAngle = -32 * DEGTORAD;
	revoluteJointDef.enableLimit = true;

	revoluteJointDef.motorSpeed				= 1500.0f * DEGTORAD;		// Motor
	revoluteJointDef.maxMotorTorque			= 1500;
	revoluteJointDef.enableMotor			= false;


	jointL = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef);

	// ---------------------------------------------------------------------------------

	// Right flipper		------------------------------------------------------------


	flipperR								= App->physics->CreateRectangle(318, 767, 70, 20, -212 * DEGTORAD);
	pivotR									= App->physics->CreateCircle(318, 767, 10, b2_staticBody);
	flipperR->body->SetGravityScale(30.0f);
	
	revoluteJointDef.bodyA					= flipperR->body;
	revoluteJointDef.bodyB					= pivotR->body;

	revoluteJointDef.localAnchorA.Set(PIXEL_TO_METERS(-30), 0);		// Set the pivot point of the rectangle where the center of the circle is
	revoluteJointDef.localAnchorB.Set(0, 0);						// Set the pivot point of the circle on its center
	revoluteJointDef.collideConnected		= false;
	revoluteJointDef.referenceAngle = 0;

	revoluteJointDef.upperAngle				= 212 * DEGTORAD;			// Angle limits
	revoluteJointDef.lowerAngle				= 148 * DEGTORAD;
	revoluteJointDef.enableLimit = true;

	revoluteJointDef.motorSpeed				= -1500.0f * DEGTORAD;		// Motor
	revoluteJointDef.maxMotorTorque			= 1500;
	revoluteJointDef.enableMotor			= false;

	jointR = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef);

	// ---------------------------------------------------------------------------------


}


void ModulePlayer::CreateLauncher()
{
	int x, y;
	//Create Launcher 2D Body
	
	b2RevoluteJointDef revoluteJointDef;

	// Ball launcher		------------------------------------------------------------

	launcher = App->physics->CreateRectangle(500, 780, 30, 80);
	launcher_pivot = App->physics->CreateRectangle(500, 830, 30, 20, 0, b2_staticBody);

	b2PrismaticJointDef prismaticJointDef;

	prismaticJointDef.bodyA = launcher->body;
	prismaticJointDef.bodyB = launcher_pivot->body;

	prismaticJointDef.localAnchorA.Set(0, 0);
	prismaticJointDef.localAnchorB.Set(0, 0);
	prismaticJointDef.collideConnected = false;

	prismaticJointDef.localAxisA.Set(0, 1);


	prismaticJointDef.enableLimit = true;
	prismaticJointDef.lowerTranslation = 0;
	prismaticJointDef.upperTranslation = PIXEL_TO_METERS(50);

	prismaticJointDef.enableMotor = false;
	prismaticJointDef.maxMotorForce = 500;
	prismaticJointDef.motorSpeed = 5000;

	jointLauncher = (b2PrismaticJoint*)App->physics->world->CreateJoint(&prismaticJointDef);


	// ---------------------------------------------------------------------------------

	//Set Launcher's Texture and Animations -------------------------------------------

	//Animations
	launcher_anim_idle.PushBack({ 0, 0, 42, 110 });
	launcher_anim_idle.loop = false;
	launcher_anim_idle.speed = 1.0f;
	
	launcher_anim_launching.PushBack({ 54, 0, 42, 110 });
	launcher_anim_launching.loop = false;
	launcher_anim_launching.speed = 1.0f;

	current_animation = &launcher_anim_idle;

}

void ModulePlayer::CreateBall()
{
	// Create Ball
	ball = App->physics->CreateCircle(500, 750, 13, b2_dynamicBody, 0.25f);
	ball->listener = this; 
}

void ModulePlayer::LauncherAnimation()
{
}

// Update: draw background
update_status ModulePlayer::Update()
{
		
	// FLIPPER INPUT	-----------------------------------------
	
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		jointL->EnableMotor(true);
		App->audio->PlayFx(flipper_fx);
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
	{
		jointL->EnableMotor(false);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		jointR->EnableMotor(true);
		App->audio->PlayFx(flipper_fx);
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
	{
		jointR->EnableMotor(false);
	}

	// -------------------------------------------------------
	

	// LAUNCHER INPUT	-----------------------------------------

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		jointLauncher->EnableMotor(true);
		current_animation = &launcher_anim_launching;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		jointLauncher->EnableMotor(false);
		current_animation = &launcher_anim_idle;
	}

	// ----------------------------------------------------------

	int x, y;

	// TEXTURE BLITS -----------------------------------------
	//Left Flipper
	flipperL->GetPosition(x, y);
	App->renderer->Blit(flipper_tex, x, y, NULL, 1.0f, flipperL->GetRotation());
	
	//Right Flipper
	flipperR->GetPosition(x, y);
	App->renderer->Blit(flipper_tex, x - 7, y - 5, NULL, 1.0f, flipperR->GetRotation(), SDL_FLIP_HORIZONTAL);
	// -------------------------------------------------------

	// Set Ball texture
	ball->GetPosition(x, y);
	App->renderer->Blit(ball_tex, x, y, NULL, 1.0f, ball->GetRotation());

	// ----------------------------------------------------------
	
	//Launcher
	launcher->GetPosition(x, y);
	App->renderer->Blit(launcher_tex, x - 3, y, &(current_animation->GetCurrentFrame()));

	// --------------------------------------------------------

	// Set score in the title 
	if (App->player->tries > 0)
	{
		tmp->create("[POKEMON PINBALL] | SCORE: %d | BALLS LEFT: %d", score, tries);
	}
	else
	{
		tries = 0;//Checking tries don't go negative
		tmp->create("[POKEMON PINBALL] | YOU LOSE! :( | TOTAL SCORE: %d | ---- Press F5 to Re-start game ----", score);
	}
	// ----------------------------------------------------------
	

	//Destroy the ball when losing
	if (must_destroy_ball && tries > 0)
	{
		App->physics->world->DestroyBody(ball->body);
		CreateBall();
		must_destroy_ball = false;
	}

	// ----------------------------------------------------------
	
	// RE-START GAME -------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		score = 0;
		tries = 3;
		App->scene_intro->panel.Reset();
		App->audio->PlayMusic("pinball/audio/Themes/Field_Theme.ogg");
	}

	// -------------------------------------------------------

	App->window->SetTitle(tmp->GetString());

	return UPDATE_CONTINUE;
}



