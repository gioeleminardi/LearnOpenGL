//
// Created by gioel on 18/04/2020.
//

#ifndef LEARNOPENGLCOM_SHADER_HPP
#define LEARNOPENGLCOM_SHADER_HPP

#include <string>

class Shader {
public:
    unsigned int ID;

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char *vertexPath, const char *fragmentPath);

    // activate the shader
    // ------------------------------------------------------------------------
    void use() const;

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setFloat4(const std::string &name, float v0, float v1, float v2, float v3) const;

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    static void checkCompileErrors(unsigned int shader, const std::string &type);
};

#endif //LEARNOPENGLCOM_SHADER_HPP
