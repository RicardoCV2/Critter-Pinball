#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	ray_on = false;
	sensed = false;
	open = false;
	canon_shoot = false;
	boost_shot = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	b2RevoluteJointDef def_1;
	b2RevoluteJointDef def_2;
	b2RevoluteJoint* joint_1;
	b2RevoluteJoint* joint_2;

	App->renderer->camera.x = App->renderer->camera.y = 0;


	//--------------------------Image sources------------------------------------------------

	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	BackGround= App->textures->Load("pinball/PinBall_Board.png");
	RightFlipper = App->textures->Load("pinball/right flipper.png");
	LeftFlipper = App->textures->Load("pinball/left flipper.png");
	RightBouncer = App->textures->Load("pinball/right block.png");
	LeftBouncer = App->textures->Load("pinball/left block.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");


	//sensor = App->physics->CreateRectangleSensor(455+10, 834+5, 25, 21);


	//--------------------------Scene Elements------------------------------------------------
	
	bouncer_1 = new PhysBody();
	bouncer_1 = App->physics->CreateCircle(267, 155, 22, b2_staticBody, 2.0f);
	B_1sensor = App->physics->CreateCircle(267, 155, 24, b2_staticBody, 2.0f,true);
	B_1sensor->listener = this;
	
	bouncer_2 = new PhysBody();
	bouncer_2 = App->physics->CreateCircle(370, 162, 22, b2_staticBody, 2.0f);
	B_2sensor = App->physics->CreateCircle(370, 162, 24, b2_staticBody, 2.0f, true);
	B_2sensor->listener = this;
	
	bouncer_3 = new PhysBody();
	bouncer_3 = App->physics->CreateCircle(306, 223, 22, b2_staticBody, 2.0f);
	B_3sensor = App->physics->CreateCircle(306, 223, 24, b2_staticBody, 2.0f, true);
	B_3sensor->listener = this;

	slide_block = App->physics->CreateRectangle(253, 26, 5, 31, b2_staticBody);
	cannon_block = App->physics->CreateRectangle(461, 150, 100, 10, b2_staticBody);
	cannon_block->body->SetTransform(cannon_block->body->GetWorldCenter(), -78 * 0.0174532925);

	sensorblocker = new PhysBody();
	sensorblocker = App->physics->CreateRectangleSensor(330, 110, 100, 1);
	sensorblocker->listener = this;
	
	canon_sensor = App->physics->CreateRectangleSensor(17, 845, 30, 30);
	canon_sensor->listener = this;

	boost_sensor = App->physics->CreateRectangleSensor(425, 400, 23, 29);
	boost_sensor->listener = this;
	
	int left_block[22] = {
		11, 16,
		11, 105,
		53, 134,
		56, 135,
		62, 135,
		69, 129,
		69, 122,
		32, 13,
		28, 10,
		17, 9,
		11, 16,
	};
	
	leftbouncer.add(App->physics->CreateChain(51, 600, left_block, 21, b2_staticBody, 0.3f));

	int left_block_bouncer[10] = {
		372, 610,
		334, 722,
		348, 723,
		380, 616,
		372, 610
	};
	
	App->physics->CreateChain(0, 0, left_block_bouncer, 9, b2_staticBody, 1.6f);


	int right_block[20] = {
		61, 10,
		50, 9,
		44, 15,
		9, 123,
		9, 129,
		16, 135,
		22, 135,
		67, 106,
		67, 16,
		61, 9
	};
	
	rightbouncer.add(App->physics->CreateChain(325, 600, right_block, 19, b2_staticBody, 0.3f));

	int right_block_bouncer[10] = {
		121, 723,
		83, 612,
		74, 616,
		107, 724,
		121, 723
	};
	
	App->physics->CreateChain(0, 0, right_block_bouncer, 9, b2_staticBody, 1.6f);
	
	//----------------------------------------------------------------------------------------
	
	
	rightflipper = App->physics->CreateRectangle(250 + 30, 790 + 15, 77, 14, b2_dynamicBody);
	leftflipper = App->physics->CreateRectangle(140 + 41, 790 + 18, 77, 14, b2_dynamicBody);

	l_flipper_joint =App->physics->CreateCircle(144+9, 800+8, 5, b2_staticBody, 0.0f);
	r_flipper_joint =App->physics->CreateCircle(300 + 7, 800 + 3, 5, b2_staticBody, 0.0f);

	def_1.Initialize(leftflipper->body, l_flipper_joint->body, l_flipper_joint->body->GetWorldCenter());
	def_2.Initialize(r_flipper_joint->body, rightflipper->body, r_flipper_joint->body->GetWorldCenter());
	
	def_1.enableLimit = true;
	def_2.enableLimit = true;
	
	def_1.lowerAngle = -30 * DEGTORAD;
	def_1.upperAngle = 30 * DEGTORAD;
	def_2.lowerAngle = -30 * DEGTORAD;
	def_2.upperAngle = 30 * DEGTORAD;

	joint_1 = (b2RevoluteJoint*)App->physics->world->CreateJoint(&def_1);
	joint_2 = (b2RevoluteJoint*)App->physics->world->CreateJoint(&def_2);

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
	App->renderer->Blit(BackGround, 0, 0, NULL);

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 11, b2_dynamicBody, 0.0f));
		circles.getLast()->data->listener = this;
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		boxes.add(App->physics->CreateRectangle(App->input->GetMouseX(), App->input->GetMouseY(), 100, 50,b2_dynamicBody));
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// Pivot 0, 0
		int rick_head[64] = {
			14, 36,
			42, 40,
			40, 0,
			75, 30,
			88, 4,
			94, 39,
			111, 36,
			104, 58,
			107, 62,
			117, 67,
			109, 73,
			110, 85,
			106, 91,
			109, 99,
			103, 104,
			100, 115,
			106, 121,
			103, 125,
			98, 126,
			95, 137,
			83, 147,
			67, 147,
			53, 140,
			46, 132,
			34, 136,
			38, 126,
			23, 123,
			30, 114,
			10, 102,
			29, 90,
			0, 75,
			30, 62
		};

		ricks.add(App->physics->CreateChain(App->input->GetMouseX(), App->input->GetMouseY(), rick_head, 64,b2_dynamicBody, 0.0f));
	
	}

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
			App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		if(ray_on)
		{
			int hit = c->data->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);
			if(hit >= 0)
				ray_hit = hit;
		}
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = leftbouncer.getFirst();
	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(LeftBouncer, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = rightbouncer.getFirst();
	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(RightBouncer, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	if (leftflipper != NULL)
	{
		int x, y;
		leftflipper->GetPosition(x, y);
		App->renderer->Blit(LeftFlipper, x, y, NULL, 1.0f, leftflipper->GetRotation());
	}


	if (rightflipper != NULL)
	{
		int x, y;
		rightflipper->GetPosition(x, y);
		App->renderer->Blit(RightFlipper, x, y, NULL, 1.0f, rightflipper->GetRotation());
	}

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

	if (open==true && slide_block->body!=nullptr)
	{
		slide_block->body->GetWorld()->DestroyBody(slide_block->body);
		slide_block->body = nullptr;
		open = false;
		cannon_block->body->SetActive(true);
	}

	if (canon_shoot == true)
	{
		if (SDL_GetTicks() >= ticks)
		{
			App->player->Ball->body->ApplyForce({ 0.0f,-200.0f },App->player->Ball->body->GetLocalCenter(),true);
			LOG("force!");
			canon_shoot = false;
		}
	}
	if (boost_shot)
	{
		if (SDL_GetTicks() >= ticks)
		{
			App->player->Ball->body->ApplyForce({ -100.0f,100.0f }, App->player->Ball->body->GetLocalCenter(), true);
			LOG("BOOOOOST!!");
			boost_shot = false;
		}
	}

	char score[64];
	char lives[4];
	char Title[64] = "PinBall Score: ";
	char Num_lives[32] = "Lives: ";
	sprintf_s(score, "%d   ", App->player->score);
	sprintf_s(lives, "%d", App->player->lives);
	strcat_s(Title, score);
	strcat_s(Title, Num_lives);
	strcat_s(Title, lives);

	App->window->SetTitle(Title);

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;
	b2Vec2 stop = { 0.0f,0.0f };

	if(bodyA)
	{
		if (bodyA == sensorblocker && slide_block->body != nullptr)
		{
			
			open = true;
		}

		if (App->player->getpoints==false && (bodyA == B_1sensor || bodyA == B_2sensor || bodyA == B_3sensor))
		{
			App->audio->PlayFx(bonus_fx);
			LOG("puntos");
			App->player->getpoints = true;
		}

		if (bodyA == canon_sensor && canon_shoot == false && App->player->Ball->body->GetLinearVelocity()==stop)
		{
			canon_shoot = true;
			Timer(1000);
		}
		if (bodyA == boost_sensor && boost_shot == false)
		{
			boost_shot = true;
		}
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}
}

void ModuleSceneIntro::Timer(int time)
{
	ticks = SDL_GetTicks() + time;
}