#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "DoganGL.hpp"

int proj_index;

vec4 vs(DoganGL::Vertex vert, const float * uniforms) {
    glm::mat4 proj = glm::make_mat4(&uniforms[proj_index]);
    return proj * vec4(vert.attribs[0], vert.attribs[1], vert.attribs[2], 1);
}

int pos_index;
int col_index;
int norm_index;

vec4 fs(const float * attribs) {
    vec3 color = vec3(attribs[col_index],  attribs[col_index + 1],  attribs[col_index + 2]);
    vec3 pos   = vec3(attribs[pos_index],  attribs[pos_index + 1],  attribs[pos_index + 2]);
    vec3 norm  = vec3(attribs[norm_index], attribs[norm_index + 1], attribs[norm_index + 2]);
    
    vec3 LPos = vec3(2.f,0.f,0.f);

    auto diffuse = clamp(glm::dot(glm::normalize(LPos - pos), glm::normalize(norm)), 0.0f, 1.0f);
    vec3 col = color * (diffuse + 0.1f);
    return vec4(col, 1.0);
}

void displayTris(std::vector<DoganGL::Triangle> &tris) {
    int i = 0;
    for (const auto &tri : tris) {
        std::cout << "Triangle " << i++ << ": {\n";
        std::cout << "\t(" << tri.A.pos.x << "," << tri.A.pos.y << "," << tri.A.pos.z << "),\n";
        std::cout << "\t(" << tri.B.pos.x << "," << tri.B.pos.y << "," << tri.B.pos.z << "),\n";
        std::cout << "\t(" << tri.C.pos.x << "," << tri.C.pos.y << "," << tri.C.pos.z << ")},\n\n";
    }
}

std::string trisToString(std::vector<DoganGL::Triangle> &tris) {
    std::stringstream str;
    int i = 0;
    for (const auto &tri : tris) {
        str << "Triangle " << i++ << ": {\n";
        str << "\t(" << tri.A.pos.x << "," << tri.A.pos.y << "," << tri.A.pos.z << "),\n";
        str << "\t(" << tri.B.pos.x << "," << tri.B.pos.y << "," << tri.B.pos.z << "),\n";
        str << "\t(" << tri.C.pos.x << "," << tri.C.pos.y << "," << tri.C.pos.z << ")},\n\n";
    }
    return str.str();
}

template <typename Func, typename... Args>
auto test(std::string name, Func&& func, Args&&... args)
    -> std::enable_if_t<std::is_same_v<std::invoke_result_t<Func, Args...>, void>>
{
    auto start = std::chrono::high_resolution_clock::now();
    std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << name << ": " << elapsed.count() << " ms" << std::endl;
}

template <typename Func, typename... Args>
auto test(std::string name, Func&& func, Args&&... args)
    -> std::enable_if_t<!std::is_same_v<std::invoke_result_t<Func, Args...>, void>,
                        std::invoke_result_t<Func, Args...>>
{
    using ReturnType = std::invoke_result_t<Func, Args...>;

    auto start = std::chrono::high_resolution_clock::now();
    ReturnType result = std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << name << ": " << elapsed.count() << " ms\n";

    return result;
}

