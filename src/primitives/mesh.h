//
// Created by juaquin on 5/7/24.
//

#ifndef RAYTRACING_WEEKEND_MESH_H
#define RAYTRACING_WEEKEND_MESH_H

#define TINYOBJLOADER_IMPLEMENTATION

#include "../rtweekend.h"
#include <vector>
#include <filesystem>
#include <unordered_map>
#include "tiny_obj_loader.h"
#include "primitive.h"
#include "../material/material.h"
#include "../texture/texture.h"

using namespace std;


class mesh : public primitive {
private:
    struct metadata_triangle {
        tinyobj::index_t vertex[3];
        int id_mat;

        metadata_triangle(tinyobj::index_t v1, tinyobj::index_t v2, tinyobj::index_t v3, const
        int &mat_id) :
                id_mat(mat_id) {
            vertex[0] = v1;
            vertex[1] = v2;
            vertex[2] = v3;
        }
    };

    point3 origin{0, 0, 0};
    float resize_factor = 1;
    std::vector<metadata_triangle> meta;
    shared_ptr<material> default_mat;

    std::unordered_map<string, shared_ptr<texture_image>> textures;
    filesystem::path work_dir;

    void initialize(const vector<tinyobj::shape_t> &shapes) {
        meta.clear();
        for (auto &shape: shapes) {
            for (int i = 0; i < shape.mesh.indices.size() / 3; i++) {
                meta.emplace_back(shape.mesh.indices.at(3 * i), shape.mesh.indices.at(3 * i + 1), shape.mesh.indices.at
                        (3 * i + 2), shape.mesh.material_ids.at(i));
            }
        }
        clog << "\nTextures:\n";
        for (auto &[k, v]: this->textures) {
            clog << '\t' << k << endl;
        }
    }


    shared_ptr<material> build_material(const tinyobj::material_t &mat) {
        const float specularThreshold = 0.5f;  // Umbral para considerar un material como metálico
        const float shininessThreshold = 50.0f; // Umbral de brillo para considerar un material como metálico
        const float emissionThreshold = 0.1f;  // Umbral para considerar un material como emisivo

        shared_ptr<texture_image> text;
        // Check if it has an associated texture
        if (!mat.diffuse_texname.empty()) {
            // check if the texture was already loaded
            auto path_abs = this->work_dir / mat.diffuse_texname;
            if (this->textures.find(path_abs.string()) == this->textures.end()) {
                text = make_shared<texture_image>(path_abs.string());
                textures[path_abs.string()] = text;
            } else
                text = textures[path_abs.string()];
        }

        bool isLambertian = (mat.specular[0] < specularThreshold &&
                             mat.specular[1] < specularThreshold &&
                             mat.specular[2] < specularThreshold &&
                             mat.shininess < shininessThreshold);

        bool isEmissive = (mat.emission[0] > emissionThreshold ||
                           mat.emission[1] > emissionThreshold ||
                           mat.emission[2] > emissionThreshold);

        bool isDielectric = (mat.ior > 1.0f);  // Índice de refracción mayor que el aire
        if (isEmissive) {
            return make_shared<diffuse_light>(color(mat.emission));
        }
        if (isDielectric) {
            return make_shared<dielectric>(mat.ior);
        }
        if (isLambertian) {
            if (!text)
                return make_shared<lambertian>(color(mat.diffuse[0] * mat.ambient[0], mat.diffuse[1] * mat.ambient[1], mat.diffuse[2] *
                mat.ambient[2]));
            else
                return make_shared<lambertian>(text);
        } else {
            if (!text)
                return make_shared<metal>(color(mat.diffuse[0] * mat.ambient[0], mat.diffuse[1] * mat.ambient[1],
                                            mat.diffuse[2] * mat.ambient[2]), 1 - mat.specular[0]);
            else
                return make_shared<metal>(text, 1 - mat.specular[0]);
        }
    }

