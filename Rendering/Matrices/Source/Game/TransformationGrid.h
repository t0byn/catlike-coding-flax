#pragma once

#include "Engine/Scripting/Script.h"

#include "Engine/Content/AssetReference.h"

#include "Engine/Level/Prefabs/Prefab.h"

#include "ITransformation.h"

API_CLASS() class GAME_API TransformationGrid : public Script
{
API_AUTO_SERIALIZATION();
DECLARE_SCRIPTING_TYPE(TransformationGrid);

    // [Script]
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate() override;

    void OnDebugDraw() override;

public:
    API_FIELD(Attributes = "ShowInEditor") AssetReference<Prefab> Prefab;
    API_FIELD(Attributes = "ShowInEditor") uint32 GridResolution = 10;

private:
    Actor* CreateGridPoint(int x, int y, int z);
    Vector3 GetCoordinates(int x, int y, int z);
    Vector3 TransformPoint(int x, int y, int z);
    void UpdateTransformation();

    Array<Actor*> _grid;
    float _coordinateScale;
    Array<ITransformation*> _transformations;
    Matrix _transformation;
};
