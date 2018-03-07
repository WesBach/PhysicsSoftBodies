#include "cSoftBody.h"
#include <glm\glm.hpp>

namespace nPhysics {

	cSoftBody::cSoftBody(const sSoftBodyDesc& desc) {
		this->ConstructNodes(desc);
		this->SetStaticNodes(desc);
	}

	cSoftBody::~cSoftBody()
	{
	}

	cSoftBody::cNode::cNode(const glm::vec3& pos) {
		this->Position = pos;
		this->PreviousPosition = pos;
	}

	void cSoftBody::cNode::ApplyForce(const glm::vec3& force) {
		this->Acceleration = force / Mass;
	}

	void cSoftBody::ConstructNodes(const sSoftBodyDesc& desc) {

		//create all the nodes wit the position adn id
		for (int i = 0; i < desc.Vertices.size(); i++)
		{
			cNode* node = new cNode(desc.Vertices[i]);
			node->Id = i;
			this->Nodes.push_back(node);
		}

		//iterate through the triangles and make the connections
		for (int i = 0; i < desc.TriangulatedIndices.size(); i++)
		{
			//TODO::
			//get the closest points to this point from the vector of points.
			//make nodes out of them and springs between them. 
			//check for duplicate springs to make sure theres no overlapping.
			//set these to null first
			cNode* node1 = this->Nodes[desc.TriangulatedIndices[i]->vertex_ID_0];
			cNode* node2 = this->Nodes[desc.TriangulatedIndices[i]->vertex_ID_1];
			cNode* node3 = this->Nodes[desc.TriangulatedIndices[i]->vertex_ID_2];;

			//check to make sure the node doesn't already have a spring with that node
			bool spring1Exists = node1->DoesSpringExist(node2);
			bool spring2Exists = node2->DoesSpringExist(node3);
			bool spring3Exists = node3->DoesSpringExist(node1);

			//create the springs for the nodes if they dont exist(each node gets one spring)
			if (spring1Exists == false)
			{
				cSpring* spring1 = new cSpring(node1, node2,glm::distance(node1->Position,node2->Position)+ 1.0f);
				node1->Springs.push_back(spring1);
			}

			if (spring2Exists == false)
			{
				cSpring* spring2 = new cSpring(node2, node3,glm::distance(node2->Position, node3->Position) + 1.0f);
				node2->Springs.push_back(spring2);
			}

			if (spring3Exists == false)
			{
				cSpring* spring3 = new cSpring(node3, node1, glm::distance(node3->Position, node1->Position) + 1.0f);
				node3->Springs.push_back(spring3);
			}
		}
	}

	bool cSoftBody::cNode::DoesSpringExist(cNode* other) {

		for (int i = 0; i < this->Springs.size(); i++)
		{
			//check to see if the spring exists
			if (this->Springs[i]->GetOther(this) == other)
			{
				return true;
			}
		}
		//doesn't exist
		return false;
	}

	void cSoftBody::SetStaticNodes(const sSoftBodyDesc& desc) {
		//set all the static indices
		for (int i = 0; i < desc.StaticIndices.size(); i++)
		{
			//go through each node to find the corrext id
			for (int nodeIndex = 0; nodeIndex < this->Nodes.size(); nodeIndex++)
			{
				//find the node and set it to static
				if (desc.StaticIndices[i] == this->Nodes[nodeIndex]->Id)
				{
					this->Nodes[desc.StaticIndices[i]]->IsStatic = true;
					break;
				}
			}
		}
	}


	cSoftBody::cSoftBody() {

	}

	void cSoftBody::GetNodePosition(size_t index, glm::vec3& nodePositionOnMesh) {
		//set the nodePositionOnMesh
		if (this->NumNodes() >= index){
			nodePositionOnMesh = this->Nodes[index]->Position;
		}
	}

	size_t cSoftBody::NumNodes() {
		if(this->Nodes.size() == 0){
			return 0;
		}
		else {
			return this->Nodes.size() - 1;
		}
	}


	cSoftBody::cSpring::cSpring(cNode* nodeA, cNode* nodeB,float restingDist) {
		this->NodeA = nodeA;
		this->NodeB = nodeB;
		this->RestingSeparation = restingDist;
	}

	cSoftBody::cNode* cSoftBody::cSpring::GetOther(cNode* me) {
		if (me == this->NodeA){
			return this->NodeB;
		}
		else{
			return this->NodeA;
		}
	}

	void cSoftBody::UpdateNodes(const float& deltaTime) {
		//go through the nodes and update each
		for (int i = 0; i < this->Nodes.size(); i++) {
			//go through each spring
			for (int springIndex = 0; springIndex < this->Nodes[i]->Springs.size(); springIndex++) {
				//Force --> Acceleration --> Velocity --> Position 
				// Force = -stiffnes * (length - rest length)
				//acceleration = Force / mass 
				//acceleration(endpoint 1) = -Force / mass(endpoint 1) 
				//acceleration(endpoint 2) = Force / mass(endpoint 2)
				
				//  then we find change in velocity and add it to the existing velocity
				//	mass_1_Velocity += mass_1_Acceleration * timestep
				//	mass_2_Velocity += mass_2_Acceleration * timestep
				//
				//  finally we find the change in position and add it to the existing position
				//	Mass_1_Position += mass_1_velocity * timestep
				//	Mass_2_Position += mass_2_velocity * timestep*/

				//F = -k(| x | -d)(x/|x | ) - bv
				//k = spring const
				//x - d = currDist - desired dist
				//(x/|x | ) = currpos - otherpos
				//use negative force for the nodeA

				cNode* nodeA = NULL;
				cNode* nodeB = NULL;
				//set the nodes
				nodeA = this->Nodes[i];
				nodeB = this->Nodes[i]->Springs[springIndex]->GetOther(nodeA);

				glm::vec3 force = -this->Nodes[i]->Springs[springIndex]->SpringConstant
								* (this->Nodes[i]->Springs[springIndex]->CurrentSeparation - this->Nodes[i]->Springs[springIndex]->RestingSeparation)
								*(nodeA->Position - nodeB->Position); //TODO:: check to make sure this is the correct direction vector

				//accelerate in the correct direction
				if (nodeA->IsStatic != true)
				{
					nodeA->Acceleration = -force / nodeA->Mass;
					nodeA->Velocity += nodeA->Acceleration * deltaTime;
					nodeA->Velocity *= 0.9f;
					nodeA->Position += (nodeA->Velocity/*/2.f*/) * deltaTime;
					nodeA->IsUpdated = true;
				}

				if (nodeB->IsStatic != true)
				{
					nodeB->Acceleration = force / nodeB->Mass;
					nodeB->Velocity += nodeB->Acceleration * deltaTime;
					nodeA->Velocity *= 0.9f;
					nodeB->Position += (nodeB->Velocity/*/2.f*/) * deltaTime;
					nodeB->IsUpdated = true;
				}
			}
		}

		this->ResetNodeFlags();
	}


	void cSoftBody::ResetNodeFlags() {
		for (int i = 0; i < this->Nodes.size(); i++) {
			this->Nodes[i]->IsUpdated = false;
		}
	}

}


