#pragma once
#include "kinematics.h"
#include "Triangle.h"
namespace geometry {
	struct Tile {
		Triangle I;
		Triangle II;
		Tile(mat::Rvec<3> x = mat::Rvec<3>{ 1.0f, 0.0f, 0.0f }, mat::Rvec<3> y = mat::Rvec<3>{ 0.0f, 1.0f, 0.0f }) :
			I{ mat::Rvec<3>{}, x, y },
			II{ x+y, y, x } {}
	};
	Tile operator*(const mat::Matrix<float, 3, 3>& lhs, const Tile& rhs) {
		Tile res;
		res.I = lhs * rhs.I;
		res.II = lhs * rhs.II;
		return res;
	}
	Tile operator+(const Tile& lhs, const mat::Rvec<3>& rhs) {
		Tile res;
		res.I = lhs.I + rhs;
		res.II = lhs.II + rhs;
		return res;
	}
}