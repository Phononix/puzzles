#pragma once
#include "ShaderProgram.h"
#include "Geometry.h"
#include <algorithm>
#include <map>
#include <string>
namespace geometry{
	template<int M, int N>
	class TerrainGen {
	public:
		std::array<Triangle, 2*M*N> t;
		TerrainGen() : t{} {
			std::array<Rvec<2>, M*N> grid;
			float cellWidth = 2.f / (float)M;
			float cellHeight = 2.f / (float)N;
			for (size_t i = 0; i < M; i++)	{
				for (size_t j = 0; j < N; j++)	{
					grid[i*N+j] = Rvec<2>{ i*cellWidth-1.f, j*cellHeight-1.f };
				}
			}
			for (size_t i = 0; i < grid.size(); i++){
				t[2 * i] = Triangle{ 
					Rvec<3>{raise(grid[i])},
					Rvec<3>{raise(grid[i] + Rvec<2>{0.f, cellHeight})},
					Rvec<3>{raise(grid[i] + Rvec<2>{cellWidth, 0.0f})}};
				t[2 * i + 1] = Triangle{ 
					Rvec<3>{raise(grid[i] + Rvec<2>{0.f, cellHeight})},
					Rvec<3>{raise(grid[i] + Rvec<2>{cellWidth, cellHeight})},
					Rvec<3>{raise(grid[i] + Rvec<2>{cellWidth, 0.0f})} };
			}
		}
		Rvec<3> raise(Rvec<2> xz) {
			auto h = [](Rvec<2> xz) {
				return -1.f*(mat::length(xz - Rvec<2>{})) + 0.f;
			};
			return Rvec<3>{xz(1, 1), h(xz), xz(2, 1)};
		}
	};
	template<int M, int N>
	std::array<FaceNormal, 2 * M*N> getFaceNormals(const TerrainGen<M, N>& terrain) {
		std::array<FaceNormal, 2 * M*N> res;
		for (size_t i = 0; i < terrain.t.size(); i++) {
				res[i] = FaceNormal(terrain.t[i]);
		}
		return res;
	}
}
template<typename... T>
class ThreeSpaceVAO {
public:
	static std::tuple<T...> y;

