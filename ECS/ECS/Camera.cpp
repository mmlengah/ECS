#include "Camera.h"

Camera::Camera(Vector2<float> position, Vector2<float> scale, float rotation, Vector2<int> viewportSize)
    : position(position), scale(scale), rotation(rotation), viewportSize(viewportSize)
{
    updateTransformMatrix();
}

void Camera::setPosition(const Vector2<float>& newPosition)
{
    position = newPosition;
    updateTransformMatrix();
}

void Camera::lookAt(const Vector2<float>& newPosition) {
    Vector2<float> screenCenter = { static_cast<float>(viewportSize.x) / 2.0f,
        static_cast<float>(viewportSize.y) / 2.0f };

    // Translates the object position so the desired position is at the center of the screen.
    position = newPosition - screenCenter;

    updateTransformMatrix();
}

void Camera::setZoom(float zoom)
{
    scale.x = zoom;
    scale.y = zoom;
    updateTransformMatrix();
}

const Vector2<float>& Camera::getPosition() const
{
    return position;
}

void Camera::setViewportSize(const Vector2<int>& newSize)
{
    viewportSize = newSize;
    updateTransformMatrix();
}

const Vector2<int>& Camera::getViewportSize() const
{
    return viewportSize;
}

const Matrix3x3<float>& Camera::getTransformMatrix() const
{
    return transformMatrix;
}

void Camera::updateTransformMatrix()
{
    transformMatrix = Matrix3x3<float>::Matrix3x3FromTranslation(-position) *
        Matrix3x3<float>::Matrix3x3FromRotation(-rotation) *
        Matrix3x3<float>::Matrix3x3FromScale(scale);
}