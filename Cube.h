#pragma once
#include "Tile.h"
namespace geometry {
	struct hemiCube {
		Tile I; //
		Tile II;
		Tile III;
		hemiCube() :
			I{ geometry::Tile{mat::Rvec<3>{-1.0f, 0.0f, 0.0f}}+mat::Rvec<3>{1.0f, 0.0f, 0.0f} },
			II{ geometry::Tile{ mat::Rvec<3>{0.0f, 0.0f, 1.0f}, mat::Rvec<3>{0.0f, 1.0f, 0.0f} } },
			III{ geometry::Tile{ mat::Rvec<3>{1.0f, 0.0f, 0.0f}, mat::Rvec<3>{0.0f, 0.0f, 1.0f} } } {}
	};
	hemiCube operator*(const mat::Matrix<float, 3, 3>& lhs, const hemiCube& rhs) {
		hemiCube res;
		res.I = lhs * rhs.I;
		res.II = lhs * rhs.II;
		res.III = lhs * rhs.III;
		return res;
	}
	hemiCube operator+(const hemiCube& lhs, const mat::Rvec<3>& rhs) {
		hemiCube res;
		res.I = lhs.I + rhs;
		res.II = lhs.II + rhs;
		res.III = lhs.III + rhs;
		return res;
	}
	struct Cube {
		hemiCube I;
		hemiCube II;
		Cube() :
			I{},
			II{ mat::rotateY(3.1415926f/2.f)*mat::rotateX(3.1415926f)*I + mat::Rvec<3>{1.0f, 1.0f, 1.0f} } {}
	};
	Cube operator*(const mat::Matrix<float, 3, 3>& lhs, const Cube& rhs) {
		Cube res;
		res.I = lhs * rhs.I;
		res.II = lhs * rhs.II;
		return res;
	}
	Cube operator+(const Cube& lhs, const mat::Rvec<3>& rhs) {
		Cube res;
		res.I = lhs.I + rhs;
		res.II = lhs.II + rhs;
		return res;
	}
}
