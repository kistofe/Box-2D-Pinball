#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
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

	ball_tex = App->textures->Load("pinball/ball.png");
	flipper_tex = App->textures->Load("pinball/flipper.png");

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

	return true;
}

void ModulePlayer::OnCollision(PhysBody * bodyA, PhysBody * bodyB)
{
	int x, y;

	App->audio->PlayFx(App->scene_intro->bonus_fx);

	if (bodyB == App->scene_intro->dying_sensor)
	{
		if (ball)
			ball->~PhysBody();

		else	
			CreateBall();
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

	revoluteJointDef.upperAngle				= 210 * DEGTORAD;			// Angle limits
	revoluteJointDef.lowerAngle				= 150 * DEGTORAD;

	revoluteJointDef.motorSpeed				= -1500.0f * DEGTORAD;		// Motor
	revoluteJointDef.maxMotorTorque			= 1500;

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
	ball = App->physics->CreateCircle(500, 740, 15, b2_dynamicBody, 0.35f);
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
	App->renderer->Blit(flipper_tex, x, y, NULL, 1.0f, 0, flipperL->GetRotation(), flipperL->GetRotation());//Not working

	//Right Flipper

	// -------------------------------------------------------
	
	//BALL BLIT
	ball->GetPosition(x, y);
	App->renderer->Blit(ball_tex, x, y, NULL, 1.0f, ball->GetRotation());

	// ----------------------------------------------------------
	

	return UPDATE_CONTINUE;
}



