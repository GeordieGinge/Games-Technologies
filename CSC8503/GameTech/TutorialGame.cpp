#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() 
{
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	physics = new PhysicsSystem(*world);

	forceMagnitude = 10.0f;
	physics->UseGravity(true);
	inSelectionMode = false;

	Debug::SetRenderer(renderer);

	InitialiseAssets();
}

/*
Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!
*/

void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into)
	{
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &cubeMesh);
	loadFunc("sphere.msh", &sphereMesh);
	loadFunc("goose.msh", &gooseMesh);
	loadFunc("CharacterA.msh", &keeperMesh);
	loadFunc("CharacterM.msh", &charA);
	loadFunc("CharacterF.msh", &charB);
	loadFunc("Apple.msh", &appleMesh);

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("brick.PNG");
	waterTex = (OGLTexture*)TextureLoader::LoadAPITexture("water.JPG");
	trampoTex = (OGLTexture*)TextureLoader::LoadAPITexture("trampo.JPG");
	forceTex = (OGLTexture*)TextureLoader::LoadAPITexture("force.JPG");
	ballTex = (OGLTexture*)TextureLoader::LoadAPITexture("ball.JPG");

	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	
{
	delete cubeMesh;
	delete sphereMesh;
	delete gooseMesh;
	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void TutorialGame::UpdateGame(float dt) {

	LockedCameraMovement();

	UpdateKeys();

	/*if (useGravity)
	{
		Debug::Print("(G)ravity on", Vector2(50, 40));
	}
	else
	{
		Debug::Print("(G)ravity off", Vector2(50, 40));
	}*/

	if (characterobj != nullptr)
	{
		characterobj->GetObjectposition(characterobj->pickedUpObject);
	}

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	Debug::FlushRenderables();
	renderer->Render();
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) 
	{
		InitWorld(); //We can reset the simulation at any time with F1
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) 
	{
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	/*if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) 
	{
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}*/

	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) 
	{
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) 
	{
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) 
	{
		world->ShuffleObjects(true);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) 
	{
		world->ShuffleObjects(false);
	}

	LockedObjectMovement();
}

void TutorialGame::LockedObjectMovement()
{
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 70, 0), rightAxis);

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
	{
		characterobj->GetPhysicsObject()->AddTorque(Vector3(0, 5, 0));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
	{
		characterobj->GetPhysicsObject()->AddTorque(-Vector3(0, 5, 0));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
	{
		characterobj->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
	{
		characterobj->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
	{
		characterobj->GetPhysicsObject()->AddForce(Vector3(0, 1000, 0));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT))
	{
		characterobj->GetPhysicsObject()->AddForce(Vector3(0, -500, 0));
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::Q))
	{
		characterobj->DropApple();
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::F))
	{
		characterobj->ThrowApple();
	}

}

	void  TutorialGame::LockedCameraMovement()
	{
		Vector3 objPos = characterobj->GetTransform().GetWorldPosition();
		Vector3 camPos = objPos + characterobj->GetConstTransform().GetLocalOrientation() * lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}


/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
}

