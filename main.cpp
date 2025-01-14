#include "DoganGL.hpp"
#include <iostream>
#include <iostream>
#include <chrono>

vec4 vs(DoganGL::Vertex vert) {
    return vec4(vert.attribs[0], vert.attribs[1], vert.attribs[2], 1);
}

int col_index;

vec4 fs(const float * attribs) {
    return vec4(attribs[col_index], attribs[col_index + 1], attribs[col_index + 2], 1.0);
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
    DoganGL::Vertex triangleArr[6] = {
        {{-0.25f, -0.5f,  0.0f, 0.565f, 0.11f, 0.89f}},
        {{ 0.75f, -0.5f,  0.0f, 0.89f, 0.345f, 0.071f}},
        {{ 0.25f,  0.5f,  0.0f, 0.392f, 0.929f, 0.141f}},
        {{-0.75f,  0.5f,  0.0f, 0.565f, 0.11f, 0.89f}},
        {{ 0.25f,  0.5f,  0.0f, 0.89f, 0.345f, 0.071f}},
        {{-0.25f, -0.5f,  0.0f, 0.392f, 0.929f, 0.141f}}
    };
    std::vector<DoganGL::Vertex> triangle(&triangleArr[0], &triangleArr[6]);
    int viewportWidth = 800;    // Screen width
    int viewportHeight = 600;   // Screen height
    float nearVal = 0.0f;       // Near depth value
    float farVal = 1.0f;        // Far depth value

    DoganGL::Context * context = new DoganGL::Context();
    DoganGL::setupViewport(context, viewportWidth, viewportHeight, nearVal, farVal, 0, 0);

    DoganGL::VAO vao;
    int pos_index = vao.addAttrib(3);
    col_index = vao.addAttrib(3);
    test("bindVAO", DoganGL::bindVAO, context, vao);

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

    test("imageWrite (external)", &DoganGL::Image::write, context->img, "C:/Users/dogan/Documents/DoganGL/img.png");
}