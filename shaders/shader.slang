static float2 Positions[3] = float2[](
    float2(0.0, -0.5),
    float2(0.5, 0.5),
    float2(-0.5, 0.5)
);

static float3 Colors[3] = float3[](
    float3(1.0, 0.0, 0.0),
    float3(0.0, 1.0, 0.0),
    float3(0.0, 0.0, 1.0)
);

struct VertexOutput {
    float3 Color;
    float4 Position : SV_Position;
};

[shader("vertex")]
VertexOutput VertexMain(uint vid : SV_VertexID) {
    VertexOutput output;
    output.Position = float4(Positions[vid], 0.0, 1.0);
    output.Color = Colors[vid];
    return output;
}

[shader("fragment")]
float4 FragmentMain(VertexOutput inVert) : SV_Target
{
    float3 color = inVert.Color;
    return float4(color, 1.0);
}