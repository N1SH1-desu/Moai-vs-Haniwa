#pragma once
#include "Artifact.h"

namespace Characters
{
	class Moai : public Artifact
	{
	public:
		Moai(const std::string modelPath, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale);
		Moai(const Moai&) = delete;
		Moai& operator=(const Moai&) = delete;
		//using Artifact::Artifact;

		virtual const std::string GetName() override { return "Moai"; }
	};
}
