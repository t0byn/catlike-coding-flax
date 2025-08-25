#include "PositionTransformation.h"

PositionTransformation::PositionTransformation(const SpawnParams& params)
    : Script(params)
{
    // Enable ticking OnUpdate function
    //_tickUpdate = true;
}

void PositionTransformation::OnEnable()
{
    // Here you can add code that needs to be called when script is enabled (eg. register for events)
}

void PositionTransformation::OnDisable()
{
    // Here you can add code that needs to be called when script is disabled (eg. unregister from events)
}

void PositionTransformation::OnUpdate()
{
    // Here you can add code that needs to be called every frame
}

Matrix PositionTransformation::GetMatrix()
{
    Matrix matx;
    matx.SetRow1({ 1.0f, 0.0f, 0.0f, Position.X });
    matx.SetRow2({ 0.0f, 1.0f, 0.0f, Position.Y });
    matx.SetRow3({ 0.0f, 0.0f, 1.0f, Position.Z });
    matx.SetRow4({ 0.0f, 0.0f, 0.0f, 1.0f });
    return matx;
}

Vector3 PositionTransformation::Apply(const Vector3& point)
{
    return point + Position;
}
