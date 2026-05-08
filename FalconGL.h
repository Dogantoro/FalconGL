#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// errors are negative
typedef enum status_t {
    STATUS_SUCCESS = 0,
    STATUS_ERROR = -1 // miscellaneous error
} status_t;

typedef struct VertexBuffer {
    float * pos;  // vec4 XYZ[W]
    float * norm; // vec4 XYZ[W]
    float * uv;   // vec2 UV
    float * col;  // vec4 RGBA
    float * usr;  // float[8] user defined attributes
} VertexBuffer;

typedef struct FrameBuffer {
    // modified in rasterizer
    float * depth;     // depth of fragment (init to INFINITY)
    uint32_t * tri_id; // id of triangle fragment belongs to (init to UINT32_MAX);

    // modified in fragment shader
    uint32_t * color;  // packed RGBA 0xRRGGBBAA (init to 0x00000000)
} FrameBuffer;

typedef struct fgl_t {
    size_t num_verts;
    size_t screen_width;
    size_t screen_height;
    size_t near_plane;
    size_t far_plane;

    size_t num_pixels;
    
    VertexBuffer buffer_0;
    VertexBuffer buffer_1;
    FrameBuffer buffer_2;
} fgl_t;

status_t fgl_init(fgl_t * fgl, size_t num_verts, size_t screen_width, size_t screen_height, float near_plane, float far_plane) {
    fgl->num_verts = num_verts;
    fgl->screen_width = screen_width;
    fgl->screen_height = screen_height;
    fgl->near_plane = near_plane;
    fgl->far_plane = far_plane;

    size_t num_pixels = screen_width * screen_height;
    fgl->num_pixels = num_pixels;

    // TODO: validate arguments

    fgl->buffer_0.pos  = malloc(num_verts * sizeof(float) * 4);
    fgl->buffer_0.norm = malloc(num_verts * sizeof(float) * 4);
    fgl->buffer_0.uv   = malloc(num_verts * sizeof(float) * 2);
    fgl->buffer_0.col  = malloc(num_verts * sizeof(float) * 4);
    fgl->buffer_0.usr  = malloc(num_verts * sizeof(float) * 8);

    fgl->buffer_1.pos  = malloc(num_verts * sizeof(float) * 4);
    fgl->buffer_1.norm = malloc(num_verts * sizeof(float) * 4);
    fgl->buffer_1.uv   = malloc(num_verts * sizeof(float) * 2);
    fgl->buffer_1.col  = malloc(num_verts * sizeof(float) * 4);
    fgl->buffer_1.usr  = malloc(num_verts * sizeof(float) * 8);

    fgl->buffer_2.depth  = malloc(num_pixels * sizeof(float));
    fgl->buffer_2.tri_id = malloc(num_pixels * sizeof(uint32_t));
    fgl->buffer_2.color  = malloc(num_pixels * sizeof(uint32_t));

    // TODO: verify allocations

    for (int i = 0; i < num_pixels; i++) {
        fgl->buffer_2.depth[i] = INFINITY;
    }

    for (int i = 0; i < num_pixels; i++) {
        fgl->buffer_2.tri_id[i] = UINT32_MAX;
    }

    for (int i = 0; i < num_pixels; i++) {
        fgl->buffer_2.color[i] = 0U;
    }
}