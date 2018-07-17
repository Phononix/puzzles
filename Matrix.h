#pragma once
#include <type_traits>
#include <functional>
#include <numeric>
#include <limits>   
#include <array>
#include <iostream>
namespace mat {
	template<bool B, typename T = void>
	using Enable_if = typename std::enable_if<B, T>::type;
	namespace detail {
		template<int... Ns>
		constexpr auto make_array()
			->std::array<int, sizeof...(Ns)> {
			return{ { (Ns)... } };
		}
	}
	template<typename ...Args>
	constexpr int product(Args&&... args) {
		return (args * ...);
	}
	template<typename T, int... Exts>
	struct Matrix {
		using value_type = T;
		static constexpr int dimension = sizeof...(Exts);
		static constexpr auto shape = detail::make_array<Exts...>();
		static constexpr int size = product(Exts...);//(Exts * ...);
													 //template <class... Args,
													 //	class Enable = std::enable_if_t<((sizeof...(Args) == size)&&(... && std::is_convertible_v<Args, value_type>))>>
													 // construction by elements is enabled iff the number of arguments equals size of matrix set.
		template<typename ...Args, typename = Enable_if<(sizeof...(Args) == size)>>
		constexpr Matrix(Args&&... args) : value{ (args)... } {}

		//template<typename ...Args, typename = Enable_if<(... && std::is_same<float, Args>::value)> >
		//constexpr Matrix( MatrixArgs&&... args) : value{} {}

