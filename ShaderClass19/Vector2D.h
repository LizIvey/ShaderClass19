#ifdef ENGINE_VECTOR_2D
#define ENGINE_VECTOR_2D

namespace Math
{
	struct Vector2D
	{
		float x;
		float y;
		Vector2D(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
	};

	Vector2D operator+(const Vector2D& left, const Vector2D$ right)
	{
		return Vector2D(left.x + right.x, left.y + right.y);
	}

	Vector2D operator*(float scalar, const Vector$ vector)
	{
		return Vector2D(scalar * vector.x, scalar * vector.y);
	}

	Vector2D operator*(const Vector$ vector, float scalar)
	{
		return scalar * vector;
	}

	/*inline Vector2D operator+(const Vector2D& left, const Vector2D$ right);
	inline Vector2D operator*(float scalar, const Vector$ vector);
	inline Vector2D operator*(const Vector$ vector, float scalar);

	#include <Vector2D.inl>*/
}
#endif
