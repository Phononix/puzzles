#pragma once
#include "Matrix.h"
namespace mat {
	template<int N>
	using Rvec = mat::Matrix<float, N, 1>;

	namespace Detail {
		template<typename T>
		T constexpr sqrtNewtonRaphson(T x, T lo, T hi) {
			const T mid = T{ 0.5 } *(lo + x / lo);
			return (lo == hi) ? lo : sqrtNewtonRaphson(x, mid, lo);
		}
		constexpr int isqrt_helper(int x, int lo, int hi) {
			const int mid = (lo + hi + 1) / 2;
			return (lo == hi) ? lo : ((x / mid < mid) ? isqrt_helper(x, lo, mid - 1) : isqrt_helper(x, mid, hi));
		}
	}
	template<typename T>
	T constexpr sqrt(T x) {
		return Detail::sqrtNewtonRaphson(x, x, T{ 0 });
	}
	constexpr int isqrt(int x) {
		return Detail::isqrt_helper(x, 0, x / 2 + 1);
	}
	constexpr bool isSquare(int n) {
		return n == (isqrt(n)*isqrt(n));
	}
	template<typename T>
	constexpr T abs(T x) {
		return (x > T{ 0 }) ? x : -x;
	}
	template<typename T>
	constexpr T power(T x, int n) {
		return n == 0 ? T{ 1 } : x * power(x, n - 1);
	}
	constexpr long long int factorial(int n) {
		return n <= 1 ? 1 : (n * factorial(n - 1));
	}
	template<typename T>
	constexpr T TaylorSin(T x, int N) {
		return -(2 * (N % 2) - 1) * (power(x, 2 * N + 1) / factorial(2 * N + 1));
	}
	template<typename T>
	constexpr T TaylorCosine(T x, int N) {
		return -(2 * (N % 2) - 1) * (power(x, 2 * N) / factorial(2 * N));
	}
	template<typename T>
	constexpr T sine(T x) {
		T res{};
		for (size_t i = 0; i < 10; i++) {
			res += TaylorSin(x, i);
		}
		return res;
	}
	template<typename T>
	constexpr T cosine(T x) {
		T res{};
		for (size_t i = 0; i < 10; i++) {
			res += TaylorCosine(x, i);
		}
		return res;
	}
	template<typename T>
	constexpr T tangent(T x) {
		return sine(x) / cosine(x);
	}
	template<typename T, int N>
	constexpr T dot(const Matrix<T, N, 1> lhs, const Matrix<T, N, 1> rhs) {
		T res{};
		for (size_t i = 0; i < N; i++) {
			res += lhs.value[i] * rhs.value[i];
		}
		return res;
	}
	template<typename T, int N>
	constexpr T length(const Matrix<T, N, 1>& x) {
		return sqrt(dot(x, x));
	}
	template<typename T, int N>
	constexpr T norm(const Matrix<T, N, 1>& x) {
		return dot(x, x);
	}
	template<typename T, int N>
	constexpr Matrix<T, N, 1> normalize(const Matrix<T, N, 1>& x) {
		return (1 / mat::length(x))*x;
	}
	constexpr float distance(const Matrix<float, 2, 1> line1, Matrix<float, 2, 1> line2, const Matrix<float, 2, 1> point) {
		auto p1 = line1;
		auto p2 = line2;

		auto x = Matrix<float, 2, 1>{ 1.0f, 0.0f };
		auto y = Matrix<float, 2, 1>{ 0.0f, 1.0f };

		auto x0 = dot(point, x);
		auto y0 = dot(point, y);
		auto x1 = dot(p1, x);
		auto y1 = dot(p1, y);
		auto x2 = dot(p2, x);
		auto y2 = dot(p2, y);
		auto rel = p2 - p1;
		return ((y2 - y1)*x0 - (x2 - x1)*y0 + x2 * y1 - y2 * x1) / length(rel);
	}
	constexpr Rvec<3> cross(Rvec<3> A, Rvec<3> B) {
		float Ax = A(1, 1);
		float Ay = A(2, 1);
		float Az = A(3, 1);
		float Bx = B(1, 1);
		float By = B(2, 1);
		float Bz = B(3, 1);
		return Rvec<3>{Ay*Bz- Az*By, Az*Bx-Ax*Bz, Ax*By-Ay*Bx};
	}
	template <typename T>
	constexpr Matrix<T, 3, 3> crossProductMatrix(const Matrix<T, 3, 1> a) {
		std::array<T, 9> tmp{
			0, a(3, 1),  a(2, 1),
			a(3, 1), 0, a(1, 1),
			a(2, 1), a(1, 1), 0
		};
		return AzMat::Matrix<T, 3, 3> {tmp};
	}
	template<typename T>
	constexpr Matrix<T, 4, 4> perspective(const T fovy, const T aspect, const T zNear, const T zFar) {
		T const tanHalfFovy = tangent(fovy / T{ 2 });
		auto i = T{ 1 } / (aspect * tanHalfFovy);
		auto j = T{ 1 } / (tanHalfFovy);
		auto l = -(zFar + zNear) / (zFar - zNear);
		auto m = -(T{ 2 } *zFar * zNear) / (zFar - zNear);
		return Matrix<T, 4, 4> {
			i, T{ 0 }, T{ 0 }, T{ 0 },
				T{ 0 }, j, T{ 0 }, T{ 0 },
				T{ 0 }, T{ 0 }, l, m,
				T{ 0 }, T{ 0 }, -T{ 1 }, T{ 0 }
		};
	}
	template <typename T, int M, int N>
	constexpr Matrix<T, N, M> transpose(const Matrix<T, M, N>& a) {
		std::array<T, N*M> tmp{};
		for (size_t i = 1; i <= M; i++) {
			for (size_t j = 1; j <= N; j++) {
				tmp[(j - 1)*M + (i - 1)] = a(i, j);
			}
		}
		return Matrix<T, N, M>{tmp};
	}
	template<typename T>
	constexpr Matrix<T, 4, 4> translationMatrix(const Matrix<T, 3> r) {
		return Matrix<float, 4, 4> {
			1.f, 0.f, 0.f, r(1),
				0.f, 1.f, 0.f, r(2),
				0.f, 0.f, 1.f, r(3),
				0.f, 0.f, 0.f, 1.f
		};
	}
	//special orthogonal group in R2 is the rotation matrices given by some radian scalar
	constexpr Matrix<float, 2, 2> SO2(const float theta) {
		return {
			cosine(theta), -sine(theta),
			sine(theta), cosine(theta)
		};
	}
	constexpr Matrix<float, 2, 2> SO2(Rvec<2> c) {
		return {
			c(1,1), -c(2,1),
			c(2,1), c(1,1)
		};
	}
	constexpr Matrix<float, 3, 3> rotateX(const float theta) {
		return {
			1.0f, 0.0f, 0.0f,
			0.0f, mat::cosine(theta), -mat::sine(theta),
			0.0f, mat::sine(theta), mat::cosine(theta) };
	}
	constexpr Matrix<float, 3, 3> rotateY(const float theta) {
		return {
			mat::cosine(theta), 0.0f, mat::sine(theta),
			0.0f, 1.0f, 0.0f,
			-mat::sine(theta), 0.0f, mat::cosine(theta) };
	}
	constexpr Matrix<float, 3, 3> rotateZ(const float theta) {
		return {
			mat::cosine(theta), -mat::sine(theta), 0.0f,
			mat::sine(theta), mat::cosine(theta), 0.0f,
			0.0f, 0.0f, 1.0f };
	}
	constexpr Matrix<float, 3, 3> scale(const float x, const float y, const float z) {
		return {
			x, 0.0f, 0.0f,
			0.0f, y, 0.0f,
			0.0f, 0.0f, z };
	}
	struct Dual_Q {
		typedef Matrix<float, 4, 1> Q;
		std::pair<Matrix<float, 4, 1>, Matrix<float, 4, 1>> dq;
		Dual_Q(const std::pair<Matrix<float, 3, 3>, Matrix<float, 3, 1>> transform) : dq{ Matrix<float, 4, 1>{},Matrix<float, 4, 1>{} } {};
	};
	
