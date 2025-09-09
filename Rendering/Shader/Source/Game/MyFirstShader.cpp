#include "MyFirstShader.h"

#include "Engine/Graphics/GPUContext.h"
#include "Engine/Graphics/GPUBuffer.h"
#include "Engine/Graphics/RenderBuffers.h"
#include "Engine/Graphics/GPUPipelineState.h"
#include "Engine/Graphics/Shaders/GPUShader.h"
#include "Engine/Graphics/Shaders/GPUVertexLayout.h"
#include "Engine/Graphics/Textures/GPUSamplerDescription.h"
#include "Engine/Graphics/Textures/GPUSampler.h"

#include "Engine/Content/Content.h"

#include "Engine/Level/Actor.h"

MyFirstShader::MyFirstShader(const SpawnParams& params)
    : PostProcessEffect(params)
{
    _pso = nullptr;
    _myTexSampler = nullptr;

    UseSingleTarget = true;
    Location = PostProcessEffectLocation::BeforeForwardPass;
}

void MyFirstShader::OnEnable()
{
#if USE_EDITOR
    Content::AssetReloading.Bind<MyFirstShader, &MyFirstShader::OnAssetReloading>(this);
#endif

    SceneRenderTask::AddGlobalCustomPostFx(this);
}

void MyFirstShader::OnDisable()
{
    SceneRenderTask::RemoveGlobalCustomPostFx(this);

#if USE_EDITOR
    Content::AssetReloading.Unbind<MyFirstShader, &MyFirstShader::OnAssetReloading>(this);
#endif

    ReleaseShader();
}

void MyFirstShader::OnUpdate()
{
    // Here you can add code that needs to be called every frame
}

bool MyFirstShader::CanRender() const
{
    return PostProcessEffect::CanRender() && MyModel && MyShader && MyShader->IsLoaded() && MyTexture;
}

void MyFirstShader::Render(GPUContext* context, RenderContext& renderContext, GPUTexture* input, GPUTexture* output)
{
    if (!_pso)
    {
        _pso = GPUPipelineState::New();
        GPUPipelineState::Description desc = GPUPipelineState::Description::Default;
        desc.VS = MyShader->GPU->GetVS("MyVertexProgram");
        desc.PS = MyShader->GPU->GetPS("MyFragmentProgram");
        _pso->Init(desc);
    }

    if (!_myTexSampler)
    {
        _myTexSampler = GPUSampler::New();
    }
    GPUSamplerDescription _myTexSamplerDesc = GPUSamplerDescription::New(FilterMode, WrapMode);
    _myTexSampler->Init(_myTexSamplerDesc);

    GPUConstantBuffer* cb0 = MyShader->GPU->GetCB(0);
    if (cb0 != nullptr && GetActor() != nullptr)
    {
        Matrix Model;
        GetActor()->GetLocalToWorldMatrix(Model);
        Matrix View = renderContext.View.View;
        Matrix Projection = renderContext.View.Projection;
        // multiply them in reverse order because these transformation matrix are transposed.
        Matrix MVP = Model * View * Projection;
        // I didn't transpose the matrix like the "Custom Geometry Drawing" example did,
        // because HLSL expects column-major matrices by default, also I wrote `mul(Matrix, Vector)` in the shader code.
        //context->UpdateCB(cb0, &MVP);
        ShaderData data;
        data.MVP = MVP;
        data.Tint = Tint;
        data.MainTexST.X = Tiling.X;
        data.MainTexST.Y = Tiling.Y;
        data.MainTexST.Z = Offset.X / 100;
        data.MainTexST.W = Offset.Y / 100;
        context->UpdateCB(cb0, &data);
    }
    context->BindCB(0, cb0);

    context->BindSR(0, MyTexture->GetTexture()->View());

    context->BindSampler(0, _myTexSampler);

    Array<const MeshBase*> meshes;
    MyModel->GetMeshes(meshes);
    int meshCount = meshes.Count();
    for (int i = 0; i < meshCount; i++)
    {
        const MeshBase* mesh = meshes.At(i);

        GPUBuffer* VertexBuf[MODEL_MAX_VB];
        VertexBuf[0] = mesh->GetVertexBuffer(0);
        VertexBuf[1] = mesh->GetVertexBuffer(1);
        VertexBuf[2] = nullptr;

        GPUBuffer* ib = mesh->GetIndexBuffer();

        context->BindVB(ToSpan(VertexBuf, MODEL_MAX_VB));
        context->BindIB(ib);
        context->SetState(_pso);
        context->SetRenderTarget(renderContext.Buffers->DepthBuffer->View(), input->View());
        context->DrawIndexed(ib->GetElementsCount());
    }
}

void MyFirstShader::OnAssetReloading(Asset* asset)
{
    if (asset == MyShader) ReleaseShader();
}

void MyFirstShader::ReleaseShader()
{
    if (MyShader != nullptr)
    {
        MyShader->DeleteObject();
    }
}

