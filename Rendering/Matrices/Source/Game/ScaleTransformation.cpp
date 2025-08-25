#include "ScaleTransformation.h"

ScaleTransformation::ScaleTransformation(const SpawnParams& params)
    : Script(params)
{
    // Enable ticking OnUpdate function
    //_tickUpdate = true;
}

void ScaleTransformation::OnEnable()
{
    // Here you can add code that needs to be called when script is enabled (eg. register for events)
}

void ScaleTransformation::OnDisable()
{
    // Here you can add code that needs to be called when script is disabled (eg. unregister from events)
}

void ScaleTransformation::OnUpdate()
{
    // Here you can add code that needs to be called every frame
}

Matrix ScaleTransformation::GetMatrix()
{
    Matrix matx;
    matx.SetRow1({ Scale.X, 0.0f, 0.0f, 0.0f });
    matx.SetRow2({ 0.0f, Scale.Y, 0.0f, 0.0f });
    matx.SetRow3({ 0.0f, 0.0f, Scale.Z, 0.0f });
    matx.SetRow4({ 0.0f, 0.0f, 0.0f, 1.0f });
    return matx;
}

Vector3 ScaleTransformation::Apply(const Vector3& point)
{
    return Vector3(
        point.X * Scale.X,
        point.Y * Scale.Y,
        point.Z * Scale.Z
    );
}
