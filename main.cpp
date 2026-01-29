#include <fstream>
#include <iostream>
#include <cmath>
#include <vector>
#include <limits>
#include <algorithm>

#include "header.h"
using namespace classes;

struct SceneObject {
    Sphere s;
    Color col;
    float reflect; 
};

Color getSkyColor(const Vector4& dir) { return Color(0, 0, 0); }

bool trace(const Ray& ray, const std::vector<SceneObject>& scene, float& tNear, int& hitIndex, Point4& hitPoint) {
    tNear = std::numeric_limits<float>::max();
    hitIndex = -1;
    for (int i = 0; i < (int)scene.size(); ++i) {
        Point4 p = scene[i].s.intersect(ray);
        if (p.d > 0.001f) { 
            Vector4 diff = p - ray.origin;
            float dist = std::sqrt(diff.dot3(diff));
            if (dist < tNear) {
                tNear = dist;
                hitIndex = i;
                hitPoint = p;
            }
        }
    }
    return (hitIndex != -1);
}

Color castRay(const Ray& ray, const std::vector<SceneObject>& scene, const Point4& lightPos, int depth) {
    float tNear; int hitIdx; Point4 hitP;
    if (depth > 2 || !trace(ray, scene, tNear, hitIdx, hitP)) {
        return getSkyColor(ray.direction);
    }

    const SceneObject& obj = scene[hitIdx];
    Vector4 N = obj.s.normal(hitP);
    float magN = std::sqrt(N.dot3(N)); Vector4 nUnit = N * (1.f/magN);

    Vector4 L = lightPos - hitP;
    float distToLight = std::sqrt(L.dot3(L));
    Vector4 lUnit = L * (1.f/distToLight);
    
    Ray shadowRay(hitP + (nUnit * 0.05f), lUnit);
    float tShad; int idxShad; Point4 pShad;
    bool inShadow = trace(shadowRay, scene, tShad, idxShad, pShad) && (tShad < distToLight);

    float ambient = 0.1f; 
    float diffuse = inShadow ? 0.f : std::max(0.0f, nUnit.dot3(lUnit));
    
    float r = obj.col.r * (diffuse + ambient);
    float g = obj.col.g * (diffuse + ambient);
    float b = obj.col.b * (diffuse + ambient);

    if (obj.reflect > 0) {
        Vector4 I = ray.direction;
        float magI = std::sqrt(I.dot3(I)); Vector4 iUnit = I * (1.f/magI);
        Vector4 R = iUnit - nUnit * (2.f * iUnit.dot3(nUnit));
        Ray reflectRay(hitP + (nUnit * 0.05f), R);
        Color reflectCol = castRay(reflectRay, scene, lightPos, depth + 1);
        r += reflectCol.r * obj.reflect;
        g += reflectCol.g * obj.reflect;
        b += reflectCol.b * obj.reflect;
    }

    return Color((int)std::min(255.f, r), (int)std::min(255.f, g), (int)std::min(255.f, b));
}

int main() {
    const int width = 1280; 
    const int height = 720;

    Point4 camPos(0.f, 3.0f, -6.f); 
    Point4 lightPos(-17.f, 28.f, -16.f); 
    
    std::vector<SceneObject> scene;
    float groundRadius = 100000.0f;
    float floorY = -2.0f;

    scene.push_back({ Sphere(Point4(0.f, floorY - groundRadius, 0.f), groundRadius), Color(40, 40, 40), 0.1f });
    scene.push_back({ Sphere(Point4(0.f, floorY + 1.5f, 5.0f), 1.5f), Color(255, 0, 0), 0.3f });     
    scene.push_back({ Sphere(Point4(-3.5f, floorY + 1.0f, 4.0f), 1.0f), Color(255, 140, 0), 0.3f }); 
    scene.push_back({ Sphere(Point4(3.5f, floorY + 1.2f, 6.0f), 1.2f), Color(0, 206, 209), 0.3f }); 

    std::ofstream img("image.ppm");
    img << "P3\n" << width << " " << height << "\n255\n";

    for (int j = height - 1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {
            float u = ((float)i / width) * 4.0f - 2.0f;
            float v = ((float)j / height) * 2.25f - 1.125f; 

            Ray ray(camPos, Vector4(u, v - 0.7f, 3.0f));
            Color pixelCol = castRay(ray, scene, lightPos, 0);

            img << pixelCol.r << " " << pixelCol.g << " " << pixelCol.b << " ";
        }
        img << "\n";
    }

    img.close();
    return 0;
}