void TutorialGame::InitWorld()
{
	world->ClearAndErase();
	physics->Clear();

	InitMixedGridWorld(10, 10, 3.5f, 3.5f);
	characterobj = AddGooseToWorld(Vector3(-60, 0.5f, -80));

	AddFloorToWorld(Vector3(0, 0, 0));

	AddAppleToWorld(Vector3(35, 2, 0));

	AddParkKeeperToWorld(Vector3(40, 2, 0));

	AddCharacterToWorld(Vector3(45, 2, 0));

	AddPlatform1ToWorld(Vector3(-68, 75, 90), Vector3(30, 1, 10));
	AddPlatform2ToWorld(Vector3(88, 75, 70), Vector3(10, 1, 30));
	AddPlatform3ToWorld(Vector3(68, 75, -90), Vector3(30, 1, 10));
	AddPlatform4ToWorld(Vector3(-88, 75, -70), Vector3(10, 1, 30));

	AddTrampolineToWorld(Vector3(20, 0.3, 10), Vector3(15, 0.2f, 15));
	AddTrampolineToWorld(Vector3(-93, 0.6f, -80), Vector3(5, 0.5f, 5));
	AddTrampolineToWorld(Vector3(-93, 10, -60), Vector3(5, 0.5f, 5));
	AddTrampolineToWorld(Vector3(-93, 20, -40), Vector3(5, 0.5f, 5));
	AddTrampolineToWorld(Vector3(-93, 30, -20), Vector3(5, 0.5f, 5));
	AddTrampolineToWorld(Vector3(-93, 40, 0), Vector3(5, 0.5f, 5));
	AddTrampolineToWorld(Vector3(-93, 50, 20), Vector3(5, 0.5f, 5));
	AddTrampolineToWorld(Vector3(-93, 60, 40), Vector3(5, 0.5f, 5));
	AddTrampolineToWorld(Vector3(-93, 70, 60), Vector3(5, 0.5f, 5));

	AddWaterToWorld(Vector3(0, -0.9, 0));

	AddWallToWorld(Vector3(Vector3(101, 1, 0)), Vector3(1, 1, 100)); // Right wall
	AddWallToWorld(Vector3(Vector3(-101, 1, 0)), Vector3(1, 1, 100)); //Left Wall
	AddWallToWorld(Vector3(Vector3(0, 1, -101)), Vector3(102, 1, 1)); // Back wall
	AddWallToWorld(Vector3(Vector3(0, 1, 101)), Vector3(102, 1, 1)); // Front wall
}

GameObject* TutorialGame::AddWallToWorld(Vector3 position, Vector3 scale)
{
	GameObject* wall = new GameObject();
	AABBVolume* volume = new AABBVolume(Vector3(scale));
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform().SetWorldPosition(position);
	wall->GetTransform().SetWorldScale(Vector3(scale));
	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, basicTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(wall);
	return wall;
}

GameObject* TutorialGame::AddPlatform1ToWorld(Vector3 position, Vector3 scale)
{
	GameObject* wall = new GameObject();
	AABBVolume* volume = new AABBVolume(Vector3(scale));
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform().SetWorldPosition(position);
	wall->GetTransform().SetWorldScale(Vector3(scale));
	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, forceTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	wall->SetName("1Platform");
	world->AddGameObject(wall);
	return wall;
}

GameObject* TutorialGame::AddPlatform2ToWorld(Vector3 position, Vector3 scale)
{
	GameObject* wall = new GameObject();
	AABBVolume* volume = new AABBVolume(Vector3(scale));
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform().SetWorldPosition(position);
	wall->GetTransform().SetWorldScale(Vector3(scale));
	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, forceTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	wall->SetName("2Platform");
	world->AddGameObject(wall);
	return wall;
}

GameObject* TutorialGame::AddPlatform3ToWorld(Vector3 position, Vector3 scale)
{
	GameObject* wall = new GameObject();
	AABBVolume* volume = new AABBVolume(Vector3(scale));
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform().SetWorldPosition(position);
	wall->GetTransform().SetWorldScale(Vector3(scale));
	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, forceTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	wall->SetName("3Platform"); 
	world->AddGameObject(wall);
	return wall;
}

GameObject* TutorialGame::AddPlatform4ToWorld(Vector3 position, Vector3 scale)
{
	GameObject* wall = new GameObject();
	AABBVolume* volume = new AABBVolume(Vector3(scale));
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform().SetWorldPosition(position);
	wall->GetTransform().SetWorldScale(Vector3(scale));
	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, forceTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	wall->SetName("4Platform");
	world->AddGameObject(wall);
	return wall;
}

