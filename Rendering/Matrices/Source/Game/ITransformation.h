#pragma once

#include "Engine/Scripting/Script.h"

#include "Engine/Core/Math/Matrix.h"

API_INTERFACE() class GAME_API ITransformation
{
DECLARE_SCRIPTING_TYPE_MINIMAL(ITransformation)

	API_FUNCTION() virtual Matrix GetMatrix() = 0;

	API_FUNCTION() virtual Vector3 Apply(const Vector3& point) {
		Matrix matx = GetMatrix();
		matx.Transpose();
		Vector4 result = Matrix::TransformPosition(matx, point);
		return Vector3(result.X / result.W, result.Y / result.W, result.Z / result.W);
	}
};
