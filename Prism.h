#pragma once
#include "Tile.h"
namespace geometry {
	template<int N>
	mat::Rvec<2> rootsOfUnity(int i, mat::Rvec<2> z = { 1.0f, 0.0f }) {
		float Theta = (2.f*3.14159f) / (float)N;
		return i == 0 ? z : rootsOfUnity<N>(i - 1, mat::SO2(Theta)*z);
	}
	template<unsigned int N>
	class Prism{
	public:
		std::array<Tile, N> faces;
		Prism() : faces{} {
			faces[0] = Tile{ mat::Rvec<3>{2.f*mat::sine((3.1415926f) / (float)N),0.0f, 0.0f} } +mat::rotateY(-(3.1415926f / 2.f) - (3.1415926f / (float)N))*mat::Rvec<3>{1.0f, 0.0f, 0.0f};
			for (size_t i = 1; i < N; i++)			{
				faces[i] = mat::rotateY(3.1415926f*(2.f / (float)N))*faces[i-1];
			}
		}
	};
	template<unsigned int N>
	Prism<N> operator*(const mat::Matrix<float, 3, 3>& lhs, const Prism<N>& rhs) {
		Prism<N> res;
		for (size_t i = 0; i < N; i++)
		{
			res.faces[i] = lhs * rhs.faces[i];
		}
		return res;
	}
	template<unsigned int N>
	Prism<N> operator+(const Prism<N>& lhs, const mat::Rvec<3>& rhs) {
		Prism<N> res;
		for (size_t i = 0; i < N; i++)
		{
			res.faces[i] = lhs.faces[i] + rhs;
		}
		return res;
	}
	template<unsigned int N>
	std::array<FaceNormal, 2*N> getFaceNormals(const Prism<N>& p) {
		std::array<FaceNormal, 2 * N> res{};
		for (size_t i = 0; i < N; i++)
		{
			res[2 * i] = FaceNormal{ p.faces[i].I };
			res[(2 * i)+1] = FaceNormal{ p.faces[i].II };
		}
		return res;
	}
}