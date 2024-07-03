#include "object3d.hlsli"

[maxvertexcount(12)]
void main(
	triangle VertexShaderOutput input[3],
	inout TriangleStream<GSOutput> output
)
{
    for (uint i = 0; i < 3; i++)
    {
        GSOutput element;
        element.position = input[i].position;
        element.normal = input[i].normal;
        element.texcoord = input[i].texcoord * 2.0f;
        element.worldPosition = input[i].worldPosition;
        output.Append(element);
    }
    output.RestartStrip();
    
    for (uint j = 0; j < 3; j++)
    {
        GSOutput element;
        element.position = input[j].position + float32_t4(5.0f,0.0f,0.0f,0.0f);
        element.normal = input[j].normal;
        element.texcoord = input[j].texcoord * 5.0f;
        element.worldPosition = input[j].worldPosition;
        output.Append(element);
    }
    
}