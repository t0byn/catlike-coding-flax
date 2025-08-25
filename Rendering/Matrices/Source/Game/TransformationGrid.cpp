#include "TransformationGrid.h"

#include "Engine/Level/Prefabs/PrefabManager.h"
#include "Engine/Level/Actor.h"
#include "Engine/Level/Actors/StaticModel.h"

#include "Engine/Graphics/Materials/MaterialInfo.h"

#include "Engine/Content/Assets/MaterialInstance.h"

#include "Engine/Core/Types/Variant.h"

#include "Engine/Debug/DebugDraw.h"

TransformationGrid::TransformationGrid(const SpawnParams& params)
    : Script(params)
{
    // Enable ticking OnUpdate function
    _tickUpdate = true;
}

void TransformationGrid::OnEnable()
{
    // Here you can add code that needs to be called when script is enabled (eg. register for events)

    if (Prefab == nullptr) return;

    for (int i = 0, z = 0; z < GridResolution; z++)
    {
        for (int y = 0; y < GridResolution; y++)
        {
            for (int x = 0; x < GridResolution; x++, i++)
            {
                Actor* point = CreateGridPoint(x, y, z);
                _grid.Add(point);
            }
        }
    }
}

void TransformationGrid::OnDisable()
{
    // Here you can add code that needs to be called when script is disabled (eg. unregister from events)

    int count = _grid.Count();
    for (int i = count - 1; i >= 0; --i)
    {
        _grid[i]->DeleteObject();
        _grid.RemoveLast();
    }
}

void TransformationGrid::OnUpdate()
{
    // Here you can add code that needs to be called every frame

    _transformations.Clear();

    Actor* actor = GetActor();
    Array<Script*> scripts = actor->GetScripts<Script>();
    for (int i = 0; i < scripts.Count(); i++)
    {
        Script* script = scripts[i];
        if (!script || !script->GetEnabled())
        {
            continue;
        }

        if (script && script->GetType().GetInterface(ITransformation::TypeInitializer))
        {
            ITransformation* tf = ToInterface<ITransformation>(script);
            _transformations.Add(tf);
        }
    }

    UpdateTransformation();

    for (int i = 0, z = 0; z < GridResolution; z++)
    {
        for (int y = 0; y < GridResolution; y++)
        {
            for (int x = 0; x < GridResolution && i < _grid.Count(); x++, i++)
            {
                _grid[i]->SetLocalPosition(TransformPoint(x, y, z));
            }
        }
    }
}

void TransformationGrid::OnDebugDraw()
{
    Actor* actor = GetActor();
    if (actor != nullptr)
    {
        Matrix matx;
        actor->GetLocalToWorldMatrix(matx);
        float min = (0 - (GridResolution - 1) * 0.5) * 100;
        float max = (GridResolution - 1 - (GridResolution - 1) * 0.5) * 100;
        Vector4 minP = Matrix::TransformPosition(matx, Vector3(min, min, min));
        Vector4 maxP = Matrix::TransformPosition(matx, Vector3(max, max, max));
        BoundingBox dbgBox(Vector3(minP.X, minP.Y, minP.Z), Vector3(maxP.X, maxP.Y, maxP.Z));
        DebugDraw::DrawWireBox(dbgBox, Color::Yellow);
    }
}

Actor* TransformationGrid::CreateGridPoint(int x, int y, int z)
{
    Actor* actor = PrefabManager::SpawnPrefab(Prefab, GetActor());
    actor->SetLocalPosition(GetCoordinates(x, y, z));
    if (actor->Is<StaticModel>())
    {
        StaticModel* model = Cast<StaticModel>(actor);
        MaterialBase* material = model->GetMaterial(0);
        if (material != nullptr)
        {
            MaterialInstance* m = material->CreateVirtualInstance();
            Color clr((float)x / GridResolution, (float)y / GridResolution, (float)z / GridResolution);
            m->SetParameterValue(TEXT("Color"), Variant(clr));
            model->SetMaterial(0, m);
        }
    }
    return actor;
}

Vector3 TransformationGrid::GetCoordinates(int x, int y, int z)
{
    // * 100 because Flax's coordinate system use cm as base unit
    // refer to: https://docs.flaxengine.com/manual/get-started/flax-for-unity-devs/index.html#gameobject-vs-actor
    return Vector3(
        ((float)x - (GridResolution - 1) * 0.5) * 100,
        ((float)y - (GridResolution - 1) * 0.5) * 100,
        ((float)z - (GridResolution - 1) * 0.5) * 100 
    );
}

Vector3 TransformationGrid::TransformPoint(int x, int y, int z)
{
    Vector3 coordinates = GetCoordinates(x, y, z);
    //for (int i = 0; i < _transformations.Count(); i++)
    //{
    //    coordinates = _transformations[i]->Apply(coordinates);
    //}
    Float4 result = Matrix::TransformPosition(_transformation, coordinates);
    return Vector3(result.X / result.W, result.Y / result.W, result.Z / result.W);
}

void TransformationGrid::UpdateTransformation()
{
    _transformation = Matrix::Identity;
    for (int i = 0; i < _transformations.Count(); i++)
    {
        Matrix matx = _transformations[i]->GetMatrix();
        _transformation = matx * _transformation;
    }
    _transformation.Transpose();
}
