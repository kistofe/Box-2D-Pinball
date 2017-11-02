#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "ModuleSceneIntro.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);
	  
	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, b2BodyType type, float restitution, uint _score)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.restitution = restitution;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;
	pbody->score = _score;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, float angle, b2BodyType type, float restitution, uint _score)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b->SetTransform(body.position, angle);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.restitution = restitution;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;
	pbody->score = _score;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height, float angle, uint _score)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b->SetTransform(body.position, angle);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;
	pbody->score = _score;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, float restitution, uint _score)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	

	b2Body* b = world->CreateBody(&body);
	
	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.restitution = restitution;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;
	pbody->score = _score;
	
	return pbody;
}

PhysBody * ModulePhysics::CreateCircleSensor(int x, int y, int radius, uint _score)
{

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;
	pbody->score = _score;

	return pbody;
}

void ModulePhysics::AddPinballParts()
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

	App->scene_intro->pinball.add(CreateChain(0, 0, pinball_shape, 84));
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

	App->scene_intro->pinball.add(CreateChain(0, 0, pinball_shape2, 58));
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

	App->scene_intro->pinball.add(CreateChain(0, 0, scenario_shape1, 32));
	//-------------------------------------------------------------------

	//Part 2 
	int scenario_shape2[16] = {
		328, 781,
		420, 719,
		420, 621,
		413, 617,
		408, 621,
		408, 692,
		319, 756,
		326, 776
	};

	App->scene_intro->pinball.add(CreateChain(0, 0, scenario_shape2, 16));
	//-------------------------------------------------------------------

	//Part 3 
	int scenario_shape3[14] = {
		71, 619,
		61, 619,
		60, 718,
		151, 781,
		160, 757,
		72, 691,
		71, 624
	};


	App->scene_intro->pinball.add(CreateChain(0, 0, scenario_shape3, 14));
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

	App->scene_intro->pinball.add(CreateChain(0, 0, scenario_shape4, 34));
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

	App->scene_intro->pinball.add(CreateChain(0, 0, scenario_shape5, 28));
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

	App->scene_intro->pinball.add(CreateChain(0, 0, scenario_shape6, 48));
	//-------------------------------------------------------------------

	//Part 7 & 8
	App->scene_intro->pinball.add(CreateRectangle(207, 167, 18, 55, 0, b2_staticBody));
	App->scene_intro->pinball.add(CreateRectangle(273, 152, 18, 45, 0, b2_staticBody));
	//-------------------------------------------------------------------

	//Part 9
	int scenario_shape9[8] = {
		336, 709,
		370, 681,
		372, 631,
		338, 703
	};

	//Main part
	App->scene_intro->pinball.add(CreateChain(0, 0, scenario_shape9, 8));
	//External part with restitution
	int secondary_shape[8] = {
		363, 626,
		326, 700,
		332, 700,
		367, 629
	};

	App->scene_intro->pinball.add(CreateChain(0, 0, secondary_shape, 8, 2.0f, 20));
	// ------------------------------------------------------------------

	//Part 10
	int scenario_shape10[8] = {
		106, 632,
		106, 680,
		144, 709,
		110, 637
	};

	//Main part
	App->scene_intro->pinball.add(CreateChain(0, 0, scenario_shape10, 8));
	//External part with restitution
	int secondary_shape2[8] = {
		114, 626,
		152, 701,
		144, 697,
		110, 629
	};

	App->scene_intro->pinball.add(CreateChain(0, 0, secondary_shape2, 8, 2.0f, 20));

	// Circle between flippers
	App->scene_intro->pinball.add(CreateCircle(240, 835, 5, b2_staticBody));
	// -----------------------------------------------------------------


}

// 
update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	b2Body* body_found = nullptr;
	b2Vec2 mouse_pos(PIXEL_TO_METERS(App->input->GetMouseX()),PIXEL_TO_METERS(App->input->GetMouseY()));
	
	
	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 0, 0, 0);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}

			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				if (f->TestPoint(mouse_pos) && !body_found)
				{
					body_found = f->GetBody();
					break;
				}
			}
			
			// test if the current body contains mouse position
		}
	}

	// If a body was selected we will attach a mouse joint to it
	// so we can pull it around
	if (body_found != nullptr)
	{
		b2MouseJointDef def;
		def.bodyA = ground;
		def.bodyB = body_found;
		def.target = mouse_pos;
		def.dampingRatio = 0.5f;
		def.frequencyHz = 2.0f;
		def.maxForce = 100.0f * body_found->GetMass();
		mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && mouse_joint)
	{
		mouse_joint->SetTarget(mouse_pos);
	}

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && mouse_joint)
	{
		world->DestroyJoint(mouse_joint);
		mouse_joint = nullptr;
	}


	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}


void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}