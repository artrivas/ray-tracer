//
// Created by juaquin on 5/7/24.
//

#ifndef RAYTRACING_WEEKEND_MESH_H
#define RAYTRACING_WEEKEND_MESH_H

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "../ext/stb/stb_image.h"
#include "../hittable/hitabble.h"
#include "../rtweekend.h"
#include <vector>
#include <filesystem>
#include "../ext/tiny_obj_loader/tiny_obj_loader.h"
#include "../primitives/triangle.h"
#include "../material/material.h"
#include "../accelerator/bvh.h"

typedef bvh::v2::Vec<float, 2> Vec2;

class mesh : public hittable {
private:
    struct metadata_triangle {
        tinyobj::index_t* vertex[3];
        int material_id;

        metadata_triangle(tinyobj::index_t* v1, tinyobj::index_t* v2, tinyobj::index_t* v3, const int mat_id):
        material_id(mat_id) {
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

    shared_ptr<material> mat = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    point3 origin{0, 0, 0};
    float resize_factor = 1;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<metadata_triangle> meta;

    std::unordered_map<string, shared_ptr<textureData>> textures;
    filesystem::path work_dir;
    // BVH
    shared_ptr<bvhTree> bvh;

    void initialize() {
        meta.clear();
        for (auto& shape: shapes) {
            for (int i = 0; i < shape.mesh.indices.size()/3; i++) {
                meta.emplace_back(&shape.mesh.indices.at(3*i), &shape.mesh.indices.at(3*i+1), &shape.mesh.indices.at
                (3*i+2), shape.mesh.material_ids.at(i));
            }
        }

        for (auto& material: this->materials) {
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

public:
    mesh(const std::string &filename) {
        std::string err;
        this->work_dir = filesystem::path(filename).parent_path();
        bool status = tinyobj::LoadObj(&this->attrib, &this->shapes, &this->materials, nullptr, &err,
                                       filename
                                               .c_str(), this->work_dir.c_str());
        if (!err.empty()) {
            std::cerr << "Failed to load: " << err << std::endl;
            exit(1);
        }
        if (!status) exit(1);

        clog << "Loaded OBJ" << endl;
        std::clog << "shapes: " << this->shapes.size() << std::endl;
        for (auto& shape: this->shapes) {
            clog << shape.name << endl;
            clog << "\ttriangles: " << shape.mesh.indices.size()/3 << endl;
        }
        clog << "materials: " << this->materials.size() << endl;
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
        for (int i = 0; i < this->attrib.vertices.size(); i++) {
            this->attrib.vertices.at(i) *= factor / this->resize_factor;
        }
        this->resize_factor = factor;
    }

    void for_each(auto fn) {
        for (auto& shape : shapes) {
            for (int i = 0; i < shape.mesh.indices.size() / 3; i++) {
                const int &idx0 = shape.mesh.indices.at(3 * i).vertex_index;
                const int &idx1 = shape.mesh.indices.at(3 * i + 1).vertex_index;
                const int &idx2 = shape.mesh.indices.at(3 * i + 2).vertex_index;
                point3 vtx1{attrib.vertices.at(3 * idx0), attrib.vertices.at(3 * idx0 + 1),
                            attrib.vertices.at(3 * idx0 + 2)};
                point3 vtx2{attrib.vertices.at(3 * idx1), attrib.vertices.at(3 * idx1 + 1),
                            attrib.vertices.at(3 * idx1 + 2)};
                point3 vtx3{attrib.vertices.at(3 * idx2), attrib.vertices.at(3 * idx2 + 1),
                            attrib.vertices.at(3 * idx2 + 2)};
                fn(vtx1, vtx2, vtx3);
            }
        }
    }

    vec3 get_color(const string& name, const Vec2& vec) const {
        vec3 color;
        const auto& texture = this->textures.find(name)->second;
        const int u = vec.values[0]*texture->width;
        const int v = texture->height - vec.values[1]*texture->height;

        const int index = (v*texture->height + u)*texture->channels;
        color.e[0] = texture->imageData[index] / 255.;
        color.e[1] = texture->imageData[index + 1] / 255.;
        color.e[2] = texture->imageData[index + 2] / 255.;
        return color;
    }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
        auto ray = Ray{
                convert::to(r.origin()),
                convert::to(r.direction()),
                static_cast<float>(ray_t.min),
                static_cast<float>(ray_t.max),
        };
        float u, v;
        size_t prim_id = this->bvh->hit(ray, rec, u, v);

        if (prim_id != std::numeric_limits<size_t>::max()) {
            auto t = bvh->precomputed.at(prim_id);
            rec.t = ray.tmax;
            // Get data about the intersected triangle
            auto triangle_data = this->meta.at(this->bvh->bvh->prim_ids[prim_id]);
            if (triangle_data.vertex[0]->texcoord_index != -1) {
                // Texture Mapping
                const float z = 1 - u - v;
                const auto t1 = Vec2{attrib.texcoords.at(2*triangle_data.vertex[0]->texcoord_index),
                                                 attrib.texcoords.at(2*triangle_data.vertex[0]->texcoord_index + 1)};

                const auto t2 = Vec2{attrib.texcoords.at(2*triangle_data.vertex[1]->texcoord_index),
                                                 attrib.texcoords.at(2*triangle_data.vertex[1]->texcoord_index + 1)};

                const auto t3 = Vec2{attrib.texcoords.at(2*triangle_data.vertex[2]->texcoord_index),
                                                 attrib.texcoords.at(2*triangle_data.vertex[2]->texcoord_index + 1)};
                const auto ans = t1*u + t2*v + t3*z;
                const auto c = materials.at(triangle_data.material_id).diffuse_texname;
                const auto diffuse = this->get_color(c, ans);
                rec.mat = make_shared<lambertian>(diffuse);
            }
            else if (triangle_data.material_id != -1) {
                auto c = materials.at(triangle_data.material_id).diffuse;
                rec.mat = make_shared<lambertian>(color(c[0], c[1], c[2]));
            }
            else {
                rec.mat = make_shared<dielectric>(0.5);
            }
            rec.p = r.at(rec.t);
            vec3 outward_normal = convert::from(normalize(t.n));
            rec.set_face_normal(r, outward_normal);
            return true;
        } else return false;

    }

    void build() {
        std::vector<Tri> tris;
        auto lm = [&tris, this](const point3 &vtx1, const point3 &vtx2, const point3 &vtx3) {
            tris.emplace_back(convert::to(vtx1), convert::to(vtx2), convert::to(vtx3));
        };
        // fill tris with the data
        this->for_each(lm);

        // Prepare to have the material_id
        this->initialize();
        this->bvh = make_shared<bvhTree>(tris);
    }
};

#endif //RAYTRACING_WEEKEND_MESH_H
