#pragma once

#include "Engine/Graphics/PostProcessEffect.h"
#include "Engine/Graphics/GPUPipelineState.h"
#include "Engine/Graphics/Textures/GPUSampler.h"

#include "Engine/Content/AssetReference.h"
#include "Engine/Content/Assets/Model.h"
#include "Engine/Content/Assets/Shader.h"
#include "Engine/Content/Assets/Texture.h"

#include "Engine/Core/Math/Matrix.h"

API_CLASS() class GAME_API MySplatMapDrawing : public PostProcessEffect
{
API_AUTO_SERIALIZATION();
DECLARE_SCRIPTING_TYPE(MySplatMapDrawing);

    // [Script]
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate() override;

    void OnDebugDraw() override;

    bool CanRender() const override;
    void Render(GPUContext* context, RenderContext& renderContext, GPUTexture* input, GPUTexture* output) override;

    API_FIELD() AssetReference<Model> MyModel;
    API_FIELD() AssetReference<Shader> MyShader;
    API_FIELD() AssetReference<Texture> SplatMap;
    API_FIELD() Float2 SplatMapTiling;
    API_FIELD() Float2 SplatMapOffset;

private:
    struct ShaderData
    {
        Matrix MVP;
        Float4 MainTexST;
    };

    void OnAssetReloading(Asset* asset);

private:
    GPUPipelineState* _pso = nullptr;
    GPUSampler* _sampler = nullptr;

};
