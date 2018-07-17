#pragma once
//#include "glGeometry.h"
#include "shader.h"
mat::Matrix<float, 3, 3> connect(mat::Rvec<3> target) {
	return mat::SO3(mat::cross(mat::Rvec<3>{0.0f, 1.0f, 0.0f}, target), theta(mat::Rvec<3>{0.0f, 1.0f, 0.0f}, target))*mat::scale(1.0f, mat::length(target), 1.0f);
}
struct Legs {
	mat::Rvec<2> phaseLeft;
	mat::Rvec<2> phaseRight;
	ThreeSpace& s;
	mat::Rvec<3> R;
	const float& dt;
	Legs(ThreeSpace& s, const float& tFrame, mat::Rvec<3> worldPosition = mat::Rvec<3>{ .0f, .0f, 0.0f }) :
		phaseLeft{ mat::Rvec<2>{0.f, 1.f}},
		phaseRight{ mat::Rvec<2>{0.f, -1.f}},
		s{ s }, R{ worldPosition }, dt{ tFrame } {	}
	void command(mat::Rvec<3> direction) {
		phaseLeft = swing(phaseLeft);
		phaseRight = swing(phaseRight);
		R += dt*direction;
		s.draw("cubeCentered", .25f*mat::rotateY(-3.14159f/2.f)*mat::make_Iden<float, 3>(), R);
		s.draw("arrow", connect(direction), R);
		s.draw("arrow", mat::rotateY(-3.14159f / 2.f)*mat::rotateZ(3.1415926535f + phaseLeft(1, 1)), R + mat::Rvec<3>{-.1f,0.f,0.f});
		s.draw("arrow", mat::rotateY(-3.14159f / 2.f)*mat::rotateZ(3.1415926535f + phaseRight(1, 1)), R + mat::Rvec<3>{.1f, 0.f, 0.f});
	}
	mat::Rvec<2> swing(const mat::Rvec<2>& phase) {
		float theta = phase(1, 1);
		float d_theta_dt = phase(2, 1);
		float l = 1.0f;
		float mass = 1.0f;
		float g = -9.81f;
		auto F_eff = mass * g*mat::sine(theta);
		d_theta_dt += (F_eff*dt) / l;
		theta += d_theta_dt * dt;
		return { theta, d_theta_dt };
	}
};