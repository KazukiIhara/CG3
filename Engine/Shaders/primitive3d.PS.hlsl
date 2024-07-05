
#include "primitive3d.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);


PixelShaderOutput main(GeometryShaderOutput input)
{
    PixelShaderOutput output;
    
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
        
    // Textureのaの値が0.5以下の時にPixelを廃却
    if (textureColor.a <= 0.5)
    {
        discard;
    }    
    
    output.color =  textureColor;
       
    // output.colorのa値が0の時にPixelを廃却
    if (output.color.a == 0.0)
    {
        discard;
    }
    
    return output;
}

