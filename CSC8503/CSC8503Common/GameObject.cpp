#include "GameObject.h"
#include "CollisionDetection.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName)	{
	name			= objectName;
	isActive		= true;
	boundingVolume	= nullptr;
	physicsObject	= nullptr;
	renderObject	= nullptr;
	networkObject	= nullptr;
	pickedUpObject = nullptr;
}

GameObject::~GameObject()	{
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;
	delete pickedUpObject;
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

//These would be better as a virtual 'ToAABB' type function, really...
void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetWorldOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
}

void GameObject::GetObjectposition(GameObject* goose)
{
	if (this != nullptr)
	{
		if (this->pickedUpObject != nullptr)
		{
			this->pickedUpObject->GetTransform().SetLocalPosition(Vector3(this->GetTransform().GetWorldPosition().x,
				this->GetTransform().GetWorldPosition().y + 4, 
				this->GetTransform().GetWorldPosition().z));
			this->pickedUpObject->GetTransform().SetLocalOrientation(this->GetTransform().GetLocalOrientation());
		}
	}
}

void GameObject::PickUpObject(GameObject* otherObj)
{
	if (otherObj != nullptr && this->pickedUpObject == nullptr)
	{
			this->pickedUpObject = otherObj;
			this->pickedUpObject->GetPhysicsObject()->SetUsingGravity(false);
	}
}

void GameObject::ThrowApple()
{
	if (this->pickedUpObject != nullptr)
	{
		this->pickedUpObject->GetPhysicsObject()->AddForceAtPosition(Vector3(5000, 0, 0),
			Vector3(this->GetTransform().GetLocalOrientation().x,
				this->GetTransform().GetLocalOrientation().y,
				this->GetTransform().GetLocalOrientation().z));

		this->pickedUpObject->GetPhysicsObject()->SetUsingGravity(true);
		this->pickedUpObject->GetTransform().SetParent(NULL);
		this->pickedUpObject = NULL;
	}
}

void GameObject::DropApple()
{
	if (this->pickedUpObject != nullptr)
	{
		this->pickedUpObject->GetPhysicsObject()->SetUsingGravity(true);
		this->pickedUpObject->GetTransform().SetParent(NULL);
		this->pickedUpObject->GetPhysicsObject()->AddForce(Vector3(1000, 0, 1000));
		this->pickedUpObject = NULL;
	}
}