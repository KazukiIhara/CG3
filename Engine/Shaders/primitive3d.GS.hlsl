
#include "primitive3d.hlsli"

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

static const uint vnum = 4;

static const float32_t4 offset_array[vnum] =
{
    float32_t4(-0.5f, -0.5f, 0.0f, 0.0f),
    float32_t4(-0.5f, +0.5f, 0.0f, 0.0f),
    float32_t4(+0.5f, -0.5f, 0.0f, 0.0f),
    float32_t4(+0.5f, +0.5f, 0.0f, 0.0f)
};

[maxvertexcount(vnum)]
void main(
	point VertexShaderOutput input[1],
	inout TriangleStream<GeometryShaderOutput> output
)
{
    GeometryShaderOutput element;
    element.position = input[0].position + float32_t4(-1.0f, -1.0f, 0.0f, 0.0f);
    element.normal = float32_t3(0.0f, 0.0f, 1.0f);
    element.texcoord = float32_t2(0.0f, 1.0f);
    element.position = mul(gTransformationMatrix.WVP, element.position);
    output.Append(element);
    element.position = input[0].position + float32_t4(-1.0f, +1.0f, 0.0f, 0.0f);
    element.normal = float32_t3(0.0f, 0.0f, 1.0f);
    element.texcoord = float32_t2(0.0f, 0.0f);
    element.position = mul(gTransformationMatrix.WVP, element.position);
    output.Append(element);
    element.position = input[0].position + float32_t4(+1.0f, -1.0f, 0.0f, 0.0f);
    element.normal = float32_t3(0.0f, 0.0f, 1.0f);
    element.texcoord = float32_t2(1.0f, 1.0f);
    element.position = mul(gTransformationMatrix.WVP, element.position);
    output.Append(element);
    element.position = input[0].position + float32_t4(+1.0f, +1.0f, 0.0f, 0.0f);
    element.normal = float32_t3(0.0f, 0.0f, 1.0f);
    element.texcoord = float32_t2(1.0f, 0.0f);
    element.position = mul(gTransformationMatrix.WVP, element.position);
    output.Append(element);
}