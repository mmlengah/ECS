#pragma once
#include "PCM.h"

using namespace PC;

class Camera {
public:
    Camera(Vector2<float> position, Vector2<float> scale, float rotation, Vector2<int> viewportSize);

    void setPosition(const Vector2<float>& newPosition);
    void lookAt(const Vector2<float>& newPosition);
    void setZoom(float zoom);
    const Vector2<float>& getPosition() const;
    void setViewportSize(const Vector2<int>& newSize);
    const Vector2<int>& getViewportSize() const;
    const Matrix3x3<float>& getTransformMatrix() const;

private:
    void updateTransformMatrix();

private:
    Vector2<float> position;
    Vector2<float> scale;
    float rotation;
    Vector2<int> viewportSize;
    Matrix3x3<float> transformMatrix;
};

