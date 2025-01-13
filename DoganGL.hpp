#include <functional>
#include <vector>
#include <glm/glm.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "libs/stb-write.hpp"
#include <string>

#include <chrono>
#include <iostream>

using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace DoganGL {
    struct Vertex {
        std::vector<float> attribs;
    };
    struct ProcessedVertex {
        std::vector<float> attribs;
        vec4 pos;
    };
    struct Triangle {
        ProcessedVertex A,B,C;
    };
    // TODO -- implement uniforms/textures
    struct Fragment {
        bool drawn;
        float z;
        std::vector<float> attribs;
        Fragment() {
            drawn = false;
        }
        Fragment(int numAttribs) {
            drawn = false;
            attribs.resize(numAttribs);
        }
    };
    struct Image {
        int width, height;
        std::vector<char> pixels;
        bool write(std::string path) {
            return stbi_write_png(path.c_str(), width, height, 3, pixels.data(), width * 3);
        }
    };
    struct VAO {
        int index;
        VAO() {
            index = 0;
        }
        int addAttrib(int size) {
            int tmp = index;
            index += size;
            return tmp;
        }
    };
    using VertexShader = std::function<vec4(const Vertex&)>;
    using FragmentShader = std::function<vec4(const Fragment&)>;
    struct Context {
        std::vector<Vertex> vertices;
        std::vector<ProcessedVertex> postVSVertices;
        std::vector<Triangle> postProcessedTris;
        std::vector<Fragment> fragments;
        Image img;
        VAO vao;
        bool VAObinded = false;
        
        // Add checks for if shaders are loaded!
        VertexShader vertexShader;
        FragmentShader fragmentShader;
        struct Viewport {
            int x = 0, y = 0; // Coordinate of lower left point
            int width, height; // size of viewport in pixels
            float nearVal = 0.f; // mapping of near clipping plane to window coords
            float farVal  = 1.f; // mapping of far  clipping plane to window coords
        } viewport;
    };
    void bindVAO(Context * context, VAO &vao) {
        context->vao = vao;
        context->VAObinded = true;
    }
    void loadVertices(Context * context, std::vector<Vertex> vertices) {
        context->vertices = vertices;
    }
    void loadVertexShader(Context * context, VertexShader vs) {
        context->vertexShader = vs;
    }
    void loadFragmentShader(Context * context, FragmentShader fs) {
        context->fragmentShader = fs;
    }
    bool applyVertexShader(Context * context) {
        if (!(context->VAObinded))
            return false;
        if (!(context->vertices.size()))
            return false;
        context->postVSVertices.clear();
        context->postVSVertices.reserve(context->vertices.size());
        for (Vertex vertex : context->vertices) {
            ProcessedVertex tmp;
            tmp.pos = context->vertexShader(vertex);
            tmp.attribs = vertex.attribs;
            context->postVSVertices.push_back(tmp);
        }
        return true;
    }
    // TODO interpolate additional attributes when arbitrary attributes added!
    void clip(std::vector<Triangle> &tris, Triangle tri, int axis, int sign) {
        // axis = 0 -> x-axis
        // axis = 1 -> y-axis
        // axis = 2 -> z-axis
        std::vector<ProcessedVertex> inside, outside;

        for (const ProcessedVertex& vertex : {tri.A, tri.B, tri.C}) {
            if (sign > 0 && vertex.pos[axis] <= vertex.pos[3]) {
                inside.push_back(vertex);
            } else if (sign < 0 && vertex.pos[axis] >= -vertex.pos[3]) {
                inside.push_back(vertex);
            } else {
                outside.push_back(vertex);
            }
        }

        switch (inside.size()) {
            case 3:
                tris.push_back(tri);
                break;
            case 2: {
                // Two new vertices needed
                ProcessedVertex v4, v5;
                auto v4alpha = ((sign * inside[0].pos[3] - inside[0].pos[axis])/(outside[0].pos[axis] - inside[0].pos[axis]));
                auto v5alpha = ((sign * inside[0].pos[3] - inside[1].pos[axis])/(outside[0].pos[axis] - inside[1].pos[axis]));
                v4.pos = inside[0].pos + v4alpha * (outside[0].pos - inside[0].pos);
                v5.pos = inside[1].pos + v4alpha *(outside[0].pos - inside[1].pos);
                v4.attribs.resize(inside[0].attribs.size());
                v5.attribs.resize(inside[0].attribs.size());
                for (int i = 0; i < inside[0].attribs.size(); i++) {
                    v4.attribs[i] = inside[0].attribs[i] + v4alpha * (outside[0].attribs[i] - inside[0].attribs[i]);
                    v5.attribs[i] = inside[1].attribs[i] + v5alpha * (outside[0].attribs[i] - inside[1].attribs[i]);
                }
                Triangle t1, t2;
                t1.A = inside[0]; t1.B = inside[1]; t1.C = v4;
                t2.A = inside[1]; t2.B = v4; t2.C = v5;
                tris.push_back(t1);
                tris.push_back(t2);
                break;
            }
            case 1: {
                // Two new vertices needed
                ProcessedVertex v4, v5;
                auto v4alpha = ((sign * inside[0].pos[3] - inside[0].pos[axis])/(outside[0].pos[axis] - inside[0].pos[axis]));
                auto v5alpha = ((sign * inside[0].pos[3] - inside[0].pos[axis])/(outside[1].pos[axis] - inside[0].pos[axis]));
                v4.pos = inside[0].pos + v4alpha * (outside[0].pos - inside[0].pos);
                v5.pos = inside[0].pos + v5alpha * (outside[1].pos - inside[0].pos);
                v4.attribs.resize(inside[0].attribs.size());
                v5.attribs.resize(inside[0].attribs.size());
                for (int i = 0; i < inside[0].attribs.size(); i++) {
                    v4.attribs[i] = inside[0].attribs[i] + v4alpha * (outside[0].attribs[i] - inside[0].attribs[i]);
                    v5.attribs[i] = inside[0].attribs[i] + v5alpha * (outside[1].attribs[i] - inside[0].attribs[i]);
                }
                Triangle t1;
                t1.A = inside[0]; t1.B = v4; t1.C = v5;
                tris.push_back(t1);
                break;
            }
            case 0:
                break;
        }
        return;
        
    }
    void viewportTransform(Context * context, ProcessedVertex &vert) {
        const auto &viewport = context->viewport;
        vert.pos.x = (viewport.width/2.0)  * (vert.pos.x + 1) + viewport.x;
        vert.pos.y = (viewport.height/2.0) * (vert.pos.y + 1) + viewport.y;
        vert.pos.z = ((viewport.farVal - viewport.nearVal)/2.0) * vert.pos.z + ((viewport.farVal + viewport.nearVal)/2.0);
    }
    bool VertexPostProcessing(Context * context) {
        if (!(context->postVSVertices.size()))
            return false;

        std::vector<Triangle> tris;

        tris.reserve(context->postVSVertices.size() / 3);

        // Primitive Assembly
        for (int i = 0; i< context->postVSVertices.size()/3; i++) {
            Triangle t1;
            t1.A = context->postVSVertices[i];
            t1.B = context->postVSVertices[i+1];
            t1.C = context->postVSVertices[i+2];
            tris.push_back(t1);
        }

        // Clipping
        for (int i = 0; i < 5; i++) {
            std::vector<Triangle> trisToClip;
            std::swap(trisToClip, tris);
            for (const auto& tri : trisToClip) {
                clip(tris, tri, i/2, ((i%2)*2) - 1);
            }
        }
        
        // Perspective Divide
        for (Triangle& tri: tris) {
            tri.A.pos /= tri.A.pos.w;
            tri.B.pos /= tri.B.pos.w;
            tri.C.pos /= tri.C.pos.w;
        }

        // Viewport Transform
        for (Triangle& tri: tris) {
            viewportTransform(context, tri.A);
            viewportTransform(context, tri.B);
            viewportTransform(context, tri.C);
        }

        context->postProcessedTris = tris;
    }
    inline vec3 barycenterize(const Triangle tri, const vec2 cartesian) {
        auto &x  = cartesian.x;
        auto &y  = cartesian.y;
        auto &x1 = tri.A.pos.x;
        auto &y1 = tri.A.pos.y;
        auto &x2 = tri.B.pos.x;
        auto &y2 = tri.B.pos.y;
        auto &x3 = tri.C.pos.x;
        auto &y3 = tri.C.pos.y;
        auto denom = ((y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3));
        auto L1 = ((y2-y3)*(x-x3) + (x3-x2)*(y-y3)) / denom;
        auto L2 = ((y3-y1)*(x-x3) + (x1-x3)*(y-y3)) / denom;
        auto L3 = 1 - L1 - L2;
        return vec3(L1, L2, L3);
    }
    inline float edgeTest(vec2 P1, vec2 P2, vec2 P) {
        return (P.y - P1.y)*(P2.x-P1.x) - (P.x-P1.x)*(P2.y-P1.y);
    }
    void rasterize(Context * context) {
        float width = context->viewport.width;
        float height = context->viewport.height;
        context->fragments.clear();
        context->fragments.reserve(width * height);
        float numAttribs = context-> vertices[0].attribs.size();
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                context->fragments.emplace_back(numAttribs);
            }
        }
        // TODO -- do not iterate over all fragments for every tri, this is so inefficient
        for (const auto &tri : context->postProcessedTris) {
            auto xmin = floorf(std::min({tri.A.pos.x, tri.B.pos.x, tri.C.pos.x}));
            auto xmax = floorf(std::max({tri.A.pos.x, tri.B.pos.x, tri.C.pos.x}));
            auto ymin = floorf(std::min({tri.A.pos.y, tri.B.pos.y, tri.C.pos.y}));
            auto ymax = floorf(std::max({tri.A.pos.y, tri.B.pos.y, tri.C.pos.y}));
            for (int x = __max(xmin, 0); x <= xmax && x < width; x++) {
                for (int y = __max(ymin, 0); y <= ymax && y < height; y++) {
                    //auto start = std::chrono::high_resolution_clock::now();
                    float e1 = edgeTest(tri.A.pos, tri.B.pos, vec2(x,y));
                    float e2 = edgeTest(tri.B.pos, tri.C.pos, vec2(x,y));
                    float e3 = edgeTest(tri.C.pos, tri.A.pos, vec2(x,y));
                    if (!((e1 < 0 && e2 < 0 && e3 < 0) || (e1 > 0 && e2 > 0 && e3 > 0)))
                        continue;
                    // at this point the fragment lies on a triangle and interpolation must occur
                    // first do z-test to prevent unneeded work
                    vec3 BC = barycenterize(tri, vec2(x,y));
                    float z = (BC.x * tri.A.pos.z) + (BC.y * tri.B.pos.z) + (BC.z * tri.C.pos.z);
                    if (context->fragments[y * width + x].drawn && z >= context->fragments[y * width + x].z)
                        continue;

                    // time to draw
                    context->fragments[y * width + x].drawn = true;
                    context->fragments[y * width + x].z = z;

                    // Temporary trivial interpolation for testing, must interpolate variable
                    // attributes once implemented!
                    for (int i = 0; i < tri.A.attribs.size(); i++) {
                        context->fragments[y * width + x].attribs[i] = 
                            (BC.x * tri.A.attribs[i]) + (BC.y * tri.B.attribs[i]) + (BC.z * tri.C.attribs[i]);
                    }
                    
                }
            }
        }
    }
    bool applyFragmentShader(Context * context) {
        auto width = context->viewport.width;
        auto height = context->viewport.height;
        if (context->img.pixels.size() != (3 * height * width))
            return false;
        // TODO implement alpha
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                if (context->fragments[j * width + i].drawn) {
                    vec4 pixel = context->fragmentShader(context->fragments[j * width + i]);
                    context->img.pixels[3 * (j * width + i)]     = ((unsigned char) (255.f * pixel.r));
                    context->img.pixels[3 * (j * width + i) + 1] = ((unsigned char) (255.f * pixel.g));
                    context->img.pixels[3 * (j * width + i) + 2] = ((unsigned char) (255.f * pixel.b));
                }
            }
        }
    }
    void clearFrameBuffer(Context * context, vec3 rgb) {
        auto width = context->viewport.width;
        auto height = context->viewport.height;
        context->img.pixels.clear();
        context->img.width = width;
        context->img.height = height;
        context->img.pixels.reserve(3 * height * width);
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                context->img.pixels.push_back((unsigned char) (255.f * rgb.r));
                context->img.pixels.push_back((unsigned char) (255.f * rgb.g));
                context->img.pixels.push_back((unsigned char) (255.f * rgb.b));
            }
        }
    }
}