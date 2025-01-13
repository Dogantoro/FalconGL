#include "DoganGL.hpp"
#include <iostream>
#include <iostream>
#include <chrono>

vec4 vs(DoganGL::Vertex vert) {
    return vec4(vert.attribs[0], vert.attribs[1], vert.attribs[2], 1);
}

int col_index;

vec4 fs(DoganGL::Fragment frag) {
    return vec4(frag.attribs[col_index], frag.attribs[col_index + 1], frag.attribs[col_index + 2], 1.0);
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

int main() {
    DoganGL::Vertex triangleArr[3] = {
        {{-0.5f, -0.5f, 0.0f, 0.565, 0.11, 0.89}}, // Bottom-left
        {{ 0.5f, -0.5f, 0.0f, 0.89, 0.345, 0.071}}, // Bottom-right
        {{ 0.0f,  0.5f, 0.0f, 0.392, 0.929, 0.141}}  // Top-center
    };
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
    std::vector<DoganGL::Vertex> triangle(&triangleArr[0], &triangleArr[3]);
    float viewportWidth = 800.0f;   // Screen width
    float viewportHeight = 600.0f;  // Screen height
    float nearVal = 0.0f;           // Near depth value
    float farVal = 1.0f;            // Far depth value

    DoganGL::Context * context = new DoganGL::Context();
    context->viewport.width = viewportWidth;
    context->viewport.height = viewportHeight;
    context->viewport.nearVal = nearVal;
    context->viewport.farVal = farVal;
    context->viewport.x = 0;
    context->viewport.y = 0;

    DoganGL::VAO vao;
    int pos_index = vao.addAttrib(3);
    col_index = vao.addAttrib(3);
    DoganGL::bindVAO(context, vao);

    DoganGL::loadVertexShader(context, vs);
    DoganGL::loadVertices(context, triangle);

    DoganGL::applyVertexShader(context);
    DoganGL::VertexPostProcessing(context);

    displayTris(context->postProcessedTris);

    DoganGL::loadFragmentShader(context, fs);

    auto start = std::chrono::high_resolution_clock::now();
    DoganGL::rasterize(context);
    DoganGL::clearFrameBuffer(context, vec3(0.98,0.73,0.01));
    DoganGL::applyFragmentShader(context);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

    context->img.write("C:/Users/dogan/Documents/DoganGL/img.png");
}