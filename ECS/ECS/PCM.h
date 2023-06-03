//PCM.h stands for Practial Components Mathematics 
#pragma once
#include <cmath>
#include <stdexcept>
#include <ostream>
#include <cmath>
#include <cassert>
#include <emmintrin.h>

namespace PC {
	template <typename T>
	struct Vector2 {
		T x, y;

		Vector2()
			: x(0), y(0)
		{}

		Vector2(T x, T y)
			: x(x), y(y)
		{}

		Vector2 operator+(const Vector2& v) const {
			return Vector2(x + v.x, y + v.y);
		}

		Vector2& operator+=(const Vector2& v) {
			x += v.x;
			y += v.y;
			return *this;
		}

		Vector2 operator+(const T& v) const {
			return Vector2(x + v, y + v);
		}

		Vector2& operator+=(const T& v) {
			x += v;
			y += v;
			return *this;
		}

		Vector2 operator-(const Vector2& v) const {
			return Vector2(x - v.x, y - v.y);
		}

		Vector2& operator-=(const Vector2& v) {
			x -= v.x;
			y -= v.y;
			return *this;
		}

		Vector2 operator-(const T& v) const {
			return Vector2(x - v, y - v);
		}

		Vector2& operator-=(const T& v) {
			x -= v;
			y -= v;
			return *this;
		}

		Vector2 operator*(const T& v) const {
			return Vector2(x * v, y * v);
		}

		Vector2& operator*=(const T& v) {
			x *= v;
			y *= v;
			return *this;
		}

		Vector2 operator/(const T& v) const {
			assert(v != 0);  // prevent division by zero
			return Vector2(x / v, y / v);
		}

		Vector2& operator/=(const T& v) {
			assert(v != 0);  // prevent division by zero
			x /= v;
			y /= v;
			return *this;
		}

		bool operator==(const Vector2& v) const {
			return x == v.x && y == v.y;
		}

		bool operator!=(const Vector2& v) const {
			return !(*this == v);
		}

		Vector2 operator-() const {
			return Vector2(-x, -y);
		}

		bool operator<(const Vector2& v) const {
			return magnitude() < v.magnitude();
		}

		bool operator>(const Vector2& v) const {
			return magnitude() > v.magnitude();
		}

		bool operator<=(const Vector2& v) const {
			return magnitude() <= v.magnitude();
		}

		bool operator>=(const Vector2& v) const {
			return magnitude() >= v.magnitude();
		}

		T dotProduct(const Vector2& v) const {
			return x * v.x + y * v.y;
		}

		T magnitude() const {
			return std::sqrt(x * x + y * y);
		}

		Vector2 normalize() const {
			T mag = magnitude();
			return Vector2(x / mag, y / mag);
		}

		// angle in degrees
		T angle(const Vector2& v) const {
			T magProduct = magnitude() * v.magnitude();
			if (magProduct == 0) { // avoid division by zero
				return 0;
			}
			T dotProd = dotProduct(v);
			T cosAngle = dotProd / magProduct;
			// Clamp cosAngle to the interval [-1, 1]
			cosAngle = std::max(T(-1), std::min(T(1), cosAngle));
			T rad = std::acos(cosAngle);
			return rad * 180.0 / M_PI; // convert to degrees
		}

		Vector2 rotate(T angle) const {
			T radian = angle * M_PI / 180.0; // convert angle to radians
			T cosAngle = std::cos(radian);
			T sinAngle = std::sin(radian);
			return Vector2(x * cosAngle - y * sinAngle, x * sinAngle + y * cosAngle);
		}

		friend std::ostream& operator<<(std::ostream& os, const Vector2& v) {
			os << "[" << "x: " << v.x << ", y: " << v.y << "]";
			return os;
		}

	};

	template <typename T>
	struct Vector3 {
		T x, y, z;

		Vector3() : x(0), y(0), z(0) {}

		Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

		Vector3 operator+(const Vector3& v) const {
			return Vector3(x + v.x, y + v.y, z + v.z);
		}

