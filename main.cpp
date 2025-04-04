#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "FalconGL.hpp"

static const std::string project_root(PROJECT_ROOT); 

int proj_index;

vec4 vs(FalconGL::Vertex vert, const float * uniforms) {
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

void displayTris(std::vector<FalconGL::Triangle> &tris) {
    int i = 0;
    for (const auto &tri : tris) {
        std::cout << "Triangle " << i++ << ": {\n";
        std::cout << "\t(" << tri.A.pos.x << "," << tri.A.pos.y << "," << tri.A.pos.z << "),\n";
        std::cout << "\t(" << tri.B.pos.x << "," << tri.B.pos.y << "," << tri.B.pos.z << "),\n";
        std::cout << "\t(" << tri.C.pos.x << "," << tri.C.pos.y << "," << tri.C.pos.z << ")},\n\n";
    }
}

std::string trisToString(std::vector<FalconGL::Triangle> &tris) {
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
    // FalconGL::Vertex triangleArr[3] = {
    //     {{-0.5f, -0.5f, 0.0f, 0.565f, 0.11f, 0.89f}}, // Bottom-left
    //     {{ 0.5f, -0.5f, 0.0f, 0.89f, 0.345f, 0.071f}}, // Bottom-right
    //     {{ 0.0f,  0.5f, 0.0f, 0.392f, 0.929f, 0.141f}}  // Top-center
    // };
    // 1 Clip
    // FalconGL::Vertex triangleArr[3] = {
    //     {{2.0f, 0.0f, 0.0f, 1.0f}}, // Outside (x > w)
    //     {{0.0f, 1.0f, 0.0f, 1.0f}}, // Inside
    //     {{0.0f, -1.0f, 0.0f, 1.0f}} // Inside
    // };
    // 2 Clip
    // FalconGL::Vertex triangleArr[3] = {
    //     {{-0.4f, -0.4f, 0.0f, 1.0f}}, // Outside (x > w)
    //     {{-0.4f,  1.4f, 0.0f, 1.0f}}, // Inside
    //     {{ 1.4f, -0.4f, 0.0f, 1.0f}} // Inside
    // };
    // FalconGL::Vertex triangleArr[6] = {
    //     {{-0.25f, -0.5f,  -15.5f, 0.565f, 0.11f,  0.89f }},
    //     {{ 0.75f, -0.5f,  -15.5f, 0.89f,  0.345f, 0.071f}},
    //     {{ 0.25f,  0.5f,  -15.5f, 0.392f, 0.929f, 0.141f}},
    //     {{-0.75f,  0.5f,  -1.0f, 0.89f,  0.345f, 0.071f}},
    //     {{ 0.25f,  0.5f,  -1.0f, 0.392f, 0.929f, 0.141f}},
    //     {{-0.25f, -0.5f,  -1.0f, 0.565f, 0.11f,  0.89f }}
    // };

    FalconGL::Vertex A = {{-0.5f, -0.5f, -1.5f, 0.8f, 0.0f, 0.0f, -0.57735027f, -0.21132487f, 0.78867513f}};
    FalconGL::Vertex B = {{ 0.5f, -0.5f, -1.5f, 0.0f, 0.8f, 0.0f,  0.57735027f, -0.21132487f, 0.78867513f}};
    FalconGL::Vertex C = {{ 0.0f,  0.5f, -1.5f, 0.0f, 0.0f, 0.8f,  0.0f,         0.70710678f, 0.70710678f}};
    FalconGL::Vertex D = {{ 0.0f,  0.0f, -1.0f, 0.8f, 0.8f, 0.8f,  0.0f,         0.07161243f, 0.99743253f}};

    FalconGL::Vertex A1 = {{-0.5f, -0.5f, -1.5f, 0.8f, 0.0f, 0.0f, -0.81649658f, 0.40824829f, 0.40824829f}};
    FalconGL::Vertex D1 = {{ 0.0f,  0.0f, -1.0f, 0.8f, 0.8f, 0.8f, -0.81649658f, 0.40824829f, 0.40824829f}};
    FalconGL::Vertex C1 = {{ 0.0f,  0.5f, -1.5f, 0.0f, 0.0f, 0.8f, -0.81649658f, 0.40824829f, 0.40824829f}};
 
    FalconGL::Vertex A2 = {{-0.5f, -0.5f, -1.5f, 0.8f, 0.0f, 0.0f, 0.0f, -0.70710678f, 0.70710678f}};
    FalconGL::Vertex B2 = {{ 0.5f, -0.5f, -1.5f, 0.0f, 0.8f, 0.0f, 0.0f, -0.70710678f, 0.70710678f}};
    FalconGL::Vertex D2 = {{ 0.0f,  0.0f, -1.0f, 0.8f, 0.8f, 0.8f, 0.0f, -0.70710678f, 0.70710678f}};

    FalconGL::Vertex B3 = {{ 0.5f, -0.5f, -1.5f, 0.0f, 0.8f, 0.0f, 0.81649658f, 0.40824829f, 0.40824829f}};
    FalconGL::Vertex C3 = {{ 0.0f,  0.5f, -1.5f, 0.0f, 0.0f, 0.8f, 0.81649658f, 0.40824829f, 0.40824829f}};
    FalconGL::Vertex D3 = {{ 0.0f,  0.0f, -1.0f, 0.8f, 0.8f, 0.8f, 0.81649658f, 0.40824829f, 0.40824829f}};

    FalconGL::Vertex triangleArr[9] = {
        A1,D1,C1,
        A2,B2,D2,
        B3,C3,D3
    };
    std::vector<FalconGL::Vertex> triangle(&triangleArr[0], &triangleArr[9]);
    int viewportWidth  = 800;   // Screen width
    int viewportHeight = 600;   // Screen height
    float nearVal = 0.1f;          // Near depth value
    float farVal = 100.0f;            // Far depth value

    FalconGL::Context * context = new FalconGL::Context();
    FalconGL::setupViewport(context, viewportWidth, viewportHeight, nearVal, farVal, 0, 0);

    FalconGL::VAO vao;
    pos_index = vao.addAttrib(3);
    col_index = vao.addAttrib(3);
    norm_index = vao.addAttrib(3);

    auto start = std::chrono::high_resolution_clock::now();
    
    test("bindVAO", FalconGL::bindVAO, context, vao);

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)viewportWidth/(float)viewportHeight, 0.1f, 100.0f);
    proj_index = FalconGL::addUniform(context, 16, glm::value_ptr(proj));

    test("loadVertexShader", FalconGL::loadVertexShader, context, vs);
    test("loadVertices", FalconGL::loadVertices, context, triangle);

    test("applyVertexShader", FalconGL::applyVertexShader, context);
    test("VertexPostProcessing", FalconGL::VertexPostProcessing, context);

    // test("displayTris", displayTris, context->postProcessedTris);

    test("loadFragmentShader", FalconGL::loadFragmentShader, context, fs);

    test("setupFrameBuffer", FalconGL::setupFrameBuffer, context);
    test("rasterize", FalconGL::rasterize, context);
    test("clearFrameBuffer", FalconGL::clearFrameBuffer, context, vec3(0.98,0.73,0.01));
    test("applyFragmentShader", FalconGL::applyFragmentShader, context);

    //test("antialiasing downscaling", FalconGL::AA, context, 2);

    test("imageWrite (external)", &FalconGL::Image::write, context->img, project_root + "/img.bmp");

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Overall" << ": " << elapsed.count() << " ms\n" << std::endl;
    std::cout << "FPS possible: " << 1000/elapsed.count() << std::endl;
}