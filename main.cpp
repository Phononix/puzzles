#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#include <iostream>
#include <fstream>

#include <string>
#include <glad\glad.h>
#include <GLFW/glfw3.h>

#include "Window.h"
#include <math.h>
#include <map>
#include "shader.h"
#include <functional>
#include <tuple>
#include "Cube.h"
namespace geometry {
	template<int M, int N>
	class TerrainGen {
		float W;
		float H;
		std::function<float(mat::Rvec<2>)> F;
	public:
		TerrainGen(std::function<float(mat::Rvec<2>)> f, float w, float h) : F{ f }, W{ w }, H{ h } {
		}
		mat::Rvec<3> raise(mat::Rvec<2> xz) {
			return mat::Rvec<3>{xz(1, 1), F(xz), xz(2, 1)};
		}
		std::array<mat::Rvec<3>, 3> cellBounds(mat::Rvec<2> xz) {
			auto xy = xz + .5f*mat::Rvec<2>{ M*W, N*H };
			auto cellX = floorf(xy(1, 1) / W);
			auto cellY = floorf(xy(2, 1) / H);
			auto Rb = mat::Rvec<2>{ W*cellX,H*cellY } -.5f*mat::Rvec<2>{M*W, N*H};
			auto Rbx = Rb + mat::Rvec<2>{W, 0.0f};
			auto Rby = Rb + mat::Rvec<2>{0.f, H};
			auto Rbxy = Rb + mat::Rvec<2>{W, H};
			auto Rr = xz - Rb;
			return (W*Rr(2, 1) + H * Rr(1, 1) < (W * H)) ? std::array<mat::Rvec<3>, 3>{raise(Rbx), raise(Rb), raise(Rby)} : std::array<mat::Rvec<3>, 3>{raise(Rbxy), raise(Rbx), raise(Rby)};
		}
		mat::Rvec<3> cellNormal(mat::Rvec<2> xz) {
			auto bounds = cellBounds(xz);
			return mat::unitNormal(bounds[0], bounds[1], bounds[2]);
		}
		mat::Rvec<3> xzProject(mat::Rvec<2> xz) {
			auto R0 = cellBounds(xz)[0];
			auto n = cellNormal(xz);
			auto A = n(1, 1);
			auto B = n(2, 1);
			auto C = n(3, 1);
			auto D = mat::dot(n, R0);
			auto x = xz(1, 1);
			auto z = xz(2, 1);
			auto y = (D - (A*x + C * z)) / B;
			return mat::Rvec<3>{x, y, z};
		}
		std::pair<std::array<GLuint, 3>, unsigned int> vbo() {
			std::array<Triangle, 2 * M*N> vertBuffer;

			std::array<mat::Rvec<2>, M*N> grid;
			for (size_t i = 0; i < M; i++) {
				for (size_t j = 0; j < N; j++) {
					grid[i*N + j] = mat::Rvec<2>{ W*(i - (M / 2.f)), H*(j - (N / 2.f)) };
				}
			}
			for (size_t i = 0; i < grid.size(); i++) {
				vertBuffer[2 * i] = Triangle{
					mat::Rvec<3>{raise(grid[i])},
					mat::Rvec<3>{raise(grid[i] + mat::Rvec<2>{0.f, H})},
					mat::Rvec<3>{raise(grid[i] + mat::Rvec<2>{W, 0.0f})} };
				vertBuffer[2 * i + 1] = Triangle{
					mat::Rvec<3>{raise(grid[i] + mat::Rvec<2>{0.f, H})},
					mat::Rvec<3>{raise(grid[i] + mat::Rvec<2>{W, H})},
					mat::Rvec<3>{raise(grid[i] + mat::Rvec<2>{W, 0.0f})} };
			}
			//std::array<geometry::FaceNormal, 2 * M*N > Norms;
			//typedef std::tuple<mat::Rvec<3>, mat::Rvec<3>, mat::Rvec<3>> faceN;

			std::array<geometry::faceN, 2 * M*N> Norms;
			std::transform(vertBuffer.begin(), vertBuffer.end(), Norms.begin(), [](const geometry::Triangle& t) {
				return geometry::faceN{ geometry::normal(t), geometry::normal(t), geometry::normal(t) }; });

			//std::transform(vertBuffer.begin(), vertBuffer.end(), Norms.begin(), [](const geometry::Triangle& t) {return geometry::getNormals(t); });
			std::array<mat::Rvec<3>, 6 * M*N> tRGBA;
			std::fill(tRGBA.begin(), tRGBA.end(), mat::Rvec<3>{ 1.0f, 1.0f, 1.0f });
			std::array<GLuint, 3> vbos;
			glCreateBuffers(3, &vbos[0]);
			glNamedBufferData(vbos[0], 6 * M*N * sizeof(mat::Rvec<3>), &vertBuffer, GL_STATIC_DRAW);
			glNamedBufferData(vbos[1], 6 * M*N * sizeof(mat::Rvec<3>), &tRGBA, GL_STATIC_DRAW);
			glNamedBufferData(vbos[2], 6 * M*N * sizeof(mat::Rvec<3>), &Norms, GL_STATIC_DRAW);
			return { vbos, 6 * M*N };
		}
	};
}