		Vector3& operator+=(const Vector3& v) {
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		Vector3 operator+(const T& v) const {
			return Vector3(x + v, y + v, z + v);
		}

		Vector3& operator+=(const T& v) {
			x += v;
			y += v;
			z += v;
			return *this;
		}

		Vector3 operator-(const Vector3& v) const {
			return Vector3(x - v.x, y - v.y, z - v.z);
		}

		Vector3& operator-=(const Vector3& v) {
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}

		Vector3 operator-(const T& v) const {
			return Vector3(x - v, y - v, z - v);
		}

		Vector3& operator-=(const T& v) {
			x -= v;
			y -= v;
			z -= v;
			return *this;
		}

		Vector3 operator*(const T& v) const {
			return Vector3(x * v, y * v, z * v);
		}		

		Vector3& operator*=(const T& v) {
			x *= v;
			y *= v;
			z *= v;
			return *this;
		}

		Vector3 operator/(const T& v) const {
			assert(v != 0); // prevent division by zero
			return Vector3(x / v, y / v, z / v);
		}

		Vector3& operator/=(const T& v) {
			assert(v != 0); // prevent division by zero
			x /= v;
			y /= v;
			z /= v;
			return *this;
		}

		bool operator==(const Vector3& v) const {
			return (x == v.x && y == v.y && z == v.z);
		}

		bool operator!=(const Vector3& v) const {
			return !(*this == v);
		}

		bool operator<(const Vector3& v) const {
			return (magnitude() < v.magnitude());
		}

		bool operator>(const Vector3& v) const {
			return (magnitude() > v.magnitude());
		}

		bool operator<=(const Vector3& v) const {
			return (magnitude() <= v.magnitude());
		}

		bool operator>=(const Vector3& v) const {
			return (magnitude() >= v.magnitude());
		}

		T dotProduct(const Vector3& v) const {
			return x * v.x + y * v.y + z * v.z;
		}

		Vector3 crossProduct(const Vector3& v) const {
			return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
		}

		T magnitude() const {
			return std::sqrt(x * x + y * y + z * z);
		}

		Vector3<T> normalise() const {
			T mag = magnitude();
			return Vector3<T>(x / mag, y / mag, z / mag);
		}

		// angle in degrees
		T angle(const Vector3& other) const {
			T dot = this->dotProduct(other);
			T magnitudes = this->magnitude() * other.magnitude();
			// Ensure magnitudes is not zero
			assert(magnitudes != 0);
			// Make sure the value is between -1 and 1 before taking acos to avoid NaN
			T cosAngle = std::max(std::min(dot / magnitudes, (T)1.0), (T)-1.0);
			T angleRad = std::acos(cosAngle);
			// Convert the angle in radians to degrees
			T angleDeg = angleRad * 180.0 / M_PI;
			return angleDeg;
		}

		Vector3 rotate(const Vector3& axis, T angle) const {
			T rad = angle * M_PI / 180;  // Convert angle to radians
			T cosAngle = std::cos(rad);
			T sinAngle = std::sin(rad);
			Vector3 u = axis.normalise(); // Normalised rotation axis

			// Using Rodrigues' rotation formula
			Vector3 rotated = (*this) * cosAngle + (u.crossProduct((*this))) * sinAngle + u * (u.dotProduct((*this))) * (1 - cosAngle);
			return rotated;
		}

		friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
			os << "[" << "x: " << v.x << ", y: " << v.y << ", z: " << v.z << "]";
			return os;
		}
	};


	template <typename T>
	struct Vector4 {
		T x, y, z, w;

		Vector4() : x(0), y(0), z(0), w(0) {}

		Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

		Vector4 operator+(const Vector4& v) const {
			return Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
		}

		Vector4& operator+=(const Vector4& v) {
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.w;
			return *this;
		}

		Vector4 operator+(const T& v) const {
			return Vector4(x + v, y + v, z + v, w + v);
		}