	constexpr Matrix<float, 3, 3> SO3(const Rvec<3>& u, const float theta) {
		const auto uHat = normalize(u);
		const float ux = uHat(1, 1);
		const float uy = uHat(2, 1);
		const float uz = uHat(3, 1);
		const float cosTheta = mat::cosine(theta);
		const float sinTheta = mat::sine(theta);
		return {
			cosTheta+ux*ux*(1.f-cosTheta), ux*uy*(1.f-cosTheta)-uz*sinTheta, ux*uz*(1.f-cosTheta)+uy*sinTheta,
			uy*ux*(1.f-cosTheta)+uz*sinTheta, cosTheta+uy*uy*(1.f-cosTheta), uy*uz*(1.f-cosTheta)-ux*sinTheta,
			uz*ux*(1.f-cosTheta)-uy*sinTheta, uz*uy*(1.f-cosTheta)+ux*sinTheta, cosTheta+uz*uz*(1.f-cosTheta) };
	}
	float theta(Rvec<3> v1, Rvec<3> v2) {
		auto n = mat::normalize(mat::cross(v1, v2));
		return atan2f(mat::dot(n, mat::cross(v1, v2)), mat::dot(v1, v2));
	}
	constexpr Rvec<3> unitNormal(Rvec<3> a, Rvec<3> b, Rvec<3> c) {
		return mat::normalize(mat::cross(b - a, c - b));
	}
	//Matrix<float, 3, 3> Rotor(const Rvec<3>& initial, const Rvec<3>& target) {
	//	auto n_initial = normalize(initial);
	//	auto n_target = normalize(target);
	//	//auto u = dot(n_initial, n_target) == 1.f ? Rvec<3>{} : mat::cross(n_initial, n_target);
	//	if (dot(cross(n_initial, n_target), cross(n_initial, n_target)) == 0.f) {
	//		if (dot(n_initial, n_target) == 1.f)
	//			return make_Iden<float, 3>();
	//		else
	//			return SO3(cross(n_initial, Rvec<3>{1.0f, 0.0f, 0.0f}), 3.1415926f);
	//	}
	//	return dot(n_initial, n_target) == 1.f ? make_Iden<float, 3>(): mat::SO3(mat::cross(n_initial, n_target), theta(n_initial, n_target));
	//}
	constexpr Matrix<float, 4, 1> HamiltonProduct(const Matrix<float, 4, 1> p, const Matrix<float, 4, 1> q) {
		return Matrix<float, 4, 1>{
			p(1, 1)*q(1, 1) - p(2, 1)*q(2, 1) - p(3, 1)*q(3, 1) - p(4, 1)*q(4, 1),
			p(1, 1)*q(2, 1) + p(2, 1)*q(1, 1) + p(3, 1)*q(4, 1) - p(4, 1)*q(3, 1),
			p(1, 1)*q(3, 1) - p(2, 1)*q(4, 1) + p(3, 1)*q(1, 1) + p(4, 1)*q(2, 1),
			p(1, 1)*q(4, 1) + p(2, 1)*q(3, 1) - p(3, 1)*q(2, 1) + p(4, 1)*q(1, 1)
		};
	}
	Matrix<float, 4, 1> Conjugate(const Matrix<float, 4, 1> p, const Matrix<float, 4, 1> q) {
		auto qinv = Matrix<float, 4, 1>{ q(1,1), -q(2,1), -q(3,1), -q(4,1) };
		return HamiltonProduct(HamiltonProduct(q, p), qinv);
	}
}