#ifndef QUICKHULL_H_
#define QUICKHULL_H_

#include "math.h"
#include "geometry.h"
#include "mesh.h"
#include "vector.h"
#include <cstdint>

#define EPSILON 0.00001f

struct sTruple {
    uint16_t p1 = 0;
    uint16_t p2 = 0;
    uint16_t p3 = 0;
};

struct sDistanceTuple {
    uint16_t point = 0;
    float    distance = 0.0f;
};

struct sQuickHullGenerator {
    // Base geometry data
    sVector3   *points = NULL;
    bool       *is_inside_hull = NULL;
    uint16_t   *hull_face_id = NULL;
    uint16_t   point_count = 0;

    uint16_t   most_outside_indices[4] = {0, 0, 0, 0};
    sVector3   point_cloud_center = {};

    // Quickhull data
    sPlane *faces = NULL;
    sTruple *faces_points = NULL;
    sDistanceTuple *buffer_points = NULL;
    uint16_t faces_cout = 0;

    void create_from_mesh(const sMesh &mesh) {
        // Data setupt
        _load_mesh_data(mesh);

        // Create the basic tetrahedron
        _compute_most_outside_indices(false);
        _create_planes_from_outside_indices();
        _calculate_points_inside_hull();

        //  Iterate throught the points
        memset(buffer_points, 0, sizeof(sDistanceTuple) * faces_cout);
        for(uint16_t point_i = 0; point_i < point_count; point_i++) {
            sVector3 &point = points[point_i];

            float max_dist = faces[0].distance(point);
            uint16_t max_dist_face = 0;
            for(uint16_t face_i = 1; face_i < faces_cout; face_i++) {
                float tmp_dist = faces[face_i].distance(point);

                if (tmp_dist > max_dist) {
                    max_dist = tmp_dist;
                    max_dist_face = face_i;
                }
            }
        }
    };

    void _load_mesh_data(const sMesh &mesh) {
        points = (sVector3*) malloc(sizeof(sVector3) * mesh.vertex_count);
        is_inside_hull = (bool*) malloc(sizeof(bool) * mesh.vertex_count);

        buffer_points = (sDistanceTuple*) malloc(sizeof(sDistanceTuple) * mesh.vertex_count);

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


    // Note: O(2N) if its not centered
    void _compute_most_outside_indices(const bool is_center_on_zero) {
        point_cloud_center = {0.0f, 0.0f, 0.0f};

        if (!is_center_on_zero) {
            for(uint16_t i = 0; i < point_count; i++) {
                point_cloud_center = point_cloud_center.sum(points[i]);
            }
            point_cloud_center = point_cloud_center.mult(1.0f / point_count);
        }

        float top_distances[4] = {0.0f, 0.0f, 0.0f, 0.0f};

        for(uint16_t i = 0; i < point_count; i++) {
            float distance = abs(points[i].subs(point_cloud_center).magnitude());

            for(uint8_t j = 0; j < 4; j++) {
                if (distance > top_distances[j]) {
                    top_distances[j] = distance;
                    most_outside_indices[j] = i;
                    is_inside_hull[i] = true;
                    break;
                }
            }
        }
    }

    void _create_planes_from_outside_indices() {
        faces_points[0] = {most_outside_indices[0], most_outside_indices[1], most_outside_indices[2]};
        faces[0].origin_point = points[most_outside_indices[0]].sum(points[most_outside_indices[1]]).sum(points[most_outside_indices[2]]);
        faces[0].origin_point = faces[0].origin_point.mult(1.0f / 0.3f);
        faces[0].normal = faces[0].origin_point.subs(point_cloud_center).normalize();

        faces_points[1] = {most_outside_indices[1], most_outside_indices[2], most_outside_indices[3]};
        faces[1].origin_point = points[most_outside_indices[1]].sum(points[most_outside_indices[2]]).sum(points[most_outside_indices[3]]);
        faces[1].origin_point = faces[1].origin_point.mult(1.0f / 0.3f);
        faces[1].normal = faces[1].origin_point.subs(point_cloud_center).normalize();

        faces_points[2] = {most_outside_indices[0], most_outside_indices[1], most_outside_indices[3]};
        faces[2].origin_point = points[most_outside_indices[0]].sum(points[most_outside_indices[1]]).sum(points[most_outside_indices[3]]);
        faces[2].origin_point = faces[2].origin_point.mult(1.0f / 0.3f);
        faces[2].normal = faces[2].origin_point.subs(point_cloud_center).normalize();

        faces_points[3] = {most_outside_indices[0], most_outside_indices[2], most_outside_indices[3]};
        faces[3].origin_point = points[most_outside_indices[0]].sum(points[most_outside_indices[2]]).sum(points[most_outside_indices[3]]);
        faces[3].origin_point = faces[3].origin_point.mult(1.0f / 0.3f);
        faces[3].normal = faces[3].origin_point.subs(point_cloud_center).normalize();

        faces_cout = 4;
    }

    void _calculate_points_inside_hull() {
        for(uint16_t i = 0; i < point_count; i++) {
            if (is_inside_hull[i]) {
                continue;
            }

            for(uint16_t face_i = 0; face_i < faces_cout; face_i++) {
                float point_distance = faces[face_i].distance(points[i]);
                if (point_distance <= EPSILON) {
                    is_inside_hull[i] = true;
                    break;
                }
            }
        }
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