		constexpr Matrix(const std::array<value_type, size>& elementContainer) : value{ elementContainer } {}
		//template<int M, Matrix<value_type, 1>... Args, typename = Enable_if<(M == shape[0])&&(sizeof...(Args) == shape[1])> >
		//constexpr Matrix(const Args&&... args) : value{} {}
		//previous constructor deactivates default constructor; reactivate.
		constexpr Matrix() = default;
		// testing
		template<size_t sz = shape[1], typename = Enable_if<(dimension == 2)>>
		constexpr Matrix<value_type, sz> row(const size_t n) const {
			std::array<T, sz> tmp{};
			for (size_t i = 0; i < tmp.size(); i++) {
				tmp[i] = value[n*shape[1] + i];
			}
			return Matrix<value_type, sz> {tmp};
		}
		template<int sz = shape[0], typename = Enable_if<(dimension == 2)>>
		constexpr Matrix<value_type, sz> col(const size_t n) const {
			std::array<T, sz> tmp{};
			for (size_t i = 0; i < tmp.size(); i++) {
				tmp[i] = value[i*shape[1] + n];
			}
			return Matrix<value_type, sz> {tmp};
		}
		Matrix& operator+=(const Matrix& rhs) {
			std::transform(value.begin(), value.end(), rhs.value.begin(), value.begin(), std::plus<value_type>());
			return *this;
		}
		Matrix& operator-=(const Matrix& rhs) {
			std::transform(value.begin(), value.end(), rhs.value.begin(), value.begin(), std::minus<value_type>());
			return *this;
		}
		//template<typename = Enable_if<(dimension == 1)>>
		//value_type operator()(int i) {
		//	return value[i-1];
		//}
		template<typename = Enable_if<(dimension == 1)>>
		constexpr value_type operator()(const int i) const {
			return value[i - 1];
		}
		//template<typename = Enable_if<(dimension == 2)>>
		//value_type operator()(int i, int j) {
		//	return value[(i - 1)*shape[1] + (j - 1)];
		//}
		template<typename = Enable_if<(dimension == 2)>>
		constexpr value_type operator()(int i, int j) const {
			return value[(i - 1)*shape[1] + (j - 1)];
		}
		//allow unary multiplication operator iff resulting matrix will be of same shape as current, meaning rhs must be square.
		Matrix& operator*=(const value_type rhs) {
			std::transform(value.begin(), value.end(), value.begin(),
				std::bind(std::multiplies<value_type>(), std::placeholders::_1, rhs));
			return *this;
		}
		constexpr Matrix operator-() {
			return Matrix{}-*this;
		}
		std::array<value_type, size> value;
	};
	template<typename T, int N>
	constexpr T dot(const Matrix<T, N> lhs, const Matrix<T, N> rhs) {
		T res{};
		for (size_t i = 0; i < N; i++) {
			res += lhs.value[i] * rhs.value[i];
		}
		return res;
	}
	template <typename T, int R, int C>
	constexpr Matrix<T, R, C> operator*(const T lhs, const Matrix<T, R, C>& rhs) {
		std::array<T, R*C> tmp{};
		for (size_t i = 1; i <= R; i++) // Rows
		{
			for (size_t j = 1; j <= C; j++) // Columns
			{
				tmp[(i - 1) * C + (j - 1)] = lhs * rhs(i, j);
			}
		}
		return Matrix<T, R, C>{tmp};
	}
	template < typename T, int N>
	constexpr Matrix<T, N> operator*(const T lhs, const Matrix<T, N>& rhs) {
		std::array<T, N> tmp{};
		for (size_t i = 1; i <= N; i++) // Rows
		{
			tmp[i - 1] = lhs * rhs(i);
		}
		return Matrix<T, N>{tmp};
	}
	//This is pretty clumsy
	template<typename T, int R1, int C1, int R2, int C2, typename = Enable_if<(C1 == R2)>>
	constexpr Matrix<T, R1, C2> operator*(const Matrix<T, R1, C1>& lhs, const Matrix<T, R2, C2>& rhs) {
		std::array<T, R1*C2> tmp{};
		for (size_t i = 1; i <= R1; i++)
		{
			Matrix<T, C1> lhRow = {};
			for (size_t j = 1; j <= C2; j++)
			{
				T inner_product{};
				for (size_t k = 1; k <= R2; k++) {
					inner_product += lhs(i, k) * rhs(k, j);
				}
				tmp[(i - 1) * C2 + (j - 1)] = inner_product;
			}
		}
		return Matrix<T, R1, C2>{tmp};
	}
	//template<typename T, int R1, int C1, int R2, int C2, typename = Enable_if<(C1 == R2)>>
	//constexpr Matrix<T, R1, C2> operator*(const Matrix<T, R1, C1>& lhs, const Matrix<T, R2, C2>& rhs) {
	template<typename T, int R>
	constexpr Matrix<T, R> operator+(const Matrix<T, R>& lhs, const Matrix<T, R>& rhs) {
		std::array<T, R> tmp{};
		for (size_t i = 1; i <= R; i++)
		{
			tmp[i - 1] = lhs(i) + rhs(i);
		}
		return Matrix<T, R>{tmp};
	}
	template<typename T, int R, int C>
	constexpr Matrix<T, R, C> operator+(const Matrix<T, R, C>& lhs, const Matrix<T, R, C>& rhs) {
		std::array<T, R*C> tmp{};
		for (size_t i = 1; i <= R; i++) // Rows
		{
			for (size_t j = 1; j <= C; j++) // Columns
			{
				tmp[(i - 1) * C + (j - 1)] = lhs(i, j) + rhs(i, j);
			}
		}
		return Matrix<T, R, C>{tmp};
	}
	template<typename T, int R>
	constexpr Matrix<T, R> operator-(const Matrix<T, R>& lhs, const Matrix<T, R>& rhs) {
		std::array<T, R> tmp{};
		for (size_t i = 1; i <= R; i++)
		{
			tmp[i - 1] = lhs(i) - rhs(i);
		}
		return Matrix<T, R>{tmp};
	}
	template<typename T, int R, int C>
	constexpr Matrix<T, R, C> operator-(const Matrix<T, R, C>& lhs, const Matrix<T, R, C>& rhs) {
		std::array<T, R*C> tmp{};
		for (size_t i = 1; i <= R; i++) // Rows
		{
			for (size_t j = 1; j <= C; j++) // Columns
			{
				tmp[(i - 1) * C + (j - 1)] = lhs(i, j) - rhs(i, j);
			}
		}
		return Matrix<T, R, C>{tmp};
	}
	template<typename T, int R, int C>
	constexpr bool operator==(const Matrix<T, R, C>& lhs, const Matrix<T, R, C>& rhs) {
		for (size_t i = 1; i <= R; i++) {
			for (size_t j = 1; j <= C; j++) {
				if (lhs(i, j) != rhs(i, j)) {
					return false;
				};
			}
		}
		return true;
	}
	template <typename T, int N>
	constexpr Matrix<T, N, N>  make_Iden() {
		Matrix<T, N, N> res{};
		for (size_t i = 0; i < res.value.size(); i++) {
			res.value[i] = (i % (N + 1) == 0) ? T{ 1 } : T{ 0 };
		}
		return res;
	}
	template<typename T, int R, int C>
	void print(const Matrix<T, R, C> val) {
		std::cout << "{\n";
		std::ostream_iterator<T> out_it(std::cout, ", ");
		for (size_t i = 0; i < R; i++)
		{
			std::copy(val.value.begin() + i * C, val.value.begin() + (i*C) + C, out_it);
			std::cout << "\n";
		}
		std::cout << "}\n";
	}
	//for vectors;
	template<typename T, int R>
	void print(const Matrix<T, R> val) {
		std::cout << "{ ";
		std::ostream_iterator<T> out_it(std::cout, ", ");
		std::copy(val.value.begin(), val.value.end(), out_it);
		std::cout << "}\n";
	}
	template<int N>
	using Rvec = Matrix<float, N, 1>;
}