int main() {
    // DoganGL::Vertex triangleArr[3] = {
    //     {{-0.5f, -0.5f, 0.0f, 0.565f, 0.11f, 0.89f}}, // Bottom-left
    //     {{ 0.5f, -0.5f, 0.0f, 0.89f, 0.345f, 0.071f}}, // Bottom-right
    //     {{ 0.0f,  0.5f, 0.0f, 0.392f, 0.929f, 0.141f}}  // Top-center
    // };
    // 1 Clip
    // DoganGL::Vertex triangleArr[3] = {
    //     {{2.0f, 0.0f, 0.0f, 1.0f}}, // Outside (x > w)
    //     {{0.0f, 1.0f, 0.0f, 1.0f}}, // Inside
    //     {{0.0f, -1.0f, 0.0f, 1.0f}} // Inside
    // };
    // 2 Clip
    // DoganGL::Vertex triangleArr[3] = {
    //     {{-0.4f, -0.4f, 0.0f, 1.0f}}, // Outside (x > w)
    //     {{-0.4f,  1.4f, 0.0f, 1.0f}}, // Inside
    //     {{ 1.4f, -0.4f, 0.0f, 1.0f}} // Inside
    // };
    // DoganGL::Vertex triangleArr[6] = {
    //     {{-0.25f, -0.5f,  -15.5f, 0.565f, 0.11f,  0.89f }},
    //     {{ 0.75f, -0.5f,  -15.5f, 0.89f,  0.345f, 0.071f}},
    //     {{ 0.25f,  0.5f,  -15.5f, 0.392f, 0.929f, 0.141f}},
    //     {{-0.75f,  0.5f,  -1.0f, 0.89f,  0.345f, 0.071f}},
    //     {{ 0.25f,  0.5f,  -1.0f, 0.392f, 0.929f, 0.141f}},
    //     {{-0.25f, -0.5f,  -1.0f, 0.565f, 0.11f,  0.89f }}
    // };

    DoganGL::Vertex A = {{-0.5f, -0.5f, -1.5f, 0.8f, 0.0f, 0.0f, -0.57735027f, -0.21132487f, 0.78867513f}};
    DoganGL::Vertex B = {{ 0.5f, -0.5f, -1.5f, 0.0f, 0.8f, 0.0f,  0.57735027f, -0.21132487f, 0.78867513f}};
    DoganGL::Vertex C = {{ 0.0f,  0.5f, -1.5f, 0.0f, 0.0f, 0.8f,  0.0f,         0.70710678f, 0.70710678f}};
    DoganGL::Vertex D = {{ 0.0f,  0.0f, -1.0f, 0.8f, 0.8f, 0.8f,  0.0f,         0.07161243f, 0.99743253f}};

    DoganGL::Vertex A1 = {{-0.5f, -0.5f, -1.5f, 0.8f, 0.0f, 0.0f, -0.81649658f, 0.40824829f, 0.40824829f}};
    DoganGL::Vertex D1 = {{ 0.0f,  0.0f, -1.0f, 0.8f, 0.8f, 0.8f, -0.81649658f, 0.40824829f, 0.40824829f}};
    DoganGL::Vertex C1 = {{ 0.0f,  0.5f, -1.5f, 0.0f, 0.0f, 0.8f, -0.81649658f, 0.40824829f, 0.40824829f}};
 
    DoganGL::Vertex A2 = {{-0.5f, -0.5f, -1.5f, 0.8f, 0.0f, 0.0f, 0.0f, -0.70710678f, 0.70710678f}};
    DoganGL::Vertex B2 = {{ 0.5f, -0.5f, -1.5f, 0.0f, 0.8f, 0.0f, 0.0f, -0.70710678f, 0.70710678f}};
    DoganGL::Vertex D2 = {{ 0.0f,  0.0f, -1.0f, 0.8f, 0.8f, 0.8f, 0.0f, -0.70710678f, 0.70710678f}};

    DoganGL::Vertex B3 = {{ 0.5f, -0.5f, -1.5f, 0.0f, 0.8f, 0.0f, 0.81649658f, 0.40824829f, 0.40824829f}};
    DoganGL::Vertex C3 = {{ 0.0f,  0.5f, -1.5f, 0.0f, 0.0f, 0.8f, 0.81649658f, 0.40824829f, 0.40824829f}};
    DoganGL::Vertex D3 = {{ 0.0f,  0.0f, -1.0f, 0.8f, 0.8f, 0.8f, 0.81649658f, 0.40824829f, 0.40824829f}};

    DoganGL::Vertex triangleArr[9] = {
        A1,D1,C1,
        A2,B2,D2,
        B3,C3,D3
    };
    std::vector<DoganGL::Vertex> triangle(&triangleArr[0], &triangleArr[9]);
    int viewportWidth  = 800;   // Screen width
    int viewportHeight = 600;   // Screen height
    float nearVal = 0.1f;          // Near depth value
    float farVal = 100.0f;            // Far depth value

    DoganGL::Context * context = new DoganGL::Context();
    DoganGL::setupViewport(context, viewportWidth, viewportHeight, nearVal, farVal, 0, 0);

    DoganGL::VAO vao;
    pos_index = vao.addAttrib(3);
    col_index = vao.addAttrib(3);
    norm_index = vao.addAttrib(3);

    auto start = std::chrono::high_resolution_clock::now();
    
    test("bindVAO", DoganGL::bindVAO, context, vao);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)viewportWidth/(float)viewportHeight, 0.1f, 100.0f);
    proj_index = DoganGL::addUniform(context, 16, glm::value_ptr(proj));

    test("loadVertexShader", DoganGL::loadVertexShader, context, vs);
    test("loadVertices", DoganGL::loadVertices, context, triangle);

    test("applyVertexShader", DoganGL::applyVertexShader, context);
    test("VertexPostProcessing", DoganGL::VertexPostProcessing, context);

    // test("displayTris", displayTris, context->postProcessedTris);

    test("loadFragmentShader", DoganGL::loadFragmentShader, context, fs);

    test("setupFrameBuffer", DoganGL::setupFrameBuffer, context);
    test("rasterize", DoganGL::rasterize, context);
    test("clearFrameBuffer", DoganGL::clearFrameBuffer, context, vec3(0.98,0.73,0.01));
    test("applyFragmentShader", DoganGL::applyFragmentShader, context);

    //test("antialiasing downscaling", DoganGL::AA, context, 2);

    test("imageWrite (external)", &DoganGL::Image::write, context->img, "C:/Users/dogan/Documents/DoganGL/img.bmp");

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Overall" << ": " << elapsed.count() << " ms\n" << std::endl;
    std::cout << "FPS possible: " << 1000/elapsed.count() << std::endl;
}