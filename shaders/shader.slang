struct VertexInput {
    float2 Position;
    float3 Color;
    float2 TexCoords;
};

struct UniformObject {
    float4x4 Model;
    float4x4 View;
    float4x4 Projection;
};

ConstantBuffer<UniformObject> ubo;

struct VertexOutput {
    float3 Color;
    float4 Position : SV_Position;
    float2 TexCoords;
};

[shader("vertex")]
VertexOutput VertexMain(VertexInput input) {
    VertexOutput output;
    float4 modelled = mul(ubo.Model, float4(input.Position, 0.0, 1.0));
    float4 viewed = mul(ubo.View, modelled);
    float4 projected = mul(ubo.Projection, viewed);
    output.Position = projected;
    output.Color = input.Color;
    output.TexCoords = input.TexCoords;
    return output;
}

ConstantBuffer<Sampler2D> sampler;

[shader("fragment")]
float4 FragmentMain(VertexOutput input) : SV_Target
{
    float4 texColor = float4(input.Color * sampler.Sample(input.TexCoords).rgb, 1);
	return texColor;
}