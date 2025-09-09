#pragma once

#include "Engine/Scripting/Script.h"

#include "Engine/Graphics/PostProcessEffect.h"
#include "Engine/Graphics/RenderTask.h"

#include "Engine/Graphics/Textures/GPUSamplerDescription.h"

#include "Engine/Content/AssetReference.h"
#include "Engine/Content/Assets/Model.h"
#include "Engine/Content/Assets/Shader.h"
#include "Engine/Content/Assets/Texture.h"

class GPUPipelineState;
class GPUSampler;

API_CLASS() class GAME_API MyFirstShader : public PostProcessEffect
{
API_AUTO_SERIALIZATION();
DECLARE_SCRIPTING_TYPE(MyFirstShader);

    // [Script]
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate() override;

    bool CanRender() const override;
    void Render(GPUContext* context, RenderContext& renderContext, GPUTexture* input, GPUTexture* output) override;

    API_FIELD() AssetReference<Model> MyModel;
    API_FIELD() AssetReference<Shader> MyShader;
    API_FIELD() AssetReference<Texture> MyTexture;

    API_FIELD() Color Tint = { 1.0f, 1.0f, 1.0f, 1.0f };

    API_FIELD() Vector2 Tiling = { 1.0f, 1.0f };
    API_FIELD() Vector2 Offset = { 0.0f, 0.0f };

    API_FIELD() GPUSamplerAddressMode WrapMode = GPUSamplerAddressMode::Wrap;
    API_FIELD() GPUSamplerFilter FilterMode = GPUSamplerFilter::Bilinear;

private:
    void OnAssetReloading(Asset* asset);

    void ReleaseShader();

    struct ShaderData
    {
        Matrix MVP;
        Color Tint;
        Vector4 MainTexST;
    };

private:
    GPUPipelineState* _pso = nullptr;
    GPUSampler* _myTexSampler = nullptr;
};
