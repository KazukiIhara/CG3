
struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4x4 WorldInverseTranspose;
};

struct Material
{
    float32_t4 color;
    int32_t enbleLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t3 worldPosition : POSITION0;
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct GeometryShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t3 normal : NORMAL;
    float32_t2 texcoord : TEXCOORD;
};