#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleTextures.h"

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

	ball_tex = App->textures->Load("pinball/ball.png");
	flipper_tex = App->textures->Load("pinball/flipper.png");
	
	// Create Ball
	ball = App->physics->CreateCircle(180, 200, 15, b2_dynamicBody, 0.5f);

	
	// Create flippers

	// Left flipper			------------------------------------------------------------

	b2RevoluteJointDef revoluteJointDef;

	flipperL = App->physics->CreateRectangle(170, 760, 80, 20);
	pivotL = App->physics->CreateCircle(170, 760, 10, b2_staticBody);
	flipperL->body->SetGravityScale(30.0f);

	revoluteJointDef.bodyA = flipperL->body;
	revoluteJointDef.bodyB = pivotL->body;

	revoluteJointDef.localAnchorA.Set(PIXEL_TO_METERS(30), 0); // Set the pivot point of the rectangle where the center of the circle is
	revoluteJointDef.localAnchorB.Set(0, 0); // Set the pivot point of the circle on its center
	revoluteJointDef.collideConnected = false;

	revoluteJointDef.enableLimit = true;					// Angle limits
	revoluteJointDef.upperAngle = 210 * DEGTORAD;
	revoluteJointDef.lowerAngle = 150 * DEGTORAD;

	revoluteJointDef.motorSpeed = 1500.0f * DEGTORAD;		// Motor
	revoluteJointDef.maxMotorTorque = 1500;
	revoluteJointDef.enableMotor = false;

	jointL = (b2RevoluteJoint*)	App->physics->world->CreateJoint(&revoluteJointDef);
	
	// ---------------------------------------------------------------------------------
	
	// Right flipper		------------------------------------------------------------

	flipperR = App->physics->CreateRectangle(327, 760, 80, 20);
	pivotR = App->physics->CreateCircle(327, 760, 10, b2_staticBody);
	flipperR->body->SetGravityScale(30.0f);

	revoluteJointDef.bodyA = flipperR->body;
	revoluteJointDef.bodyB = pivotR->body;

	revoluteJointDef.localAnchorA.Set(PIXEL_TO_METERS(-30), 0); // Set the pivot point of the rectangle where the center of the circle is
	revoluteJointDef.localAnchorB.Set(0, 0); // Set the pivot point of the circle on its center

	revoluteJointDef.upperAngle = 210 * DEGTORAD;			// Angle limits
	revoluteJointDef.lowerAngle = 150 * DEGTORAD;

	revoluteJointDef.motorSpeed = -1500.0f * DEGTORAD;		// Motor
	revoluteJointDef.maxMotorTorque = 1500;

	jointR = (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef);
	// ---------------------------------------------------------------------------------
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	// FLIPPER INPUT -----------------------------------------
	
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
	int x, y;

	// FLIPPER BLITS -----------------------------------------
	//Left Flipper
	pivotL->GetPosition(x, y);
	App->renderer->Blit(flipper_tex, x, y, NULL, 1.0f, 0, flipperL->GetRotation(), flipperL->GetRotation());//Not working

	//Right Flipper

	// -------------------------------------------------------
	
	//BALL BLIT
	ball->GetPosition(x, y);
	App->renderer->Blit(ball_tex, x, y, NULL, 1.0f, ball->GetRotation());

	return UPDATE_CONTINUE;
}



