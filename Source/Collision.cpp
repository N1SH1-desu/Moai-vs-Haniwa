#include <DirectXCollision.h>
#include "Collision.h"

// レイキャスト
bool Collision::RayCast(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const DirectX::XMFLOAT4X4& worldTransform,
	const Model* model,
	DirectX::XMFLOAT3& hitPosition,
	DirectX::XMFLOAT3& hitNormal)
{
	bool hit = false;

	// 始点と終点からレイのベクトルと長さを求める
	DirectX::XMVECTOR WorldRayStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldRayEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldRayEnd, WorldRayStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);

	float nearestDist = DirectX::XMVectorGetX(WorldRayLength);
	if (nearestDist <= 0.0f) return false;

	DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&worldTransform);

	// モデル内の全てのメッシュと交差判定を行う
	const ModelResource* resource = model->GetResource();
	for (const ModelResource::Mesh& mesh : resource->GetMeshes())
	{
		// メッシュのワールド行列を求める
		const Model::Node& node = model->GetNodes().at(mesh.nodeIndex);
		DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&node.globalTransform);
		DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixMultiply(GlobalTransform, ParentWorldTransform);

		// レイをメッシュのローカル空間に変換する
		DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
		DirectX::XMVECTOR LocalRayStart = DirectX::XMVector3Transform(WorldRayStart, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayEnd = DirectX::XMVector3Transform(WorldRayEnd, InverseWorldTransform);
		DirectX::XMVECTOR LocalRayVec = DirectX::XMVectorSubtract(LocalRayEnd, LocalRayStart);
		DirectX::XMVECTOR LocalRayDirection = DirectX::XMVector3Normalize(LocalRayVec);
		float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(LocalRayVec));

		// メッシュ内の全ての三角形と交差判定を行う
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			// 三角形の頂点座標を取得
			const ModelResource::Vertex& a = mesh.vertices.at(mesh.indices.at(i + 0));
			const ModelResource::Vertex& b = mesh.vertices.at(mesh.indices.at(i + 1));
			const ModelResource::Vertex& c = mesh.vertices.at(mesh.indices.at(i + 2));

			DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
			DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);

			// 三角形の交差判定
			if (DirectX::TriangleTests::Intersects(LocalRayStart, LocalRayDirection, A, B, C, distance))
			{
				// メッシュのローカル空間での交点を求める
				DirectX::XMVECTOR LocalHitVec = DirectX::XMVectorScale(LocalRayDirection, distance);
				DirectX::XMVECTOR LocalHitPosition = DirectX::XMVectorAdd(LocalRayStart, LocalHitVec);

				// メッシュのローカル空間での交点をワールド空間に変換する
				DirectX::XMVECTOR WorldHitPosition = DirectX::XMVector3Transform(LocalHitPosition, WorldTransform);

				// ワールド空間でのレイの始点から交点までの距離を求める
				DirectX::XMVECTOR WorldHitVec = DirectX::XMVectorSubtract(WorldHitPosition, WorldRayStart);
				DirectX::XMVECTOR WorldHitDist = DirectX::XMVector3Length(WorldHitVec);
				float worldHitDist = DirectX::XMVectorGetX(WorldHitDist);

				// 交点までの距離が今までに計算した交点より近い場合は交差したと判定する
				if (worldHitDist <= nearestDist)
				{
					// メッシュのローカル空間での三角形の法線ベクトルを算出
					DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
					DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
					DirectX::XMVECTOR LocalHitNormal = DirectX::XMVector3Cross(AB, BC);

					// メッシュのローカル空間での法線ベクトルをワールド空間に変換する
					DirectX::XMVECTOR WorldHitNormal = DirectX::XMVector3TransformNormal(LocalHitNormal, WorldTransform);

					// 三角形の裏表判定（内積の結果がマイナスならば表向き）
					DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(LocalRayDirection, LocalHitNormal);
					float dot = DirectX::XMVectorGetX(Dot);
					if (dot < 0)
					{
						// 交差したのでヒット情報を格納
						DirectX::XMStoreFloat3(&hitNormal, DirectX::XMVector3Normalize(WorldHitNormal));
						DirectX::XMStoreFloat3(&hitPosition, WorldHitPosition);

						// 交点までの距離を更新する
						nearestDist = worldHitDist;
						hit = true;
					}
				}
			}
		}
	}
	return hit;
}

// 球と球の交差判定
bool Collision::IntersectSphereVsSphere(const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outPositionB)
{
	// A->Bの単位ベクトルを算出
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector4LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	// 距離判定
	float range = radiusA + radiusB;
	if (range * range < lengthSq)
	{
		return false;
	}

	// AがBを押し出す
	DirectX::XMVECTOR range_b = DirectX::XMVectorScale(DirectX::XMVector3Normalize(Vec), range);
	DirectX::XMVECTOR out = DirectX::XMVectorAdd(DirectX::XMVectorSubtract(range_b, Vec), PositionB);
	DirectX::XMStoreFloat3(&outPositionB, out);

	return true;
}

// 円柱と円柱と交差判定
bool Collision::IntersectCylinderVsCylinder(
	const DirectX::XMFLOAT3& positionA, float radiusA, float heightA,
	const DirectX::XMFLOAT3& positionB, float radiusB, float heightB,
	DirectX::XMFLOAT3& outPositionB)
{
	// Aの足元がBの頭より上なら当たってない
	if (positionA.y > (positionB.y + heightB))
	{
		return false;
	}
	// Aの頭がBの足元より下なら当たってない
	if ((positionA.y + heightA) < positionB.y)
	{
		return false;
	}
	// XZ平面での範囲チェック
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract({ positionB.x, 0.0f, positionB.z }, { positionA.x, 0.0f, positionA.z });
	DirectX::XMVECTOR norV = DirectX::XMVector3Normalize(Vec);
	float lengthSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(Vec));
	if (pow((radiusA + radiusB), 2) < lengthSq)
	{
		return false;
	}
	// AがBを押し出す
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