		Vector4& operator+=(const T& v) {
			x += v;
			y += v;
			z += v;
			w += v;
			return *this;
		}

		Vector4 operator-(const Vector4& v) const {
			return Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
		}

		Vector4& operator-=(const Vector4& v) {
			x -= v.x;
			y -= v.y;
			z -= v.z;
			w -= v.w;
			return *this;
		}

		Vector4 operator-(const T& v) const {
			return Vector4(x - v, y - v, z - v, w - v);
		}

		Vector4& operator-=(const T& v) {
			x -= v;
			y -= v;
			z -= v;
			w -= v;
			return *this;
		}

		Vector4 operator*(const T& v) const {
			return Vector4(x * v, y * v, z * v, w * v);
		}

		Vector4& operator*=(const T& v) {
			x *= v;
			y *= v;
			z *= v;
			w *= v;
			return *this;
		}

		Vector4 operator/(const T& v) const {
			assert(v != 0); // prevent division by zero
			return Vector4(x / v, y / v, z / v, w / v);
		}

		Vector4& operator/=(const T& v) {
			assert(v != 0); // prevent division by zero
			x /= v;
			y /= v;
			z /= v;
			w /= v;
			return *this;
		}

		bool operator==(const Vector4& v) const {
			return (x == v.x && y == v.y && z == v.z && w == v.w);
		}

		bool operator!=(const Vector4& v) const {
			return !(*this == v);
		}

		T dotProduct(const Vector4& v) const {
			return x * v.x + y * v.y + z * v.z + w * v.w;
		}

		T magnitude() const {
			return std::sqrt(x * x + y * y + z * z + w * w);
		}

		Vector4 normalise() const {
			T mag = magnitude();
			return Vector4(x / mag, y / mag, z / mag, w / mag);
		}

		friend std::ostream& operator<<(std::ostream& os, const Vector4& v) {
			os << "[" << "x: " << v.x << ", y: " << v.y << ", z: " << v.z << ", w: " << v.w << "]";
			return os;
		}
	};

	template <typename T>
	struct Matrix3x3 {
	private:
		T determinant2x2(T a, T b, T c, T d) {
			return a * d - b * c;
		}

		T cofactor(int row, int column) {
			int sign = ((row + column) % 2 == 0) ? 1 : -1;
			T minor = determinant2x2(
				matrix[(row + 1) % 3][(column + 1) % 3],
				matrix[(row + 1) % 3][(column + 2) % 3],
				matrix[(row + 2) % 3][(column + 1) % 3],
				matrix[(row + 2) % 3][(column + 2) % 3]
			);
			return sign * minor;
		}

	public:
		int const rows = 3;
		int const columns = 3;
		T matrix[3][3];

