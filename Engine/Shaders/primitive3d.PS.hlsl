
#include "primitive3d.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gMaterial.color;
    
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
        
    // Textureのaの値が0.5以下の時にPixelを廃却
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    
   
    
    output.color = gMaterial.color * textureColor;
    
    
    // output.colorのa値が0の時にPixelを廃却
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    return output;
}

