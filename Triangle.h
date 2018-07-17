#pragma once
#include "kinematics.h"
#include <vector>
#include <boost/graph/adjacency_list.hpp>
#include <tuple>
namespace geometry {

	struct Triangle {
		mat::Rvec<3> A;
		mat::Rvec<3> B;
		mat::Rvec<3> C;
	};
	mat::Rvec<3> normal(const Triangle& t) {
		return mat::normalize(mat::cross(t.B - t.A, t.C - t.B));
	}
	Triangle operator*(const mat::Matrix<float, 3, 3>& lhs, const Triangle& rhs) {
		Triangle res;
		res.A = lhs * rhs.A;
		res.B = lhs * rhs.B;
		res.C = lhs * rhs.C;
		return res;
	}
	Triangle operator+(const Triangle& lhs, const mat::Rvec<3>& rhs) {
		Triangle res;
		res.A = lhs.A + rhs;
		res.B = lhs.B + rhs;
		res.C = lhs.C + rhs;
		return res;
	}
	struct FaceNormal {
		mat::Rvec<3> A;
		mat::Rvec<3> B;
		mat::Rvec<3> C;
		FaceNormal() = default;
		FaceNormal(const geometry::Triangle& tri) : A{ geometry::normal(tri) }, B{ A }, C{ B } {};
		FaceNormal(mat::Rvec<3> a, mat::Rvec<3> b, mat::Rvec<3> c) : A{ a }, B{ b }, C{ c } {}
	};
	typedef std::tuple<mat::Rvec<3>, mat::Rvec<3>, mat::Rvec<3>> faceN;
}
