
#include "primitive3d.hlsli"

[maxvertexcount(3)]
void main(
	point VertexShaderOutput input[1],
	inout TriangleStream<GeometryShaderOutput> output
)
{
    GeometryShaderOutput element;
    element.position = input[0].position;
    element.normal = input[0].normal;
    element.texcoord = input[0].texcoord;
    output.Append(element);
    element.position = input[0].position + float4(10.0f, 10.0f, 0.0f, 0.0f);
    element.normal = input[0].normal;
    element.texcoord = input[0].texcoord;
    output.Append(element);
    element.position = input[0].position + float4(10.0f, 0.0f, 0.0f, 0.0f);
    element.normal = input[0].normal;
    element.texcoord = input[0].texcoord;
    output.Append(element);
}