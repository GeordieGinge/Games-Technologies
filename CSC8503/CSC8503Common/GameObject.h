#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

#include <vector>

using std::vector;

namespace NCL {
	namespace CSC8503 {
		class NetworkObject;

		class GameObject	{
		public:
			GameObject(string name = "");
			~GameObject();

			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}

			bool IsActive() const {
				return isActive;
			}

			const Transform& GetConstTransform() const {
				return transform;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			NetworkObject* GetNetworkObject() const {
				return networkObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			void PickUpObject(GameObject* otherObj);
			void GetObjectposition(GameObject* goose);
			void DropApple();
			void ThrowApple();

			GameObject* pickedUpObject;

			const string& GetName() const {
				return name;
			}
			void SetName(string n) { name = n; }

			virtual void OnCollisionBegin(GameObject* otherObject)
			{
				if (this->name == "goose" && otherObject->name == "trampoline" || this->name == "ball" && otherObject->name == "trampoline")
				{
					if (this->name == "goose") { this->GetPhysicsObject()->AddForce(Vector3(0, 4500, 0)); }
					else if (this->name == "ball") { this->GetPhysicsObject()->AddForce(Vector3(0, 200, 0)); }
				}
				if (this->name == "goose" && otherObject->name == "1Platform")
				{
					this->GetPhysicsObject()->AddForce(Vector3(50000, 100, 0));
				}

				if (this->name == "goose" && otherObject->name == "2Platform")
				{
					this->GetPhysicsObject()->AddForce(Vector3(0, 100, -50000));
				}

				if (this->name == "goose" && otherObject->name == "3Platform")
				{
					this->GetPhysicsObject()->AddForce(Vector3(-50000, 100, 0));
				}

				if (this->name == "goose" && otherObject->name == "4Platform")
				{
					this->GetPhysicsObject()->AddForce(Vector3(0, 100, 30000));
				}

				if (otherObject->name == "water") 
				{
					this->GetPhysicsObject()->SetFriction(Vector3(5, 1, 5));
				}

				if (this->name == "goose" && otherObject->name == "apple")
				{
					PickUpObject(otherObject);
				}

			}

			virtual void OnCollisionEnd(GameObject* otherObject) 
			{
				if (otherObject->name == "water") 
				{
					this->GetPhysicsObject()->SetFriction(Vector3(1, 1, 1));
				}
			}

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();

		protected:
			Transform			transform;

			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;
			NetworkObject*		networkObject;

			bool	isActive;
			string	name;

			Vector3 broadphaseAABB;
		};
	}
}

