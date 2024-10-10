#pragma once
#include "Artifact.h"

namespace Characters
{
	class Haniwa : public Artifact
	{
	public:
		using Artifact::Artifact;

		virtual void Attack() override {};
	};
}
