#include <DirectXCollision.h>
#include "Collision.h"

// ���C�L���X�g
bool Collision::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const DirectX::XMFLOAT4X4& worldTransform,
	const Model* model,
	DirectX::XMFLOAT3& hitPosition,
	DirectX::XMFLOAT3& hitNormal)
{
	bool hit = false;

	// �n�_�ƏI�_���烌�C�̃x�N�g���ƒ��������߂�
	DirectX::XMVECTOR WorldRayStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldRayEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldRayEnd, WorldRayStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	float nearestDist = DirectX::XMVectorGetX(WorldRayLength);
	if (nearestDist <= 0.0f) return false;

	DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);

	// ���f�����̑S�Ẵ��b�V���ƌ���������s��
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// ���b�V���̃��[���h�s������߂�
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
		DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
		DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(GlobalTransform, ParentWorldTransform);

		// ���C�����b�V���̃��[�J����Ԃɕϊ�����
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
		DirectX::XMVECTOR LocalRayStart = DirectX::XMVector3Transform(WorldRayStart, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayEnd = DirectX::XMVector3Transform(WorldRayEnd, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayVec = DirectX::XMVectorSubtract(LocalRayEnd, LocalRayStart);
		DirectX::XMVECTOR LocalRayDirection = DirectX::XMVector3Normalize(LocalRayVec);
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(LocalRayVec));

		// ���b�V�����̑S�Ă̎O�p�`�ƌ���������s��
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			// �O�p�`�̒��_���W���擾
			const ModelResource::Vertex& a = mesh.vertices.at(mesh.indices.at(i + 0));
			const ModelResource::Vertex& b = mesh.vertices.at(mesh.indices.at(i + 1));
			const ModelResource::Vertex& c = mesh.vertices.at(mesh.indices.at(i + 2));

			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

			// �O�p�`�̌�������
			if (DirectX::TriangleTests::Intersects(LocalRayStart, LocalRayDirection, A, B, C, distance))
			{
				// ���b�V���̃��[�J����Ԃł̌�_�����߂�
				DirectX::XMVECTOR LocalHitVec = DirectX::XMVectorScale(LocalRayDirection, distance);
				DirectX::XMVECTOR LocalHitPosition = DirectX::XMVectorAdd(LocalRayStart, LocalHitVec);

				// ���b�V���̃��[�J����Ԃł̌�_�����[���h��Ԃɕϊ�����
				DirectX::XMVECTOR WorldHitPosition = DirectX::XMVector3Transform(LocalHitPosition, WorldTransform);

				// ���[���h��Ԃł̃��C�̎n�_�����_�܂ł̋��������߂�
				DirectX::XMVECTOR WorldHitVec = DirectX::XMVectorSubtract(WorldHitPosition, WorldRayStart);
				DirectX::XMVECTOR WorldHitDist = DirectX::XMVector3Length(WorldHitVec);
				float worldHitDist = DirectX::XMVectorGetX(WorldHitDist);

				// ��_�܂ł̋��������܂łɌv�Z������_���߂��ꍇ�͌��������Ɣ��肷��
				if (worldHitDist <= nearestDist)
				{
					// ���b�V���̃��[�J����Ԃł̎O�p�`�̖@���x�N�g�����Z�o
					DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR LocalHitNormal = DirectX::XMVector3Cross(AB, BC);

					// ���b�V���̃��[�J����Ԃł̖@���x�N�g�������[���h��Ԃɕϊ�����
					DirectX::XMVECTOR WorldHitNormal = DirectX::XMVector3TransformNormal(LocalHitNormal, WorldTransform);

					// �O�p�`�̗��\����i���ς̌��ʂ��}�C�i�X�Ȃ�Ε\�����j
					DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(LocalRayDirection, LocalHitNormal);
					float dot = DirectX::XMVectorGetX(Dot);
					if (dot < 0)
					{
						// ���������̂Ńq�b�g�����i�[
						DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(WorldHitNormal));
						DirectX::XMStoreFloat3(&hitPosition, WorldHitPosition);

						// ��_�܂ł̋������X�V����
						nearestDist = worldHitDist;
						hit = true;
					}
				}
			}
		}
	}
	return hit;
}

// ���Ƌ��̌�������
bool Collision::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB)
{
	// A->B�̒P�ʃx�N�g�����Z�o
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector4LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	// ��������
	float range = radiusA + radiusB;
	if (range * range < lengthSq)
	{
		return false;
	}

	// A��B�������o��
	DirectX::XMVECTOR range_b = DirectX::XMVectorScale(DirectX::XMVector3Normalize(Vec), range);
	DirectX::XMVECTOR out = DirectX::XMVectorAdd(DirectX::XMVectorSubtract(range_b, Vec), PositionB);
	DirectX::XMStoreFloat3(&outPositionB, out);

	return true;
}

// �~���Ɖ~���ƌ�������
bool Collision::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA, float radiusA, float heightA,
	const DirectX::XMFLOAT3& positionB, float radiusB, float heightB,
	DirectX::XMFLOAT3& outPositionB)
{
	// A�̑�����B�̓�����Ȃ瓖�����ĂȂ�
	if (positionA.y > (positionB.y + heightB))
	{
		return false;
	}
	// A�̓���B�̑�����艺�Ȃ瓖�����ĂȂ�
	if ((positionA.y + heightA) < positionB.y)
	{
		return false;
	}
	// XZ���ʂł͈̔̓`�F�b�N
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract({ positionB.x, 0.0f, positionB.z }, { positionA.x, 0.0f, positionA.z });
	DirectX::XMVECTOR norV = DirectX::XMVector3Normalize(Vec);
	float lengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));
	if (pow((radiusA + radiusB), 2) < lengthSq)
	{
		return false;
	}
	// A��B�������o��
	DirectX::XMFLOAT3 out;
	DirectX::XMStoreFloat3(&out, DirectX::XMVectorSubtract(DirectX::XMVectorScale(norV, (radiusA + radiusB)), Vec));
	outPositionB.x = out.x + positionB.x;
	outPositionB.y = out.y + positionB.y;
	outPositionB.z = out.z + positionB.z;

	return true;
}

bool Collision::IntersectSphereVsCylider(const DirectX::XMFLOAT3& spherePosition, float sphereRadius, const DirectX::XMFLOAT3& cylinderPosition, float cylinderRadius, float cylinderHeight, DirectX::XMFLOAT3& outCylinderPosition)
{
	if (spherePosition.y > cylinderPosition.y + cylinderHeight)
	{
		return false;
	}
	if (spherePosition.y < cylinderPosition.y)
	{
		return false;
	}

	DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&spherePosition);
	DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&cylinderPosition);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(B, A);
	DirectX::XMVECTOR NorVec = DirectX::XMVector3Normalize(Vec);

	float VecLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(Vec));
	float range = sphereRadius + cylinderRadius;
	if (VecLength > range)
	{
		return false;
	}

	DirectX::XMVECTOR rangeVec = DirectX::XMVectorScale(NorVec, range);
	DirectX::XMFLOAT3 outPutPos;
	DirectX::XMStoreFloat3(&outPutPos, DirectX::XMVectorSubtract(rangeVec, Vec));
	outCylinderPosition.x += outPutPos.x;
	outCylinderPosition.y += outPutPos.y;
	outCylinderPosition.z += outPutPos.z;

	return true;
}
