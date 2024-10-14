#pragma once
#include "Artifact.h"

namespace Characters
{
	class Haniwa : public Artifact
	{
	public:
		Haniwa(const std::string modelPath, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale);
		//using Artifact::Artifact;

		virtual const std::string GetName() override { return "Haniwa"; }
	};
}
