#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm> // for std::max

#include "header.h"
using namespace classes;

int main() {
    const int width  = 400;
    const int height = 400;

    // 1. Setup Camera and Scene Objects
    Point4 camPos(0.f, 0.f, -3.f);
    Sphere sphere(Point4(0.f, 0.f, 5.f), 1.5f);

    // 2. Setup Light Source

    Point4 lightPos(5.f, 5.f, 0.f); // lumière à droite
    float lightIntensity = 1.0f;

    std::ofstream img("sphere_diffuse.ppm");
    img << "P3\n" << width << " " << height << "\n255\n";

    float viewportSize = 2.0f;

    for (int j = height - 1; j >= 0; --j) {
        for (int i = 0; i < width; ++i) {
            float u = (i + 0.5f) / width  * viewportSize - viewportSize / 2;
            float v = (j + 0.5f) / height * viewportSize - viewportSize / 2;

            Point4 pixelPos(u, v, 0.f);
            Vector4 dir = pixelPos - camPos;
            Ray ray(camPos, dir);

            Point4 hit = sphere.intersect(ray);

            if (hit.d != 0.f) {
                // --- DIFFUSE CALCULATION ---
                
                // A. Calculate the Normal at the hit point
                Vector4 N = sphere.normal(hit);
                // Normalize N (important for dot product)
                float magN = std::sqrt(N.dot3(N));
                Vector4 nUnit = N * (1.0f / magN);

                // B. Calculate Light Vector (from hit point to light)
                Vector4 L = lightPos - hit;
                float magL = std::sqrt(L.dot3(L));
                Vector4 lUnit = L * (1.0f / magL);

                // C. Calculate Dot Product (Lambert's Law)
                // dot product = cos(theta)
                float cosTheta = nUnit.dot3(lUnit);
                float diffuse = std::max(0.0f, cosTheta);

                // D. Apply to color (Red sphere)
                int r = static_cast<int>(255 * diffuse * lightIntensity);
                img << r << " 0 0 ";
            } else {
                img << "0 0 0 ";
            }
        }
        img << "\n";
    }

    img.close();
    std::cout << "Image generated: sphere_diffuse.ppm" << std::endl;
    return 0;
}