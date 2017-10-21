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
//	ball_tex = NULL;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

//	ball_tex = App->textures->Load("pinball/ball.png");


	// Create Ball
//	App->scene_intro->circles.add(App->physics->CreateCircle(487,0, 15)); //Until there exists a launcher, the ball will be created and will fall
//	App->scene_intro->circles.getLast()->data->listener = this;


	// Create flippers

	// Left flipper			------------------------------------------------------------

	b2RevoluteJointDef revoluteJointDef;

	flipperL							= App->physics->CreateRectangle(155, 1000, 80, 20);
	pivotL								= App->physics->CreateCircle(155, 770, 10, b2_staticBody);

	revoluteJointDef.bodyA				= flipperL->body;
	revoluteJointDef.bodyB				= pivotL->body;

	revoluteJointDef.localAnchorA.Set(PIXEL_TO_METERS(30), 0); // Set the pivot point of the rectangle where the center of the circle is
	revoluteJointDef.localAnchorB.Set(0, 0); // Set the pivot point of the circle on its center
	revoluteJointDef.collideConnected	= false;

	revoluteJointDef.enableLimit		= true;					// Angle limits
	revoluteJointDef.upperAngle			= 210 * DEGTORAD;
	revoluteJointDef.lowerAngle			= 150 * DEGTORAD;

	revoluteJointDef.motorSpeed			= 1500.0f * DEGTORAD;		// Motor
	revoluteJointDef.maxMotorTorque		= 1500;
	revoluteJointDef.enableMotor		= false;

	jointL								= (b2RevoluteJoint*)	App->physics->world->CreateJoint(&revoluteJointDef);
	// ---------------------------------------------------------------------------------
	
	// Right flipper		------------------------------------------------------------

	flipperR							= App->physics->CreateRectangle(327, 770, 80, 20);
	pivotR								= App->physics->CreateCircle(327, 770, 10, b2_staticBody);


	revoluteJointDef.bodyA				= flipperR->body;
	revoluteJointDef.bodyB				= pivotR->body;

	revoluteJointDef.localAnchorA.Set(PIXEL_TO_METERS(-30), 0);		// Set the pivot point of the rectangle where the center of the circle is
	revoluteJointDef.localAnchorB.Set(0, 0);						// Set the pivot point of the circle on its center

	revoluteJointDef.upperAngle			= 210 * DEGTORAD;			// Angle limits
	revoluteJointDef.lowerAngle			= 150 * DEGTORAD;

	revoluteJointDef.motorSpeed			= -1500.0f * DEGTORAD;		// Motor
	revoluteJointDef.maxMotorTorque		= 1500;

	jointR								= (b2RevoluteJoint*)App->physics->world->CreateJoint(&revoluteJointDef);

	// ---------------------------------------------------------------------------------

	// Ball launcher		------------------------------------------------------------

	launcher							= App->physics->CreateRectangle(800, 500, 20, 80);
	launcher_pivot						= App->physics->CreateRectangle(800, 550, 20, 20, b2_staticBody);

	b2PrismaticJointDef prismaticJointDef;

	prismaticJointDef.bodyA				= launcher->body;
	prismaticJointDef.bodyB				= launcher_pivot->body;

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
	// ----------------------------------------------------------

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

	// BALL BLIT		-----------------------------------------
	
	int x, y;
	//ball->GetPosition(x, y);
	//App->renderer->Blit(ball_tex, x, y, NULL, 1.0f, ball->GetRotation());
		
	// ----------------------------------------------------------


	return UPDATE_CONTINUE;
}



