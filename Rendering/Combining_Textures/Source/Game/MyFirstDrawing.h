#pragma once

#include "Engine/Graphics/PostProcessEffect.h"
#include "Engine/Graphics/GPUPipelineState.h"
#include "Engine/Graphics/Textures/GPUSampler.h"

#include "Engine/Content/AssetReference.h"
#include "Engine/Content/Assets/Model.h"
#include "Engine/Content/Assets/Shader.h"
#include "Engine/Content/Assets/Texture.h"

#include "Engine/Core/Math/Matrix.h"

API_CLASS() class GAME_API MyFirstDrawing : public PostProcessEffect
{
API_AUTO_SERIALIZATION();
DECLARE_SCRIPTING_TYPE(MyFirstDrawing);

    // [Script]
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate() override;

    void OnDebugDraw() override;

    bool CanRender() const override;
    void Render(GPUContext* context, RenderContext& renderContext, GPUTexture* input, GPUTexture* output) override;

    API_FIELD() AssetReference<Model> MyModel;
    API_FIELD() AssetReference<Shader> MyShader;
    API_FIELD() Color Tint = Color::White;
    API_FIELD() AssetReference<Texture> MainTexture;
    API_FIELD() Float2 MainTextureTiling;
    API_FIELD() Float2 MainTextureOffset;
    API_FIELD() AssetReference<Texture> DetailTexture;
    API_FIELD() Float2 DetailTextureTiling;
    API_FIELD() Float2 DetailTextureOffset;

private:
    struct ShaderData
    {
        Matrix MVP;
        Float4 Tint;
        Float4 MainTexST;
        Float4 DetailTexST;
    };

    void OnAssetReloading(Asset* asset);

private:
    GPUPipelineState* _pso = nullptr;
    GPUSampler* _sampler = nullptr;
};
