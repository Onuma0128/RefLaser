

struct PS_Output
{
    float4 color : SV_TARGET0;
};

PS_Output main()
{
    PS_Output output;
    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output; // 頂点から受け取った色をそのまま出力
}