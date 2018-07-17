#pragma once
#include "kinematics.h"
#include "Triangle.h"
namespace geometry {
	template<unsigned int N>
	class Pyramid {
	public:
		std::array<Triangle, N> faces;
		Pyramid() : faces{} {
			faces[0] = Triangle{
				{ -mat::sine((3.1415926f) / (float)N) ,0.0f, mat::cosine((3.1415926f) / (float)N) },
				{ mat::sine((3.1415926f) / (float)N), 0.0f, mat::cosine((3.1415926f) / (float)N) },
				{ 0.0f, 1.0f, 0.0f} };
			for (size_t i = 1; i < N; i++) {
				faces[i] = mat::rotateY(3.1415926f*(2.f / (float)N))*faces[i - 1];
			}
		}
	};
	template<unsigned int N>
	Pyramid<N> operator*(const mat::Matrix<float, 3, 3>& lhs, const Pyramid<N>& rhs) {
		Pyramid<N> res;
		for (size_t i = 0; i < N; i++)
		{
			res.faces[i] = lhs * rhs.faces[i];
		}
		return res;
	}
	template<unsigned int N>
	Pyramid<N> operator+(const Pyramid<N>& lhs, const mat::Rvec<3>& rhs) {
		Pyramid<N> res;
		for (size_t i = 0; i < N; i++)
		{
			res.faces[i] = lhs.faces[i] + rhs;
		}
		return res;
	}
}