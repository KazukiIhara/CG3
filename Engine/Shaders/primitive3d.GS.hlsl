
#include "primitive3d.hlsli"

[maxvertexcount(6)]
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
    
    output.RestartStrip();
    
    for (uint j = 0; j < 3; j++)
    {
        GeometryShaderOutput element;
        element.position = input[j].position + float32_t4(7.0f, 0.0f, 0.0f, 0.0f);
        element.normal = input[j].normal;
        element.texcoord = input[j].texcoord * 5.0f;
       
        output.Append(element);
    }
}