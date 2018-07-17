#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "kinematics.h"
#include "Triangle.h"
class ThreeSpaceProgram {
	std::map<std::string, GLint> uniformLocations;
	GLuint programID;
	mat::Matrix<float, 4, 4> Perspective;
public:
	mat::Rvec<3> lightPosition;
	ThreeSpaceProgram(mat::Matrix<float, 4, 4> perspective) :
		Perspective{ perspective } {
		programID = glCreateProgram();
		loadVertexShader("shaders/vSpace.txt");
		loadFragmentShader("shaders/colorForward.txt");
		glLinkProgram(programID);
		uniformLocations["Pr"] = glGetUniformLocation(programID, "Pr");
		uniformLocations["M"] = glGetUniformLocation(programID, "M");
		uniformLocations["b"] = glGetUniformLocation(programID, "b");
		uniformLocations["lightR"] = glGetUniformLocation(programID, "lightR");
		glUseProgram(programID);

	}
	void setUniforms(mat::Matrix<float, 3, 3> M, mat::Rvec<3> offset, mat::Rvec<3> light) const {
		glUseProgram(programID);
		glUniformMatrix3fv(uniformLocations.at("M"), 1, GL_TRUE, (GLfloat*)&M);
		glUniform3fv(uniformLocations.at("b"), 1, (GLfloat*)&offset);
		glUniform3fv(uniformLocations.at("lightR"), 1, (GLfloat*)&light);
		glUniformMatrix4fv(uniformLocations.at("Pr"), 1, GL_TRUE, (GLfloat*)&Perspective);
	}
	~ThreeSpaceProgram() {
		glDeleteProgram(programID);
	}
private:
	void loadVertexShader(const std::string filename) {
		std::ifstream shaderStream;
		std::string vSpaceString;
		shaderStream.open(filename);
		while (!shaderStream.eof()) {
			std::string shaderline;
			std::getline(shaderStream, shaderline);
			vSpaceString += shaderline + "\n";
		}
		shaderStream.close();
		const char* vertptr = &vSpaceString[0];
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertptr, NULL);
		glCompileShader(vertexShader);
		glAttachShader(programID, vertexShader);
		glDeleteShader(vertexShader);
	}
	void loadFragmentShader(const std::string filename) {
		std::ifstream shaderStream;
		std::string fSpaceString;
		shaderStream.open(filename);
		while (!shaderStream.eof()) {
			std::string shaderline;
			std::getline(shaderStream, shaderline);
			fSpaceString += shaderline + "\n";
		}
		shaderStream.close();
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fragptr = &fSpaceString[0];
		glShaderSource(fragmentShader, 1, &fragptr, NULL);
		glCompileShader(fragmentShader);
		glAttachShader(programID, fragmentShader);
		glDeleteShader(fragmentShader);
	}
};
class ThreeSpaceVAO {
public:
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
		glBindVertexBuffer(0, vbo[0], 0, sizeof(mat::Rvec<3>));
		glBindVertexBuffer(1, vbo[1], 0, sizeof(mat::Rvec<3>));
		glBindVertexBuffer(2, vbo[2], 0, sizeof(mat::Rvec<3>));
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
	ThreeSpaceVAO vertexArrayObject;
	std::map<std::string, std::pair<std::array<GLuint, 3>, unsigned int>> shapeKeys;
public:
	mat::Rvec<3> cameraPosition;
	mat::Matrix<float, 3, 3> cameraRotation;
	mat::Rvec<3> lightSource;
	ThreeSpace() :
		glProgram{ mat::perspective(3.14f / 4, 1.0f, .3f, 1000.f) },
		vertexArrayObject{}, shapeKeys{} {	}

