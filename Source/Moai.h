#pragma once
#include "Artifact.h"

namespace Characters
{
	class Moai : public Artifact
	{
	public:
		Moai() = delete;
		Moai(const Moai&) = delete;
		Moai& operator=(const Moai&) = delete;
		using Artifact::Artifact;

		virtual void Attack() override {};
	};
}
