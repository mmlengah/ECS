#include "Camera.h"

Camera::Camera(Vector2<float> position, Vector2<int> viewportSize) : position(position), viewportSize(viewportSize)
{
	updateTransformMatrix();
}

void Camera::setPosition(const Vector2<float>& newPosition)
{
	position = newPosition;
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
	transformMatrix = Matrix3x3<float>::Matrix3x3FromTranslation(-position);
}
