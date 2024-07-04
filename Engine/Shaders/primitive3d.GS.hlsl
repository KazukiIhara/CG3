
#include "primitive3d.hlsli"

[maxvertexcount(3)]
void main(
	triangle VertexShaderOutput input[3],
	inout TriangleStream<GeometryShaderOutput> output
)
{
    for (uint i = 0; i < 3; i++)
    {
        GeometryShaderOutput element;
        element.position = input[i].position;
        element.normal = input[i].normal;
        element.texcoord = input[i].texcoord;
        output.Append(element);
    }
}