#include "MySplatMapDrawing.h"

#include "Engine/Content/Content.h"

#include "Engine/Level/Actor.h"

#include "Engine/Graphics/RenderTask.h"
#include "Engine/Graphics/RenderBuffers.h"
#include "Engine/Graphics/GPUContext.h"
#include "Engine/Graphics/GPUPipelineState.h"
#include "Engine/Graphics/GPUBuffer.h"
#include "Engine/Graphics/Textures/GPUTexture.h"
#include "Engine/Graphics/Shaders/GPUShader.h"

#include "Engine/Debug/DebugDraw.h"

MySplatMapDrawing::MySplatMapDrawing(const SpawnParams& params)
    : PostProcessEffect(params)
{
    // Enable ticking OnUpdate function
    _tickUpdate = true;

    MyModel = nullptr;
    MyShader = nullptr;
    SplatMap = nullptr;
    SplatMapTiling = { 1,1 };
    SplatMapOffset = { 0,0 };

    _pso = nullptr;
    _sampler = nullptr;

    UseSingleTarget = true;
    Location = PostProcessEffectLocation::BeforeForwardPass;
}

void MySplatMapDrawing::OnEnable()
{
    // Here you can add code that needs to be called when script is enabled (eg. register for events)

    SceneRenderTask::AddGlobalCustomPostFx(this);
#if USE_EDITOR
    Content::AssetReloading.Bind<MySplatMapDrawing, &MySplatMapDrawing::OnAssetReloading>(this);
#endif
}

void MySplatMapDrawing::OnDisable()
{
    // Here you can add code that needs to be called when script is disabled (eg. unregister from events)

    SceneRenderTask::RemoveGlobalCustomPostFx(this);
#if USE_EDITOR
    Content::AssetReloading.Unbind<MySplatMapDrawing, &MySplatMapDrawing::OnAssetReloading>(this);
#endif

    if (MyModel) MyModel->DeleteObject();
    if (MyShader) MyShader->DeleteObject();
    if (SplatMap) SplatMap->DeleteObject();
}

void MySplatMapDrawing::OnUpdate()
{
    // Here you can add code that needs to be called every frame
}

void MySplatMapDrawing::OnDebugDraw()
{
    Actor* actor = GetActor();
    if (actor == nullptr)
    {
        return;
    }

    BoundingSphere s(actor->GetPosition(), 10);
    DebugDraw::DrawWireSphere(s);
}

bool MySplatMapDrawing::CanRender() const
{
    bool model = MyModel && MyModel->IsLoaded();
    bool shader = MyShader && MyShader->IsLoaded();
    bool splatMap = SplatMap && SplatMap->IsLoaded();
    return PostProcessEffect::CanRender() && model && shader && splatMap;
}

void MySplatMapDrawing::Render(GPUContext* context, RenderContext& renderContext, GPUTexture* input, GPUTexture* output)
{
    // PSO
    if (!_pso)
    {
        GPUPipelineState::Description desc = GPUPipelineState::Description::Default;
        desc.VS = MyShader->GetShader()->GetVS("MyVertexProgram");
        desc.PS = MyShader->GetShader()->GetPS("MyFragmentProgram");
        _pso = GPUPipelineState::New();
        _pso->Init(desc);
    }
    context->SetState(_pso);
    context->SetRenderTarget(renderContext.Buffers->DepthBuffer->View(), input->View());

    // CB
    GPUConstantBuffer* cb0 = MyShader->GetShader()->GetCB(0);
    if (cb0 != nullptr && GetActor() != nullptr)
    {
        Actor* actor = GetActor();
        Matrix model;
        actor->GetLocalToWorldMatrix(model);
        Matrix view = renderContext.View.View;
        Matrix proj = renderContext.View.Projection;

        ShaderData data;
        data.MVP = model * view * proj;
        data.MainTexST.X = SplatMapTiling.X;
        data.MainTexST.Y = SplatMapTiling.Y;
        data.MainTexST.Z = SplatMapOffset.X / 100;
        data.MainTexST.W = SplatMapOffset.Y / 100;

        context->UpdateCB(cb0, &data);
        context->BindCB(0, cb0);
    }

    // SR
    if (_sampler == nullptr)
    {
        _sampler = GPUSampler::New();
    }
    {
        GPUSamplerDescription desc = GPUSamplerDescription::New(GPUSamplerFilter::Bilinear, GPUSamplerAddressMode::Wrap);
        _sampler->Init(desc);
    }
    context->BindSampler(6, _sampler);
    context->BindSR(0, SplatMap->GetTexture()->View());

    // VB
    Array<MeshBase*> meshes;
    MyModel->GetMeshes(meshes);
    int32 meshCount = meshes.Count();
    for (int i = 0; i < meshCount; i++)
    {
        MeshBase* mesh = meshes.At(i);

        GPUBuffer* vb[MODEL_MAX_VB];
        vb[0] = mesh->GetVertexBuffer(0);
        vb[1] = mesh->GetVertexBuffer(1);
        vb[2] = nullptr;
        GPUBuffer* ib = mesh->GetIndexBuffer();

        context->BindVB(ToSpan(vb, MODEL_MAX_VB));
        context->BindIB(ib);
        context->DrawIndexed(ib->GetElementsCount());
    }
}

void MySplatMapDrawing::OnAssetReloading(Asset* asset)
{
    if (!asset) return;

    if (asset == MyModel) MyModel->DeleteObject();
    if (asset == MyShader) MyShader->DeleteObject();
    if (asset == SplatMap) SplatMap->DeleteObject();
}

