//
// Created by juaquin on 5/7/24.
//

#ifndef RAYTRACING_WEEKEND_MESH_H
#define RAYTRACING_WEEKEND_MESH_H

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "../ext/stb/stb_image.h"
#include "../rtweekend.h"
#include <vector>
#include <filesystem>
#include <unordered_map>
#include "tiny_obj_loader.h"
#include "primitive.h"
#include "../material/material.h"

using namespace std;


class mesh: public primitive {
private:
    struct metadata_triangle {
        tinyobj::index_t vertex[3];
        int id_mat;

        metadata_triangle(tinyobj::index_t v1, tinyobj::index_t v2, tinyobj::index_t v3, const
        int& mat_id):
        id_mat(mat_id) {
            vertex[0] = v1;
            vertex[1] = v2;
            vertex[2] = v3;
        }
    };

    struct textureData {
        unsigned char *imageData;
        int width{}, height{}, channels{};
        textureData(const string& path) {
            this->imageData = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);
            if (!imageData) {
                cerr << "Error: Can't load the image " << path << endl;
            }
        }

        ~textureData() {
            stbi_image_free(imageData);
        }
    };

    point3 origin{0, 0, 0};
    float resize_factor = 1;
    std::vector<metadata_triangle> meta;
    shared_ptr<material> default_mat;

    std::unordered_map<string, shared_ptr<textureData>> textures;
    filesystem::path work_dir;

    void initialize(const vector<tinyobj::shape_t>& shapes) {
        meta.clear();
        for (auto& shape: shapes) {
            for (int i = 0; i < shape.mesh.indices.size()/3; i++) {
                meta.emplace_back(shape.mesh.indices.at(3*i), shape.mesh.indices.at(3*i+1), shape.mesh.indices.at
                (3*i+2),  shape.mesh.material_ids.at(i));
            }
        }
        // TODO: support textures
        for (auto& material: all_material_info) {
            if (!material.diffuse_texname.empty() and this->textures.find(material.diffuse_texname) == this->textures
            .end()) {
                this->textures.insert({material.diffuse_texname, make_shared<textureData>(this->work_dir / material
                .diffuse_texname)});
            }
        }
        clog << "\nTextures:\n";
        for (auto& [k, v]: this->textures) {
            clog << '\t' << k << endl;
        }
    }


    shared_ptr<material> build_material(const tinyobj::material_t& mat) {
        const float specularThreshold = 0.5f;  // Umbral para considerar un material como metálico
        const float shininessThreshold = 50.0f; // Umbral de brillo para considerar un material como metálico
        const float emissionThreshold = 0.1f;  // Umbral para considerar un material como emisivo

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
            return make_shared<lambertian>(color(mat.diffuse[0]*mat.ambient[0], mat.diffuse[1]*mat.ambient[1], mat
            .diffuse[2]*mat.ambient[2]));
        }
        else {
            return make_shared<metal>(color(mat.diffuse[0]*mat.ambient[0], mat.diffuse[1]*mat.ambient[1], mat.diffuse[2]*mat.ambient[2]), mat.specular[0]);
        }
    }

    void _log_mesh(const vector<tinyobj::shape_t>& shapes) const {
        clog << "Loaded OBJ" << endl;
        std::clog << "shapes: " << shapes.size() << std::endl;
        for (auto& shape: shapes) {
            clog << shape.name << endl;
            clog << "\ttriangles: " << shape.mesh.indices.size()/3 << endl;
        }
        clog << "materials: " << materials.size() << endl;
    }
    void build_materials(const vector<tinyobj::material_t>& mats) {
        this->materials.reserve(mats.size());
        for (int i = 0; i < mats.size(); i++) {
            materials.emplace_back(build_material(mats.at(i)));
        }
    }
public:
    tinyobj::attrib_t attrib;
    vector<shared_ptr<struct material>> materials;
    vector<tinyobj::material_t> all_material_info;

    mesh(const std::string &filename, const shared_ptr<material>& mati = make_shared<lambertian>(point3(1, 0.5, 0.5))):
    default_mat(mati) {
        this->work_dir = filesystem::path(filename).parent_path();
        vector<tinyobj::shape_t> shapes;
        {
            std::string err;
            bool status = tinyobj::LoadObj(&this->attrib, &shapes, &all_material_info, nullptr, &err,
                                           filename
                                                   .c_str(), this->work_dir.c_str());
            if (!err.empty()) {
                std::cerr << "Failed to load: " << err << std::endl;
                exit(1);
            }
            if (!status) exit(1);
        }
        build_materials(all_material_info);
        _log_mesh(shapes);
        initialize(shapes);
    }

    void set_origin(const point3& point) {
        for (int i = 0; i < this->attrib.vertices.size()/3; i++) {
            this->attrib.vertices.at(3*i) += point.e[0] - this->origin.e[0];
            this->attrib.vertices.at(3*i + 1) += point.e[1] - this->origin.e[1];
            this->attrib.vertices.at(3*i + 2) += point.e[2] - this->origin.e[2];
        }
        this->origin = point;
    }

    void rescale(const float& factor) {
        for (float & vertice : this->attrib.vertices) {
            vertice *= factor / this->resize_factor;
        }
        this->resize_factor = factor;
    }

    vec3 get_color(const string& name, const float& _u, const float& _v) const {
        vec3 color;
        const auto& texture = this->textures.find(name)->second;
        const int u = _u*texture->width;
        const int v = texture->height - _v*texture->height;

        const int index = (v*texture->height + u)*texture->channels;
        color.e[0] = texture->imageData[index] / 255.;
        color.e[1] = texture->imageData[index + 1] / 255.;
        color.e[2] = texture->imageData[index + 2] / 255.;
        return color;
    }

    void set_material(hit_record& rec, const unsigned int& primID, const float& u, const float& v) override {
        auto triangle = meta.at(primID);
        if (triangle.vertex[0].texcoord_index != -1 and this->textures.size() != 0) {
                const float z = 1 - u - v;
                auto t1u = attrib.texcoords.at(2*triangle.vertex[0].texcoord_index);
                auto t1v = attrib.texcoords.at(2*triangle.vertex[0].texcoord_index + 1);

                auto t2u = attrib.texcoords.at(2*triangle.vertex[1].texcoord_index);
                auto t2v = attrib.texcoords.at(2*triangle.vertex[1].texcoord_index + 1);

                auto t3u = attrib.texcoords.at(2*triangle.vertex[2].texcoord_index);
                auto t3v = attrib.texcoords.at(2*triangle.vertex[2].texcoord_index + 1);
                const auto ansu = t1u*u + t2u*v + t3u*z;
                const auto ansv = t1v*u + t2v*v + t3v*z;
                const auto c = all_material_info.at(triangle.id_mat).diffuse_texname;
                const auto diffuse = this->get_color(c, ansu, ansv);
                rec.mat = make_shared<lambertian>(diffuse);
            }
        else {
            rec.mat = materials.at(triangle.id_mat);
        }
    }

    vector<unsigned int> get_indices() {
        vector<unsigned int> ans;
        ans.reserve(this->meta.size()*3);
        for (const auto& m: meta) {
            ans.emplace_back(m.vertex[0].vertex_index);
            ans.emplace_back(m.vertex[1].vertex_index);
            ans.emplace_back(m.vertex[2].vertex_index);
        }
        return ans;
    }
};

#endif //RAYTRACING_WEEKEND_MESH_H
