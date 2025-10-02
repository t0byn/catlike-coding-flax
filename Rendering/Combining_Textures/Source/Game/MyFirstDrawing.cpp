#include "MyFirstDrawing.h"

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

MyFirstDrawing::MyFirstDrawing(const SpawnParams& params)
    : PostProcessEffect(params)
{
    // Enable ticking OnUpdate function
    _tickUpdate = true;

    MyModel = nullptr;
    MyShader = nullptr;
    Tint = Color::White;
    MainTexture = nullptr;
    MainTextureTiling = { 1,1 };
    MainTextureOffset = { 0,0 };
    DetailTexture = nullptr;
    DetailTextureTiling = { 1,1 };
    DetailTextureOffset = { 0,0 };

    _pso = nullptr;
    _sampler = nullptr;

    UseSingleTarget = true;
    Location = PostProcessEffectLocation::BeforeForwardPass;
}

void MyFirstDrawing::OnEnable()
{
    // Here you can add code that needs to be called when script is enabled (eg. register for events)

    SceneRenderTask::AddGlobalCustomPostFx(this);
#if USE_EDITOR
    Content::AssetReloading.Bind<MyFirstDrawing, &MyFirstDrawing::OnAssetReloading>(this);
#endif
}

void MyFirstDrawing::OnDisable()
{
    // Here you can add code that needs to be called when script is disabled (eg. unregister from events)

    SceneRenderTask::RemoveGlobalCustomPostFx(this);
#if USE_EDITOR
    Content::AssetReloading.Unbind<MyFirstDrawing, &MyFirstDrawing::OnAssetReloading>(this);
#endif

    if (MyModel) MyModel->DeleteObject();
    if (MyShader) MyShader->DeleteObject();
    if (MainTexture) MainTexture->DeleteObject();
    if (DetailTexture) DetailTexture->DeleteObject();
}

void MyFirstDrawing::OnUpdate()
{
    // Here you can add code that needs to be called every frame
}

void MyFirstDrawing::OnDebugDraw()
{
    Actor* actor = GetActor();
    if (actor == nullptr)
    {
        return;
    }

    BoundingSphere s(actor->GetPosition(), 10);
    DebugDraw::DrawWireSphere(s);
}

bool MyFirstDrawing::CanRender() const
{
    bool model = MyModel && MyModel->IsLoaded();
    bool shader = MyShader && MyShader->IsLoaded();
    bool mainTex = MainTexture && MainTexture->IsLoaded();
    bool detailTex = DetailTexture && DetailTexture->IsLoaded();
    return PostProcessEffect::CanRender() && model && shader && mainTex && detailTex;
}

void MyFirstDrawing::Render(GPUContext* context, RenderContext& renderContext, GPUTexture* input, GPUTexture* output)
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
        data.Tint = Tint.ToFloat4();
        data.MainTexST.X = MainTextureTiling.X;
        data.MainTexST.Y = MainTextureTiling.Y;
        data.MainTexST.Z = MainTextureOffset.X / 100;
        data.MainTexST.W = MainTextureOffset.Y / 100;
        data.DetailTexST.X = DetailTextureTiling.X;
        data.DetailTexST.Y = DetailTextureTiling.Y;
        data.DetailTexST.Z = DetailTextureOffset.X / 100;
        data.DetailTexST.W = DetailTextureOffset.Y / 100;

        context->UpdateCB(cb0, &data);
        context->BindCB(0, cb0);
    }

    // SR
    if (_sampler == nullptr)
    {
        _sampler = GPUSampler::New();
    }
    {
        GPUSamplerDescription desc = GPUSamplerDescription::New(GPUSamplerFilter::Bilinear, GPUSamplerAddressMode::Clamp);
        _sampler->Init(desc);
    }
    context->BindSampler(6, _sampler);
    context->BindSR(0, MainTexture->GetTexture()->View());
    context->BindSR(1, DetailTexture->GetTexture()->View());

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

void MyFirstDrawing::OnAssetReloading(Asset* asset)
{
    if (!asset) return;

    if (asset == MyModel) MyModel->DeleteObject();
    if (asset == MyShader) MyShader->DeleteObject();
    if (asset == MainTexture) MainTexture->DeleteObject();
    if (asset == DetailTexture) DetailTexture->DeleteObject();
}
