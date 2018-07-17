#pragma once
#include "Prism.h"
#include "Pyramid.h"
#include "Octahedron.h"
#include "Cube.h"
namespace geometry {
	mat::Matrix<float, 3, 3> connect(mat::Rvec<3> target) {
		if (target == mat::Rvec<3>{0.f, 1.f, 0.f}) {
			return mat::make_Iden<float, 3>();
		}
		if (target == mat::Rvec<3>{0.f, -1.0f, 0.0f}) {
			return mat::rotateX(3.1415926f);
		}
		return mat::SO3(mat::cross(mat::Rvec<3>{0.0f, 1.0f, 0.0f}, target), theta(mat::Rvec<3>{0.0f, 1.0f, 0.0f}, target))*mat::scale(1.0f, mat::length(target), 1.0f);
	}
	struct Arrow {
		Pyramid<8> head;
		Prism<8> shaft;
		Arrow() :
			head{ (mat::scale(.125f, .5f, .125f)*Pyramid<8>{}+mat::Rvec<3>{0.0f, .5f, 0.0f}) },
			shaft{ (mat::scale(.05f, .5f, .05f)*Prism<8>{}) } {}
	};
	Arrow operator*(const mat::Matrix<float, 3, 3>& lhs, const Arrow& rhs) {
		Arrow res;
		res.head = lhs * rhs.head;
		res.shaft = lhs * rhs.shaft;
		return res;
	}
	Arrow operator+(const Arrow& lhs, const mat::Rvec<3>& rhs) {
		Arrow res;
		res.head = lhs.head + rhs;
		res.shaft = lhs.shaft + rhs;
		return res;
	}
	struct StandardBasis {
		Arrow x;
		Arrow y;
		Arrow z;
		Cube O;
		StandardBasis() :
			x{ connect(mat::Rvec<3>{1.f, 0.f, 0.f})*Arrow {} },
			y{ Arrow{} },
			z{ connect(mat::Rvec<3>{0.f, 0.f, 1.f})*Arrow {} },
			O{ .1f*mat::make_Iden<float, 3>()*(Cube{}+mat::Rvec<3>{-.5f, -.5f, -.5f}) } {}
	};
}