namespace geometry {
	struct RubikCube {
		std::array<mat::Rvec<3>, 26> elems;
		std::vector<std::pair<mat::Matrix<float, 3, 3>, mat::Rvec<3>>> positions;

		float theta;
		RubikCube() {
			theta = 0.f;
			for (size_t i = 0; i < 3; i++) {
				for (size_t j = 0; j < 3; j++) {
					for (size_t k = 0; k < 3; k++) {
						positions.push_back(
							{ mat::make_Iden<float, 3>(),mat::Rvec<3>{i*1.f, j*1.f, k*1.f}-mat::Rvec<3>{1.f, 1.f, 1.f} });
					}
				}
			}
		}
		std::pair<std::array<GLuint, 3>, unsigned int> getVBO() {
			auto buffer = geometry::Cube{}+(-1.f)*mat::Rvec<3>{.5f, .5f, .5f};
			std::array<Triangle, 12> vertBuffer;
			std::memcpy(&vertBuffer[0], &buffer, sizeof(buffer));
			std::array<faceN, 12> Norms;
			std::transform(vertBuffer.begin(), vertBuffer.end(), Norms.begin(), [](const geometry::Triangle& t) {
				return faceN{ geometry::normal(t), geometry::normal(t), geometry::normal(t) }; });
			std::array<mat::Rvec<3>, 36> tRGBA;
			std::fill(tRGBA.begin(), tRGBA.begin()+6, mat::Rvec<3>{ 0.0f, 0.0f, 1.0f });
			std::fill(tRGBA.begin() + 6, tRGBA.begin()+12, mat::Rvec<3>{ 1.0f, 1.0f, 0.0f });
			std::fill(tRGBA.begin() + 12, tRGBA.begin()+18, mat::Rvec<3>{ 1.0f, 0.0f, 1.0f });
			std::fill(tRGBA.begin() + 18, tRGBA.begin()+24, mat::Rvec<3>{ 0.0f, 1.0f, 1.0f });
			std::fill(tRGBA.begin() + 24, tRGBA.begin()+30, mat::Rvec<3>{ 1.0f, 0.0f, 0.0f });
			std::fill(tRGBA.begin() + 30, tRGBA.end(), mat::Rvec<3>{ 0.0f, 1.0f, 0.0f });
			std::array<GLuint, 3> vbos;
			glCreateBuffers(3, &vbos[0]);
			glNamedBufferData(vbos[0], 36 * sizeof(mat::Rvec<3>), &vertBuffer, GL_STATIC_DRAW);
			glNamedBufferData(vbos[1], 36 * sizeof(mat::Rvec<3>), &tRGBA, GL_STATIC_DRAW);
			glNamedBufferData(vbos[2], 36 * sizeof(mat::Rvec<3>), &Norms, GL_STATIC_DRAW);
			return { vbos, 36 };
		}
		void nit(const float dt, ThreeSpace& euclidean) {
			if (mat::norm(win::Plane) != 0.f) { theta += 2.5f*dt; }
			if (theta > 3.14159f / 2.f) {
				theta = 3.14159f / 2.f;
				for (auto& r : positions) {
					if (mat::dot(r.second, win::Plane) > .5f) {
						r = { mat::SO3(win::Plane,theta)*r.first, mat::SO3(win::Plane,theta)*r.second };
					}
				}
				theta = 0.f;
				win::Plane = mat::Rvec<3>{};
			}
			for (const auto& r : positions) {
				euclidean.draw("cube", (mat::dot(r.second, win::Plane) > .5f) ?
					std::pair<mat::Matrix<float, 3, 3>, mat::Rvec<3>>{ 
					mat::SO3(win::Plane, theta)*r.first, mat::SO3(win::Plane, theta)*r.second } : r);
			}
		}
	};
}

int main(void) {
	win::Window w{};
	ThreeSpace euclidean{};
	w.firstPerson();

	euclidean.cameraPosition = mat::Rvec<3>{ 3.0f, 4.0f, 7.f };
	euclidean.cameraRotation = mat::make_Iden<float, 3>();

	mat::Matrix<float, 3, 3> rRotate = mat::make_Iden<float, 3>();
	mat::Rvec<2> R{};
	geometry::RubikCube puzzle{};
	euclidean.assignVbo(puzzle.getVBO(), "cube");
	while (!glfwWindowShouldClose(win::pWin)) {
		R += w.tFrame * win::arrowKeys;
		euclidean.firstPersonControl(win::cursor);
		euclidean.freeNavigate( w.tFrame*win::WASD );
		w.swapAndPoll();
		puzzle.nit(w.tFrame, euclidean);
	}
	return 0;
}