	GLuint id;
	ThreeSpaceVAO() {
		glGenVertexArrays(1, &id);
		glBindVertexArray(id);
		glEnableVertexAttribArray(0);
		glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexAttribArray(2);
		glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);
	}
	void bind(const std::array<GLuint, 3>& vbo) const {
		glBindVertexArray(id);
		glBindVertexBuffer(0, vbo[0], 0, sizeof(Rvec<3>));
		glBindVertexBuffer(1, vbo[1], 0, sizeof(Rvec<3>));
		glBindVertexBuffer(2, vbo[2], 0, sizeof(Rvec<3>));
	}
	~ThreeSpaceVAO() {
		glDeleteVertexArrays(1, &id);
	}
};
struct ShapeTag {
	std::string shapeName;
	mat::Matrix<float, 3, 3> M;
	mat::Rvec<3> affine;
};
class ThreeSpace {
	const ThreeSpaceProgram glProgram;
	ThreeSpaceVAO<> vertexArrayObject;
	std::map<std::string, std::pair<std::array<GLuint, 3>, unsigned int>> shapeKeys;
public:
	Rvec<3> cameraPosition;
	mat::Matrix<float, 3, 3> cameraRotation;
	Rvec<3> lightSource;
	ThreeSpace() :
		glProgram{ mat::perspective(3.14f / 4, 1.0f, .3f, 1000.f) }, 
		vertexArrayObject{  }, shapeKeys{} {}
	template<typename T1>
	void assign(const T1& buffer, std::string shapeName = "shape") {
		const unsigned int vertexCount = sizeof(T1) / sizeof(mat::Rvec<3>);
		auto normals = geometry::getFaceNormals(buffer);
		std::array<Rvec<3>, vertexCount> tRGBA;
		std::fill(tRGBA.begin(), tRGBA.end(), Rvec<3>{ 1.0f, 1.0f, 1.0f });
		std::array<GLuint, 3> vbos;
		glCreateBuffers(3, &vbos[0]);
		glNamedBufferData(vbos[0], vertexCount * sizeof(Rvec<3>), &buffer, GL_STATIC_DRAW);
		glNamedBufferData(vbos[1], vertexCount * sizeof(Rvec<3>), &tRGBA, GL_STATIC_DRAW);
		glNamedBufferData(vbos[2], vertexCount * sizeof(Rvec<3>), &normals, GL_STATIC_DRAW);
		shapeKeys[shapeName] = { vbos, vertexCount };
	}
	void draw(std::string s, mat::Matrix<float, 3, 3> transform, Rvec<3> location) const {
		auto camRotate = mat::transpose(cameraRotation);cccccc
		glProgram.setUniforms(camRotate*transform, camRotate*(location -cameraPosition), camRotate*(Rvec<3>{ 5.0f, 10.0f, 0.0f } - cameraPosition));
		vertexArrayObject.bind(shapeKeys.at(s).first);
		glDrawArrays(GL_TRIANGLES, 0, shapeKeys.at(s).second);
		glBindVertexArray(0);
	}
	void draw(const ShapeTag& t) const {
		auto camRotate = mat::transpose(cameraRotation);
		glProgram.setUniforms(camRotate*t.M, camRotate*(t.affine - cameraPosition), camRotate*(Rvec<3>{ 5.0f, 10.0f, 0.0f } -cameraPosition));
		vertexArrayObject.bind(shapeKeys.at(t.shapeName).first);
		glDrawArrays(GL_TRIANGLES, 0, shapeKeys.at(t.shapeName).second);
		glBindVertexArray(0);
	}
	void lookAt(Rvec<3> spot) {
		auto xplane = mat::dot(Rvec<3>{0.0f, 0.0f, -1.0f}, spot - cameraPosition);
		auto yplane = mat::dot(Rvec<3>{-1.0f, 0.0f, 0.0f}, spot - cameraPosition);
		auto yplane2 = mat::dot(Rvec<3>{0.0f, 1.0f, 0.0f}, spot - cameraPosition);
		auto xplane2 = mat::dot(mat::rotateY(atan2f(yplane, xplane))*Rvec<3>{0.0f, 0.0f, -1.0f}, spot - cameraPosition);
		cameraRotation =  mat::rotateY(atan2f(yplane, xplane))*mat::rotateX(atan2f(yplane2, xplane2));
	}
	void firstPersonControl(mat::Rvec<2>& c) {
		cameraRotation = mat::rotateY(-c(1, 1))*mat::rotateX(c(2, 1));
	}
};
void assignBasicGeometry(ThreeSpace& euclidean) {
	euclidean.assign(geometry::Cube{}, "cube");
	euclidean.assign(geometry::Cube{}+Rvec<3>{-.5f, -.5f, -.5f }, "cubeCentered");
	euclidean.assign(geometry::Octahedron{}, "octahedron");
	euclidean.assign(geometry::Octahedron{}+Rvec<3>{-.5f, -.5f, -.5f }, "octahedronCentered");
	euclidean.assign(geometry::Tetrahedron{}, "tetrahedron");
	euclidean.assign(geometry::StellaOctangula{}, "stella Octangula");
	euclidean.assign(geometry::StellaOctangula{}+Rvec<3>{-.5f, -.5f, -.5f }, "stella Octangula Centered");

	euclidean.assign(mat::rotateX(-3.14159f / 2.f)*geometry::Tile{}+Rvec<3>{-.5f, 0.0f, .5f}, "square");
	euclidean.assign(geometry::Prism<4>{}, "prism");
	euclidean.assign(geometry::Pyramid<4>{}, "pyramid");
	euclidean.assign(geometry::Arrow{}, "arrow");
	euclidean.assign(geometry::StandardBasis{}, "basis");
}