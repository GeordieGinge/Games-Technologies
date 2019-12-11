#pragma once
#include "../../Common/Vector3.h"
#include "NavigationPath.h"
namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		class NavigationMap
		{
		public:
			NavigationMap() {}
			~NavigationMap() {}

			virtual bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) = 0;

			struct GridNode
			{
				GridNode* parent;

				GridNode* connected[4];
				int costs[4];

				Vector3 position;

				float f;
				float g;

				int type;

				GridNode()
				{
					for (int i = 0; i < 4; ++i)
					{
						connected[i] = nullptr;
						costs[i] = 0;
					}
					f = 0;
					g = 0;
					type = 0;
					parent = nullptr;
				}
			};
		};
	}
}

