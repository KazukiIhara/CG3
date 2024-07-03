#include "object3d.hlsli"

[maxvertexcount(3)]
void main(
	point VertexShaderOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    GSOutput element;
    element.normal = input[0].normal;
    element.texcoord = input[0].texcoord;
   
    
    element.position = input[0].position;
    element.worldPosition = input[0].worldPosition;
    output.Append(element);
    element.position = input[0].position + float32_t4(10.0f, 10.0f, 0.0f, 0.0f);
    element.worldPosition = input[0].worldPosition;
    output.Append(element);
    element.position = input[0].position + float32_t4(10.0f, 0.0f, 0.0f, 0.0f);
    element.worldPosition = input[0].worldPosition;
    output.Append(element);
}