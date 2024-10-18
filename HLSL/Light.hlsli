//// 平行光源情報
//struct DirectionalLightData
//{
//    float4 direction; // 向き
//    float4 color; // 色
//};
//// 点光源情報
//struct PointLightData
//{
//    float4 position; // 座標
//    float4 color; // 色
//    float range; // 範囲
//    float3 dummy;
//};

//// 点光源の最大数
//static const int PointLightMax = 8;

////--------------------------------------------
////	ランバート拡散反射計算関数
////--------------------------------------------
//// normal		: 法線(正規化済み)
//// lightVector	: 入射ベクトル(正規化済み)
//// lightColor	: 入射光色
//// kd			: 反射率(反射の強さ)
//// 返す値		: 拡散反射色
//float3 CalcLambertDiffuse(float3 normal, float3 lightVector, float3 lightColor, float3 kd)
//{
//	// 法線とライト方向へのベクトルでの内積 
//	// 内積した結果、ライトから見て裏向きの面だとマイナスの結果が出てしまうため
//	// それをそのまま乗算すると色がおかしくなります。
//	// 0以下であれば0になるように制限をつけましょう。
//    float d = dot(-lightVector, normal);
//    d = max(d, 0);  
//    //float d = max(dot(-lightVector, normal), 0);

//	// 入射光色と内積の結果、及び反射率をすべて乗算して返却しましょう。
//    return d * kd * lightColor;

//}

////--------------------------------------------
////	フォンの鏡面反射計算関数
////--------------------------------------------
//// normal		: 法線(正規化済み)
//// lightVector	: 入射ベクトル(正規化済み)
//// lightColor	: 入射光色
//// eyeVector		: 視線ベクトル(正規化済み)
//// shininess		: 光沢度(質感調整値)
//// ks			: 反射率(反射の強さ)
//// 返す値		: 鏡面反射色
//float3 CalcPhongSpecular(float3 normal, float3 lightVector, float3 lightColor, float3 eyeVector, float shininess, float3 ks)
//{
//	// 入射ベクトルを法線方向に対して反射させた反射ベクトルを算出しましょう
//    float3 R = reflect(lightVector, normal);

//	// 反射ベクトルと視線ベクトルとで内積
//	// 内積した結果を0以下であれば0になるように制限をつけましょう。
//    float d = dot(-eyeVector, R);
//    d = max(d, 0);
//    //float d = max(dot(-eyeVector, R), 0);
	
//	// 光沢処理
//	// 反射は表面がツルツルなほどハイライトは強くなり、範囲が小さくなります。
//	// 逆に表面がザラザラなほど、ハイライトは弱くなり、範囲は広がります。
//	// この変化は指数的に変化するため光沢度を乗数として使用します。
//    d = pow(d, shininess);

//	// 入射光色と内積の結果、及び反射率をすべて乗算して返却しましょう。
//    return lightColor * d * ks;

//}

// 平行光源情報
struct DirectionalLightData
{
    float4 direction; // 向き
    float4 color; // 色
};
// 点光源情報
struct PointLightData
{
    float4 position; // 座標
    float4 color; // 色
    float range; // 範囲
    float influence;//光の影響力
    float2 dummy;
};

// 点光源の最大数
static const int PointLightMax = 8;

// スポットライト情報
struct SpotLightData
{
    float4 position; // 座標
    float4 direction; // 向き
    float4 color; // 色
    float range; // 範囲
    float innerCorn;
    float outerCorn;
    float influence;
    //float dummy;
};
// スポットライトの最大数
static const int SpotLightMax = 8;

//--------------------------------------------
//	ランバート拡散反射計算関数
//--------------------------------------------
// normal		: 法線(正規化済み)
// lightVector	: 入射ベクトル(正規化済み)
// lightColor	: 入射光色
// kd			: 反射率(反射の強さ)
// 返す値		: 拡散反射色
float3 CalcLambertDiffuse(float3 normal, float3 lightVector, float3 lightColor, float3 kd)
{
	// 法線とライト方向へのベクトルでの内積 
	// 内積した結果、ライトから見て裏向きの面だとマイナスの結果が出てしまうため
    // それをそのまま乗算すると色がおかしくなります。
	// 0以下であれば0になるように制限をつけましょう。
    float d = dot(-lightVector, normal);
    d = max(d, 0);
	// 入射光色と内積の結果、及び反射率をすべて乗算して返却しましょう。
    return d * kd * lightColor;
    
}
//--------------------------------------------
//	フォンの鏡面反射計算関数
//--------------------------------------------
// normal		: 法線(正規化済み)
// lightVector	: 入射ベクトル(正規化済み)
// lightColor	: 入射光色
// eyeVector		: 視線ベクトル(正規化済み)
// shininess		: 光沢度(質感調整値)
// ks			: 反射率(反射の強さ)
// 返す値		: 鏡面反射色
float3 CalcPhongSpecular(float3 normal, float3 lightVector, float3 lightColor, float3 eyeVector, float shininess, float3 ks)
{
	// 入射ベクトルを法線方向に対して反射させた反射ベクトルを算出しましょう
    float3 R = reflect(lightVector, normal);

	// 反射ベクトルと視線ベクトルとで内積
	// 内積した結果を0以下であれば0になるように制限をつけましょう。
    float d = dot(-eyeVector, R);
    d = max(d, 0);
    //float d = dot((-eyeVector, R),0);
	
    // 光沢処理
	// 反射は表面がツルツルなほどハイライトは強くなり、範囲が小さくなります。
	// 逆に表面がザラザラなほど、ハイライトは弱くなり、範囲は広がります。
	// この変化は指数的に変化するため光沢度を乗数として使用します。
    d = pow(d, shininess);

	// 入射光色と内積の結果、及び反射率をすべて乗算して返却しましょう。
    return lightColor * d * ks;

}
//--------------------------------------------
//	ハーフランバート拡散反射計算関数
//--------------------------------------------
// normal		: 法線(正規化済み)
// lightVector	: 入射ベクトル(正規化済み)
// lightColor	: 入射光色
// kd			: 反射率(反射の強さ)
float3 ClacHalfLambert(float3 normal, float3 lightVector, float3 lightColor, float3 kd)
{
    float d = saturate(dot(normal, -lightVector));
    return lightColor * d * kd;
}


