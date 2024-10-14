#include "Moai.h"

namespace Characters
{
	Moai::Moai(const std::string modelPath, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale)
		:
		Artifact(modelPath, position, scale)
	{
		gamePad.SetSlot(0);
	}
}
