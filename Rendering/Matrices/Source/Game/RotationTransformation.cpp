#include "RotationTransformation.h"

RotationTransformation::RotationTransformation(const SpawnParams& params)
    : Script(params)
{
    // Enable ticking OnUpdate function
    //_tickUpdate = true;
}

void RotationTransformation::OnEnable()
{
    // Here you can add code that needs to be called when script is enabled (eg. register for events)
}

void RotationTransformation::OnDisable()
{
    // Here you can add code that needs to be called when script is disabled (eg. unregister from events)
}

void RotationTransformation::OnUpdate()
{
    // Here you can add code that needs to be called every frame
}

Matrix RotationTransformation::GetMatrix()
{
    // rotation order: ZYX
    float radX = Rotation.X * DegreesToRadians;
    float radY = Rotation.Y * DegreesToRadians;
    float radZ = Rotation.Z * DegreesToRadians;

    float sinX, sinY, sinZ;
    float cosX, cosY, cosZ;
    Math::SinCos(radX, sinX, cosX);
    Math::SinCos(radY, sinY, cosY);
    Math::SinCos(radZ, sinZ, cosZ);

    Matrix matx;
    matx.SetColumn1({
        cosY * cosZ,
        cosX * sinZ + sinX * sinY * cosZ,
        sinX * sinZ - cosX * sinY * cosZ,
        0.0f
    });
    matx.SetColumn2({
        -cosY * sinZ,
        cosX * cosZ - sinX * sinY * sinZ,
        sinX * cosZ + cosX * sinY * sinZ,
        0.0f
    });
    matx.SetColumn3({
        sinY,
        -sinX * cosY,
        cosX * cosY,
        0.0f
    });
    matx.SetColumn4({ 0.0f, 0.0f, 0.0f, 1.0f });

    return matx;
}

Vector3 RotationTransformation::Apply(const Vector3& point)
{
    // rotation order: ZYX
    float radX = Rotation.X * DegreesToRadians;
    float radY = Rotation.Y * DegreesToRadians;
    float radZ = Rotation.Z * DegreesToRadians;

    float sinX, sinY, sinZ;
    float cosX, cosY, cosZ;
    Math::SinCos(radX, sinX, cosX);
    Math::SinCos(radY, sinY, cosY);
    Math::SinCos(radZ, sinZ, cosZ);

    Vector3 axisX = { cosY * cosZ, cosX * sinZ + sinX * sinY * cosZ, sinX * sinZ - cosX * sinY * cosZ };
    Vector3 axisY = { -cosY * sinZ, cosX * cosZ - sinX * sinY * sinZ, sinX * cosZ + cosX * sinY * sinZ };
    Vector3 axisZ = { sinY, -sinX * cosY, cosX * cosY };

    return point.X * axisX + point.Y * axisY + point.Z * axisZ;
}
