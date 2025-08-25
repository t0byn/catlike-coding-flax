#pragma once

#include "Engine/Scripting/Script.h"

#include "ITransformation.h"

API_CLASS() class GAME_API CameraTransformation : public Script, public ITransformation
{
API_AUTO_SERIALIZATION();
DECLARE_SCRIPTING_TYPE(CameraTransformation);

    // [Script]
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate() override;

    Matrix GetMatrix() override;

    API_FIELD() float FocalLength = 1.0F;
};
