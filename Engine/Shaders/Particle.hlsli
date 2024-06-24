
struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

struct Material
{
    float32_t4 color;
    int32_t enbleLighting;
    float32_t4x4 uvTransform;
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};
