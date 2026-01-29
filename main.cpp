#include <fstream>
#include <iostream>
#include <cmath>

#include "header.h"   // tes classes : Point4, Vector4, Ray, Sphere
using namespace classes;

int main() {
    const int width  = 400;
    const int height = 400;

    // ===== Caméra =====
    Point4 camPos(0.f, 0.f, -3.f);

    // ===== Sphère =====
    Sphere sphere(
        Point4(0.f, 0.f, 5.f), // centre
        1.0f                   // rayon
    );

    // ===== Image PPM =====
    std::ofstream img("sphere.ppm");
    img << "P3\n" << width << " " << height << "\n255\n";

    // Plan image à z = 0
    float viewportSize = 2.0f;

    for (int j = height - 1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {

            // Coordonnées normalisées [-1,1]
            float u = (i + 0.5f) / width  * viewportSize - viewportSize / 2;
            float v = (j + 0.5f) / height * viewportSize - viewportSize / 2;

            Point4 pixel(u, v, 0.f);

            Vector4 dir = pixel - camPos;

            Ray ray(camPos, dir);

            Point4 hit = sphere.intersect(ray);

            if (hit.d != 0.f) {
                // Rouge
                img << "255 0 0 ";
            } else {
                // Noir
                img << "0 0 0 ";
            }
        }
        img << "\n";
    }

    img.close();
    std::cout << "Image generee : sphere.ppm\n";
    return 0;
}
