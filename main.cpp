#include <iostream>
#include "header.h"

int main() {
    using namespace classes;

    std::cout << "===== TEST COLOR =====\n";
    Color black;
    Color red(255, 0, 0);
    Color weird(300, -20, 100); // clamp test

    std::cout << "black = (" << black.r << ", " << black.g << ", " << black.b << ")\n";
    std::cout << "red   = (" << red.r << ", " << red.g << ", " << red.b << ")\n";
    std::cout << "weird = (" << weird.r << ", " << weird.g << ", " << weird.b << ")\n\n";

    std::cout << "===== TEST VECTOR4 =====\n";
    Vector4 v1(1.0f, 2.0f, 3.0f);
    Vector4 v2(-1.0f, 0.5f, 4.0f);

    std::cout << "v1      = " << v1 << "\n";
    std::cout << "v2      = " << v2 << "\n";
    std::cout << "v1+v2   = " << (v1 + v2) << "\n";
    std::cout << "v1-v2   = " << (v1 - v2) << "\n";
    std::cout << "v1*2    = " << (v1 * 2.0f) << "\n\n";

    std::cout << "===== TEST POINT4 =====\n";
    Point4 p1(10.0f, 0.0f, 5.0f);
    Point4 p2 = p1 + v1;
    Vector4 dp = p2 - p1;

    std::cout << "p1      = " << p1 << "\n";
    std::cout << "p2=p1+v1= " << p2 << "\n";
    std::cout << "p2-p1   = " << dp << "\n\n";

    std::cout << "===== TEST IMAGE & PIXEL =====\n";
    Image img(4, 6); // h=4, w=6

    std::cout << "Initial image: " << img << "\n";

    img.set_pixel(Pixel(0, 0, red));
    img.set_pixel(Pixel(5, 0, Color(0, 255, 0)));
    img.set_pixel(Pixel(0, 3, Color(0, 0, 255)));
    img.set_pixel(Pixel(5, 3, Color(255, 255, 255)));
    img.set_pixel(Pixel(3, 2, Color(128, 128, 128)));

    // pixels hors image (doivent être ignorés)
    img.set_pixel(Pixel(-1, 0, red));
    img.set_pixel(Pixel(10, 10, red));

    std::cout << "After set_pixel: " << img << "\n";

    std::cout << "\n===== ALL TESTS DONE =====\n";
    const int width  = 800;
    const int height = 600;

    std::cout << "Creating image " << width << " x " << height << std::endl;

    Image nimg(height, width);

    // Dégradé horizontal + vertical
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {

            int r = (255 * x) / (width  - 1);   // gauche -> droite
            int g = (255 * y) / (height - 1);   // haut -> bas
            int b = 128;                        // constant

            nimg.set_pixel(Pixel(x, y, Color(r, g, b)));
        }
    }

    nimg.savePPM("big_test.ppm");

    std::cout << "Image saved as big_test.ppm" << std::endl;
    return 0;
}