		Matrix3x3(T matrix[3][3]) {
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					this->matrix[i][j] = matrix[i][j];
				}
			}
		}

		Matrix3x3() {
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					matrix[i][j] = 0;
				}
			}
		}

		void SetValue(int row, int column, T value) { matrix[row][column] = value; }

		T GetValue(int row, int column) const { return matrix[row][column]; }

		// I removed the SIMD implementation for brevity and compatibility.
		// Please add it back if you need it.
		Matrix3x3 operator*(const Matrix3x3& m) const {
			Matrix3x3 temp;
			__m128 aLine, bLine, rLine;

			for (int i = 0; i < 3; i++) {
				// Load the whole row:
				aLine = _mm_loadu_ps(&matrix[i][0]);
				for (int j = 0; j < 3; j++) {
					// Broadcast the element across the SIMD vector:
					bLine = _mm_set1_ps(m.matrix[j][0]);
					// Multiply the row by the column element:
					rLine = _mm_mul_ps(aLine, bLine);

					// Sum the result: this is the dot product:
					rLine = _mm_hadd_ps(rLine, rLine);
					rLine = _mm_hadd_ps(rLine, rLine);

					// Store the result back into the matrix:
					_mm_store_ss(&temp.matrix[i][j], rLine);
				}
			}
			return temp;
		}

		Matrix3x3 operator=(const Matrix3x3& m) {
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					matrix[i][j] = m.matrix[i][j];
				}
			}
			return *this;
		}

		Matrix3x3 transpose() {
			Matrix3x3 temp;
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					temp.SetValue(i, j, matrix[j][i]);
				}
			}
			return temp;
		}

		T determinant() {
			T a = matrix[0][0], b = matrix[0][1], c = matrix[0][2];
			T d = matrix[1][0], e = matrix[1][1], f = matrix[1][2];
			T g = matrix[2][0], h = matrix[2][1], i = matrix[2][2];

			return a * e * i + b * f * g + c * d * h - c * e * g - b * d * i - a * f * h;
		}

		// Create a 3x3 transformation matrix for translation
		static Matrix3x3 Matrix3x3FromTranslation(const Vector2<T>& v) {
			Matrix3x3 temp;
			temp.SetValue(0, 0, 1.0f);
			temp.SetValue(0, 2, v.x);
			temp.SetValue(1, 1, 1.0f);
			temp.SetValue(1, 2, v.y);
			temp.SetValue(2, 2, 1.0f);
			return temp;
		}

		// Create a 3x3 transformation matrix for rotation
		static Matrix3x3 Matrix3x3FromRotation(const T& rotationAngle) {
			Matrix3x3 temp;
			float cosTheta = cos(rotationAngle * 3.14159f / 180.0f);
			float sinTheta = sin(rotationAngle * 3.14159f / 180.0f);

			temp.SetValue(0, 0, cosTheta);
			temp.SetValue(0, 1, -sinTheta);
			temp.SetValue(0, 2, 0.0f);
			temp.SetValue(1, 0, sinTheta);
			temp.SetValue(1, 1, cosTheta);
			temp.SetValue(1, 2, 0.0f);
			temp.SetValue(2, 2, 1.0f);

			return temp;
		}

		// Create a 3x3 transformation matrix for scaling
		static Matrix3x3 Matrix3x3FromScale(const Vector2<T>& scaleFactors) {
			Matrix3x3 temp;
			temp.SetValue(0, 0, scaleFactors.x);
			temp.SetValue(0, 2, 0.0f);
			temp.SetValue(1, 1, scaleFactors.y);
			temp.SetValue(1, 2, 0.0f);
			temp.SetValue(2, 2, 1.0f);
			return temp;
		}

	};

	template <typename T>
	struct Matrix4x4 {
	private:
		T determinant3x3(T a, T b, T c, T d, T e, T f, T g, T h, T i) {
			return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
		}

		T cofactor(int row, int column) {
			int sign = ((row + column) % 2 == 0) ? 1 : -1;
			T minor = determinant3x3(
				matrix[(row + 1) % 4][(column + 1) % 4],
				matrix[(row + 1) % 4][(column + 2) % 4],
				matrix[(row + 1) % 4][(column + 3) % 4],
				matrix[(row + 2) % 4][(column + 1) % 4],
				matrix[(row + 2) % 4][(column + 2) % 4],
				matrix[(row + 2) % 4][(column + 3) % 4],
				matrix[(row + 3) % 4][(column + 1) % 4],
				matrix[(row + 3) % 4][(column + 2) % 4],
				matrix[(row + 3) % 4][(column + 3) % 4]
			);

			return sign * minor;
		}
	public:
		int const rows = 4;
		int const columns = 4;
		T matrix[4][4];

		Matrix4x4(T matrix[4][4]) {
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					this->matrix[i][j] = matrix[i][j];
				}
			}
		}
		Matrix4x4() {
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					matrix[i][j] = 0;
				}
			}
		}
		void SetValue(int row, int column, T value) { matrix[row][column] = value; }

		T GetValue(int row, int column) const { return matrix[row][column]; }

		Matrix4x4 operator*(const Matrix4x4& m) const {
			Matrix4x4 temp;
			for (int i = 0; i < rows; i++) {
				__m128 row = _mm_load_ps(matrix[i]);
				for (int j = 0; j < columns; j++) {
					__m128 col = _mm_set_ps(m.matrix[0][j], m.matrix[1][j], m.matrix[2][j], m.matrix[3][j]);
					__m128 res = _mm_mul_ps(row, col);

					// Sum the four float values of res into a single float
					res = _mm_hadd_ps(res, res);
					res = _mm_hadd_ps(res, res);

					// Store the result back into temp
					_mm_store_ss(&(temp.matrix[i][j]), res);
				}
			}
			return temp;
		}

		Matrix4x4 operator=(const Matrix4x4& m) {
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					matrix[i][j] = m.matrix[i][j];
				}
			}
			return *this;
		}

		Matrix4x4 transpose() {
			Matrix4x4 temp;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					temp.SetValue(i, j, matrix[j][i]);
				}
			}
			return temp;
		}

		T determinant() {
			T a = matrix[0][0], b = matrix[0][1], c = matrix[0][2], d = matrix[0][3];
			T e = matrix[1][0], f = matrix[1][1], g = matrix[1][2], h = matrix[1][3];
			T i = matrix[2][0], j = matrix[2][1], k = matrix[2][2], l = matrix[2][3];
			T m = matrix[3][0], n = matrix[3][1], o = matrix[3][2], p = matrix[3][3];

			T cofactor1 = determinant3x3(f, g, h, j, k, l, n, o, p);
			T cofactor2 = determinant3x3(e, g, h, i, k, l, m, o, p);
			T cofactor3 = determinant3x3(e, f, h, i, j, l, m, n, p);
			T cofactor4 = determinant3x3(e, f, g, i, j, k, m, n, o);

			return a * cofactor1 - b * cofactor2 + c * cofactor3 - d * cofactor4;
		}

		Matrix4x4 cofactor() {
			Matrix4x4 temp;
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					temp.matrix[i][j] = cofactor(i, j);
				}
			}
			return temp;
		}

		Matrix4x4<float> inverse() {
			Matrix4x4 cofactors = cofactor();
			Matrix4x4 adjugate = cofactors.transpose();
			T det = determinant();

			if (det == 0) {
				throw std::runtime_error("Matrix is not invertible");
			}
			else {
				Matrix4x4<float> result;
				for (int i = 0; i < rows; i++) {
					for (int j = 0; j < columns; j++) {
						result.matrix[i][j] = static_cast<float>(adjugate.matrix[i][j] / det);
					}
				}
				return result;
			}
		}

		static Matrix4x4 Matrix4x4FromTranslation(const Vector3<T>& v) {
			Matrix4x4 temp;
			temp.SetValue(0, 0, 1.0f);
			temp.SetValue(0, 3, v.x);
			temp.SetValue(1, 1, 1.0f);
			temp.SetValue(1, 3, v.y);
			temp.SetValue(2, 2, 1.0f);
			temp.SetValue(2, 3, v.z);
			temp.SetValue(3, 3, 1.0f);
			return temp;
		}

		static Matrix4x4 Matrix4x4FromRotation(const Vector3<T>& rotationAngles) {
			Matrix4x4 temp;
			float cosX = cos(rotationAngles.x * 3.14159f / 180.0f);
			float sinX = sin(rotationAngles.x * 3.14159f / 180.0f);
			float cosY = cos(rotationAngles.y * 3.14159f / 180.0f);
			float sinY = sin(rotationAngles.y * 3.14159f / 180.0f);
			float cosZ = cos(rotationAngles.z * 3.14159f / 180.0f);
			float sinZ = sin(rotationAngles.z * 3.14159f / 180.0f);

			temp.SetValue(0, 0, cosY * cosZ);
			temp.SetValue(0, 1, cosY * sinZ);
			temp.SetValue(0, 2, -sinY);
			temp.SetValue(0, 3, 0.0f);

			temp.SetValue(1, 0, sinX * sinY * cosZ - cosX * sinZ);
			temp.SetValue(1, 1, sinX * sinY * sinZ + cosX * cosZ);
			temp.SetValue(1, 2, sinX * cosY);
			temp.SetValue(1, 3, 0.0f);

			temp.SetValue(2, 0, cosX * sinY * cosZ + sinX * sinZ);
			temp.SetValue(2, 1, cosX * sinY * sinZ - sinX * cosZ);
			temp.SetValue(2, 2, cosX * cosY);
			temp.SetValue(2, 3, 0.0f);

			temp.SetValue(3, 0, 0.0f);
			temp.SetValue(3, 1, 0.0f);
			temp.SetValue(3, 2, 0.0f);
			temp.SetValue(3, 3, 1.0f);

			return temp;
		}

		static Matrix4x4 Matrix4x4FromScale(const Vector3<T>& scaleFactors) {
			Matrix4x4 temp;
			temp.SetValue(0, 0, scaleFactors.x);
			temp.SetValue(0, 3, 0.0f);
			temp.SetValue(1, 1, scaleFactors.y);
			temp.SetValue(1, 3, 0.0f);
			temp.SetValue(2, 2, scaleFactors.z);
			temp.SetValue(2, 3, 0.0f);
			temp.SetValue(3, 3, 1.0f);
			return temp;
		}

		static Matrix4x4 lookAt(const Vector3<T>& eye, const Vector3<T>& center, const Vector3<T>& up) {
			Vector3<T> f = (center - eye).normalize();
			Vector3<T> u = up.normalize();
			Vector3<T> s = f.cross(u).normalize();
			u = s.cross(f);

			Matrix4x4 temp;
			temp.SetValue(0, 0, s.x);
			temp.SetValue(0, 1, s.y);
			temp.SetValue(0, 2, s.z);
			temp.SetValue(1, 0, u.x);
			temp.SetValue(1, 1, u.y);
			temp.SetValue(1, 2, u.z);
			temp.SetValue(2, 0, -f.x);
			temp.SetValue(2, 1, -f.y);
			temp.SetValue(2, 2, -f.z);
			temp.SetValue(0, 3, -s.dot(eye));
			temp.SetValue(1, 3, -u.dot(eye));
			temp.SetValue(2, 3, f.dot(eye));
			temp.SetValue(3, 3, 1.0f);
			return temp;
		}

		static Matrix4x4 perspective(T fov, T aspectRatio, T nearPlane, T farPlane) {
			Matrix4x4 temp;

			T tanHalfFov = tan(fov / 2.0);
			temp.SetValue(0, 0, 1.0 / (aspectRatio * tanHalfFov));
			temp.SetValue(1, 1, 1.0 / tanHalfFov);
			temp.SetValue(2, 2, -(farPlane + nearPlane) / (farPlane - nearPlane));
			temp.SetValue(2, 3, -2.0 * farPlane * nearPlane / (farPlane - nearPlane));
			temp.SetValue(3, 2, -1.0);
			temp.SetValue(3, 3, 0.0);

			return temp;
		}

		static Matrix4x4 orthographic(T left, T right, T bottom, T top, T nearPlane, T farPlane) {
			Matrix4x4 temp;

			temp.SetValue(0, 0, 2.0 / (right - left));
			temp.SetValue(0, 3, -(right + left) / (right - left));
			temp.SetValue(1, 1, 2.0 / (top - bottom));
			temp.SetValue(1, 3, -(top + bottom) / (top - bottom));
			temp.SetValue(2, 2, -2.0 / (farPlane - nearPlane));
			temp.SetValue(2, 3, -(farPlane + nearPlane) / (farPlane - nearPlane));
			temp.SetValue(3, 3, 1.0);

			return temp;
		}

		friend std::ostream& operator<<(std::ostream& os, const Matrix4x4& m) {
			for (int i = 0; i < m.rows; i++) {
				for (int j = 0; j < m.columns; j++) {
					os << m.matrix[i][j] << " ";
				}
				os << " \n";
			}
			return os;
		}

	};
}