    void _log_mesh(const vector<tinyobj::shape_t> &shapes) const {
        clog << "Loaded OBJ" << endl;
        std::clog << "shapes: " << shapes.size() << std::endl;
        for (auto &shape: shapes) {
            clog << shape.name << endl;
            clog << "\ttriangles: " << shape.mesh.indices.size() / 3 << endl;
        }
        clog << "materials: " << materials.size() << endl;
    }

    void build_materials(const vector<tinyobj::material_t> &mats) {
        this->materials.reserve(mats.size());
        for (int i = 0; i < mats.size(); i++) {
            materials.emplace_back(build_material(mats.at(i)));
        }
    }

public:
    tinyobj::attrib_t attrib;
    vector<shared_ptr<struct material>> materials;

    mesh(const std::string &filename, const shared_ptr<material> &mati = make_shared<lambertian>(point3(1, 0.5, 0.5))) :
            default_mat(mati) {
        this->work_dir = filesystem::path(filename).parent_path();
        vector<tinyobj::material_t> mats;
        vector<tinyobj::shape_t> shapes;
        {
            std::string err;
            bool status = tinyobj::LoadObj(&this->attrib, &shapes, &mats, nullptr, &err,
                                           filename
                                                   .c_str(), this->work_dir.c_str());
            if (!err.empty()) {
                std::cerr << "Failed to load: " << err << std::endl;
                exit(1);
            }
            if (!status) exit(1);
        }
        build_materials(mats);
        _log_mesh(shapes);
        initialize(shapes);
    }

    void set_origin(const point3 &point) {
        for (int i = 0; i < this->attrib.vertices.size() / 3; i++) {
            this->attrib.vertices.at(3 * i) += point.e[0] - this->origin.e[0];
            this->attrib.vertices.at(3 * i + 1) += point.e[1] - this->origin.e[1];
            this->attrib.vertices.at(3 * i + 2) += point.e[2] - this->origin.e[2];
        }
        this->origin = point;
    }

    void rescale(const float &factor) {
        for (float &vertice: this->attrib.vertices) {
            vertice *= factor / this->resize_factor;
        }
        this->resize_factor = factor;
    }

    void set_material(hit_record &rec, const unsigned int &primID, const float &u, const float &v) override {
        auto triangle = meta.at(primID);
        if (triangle.vertex[0].texcoord_index != -1) {
            const float z = 1 - u - v;
            auto t1u = attrib.texcoords.at(2 * triangle.vertex[0].texcoord_index);
            auto t1v = attrib.texcoords.at(2 * triangle.vertex[0].texcoord_index + 1);

            auto t2u = attrib.texcoords.at(2 * triangle.vertex[1].texcoord_index);
            auto t2v = attrib.texcoords.at(2 * triangle.vertex[1].texcoord_index + 1);

            auto t3u = attrib.texcoords.at(2 * triangle.vertex[2].texcoord_index);
            auto t3v = attrib.texcoords.at(2 * triangle.vertex[2].texcoord_index + 1);
            
            rec.u = t1u * u + t2u * v + t3u * z;
            rec.v = t1v * u + t2v * v + t3v * z;

            long dec_u = (long) rec.u;
            long dec_v = (long) rec.v;
            rec.u = (float) rec.u - dec_u;
            rec.v = (float) rec.v - dec_v;
        }
        rec.mat = triangle.id_mat == -1? default_mat: materials.at(triangle.id_mat);
    }

    vector<unsigned int> get_indices() {
        vector<unsigned int> ans;
        ans.reserve(this->meta.size() * 3);
        for (const auto &m: meta) {
            ans.emplace_back(m.vertex[0].vertex_index);
            ans.emplace_back(m.vertex[1].vertex_index);
            ans.emplace_back(m.vertex[2].vertex_index);
        }
        return ans;
    }
};

#endif //RAYTRACING_WEEKEND_MESH_H
