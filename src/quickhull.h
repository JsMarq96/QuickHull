#ifndef QUICKHULL_H_
#define QUICKHULL_H_

#include "math.h"
#include "geometry.h"
#include "mesh.h"
#include <cstdint>

struct sTruple {
    uint16_t p1 = 0;
    uint16_t p2 = 0;
    uint16_t p3 = 0;
};

struct sQuickHullGenerator {
    // Base geometry data
    sVector3   *points = NULL;
    bool       *is_inside_hull = NULL;
    uint16_t   *hull_face_id = NULL;
    uint16_t   point_count = 0;

    uint16_t   most_outside_indices[4] = {0, 0, 0, 0};


    // Quickhull data
    sPlane *faces = NULL;
    sTruple *faces_points = NULL;
    uint16_t faces_cout = 0;

    void create_from_mesh(const sMesh &mesh) {

    };

    void _load_mesh_data(const sMesh &mesh) {
        points = (sVector3*) malloc(sizeof(sVector3) * mesh.vertex_count);
        is_inside_hull = (bool*) malloc(sizeof(bool) * mesh.vertex_count);

        // NOTE: we use the size of the points as a MAX face count, even thought
        // they are going to be much less
        faces = (sPlane*) malloc(sizeof(sPlane) * mesh.vertex_count);
        faces_points = (sTruple*) malloc(sizeof(sTruple) * mesh.vertex_count);

        // Store the vertices
        for(point_count = 0; point_count < mesh.vertex_count; point_count++) {
            points[point_count] = mesh.vertices[point_count].vertex;
        }

        memset(is_inside_hull, false, point_count * sizeof(bool));
    }


    // Note: O(2N)
    void _compute_most_outside_indices(const bool is_center_on_zero) {
        sVector3 center = {0.0f, 0.0f};

        if (!is_center_on_zero) {
            for(uint16_t i = 0; i < point_count; i++) {
                center = center.sum(points[i]);
            }
            center = center.mult(1.0f / point_count);
        }

        float top_distances[4] = {0.0f, 0.0f, 0.0f, 0.0f};

        for(uint16_t i = 0; i < point_count; i++) {
            float distance = abs(points[i].subs(center).magnitude());
        }

        // Is this correct?? What is asked?
    }

    void clean() {
        free(faces);
        free(faces_points);
        //free(hull_face_id);
        free(is_inside_hull);
        free(points);
    }
};

#endif // QUICKHULL_H_
