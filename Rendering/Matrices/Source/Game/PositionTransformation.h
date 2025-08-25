#pragma once

#include "Engine/Scripting/Script.h"

#include "Engine/Core/Math/Vector3.h"

#include "ITransformation.h"

API_CLASS() class GAME_API PositionTransformation : public Script, public ITransformation
{
API_AUTO_SERIALIZATION();
DECLARE_SCRIPTING_TYPE(PositionTransformation);

    // [Script]
    void OnEnable() override;
    void OnDisable() override;
    void OnUpdate() override;

    Matrix GetMatrix() override;

	Vector3 Apply(const Vector3& point) override;

    API_FIELD() Vector3 Position = { 0.0f, 0.0f, 0.0f };
};