GameObject* TutorialGame::AddTrampolineToWorld(Vector3 position, Vector3 scale)
{
	GameObject* wall = new GameObject();
	AABBVolume* volume = new AABBVolume(Vector3(scale));
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform().SetWorldPosition(position);
	wall->GetTransform().SetWorldScale(Vector3(scale));
	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, trampoTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();
	
	wall->SetName("trampoline");

	world->AddGameObject(wall);
	return wall;
}

GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject();

	Vector3 floorSize = Vector3(100, 0.1, 100);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

GameObject* TutorialGame::AddWaterToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("water");

	Vector3 floorSize = Vector3(200, 0.5f, 200);
	AABBVolume* volume = new AABBVolume(floorSize - Vector3(0, 0.9f, 0));
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(floorSize);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, waterTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, ballTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();
	sphere->SetName("ball");

	sphere->GetPhysicsObject()->SetUsingGravity(true);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddGooseToWorld(const Vector3& position)
{
	float size			= 1.0f;
	float inverseMass	= 1.0f;

	GameObject* goose = new GameObject();

	SphereVolume* volume = new SphereVolume(size);
	goose->SetBoundingVolume((CollisionVolume*)volume);


	goose->GetTransform().SetWorldScale(Vector3(size,size,size) );
	goose->GetTransform().SetWorldPosition(position);
	goose->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(0, 180, 0));//Fixes the goose rotation

	goose->SetRenderObject(new RenderObject(&goose->GetTransform(), gooseMesh, nullptr, basicShader));
	goose->SetPhysicsObject(new PhysicsObject(&goose->GetTransform(), goose->GetBoundingVolume()));

	goose->GetPhysicsObject()->SetInverseMass(inverseMass);
	goose->GetPhysicsObject()->InitSphereInertia();
	goose->SetName("goose");

	goose->GetPhysicsObject()->SetUsingGravity(true);

	world->AddGameObject(goose);
	return goose;
}

GameObject* TutorialGame::AddParkKeeperToWorld(const Vector3& position)
{
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	

	GameObject* keeper = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	keeper->SetBoundingVolume((CollisionVolume*)volume);


	keeper->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	keeper->GetTransform().SetWorldPosition(position);

	keeper->SetRenderObject(new RenderObject(&keeper->GetTransform(), keeperMesh, nullptr, basicShader));
	keeper->SetPhysicsObject(new PhysicsObject(&keeper->GetTransform(), keeper->GetBoundingVolume()));

	keeper->GetPhysicsObject()->SetInverseMass(inverseMass);
	keeper->GetPhysicsObject()->InitCubeInertia();

	keeper->GetPhysicsObject()->SetUsingGravity(true);
	world->AddGameObject(keeper);

	return keeper;
}

GameObject* TutorialGame::AddCharacterToWorld(const Vector3& position) {
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	auto pos = keeperMesh->GetPositionData();



	Vector3 minVal = pos[0];
	Vector3 maxVal = pos[0];

	for (auto& i : pos) {
		maxVal.y = max(maxVal.y, i.y);
		minVal.y = min(minVal.y, i.y);
	}

	GameObject* character = new GameObject();

	float r = rand() / (float)RAND_MAX;


	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	character->GetTransform().SetWorldPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), r > 0.5f ? charA : charB, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitCubeInertia();

	character->GetPhysicsObject()->SetUsingGravity(true);
	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddAppleToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();


	SphereVolume* volume = new SphereVolume(0.7f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform().SetWorldScale(Vector3(-5, 4, 4));
	apple->GetTransform().SetWorldPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), appleMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	apple->SetName("apple");
	apple->GetPhysicsObject()->SetUsingGravity(true);
	world->AddGameObject(apple);

	return apple;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) 
{
	float sphereRadius = 1.0f;
	
	for (int x = 0; x < numCols; ++x)
	{
		for (int z = 0; z < numRows; ++z)
		{
			Vector3 position = Vector3(x * colSpacing, 10.0f , z * rowSpacing);
			{
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

