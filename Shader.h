#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class Shader {
public:
	Shader(const std::string filename);
	~Shader();

	void Bind() const;
	void Unbind() const;

	unsigned int GetUniformLocation(std::string name);
	void SetUniform1i(std::string name, int value);
	void SetUniform1f(std::string name, float value);
	void SetUniform4f(std::string name, float v0, float v1, float v2, float v3);
	float GetUniform1f(std::string name);
	void SetUniformMat4f(std::string name, glm::mat4 value);
	void SetUniformVec3f(std::string name, glm::vec3 value);


private:
	unsigned int id;
	std::unordered_map<std::string, int> cache;
};