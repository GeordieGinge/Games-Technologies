#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"


namespace NCL {
	namespace CSC8503 {
		class TutorialGame {
		public:
			TutorialGame();
			~TutorialGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld();

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on).
			*/
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			//void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void BridgeConstraintTest();
			void SimpleGJKTest();

			void LockedObjectMovement();
			void LockedCameraMovement();

			GameObject* AddWallToWorld(Vector3 position, Vector3 scale);
			GameObject* AddPlatform1ToWorld(Vector3 position, Vector3 scale);
			GameObject* AddPlatform2ToWorld(Vector3 position, Vector3 scale);
			GameObject* AddPlatform3ToWorld(Vector3 position, Vector3 scale);
			GameObject* AddPlatform4ToWorld(Vector3 position, Vector3 scale);
			GameObject* AddTrampolineToWorld(Vector3 position, Vector3 scale);
			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddWaterToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);

			//IT'S HAPPENING
			GameObject* AddGooseToWorld(const Vector3& position);
			GameObject* AddParkKeeperToWorld(const Vector3& position);
			GameObject* AddCharacterToWorld(const Vector3& position);
			GameObject* AddAppleToWorld(const Vector3& position);


			GameTechRenderer* renderer;
			PhysicsSystem* physics;
			GameWorld* world;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;


			OGLMesh* cubeMesh = nullptr;
			OGLMesh* sphereMesh = nullptr;
			OGLTexture* basicTex = nullptr;
			OGLTexture* waterTex = nullptr;
			OGLShader* basicShader = nullptr;

			//Coursework Meshes
			OGLMesh* gooseMesh = nullptr;
			OGLMesh* keeperMesh = nullptr;
			OGLMesh* appleMesh = nullptr;
			OGLMesh* charA = nullptr;
			OGLMesh* charB = nullptr;

			//Coursework Additional functionality	
			GameObject* characterobj = nullptr;
			Vector3 lockedOffset = Vector3(0, 3.5f, -20);
		};
	}
}

