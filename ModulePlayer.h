#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "Box2D/Box2D/Box2D.h"

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

private:

public:

private:

	PhysBody* flipperL;
	PhysBody* pivotL;
	b2RevoluteJoint* jointL;

	PhysBody* flipperR;
	PhysBody* pivotR;
	b2RevoluteJoint* jointR;

	PhysBody* launcher;
	PhysBody* launcher_pivot;
	b2PrismaticJoint* jointLauncher;

	//PhysBody* ball;
	//SDL_Texture* ball_tex;
};