	template<typename T1>
	void assign(const T1& buffer, std::string shapeName = "shape") {
		const unsigned int vertexCount = sizeof(T1) / sizeof(mat::Rvec<3>);
		std::array<geometry::Triangle, vertexCount / 3> vertBuffer;
		std::memcpy(&vertBuffer[0], &buffer, sizeof(T1));
		std::array<geometry::FaceNormal, vertexCount / 3> Norms;
		std::transform(vertBuffer.begin(), vertBuffer.end(), Norms.begin(), [](const geometry::Triangle& t) {return geometry::FaceNormal(t); });
		//auto normals = geometry::getFaceNormals(buffer);
		std::array<mat::Rvec<3>, vertexCount> tRGBA;
		std::fill(tRGBA.begin(), tRGBA.end(), mat::Rvec<3>{ 1.0f, 1.0f, 1.0f });
		std::array<GLuint, 3> vbos;
		glCreateBuffers(3, &vbos[0]);
		glNamedBufferData(vbos[0], vertexCount * sizeof(mat::Rvec<3>), &vertBuffer, GL_STATIC_DRAW);
		glNamedBufferData(vbos[1], vertexCount * sizeof(mat::Rvec<3>), &tRGBA, GL_STATIC_DRAW);
		glNamedBufferData(vbos[2], vertexCount * sizeof(mat::Rvec<3>), &Norms, GL_STATIC_DRAW);
		shapeKeys[shapeName] = { vbos, vertexCount };
	}
	void assignVbo(std::pair<std::array<GLuint, 3>, unsigned int> vbo, std::string shapeName = "shape") {
		shapeKeys[shapeName] = vbo;
	}
	void draw(std::string s, mat::Matrix<float, 3, 3> transform, mat::Rvec<3> location) const {
		auto camRotate = mat::transpose(cameraRotation);
		glProgram.setUniforms(camRotate*transform, camRotate*(location - cameraPosition), camRotate*mat::Rvec<3>{});
		vertexArrayObject.bind(shapeKeys.at(s).first);
		glDrawArrays(GL_TRIANGLES, 0, shapeKeys.at(s).second);
		glBindVertexArray(0);
	}
	void draw(std::string s, std::pair<mat::Matrix<float, 3, 3>, mat::Rvec<3>> loc) const {
		auto camRotate = mat::transpose(cameraRotation);
		glProgram.setUniforms(camRotate*loc.first, camRotate*(loc.second - cameraPosition), camRotate*mat::Rvec<3>{});
		vertexArrayObject.bind(shapeKeys.at(s).first);
		glDrawArrays(GL_TRIANGLES, 0, shapeKeys.at(s).second);
		glBindVertexArray(0);
	}
	void draw(const ShapeTag& t) const {
		auto camRotate = mat::transpose(cameraRotation);
		glProgram.setUniforms(camRotate*t.M, camRotate*(t.affine - cameraPosition), camRotate*mat::Rvec<3>{});
		vertexArrayObject.bind(shapeKeys.at(t.shapeName).first);
		glDrawArrays(GL_TRIANGLES, 0, shapeKeys.at(t.shapeName).second);
		glBindVertexArray(0);
	}
	void lookAt(mat::Rvec<3> spot) {
		auto xplane = mat::dot(mat::Rvec<3>{0.0f, 0.0f, -1.0f}, spot - cameraPosition);
		auto yplane = mat::dot(mat::Rvec<3>{-1.0f, 0.0f, 0.0f}, spot - cameraPosition);
		auto yplane2 = mat::dot(mat::Rvec<3>{0.0f, 1.0f, 0.0f}, spot - cameraPosition);
		auto xplane2 = mat::dot(mat::rotateY(atan2f(yplane, xplane))*mat::Rvec<3>{0.0f, 0.0f, -1.0f}, spot - cameraPosition);
		cameraRotation = mat::rotateY(atan2f(yplane, xplane))*mat::rotateX(atan2f(yplane2, xplane2));
	}
	void firstPersonControl(const mat::Rvec<2>& c) {
		cameraRotation = mat::rotateY(-c(1, 1))*mat::rotateX(c(2, 1));
	}
	void freeNavigate(const mat::Rvec<2>& dir) {
		cameraPosition += cameraRotation * mat::Rvec<3>{dir(1,1), 0.f, dir(2,1)};
	}
};
