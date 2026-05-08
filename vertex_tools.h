#include "FalconGL.h"

// positions

status_t fgl_load_verts_pos_xyz(fgl_t * fgl, const float * vertex_positions) {
    if (!fgl || !vertex_positions || !fgl->buffer_0.pos)
        return FGL_STATUS_ERROR_ARGS;
    
    for (size_t i = 0; i < fgl->num_verts; i++) {
        fgl->buffer_0.pos[4 * i + 0] = vertex_positions[3 * i + 0];
        fgl->buffer_0.pos[4 * i + 1] = vertex_positions[3 * i + 1];
        fgl->buffer_0.pos[4 * i + 2] = vertex_positions[3 * i + 2];
        fgl->buffer_0.pos[4 * i + 3] = 1.0f;
    }

    return FGL_STATUS_SUCCESS;
}

status_t fgl_load_verts_pos_xyzw(fgl_t * fgl, const float * vertex_positions) {
    if (!fgl || !vertex_positions || !fgl->buffer_0.pos)
        return FGL_STATUS_ERROR_ARGS;

    memcpy(fgl->buffer_0.pos, vertex_positions, fgl->num_verts * 4 * sizeof(*fgl->buffer_0.pos));

    return FGL_STATUS_SUCCESS;
}

// normals

status_t fgl_load_verts_norm_xyz(fgl_t * fgl, const float * vertex_normals) {
    if (!fgl || !vertex_normals || !fgl->buffer_0.norm)
        return FGL_STATUS_ERROR_ARGS;
    
    for (size_t i = 0; i < fgl->num_verts; i++) {
        fgl->buffer_0.norm[4 * i + 0] = vertex_normals[3 * i + 0];
        fgl->buffer_0.norm[4 * i + 1] = vertex_normals[3 * i + 1];
        fgl->buffer_0.norm[4 * i + 2] = vertex_normals[3 * i + 2];
        fgl->buffer_0.norm[4 * i + 3] = 0.0f;
    }

    return FGL_STATUS_SUCCESS;
}

status_t fgl_load_verts_norm_xyzw(fgl_t * fgl, const float * vertex_normals) {
    if (!fgl || !vertex_normals || !fgl->buffer_0.norm)
        return FGL_STATUS_ERROR_ARGS;

    memcpy(fgl->buffer_0.norm, vertex_normals, fgl->num_verts * 4 * sizeof(*fgl->buffer_0.norm));

    return FGL_STATUS_SUCCESS;
}

// uv

status_t fgl_load_verts_uv(fgl_t * fgl, const float * vertex_uv) {
    if (!fgl || !vertex_uv || !fgl->buffer_0.uv)
        return FGL_STATUS_ERROR_ARGS;

    memcpy(fgl->buffer_0.uv, vertex_uv, fgl->num_verts * 2 * sizeof(*fgl->buffer_0.uv));

    return FGL_STATUS_SUCCESS;
}

// colors

status_t fgl_load_verts_col_rgb(fgl_t * fgl, const float * vertex_colors) {
    if (!fgl || !vertex_colors || !fgl->buffer_0.col)
        return FGL_STATUS_ERROR_ARGS;

    for (size_t i = 0; i < fgl->num_verts; i++) {
        fgl->buffer_0.col[4 * i + 0] = vertex_colors[3 * i + 0];
        fgl->buffer_0.col[4 * i + 1] = vertex_colors[3 * i + 1];
        fgl->buffer_0.col[4 * i + 2] = vertex_colors[3 * i + 2];
        fgl->buffer_0.col[4 * i + 3] = 1.0f;
    }

    return FGL_STATUS_SUCCESS;
}

status_t fgl_load_verts_col_rgba(fgl_t * fgl, const float * vertex_colors) {
    if (!fgl || !vertex_colors || !fgl->buffer_0.col)
        return FGL_STATUS_ERROR_ARGS;

    memcpy(fgl->buffer_0.col, vertex_colors, fgl->num_verts * 4 * sizeof(*fgl->buffer_0.col));

    return FGL_STATUS_SUCCESS;
}

// user defined attributes

// load 1 floating-point user-defined attribute at given offset [0,7]
status_t fgl_load_verts_usr(fgl_t * fgl, const float * vertex_attrib, const unsigned char offset) {
    if (!fgl || !vertex_attrib || !fgl->buffer_0.usr)
        return FGL_STATUS_ERROR_ARGS;
    if (offset >= 8)
        return FGL_STATUS_ERROR_ARGS;
    for (size_t i = 0; i < fgl->num_verts; i++) {
        fgl->buffer_0.usr[8 * i + offset] = vertex_attrib[i];
    }
    return FGL_STATUS_SUCCESS;
}

// load num_attribs number of floating-point user-defined attributes starting at given offset [0,8 - num_attribs]
status_t fgl_load_verts_usr_batch(fgl_t * fgl, const float * vertex_attrib, const unsigned char offset, const unsigned char num_attribs) {
    if (!fgl || !vertex_attrib || !fgl->buffer_0.usr)
        return FGL_STATUS_ERROR_ARGS;
    if (num_attribs > 8 || !num_attribs || offset > 8 - num_attribs)
        return FGL_STATUS_ERROR_ARGS;
    for (size_t i = 0; i < fgl->num_verts; i++) {
        for (size_t j = 0; j < num_attribs; j++) {
            fgl->buffer_0.usr[8 * i + offset + j] = vertex_attrib[num_attribs * i + j];
        }
    }
    return FGL_STATUS_SUCCESS;
}