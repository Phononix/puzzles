#pragma once
#include "Triangle.h"
namespace geometry {
	struct Octahedron {
		std::array<Triangle, 8> faces;
		Octahedron() : faces{ 
			Triangle{ mat::Rvec<3>{0.5f, 0.0f, 0.5f}, mat::Rvec<3>{0.0f, 0.5f, 0.5f}, mat::Rvec<3>{0.5f, 0.5f, 0.0f} },
			Triangle{ mat::Rvec<3>{0.5f, 0.0f, 0.5f}, mat::Rvec<3>{0.5f, 0.5f, 0.0f}, mat::Rvec<3>{1.0f, 0.5f, 0.5f} },
			Triangle{ mat::Rvec<3>{0.5f, 0.0f, 0.5f}, mat::Rvec<3>{1.0f, 0.5f, 0.5f}, mat::Rvec<3>{0.5f, 0.5f, 1.0f} },
			Triangle{ mat::Rvec<3>{0.5f, 0.0f, 0.5f}, mat::Rvec<3>{0.5f, 0.5f, 1.0f}, mat::Rvec<3>{0.0f, 0.5f, 0.5f} },
			Triangle{ mat::Rvec<3>{0.5f, 1.0f, 0.5f}, mat::Rvec<3>{1.0f, 0.5f, 0.5f}, mat::Rvec<3>{0.5f, 0.5f, 0.0f} },
			Triangle{ mat::Rvec<3>{0.5f, 1.0f, 0.5f}, mat::Rvec<3>{0.5f, 0.5f, 0.0f}, mat::Rvec<3>{0.0f, 0.5f, 0.5f} },
			Triangle{ mat::Rvec<3>{0.5f, 1.0f, 0.5f}, mat::Rvec<3>{0.0f, 0.5f, 0.5f}, mat::Rvec<3>{0.5f, 0.5f, 1.0f} },
			Triangle{ mat::Rvec<3>{0.5f, 1.0f, 0.5f}, mat::Rvec<3>{0.5f, 0.5f, 1.0f}, mat::Rvec<3>{1.0f, 0.5f, 0.5f} } } {}
	};
	Octahedron operator*(const mat::Matrix<float, 3, 3>& lhs, const Octahedron& rhs) {
		Octahedron res;
		for (size_t i = 0; i < 8; i++)	{
			res.faces[i] = lhs * rhs.faces[i];
		}
		return res;
	}
	Octahedron operator+(const Octahedron& lhs, const mat::Rvec<3>& rhs) {
		Octahedron res;
		for (size_t i = 0; i < 8; i++)	{
			res.faces[i] = lhs.faces[i] + rhs;
		}
		return res;
	}
}