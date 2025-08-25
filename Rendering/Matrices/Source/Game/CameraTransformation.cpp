#include "CameraTransformation.h"

CameraTransformation::CameraTransformation(const SpawnParams& params)
    : Script(params)
{
    // Enable ticking OnUpdate function
    //_tickUpdate = true;
}

void CameraTransformation::OnEnable()
{
    // Here you can add code that needs to be called when script is enabled (eg. register for events)
}

void CameraTransformation::OnDisable()
{
    // Here you can add code that needs to be called when script is disabled (eg. unregister from events)
}

void CameraTransformation::OnUpdate()
{
    // Here you can add code that needs to be called every frame
}

Matrix CameraTransformation::GetMatrix()
{
    Matrix matx;
    matx.SetRow1({ FocalLength, 0.0f, 0.0f, 0.0f });
    matx.SetRow2({ 0.0f, FocalLength, 0.0f, 0.0f });
    //matx.SetRow3({ 0.0f, 0.0f, 1.0f, 0.0f });
    matx.SetRow3({ 0.0f, 0.0f, 0.0f, 0.0f });
    //matx.SetRow4({ 0.0f, 0.0f, 0.0f, 1.0f });
    matx.SetRow4({ 0.0f, 0.0f, 1.0f, 0.0f });

    return matx;
}
