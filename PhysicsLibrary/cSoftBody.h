#ifndef _cSoftBody_HG_
#define _cSoftBody_HG_
#include <iSoftBody.h>
#include <sSoftBodyDesc.h>
#include <vector>

namespace nPhysics {

	class cSoftBody : public iSoftBody {
	public:
		cSoftBody(const sSoftBodyDesc& desc);
		virtual ~cSoftBody();
		//updates the node velocities to make sure it comes back together or moves apart as needed
		void UpdateNodes(const float& deltaTime);
		void ResetNodeFlags();
		class cNode;

		//spring class(connector for nodes)
		class cSpring {
		public:
			cSpring(cNode* nodeA, cNode* nodeB,float restingSep);

			cNode* GetOther(cNode* me);
			float RestingSeparation;
			float CurrentSeparation;
			float SpringConstant = 10.0f;
			cNode* NodeA;
			cNode* NodeB;
		};

		//node class (each vert in the mesh)
		class cNode {
		public:
			cNode(const glm::vec3& pos);
			std::vector<cSpring*> Springs;

			bool DoesSpringExist(cNode* springB);
			bool IsStatic;
			size_t Id;
			void ApplyForce(const glm::vec3& force);

			glm::vec3 Position;
			glm::vec3 PreviousPosition;
			glm::vec3 Velocity;
			glm::vec3 Acceleration;
			float Mass = 1.f;
			bool IsUpdated = false;
		};

		std::vector<cNode*> Nodes;

	protected:
		cSoftBody();
		virtual void GetNodePosition(size_t index, glm::vec3& nodePositionOnMesh);
		virtual size_t NumNodes();
	private:
		void SetStaticNodes(const sSoftBodyDesc& desc);
		void ConstructNodes(const sSoftBodyDesc& desc);
	};
}
#endif // !_cSoftBody_HG_

