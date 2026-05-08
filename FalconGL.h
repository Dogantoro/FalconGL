#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// errors are negative
typedef enum status_t {
    FGL_STATUS_SUCCESS = 0,
    FGL_STATUS_ERROR = -1, // miscellaneous error
    FGL_STATUS_ERROR_ALLOC = -2, // Failed to allocate memory
    FGL_STATUS_ERROR_ARGS = -3 // One or more incorrect arguments
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

#define FGL_CHECK_ALLOC(x) do {if ((x) == NULL) goto alloc_fail;} while (0)

status_t fgl_init(fgl_t * fgl, size_t num_verts, size_t screen_width, size_t screen_height, float near_plane, float far_plane) {

    if (!fgl)
        return FGL_STATUS_ERROR_ARGS;

    if (near_plane >= far_plane)
        return FGL_STATUS_ERROR_ARGS;

    if (screen_width == 0 || screen_height == 0)
        return FGL_STATUS_ERROR_ARGS;
    
    if (screen_height > SIZE_MAX / screen_width)
        return FGL_STATUS_ERROR_ARGS;

    size_t num_pixels = screen_width * screen_height;

    if (num_pixels > SIZE_MAX / sizeof(float))
        return FGL_STATUS_ERROR_ARGS;

    if (num_verts > SIZE_MAX / sizeof(float)) 
        return FGL_STATUS_ERROR_ARGS;

    if (num_verts == 0 || num_verts % 3)
        return FGL_STATUS_ERROR_ARGS;

    fgl->num_verts = num_verts;
    fgl->screen_width = screen_width;
    fgl->screen_height = screen_height;
    fgl->near_plane = near_plane;
    fgl->far_plane = far_plane;
    fgl->num_pixels = num_pixels;

    fgl->buffer_0.pos = NULL;
    fgl->buffer_0.norm = NULL;
    fgl->buffer_0.uv = NULL;
    fgl->buffer_0.col = NULL;
    fgl->buffer_0.usr = NULL;
    fgl->buffer_1.pos = NULL;
    fgl->buffer_1.norm = NULL;
    fgl->buffer_1.uv = NULL;
    fgl->buffer_1.col = NULL;
    fgl->buffer_1.usr = NULL;
    fgl->buffer_2.depth = NULL;
    fgl->buffer_2.tri_id = NULL;
    fgl->buffer_2.color = NULL;


    fgl->buffer_0.pos  = malloc(num_verts * sizeof(float) * 4);
    FGL_CHECK_ALLOC(fgl->buffer_0.pos);
    fgl->buffer_0.norm = malloc(num_verts * sizeof(float) * 4);
    FGL_CHECK_ALLOC(fgl->buffer_0.norm);
    fgl->buffer_0.uv   = malloc(num_verts * sizeof(float) * 2);
    FGL_CHECK_ALLOC(fgl->buffer_0.uv);
    fgl->buffer_0.col  = malloc(num_verts * sizeof(float) * 4);
    FGL_CHECK_ALLOC(fgl->buffer_0.col);
    fgl->buffer_0.usr  = malloc(num_verts * sizeof(float) * 8);
    FGL_CHECK_ALLOC(fgl->buffer_0.usr);

    fgl->buffer_1.pos  = malloc(num_verts * sizeof(float) * 4);
    FGL_CHECK_ALLOC(fgl->buffer_1.pos);
    fgl->buffer_1.norm = malloc(num_verts * sizeof(float) * 4);
    FGL_CHECK_ALLOC(fgl->buffer_1.norm);
    fgl->buffer_1.uv   = malloc(num_verts * sizeof(float) * 2);
    FGL_CHECK_ALLOC(fgl->buffer_1.uv);
    fgl->buffer_1.col  = malloc(num_verts * sizeof(float) * 4);
    FGL_CHECK_ALLOC(fgl->buffer_1.col);
    fgl->buffer_1.usr  = malloc(num_verts * sizeof(float) * 8);
    FGL_CHECK_ALLOC(fgl->buffer_1.usr);

    fgl->buffer_2.depth  = malloc(num_pixels * sizeof(float));
    FGL_CHECK_ALLOC(fgl->buffer_2.depth);
    fgl->buffer_2.tri_id = malloc(num_pixels * sizeof(uint32_t));
    FGL_CHECK_ALLOC(fgl->buffer_2.tri_id);
    fgl->buffer_2.color  = malloc(num_pixels * sizeof(uint32_t));
    FGL_CHECK_ALLOC(fgl->buffer_2.color);

    for (size_t i = 0; i < num_pixels; i++) {
        fgl->buffer_2.depth[i] = INFINITY;
    }

    for (size_t i = 0; i < num_pixels; i++) {
        fgl->buffer_2.tri_id[i] = UINT32_MAX;
    }

    for (size_t i = 0; i < num_pixels; i++) {
        fgl->buffer_2.color[i] = 0U;
    }

    return FGL_STATUS_SUCCESS;

    alloc_fail:
    free(fgl->buffer_0.pos);
    free(fgl->buffer_0.norm);
    free(fgl->buffer_0.uv);
    free(fgl->buffer_0.col);
    free(fgl->buffer_0.usr);
    free(fgl->buffer_1.pos);
    free(fgl->buffer_1.norm);
    free(fgl->buffer_1.uv);
    free(fgl->buffer_1.col);
    free(fgl->buffer_1.usr);
    free(fgl->buffer_2.depth);
    free(fgl->buffer_2.tri_id);
    free(fgl->buffer_2.color);
    return FGL_STATUS_ERROR_ALLOC;
}

