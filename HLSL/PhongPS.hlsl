#include "Phong.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;
    float3 N = normalize(pin.normal);
    float3 L = normalize(directionalLightData.direction.xyz);
    float3 E = -normalize(viewPosition.xyz - pin.world_position.xyz);


	// �}�e���A���萔
    float3 ka = float3(1, 1, 1);
    float3 kd = float3(1, 1, 1);
    float3 ks = float3(1, 1, 1);
    float shiness = 32;


	// ���s�����̃��C�e�B���O�v�Z
    float3 directionalDiffuse = CalcLambertDiffuse(N, L, directionalLightData.color.rgb, kd);
    float3 directionalSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, E, shiness, ks);
    // �����̌v�Z
    float3 ambient = ka * ambientLightColor.rgb;
    // �_�����̏���
    float3 pointDiffuse = (float3) 0;
    float3 pointSpecular = (float3) 0;
    int i;
    int j;
    for (i = 0; i < pointLightCount; ++i)
    {
		// ���C�g�x�N�g�����Z�o
        float3 lightVector = pin.world_position.xyz - pointLightData[i].position.xyz;

		// ���C�g�x�N�g���̒������Z�o
        float lightLength = length(lightVector);

		// ���C�g�̉e���͈͊O�Ȃ��̌v�Z�����Ȃ��B
        if (lightLength >= pointLightData[i].range)
            continue;

		// �����������Z�o����
        float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);
        attenuate *= pointLightData[i].influence;
        lightVector = lightVector / lightLength;
        pointDiffuse += CalcLambertDiffuse(N, lightVector,
									pointLightData[i].color.rgb, kd.rgb) * attenuate;
        pointSpecular += CalcPhongSpecular(N, lightVector,
									pointLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
    }

  //  	// �X�|�b�g���C�g�̏���
    float3 spotDiffuse = (float3) 0;
    float3 spotSpecular = (float3) 0;
    for (j = 0; j < spotLightCount; ++j)
    {
		// ���C�g�x�N�g�����Z�o
        float3 lightVector = pin.world_position.xyz - spotLightData[j].position.xyz;

		// ���C�g�x�N�g���̒������Z�o
        float lightLength = length(lightVector);

        if (lightLength >= spotLightData[j].range)
            continue;

		// �����������Z�o����
        float attenuate = saturate(1.0f - lightLength / spotLightData[j].range);
        attenuate*=spotLightData[j].influence;
        lightVector = normalize(lightVector);

		// �p�x�������Z�o����attenuate�ɏ�Z����
        float3 spotDirection = normalize(spotLightData[j].direction.xyz);
        float angle = dot(spotDirection, lightVector);
        float area = spotLightData[j].innerCorn - spotLightData[j].outerCorn;
        attenuate *= saturate(1.0f - (spotLightData[j].innerCorn - angle) / area);

        spotDiffuse += CalcLambertDiffuse(N, lightVector,
									spotLightData[j].color.rgb, kd.rgb) * attenuate;
        spotSpecular += CalcPhongSpecular(N, lightVector,
									spotLightData[j].color.rgb, E, shiness, ks.rgb) * attenuate;
    }


    //float4 color = diffuseColor;
    float4 color = float4(ambient, diffuseColor.a);
    color.rgb += diffuseColor.rgb * (directionalDiffuse + pointDiffuse + spotDiffuse);
    color.rgb += directionalSpecular + pointSpecular + spotSpecular;


    
    return color;
    
    
    
    
}
