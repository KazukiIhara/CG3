
#include "object3d.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gMaterial.color;
    
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);

    float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    
    // DirectionalLight
    float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
    float RdotE = dot(reflectLight, toEye);
    float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
    float NDotH = dot(normalize(input.normal), halfVector);
    float specularPow = pow(saturate(NDotH), gMaterial.shininess);
    

    // PointLight
    float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
    float32_t distance = length(gPointLight.position - input.worldPosition);
    float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay);
    float32_t3 reflectPointLight = reflect(pointLightDirection, normalize(input.normal));
    float RdotEPoint = dot(reflectPointLight, toEye);
    float32_t3 halfVectorPointLight = normalize(-pointLightDirection + toEye);
    float NDotHPoint = dot(normalize(input.normal), halfVectorPointLight);
    float specularPowPointLight = pow(saturate(NDotHPoint), gMaterial.shininess);
    
    // Textureのaの値が0.5以下の時にPixelを廃却
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    
    if (gMaterial.enbleLighting != 0)
    {
        float nDotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(nDotL * 0.5f + 0.5f, 2.0f);
             
        float nDotSpotL = dot(normalize(input.normal), -pointLightDirection);
        float cosSpotLight = pow(nDotSpotL * 0.5f + 0.5f, 2.0f);
        
        // 拡散反射
        // DirectionalLight
        float32_t3 diffuseDirectionalLight =
        gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        // PointLight
        float32_t3 diffusePointLight =
        gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cosSpotLight * gPointLight.intensity * factor;
        // 鏡面反射
        // DirectionalLight
        float32_t3 specularDirectionalLight = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        // PointLight
        float32_t3 specularPointLight = gPointLight.color.rgb * gPointLight.intensity * factor * specularPowPointLight * float32_t3(1.0f, 1.0f, 1.0f);
        // 拡散反射、鏡面反射
        output.color.rgb = diffuseDirectionalLight + specularDirectionalLight + diffusePointLight + specularPointLight;
        // アルファ値は今まで通り
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    
    // output.colorのa値が0の時にPixelを廃却
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    return output;
}

