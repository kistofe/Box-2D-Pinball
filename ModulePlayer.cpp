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

	ball_tex = flipper_tex = NULL;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	ball_tex = App->textures->Load("pinball/images/ball.png");
	flipper_tex = App->textures->Load("pinball/images/flipper.png");

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

	if (bodyB == App->scene_intro->ball_catcher)
	{
				
	}
}

void ModulePlayer::CreateFlippers()
{
	// Left flipper			------------------------------------------------------------

	b2RevoluteJointDef revoluteJointDef;
	

	flipperL								= App->physics->CreateRectangle(170, 770, 65, 20);
	pivotL									= App->physics->CreateCircle(170, 770, 10, b2_staticBody);
	flipperL->body->SetGravityScale(30.0f);

	revoluteJointDef.bodyA					= flipperL->body;
	revoluteJointDef.bodyB					= pivotL->body;

	revoluteJointDef.localAnchorA.Set(PIXEL_TO_METERS(30), 0);			// Set the pivot point of the rectangle where the center of the circle is
	revoluteJointDef.localAnchorB.Set(0, 0);							// Set the pivot point of the circle on its center
	revoluteJointDef.collideConnected		= false;

	revoluteJointDef.enableLimit			= true;						// Angle limits
	revoluteJointDef.upperAngle				= 210 * DEGTORAD;
	revoluteJointDef.lowerAngle				= 150 * DEGTORAD;

	revoluteJointDef.motorSpeed				= 1500.0f * DEGTORAD;		// Motor
	revoluteJointDef.maxMotorTorque			= 1500;
	revoluteJointDef.enableMotor			= false;


	jointL = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef);

	// ---------------------------------------------------------------------------------

	// Right flipper		------------------------------------------------------------


	flipperR								= App->physics->CreateRectangle(318, 700, 65, 20);
	pivotR									= App->physics->CreateCircle(318, 770, 10, b2_staticBody);
	flipperR->body->SetGravityScale(30.0f);
	
	revoluteJointDef.bodyA					= flipperR->body;
	revoluteJointDef.bodyB					= pivotR->body;

	revoluteJointDef.localAnchorA.Set(PIXEL_TO_METERS(-30), 0);		// Set the pivot point of the rectangle where the center of the circle is
	revoluteJointDef.localAnchorB.Set(0, 0);						// Set the pivot point of the circle on its center
	revoluteJointDef.collideConnected		= false;

	revoluteJointDef.enableLimit			= true;
	revoluteJointDef.upperAngle				= 210 * DEGTORAD;			// Angle limits
	revoluteJointDef.lowerAngle				= 150 * DEGTORAD;

	revoluteJointDef.motorSpeed				= -1500.0f * DEGTORAD;		// Motor
	revoluteJointDef.maxMotorTorque			= 1500;
	revoluteJointDef.enableMotor			= false;

	jointR = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef);

	// ---------------------------------------------------------------------------------


}


void ModulePlayer::CreateLauncher()
{
	b2RevoluteJointDef revoluteJointDef;

	// Ball launcher		------------------------------------------------------------

	launcher = App->physics->CreateRectangle(500, 780, 30, 80);
	launcher_pivot = App->physics->CreateRectangle(500, 830, 30, 20, b2_staticBody);

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


}

void ModulePlayer::CreateBall()
{
	// Create Ball
	ball = App->physics->CreateCircle(500, 750, 13, b2_dynamicBody, 0.25f);
	ball->listener = this; //calls OnCollision function
}

// Update: draw background
update_status ModulePlayer::Update()
{
	// FLIPPER INPUT	-----------------------------------------
	
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		jointL->EnableMotor(true);
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
	{
		jointL->EnableMotor(false);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		jointR->EnableMotor(true);
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
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
	{
		jointLauncher->EnableMotor(false);
	}

	// ----------------------------------------------------------

	int x, y;

	// FLIPPER BLITS -----------------------------------------
	//Left Flipper
	pivotL->GetPosition(x, y);
	App->renderer->Blit(flipper_tex, x, y, NULL, 1.0f, flipperL->GetRotation(), SDL_FLIP_VERTICAL, 2147483647);//Not working

	//Right Flipper
	flipperR->GetPosition(x, y);
	App->renderer->Blit(flipper_tex, x, y, NULL, 1.0f, flipperR->GetRotation(), SDL_FLIP_HORIZONTAL);//Not working
	// -------------------------------------------------------
	

	//BALL BLIT
	ball->GetPosition(x, y);
	App->renderer->Blit(ball_tex, x, y, NULL, 1.0f, ball->GetRotation());

	// ----------------------------------------------------------

	// Set score in the title
	if (tries <= 0)
	{
		tries = 0;
	}
	if (App->player->tries > 0)
		tmp->create("[POKEMON PINBALL] | SCORE: %d | BALLS LEFT: %d", score, tries);
	else
		tmp->create("[POKEMON PINBALL] | YOU LOSE! :( | TOTAL SCORE: %d | ---- Press F5 to Re-start game ----", score);

	App->window->SetTitle(tmp->GetString());

	// ----------------------------------------------------------
	
	//Destroy the ball when losing
	if (must_destroy_ball && tries > 0)
	{
		App->physics->world->DestroyBody(ball->body);
		CreateBall();
		must_destroy_ball = false;
	}

	// ----------------------------------------------------------

	
	return UPDATE_CONTINUE;
}



