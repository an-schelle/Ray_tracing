#include "header.h"
#include <fstream>
#include <vector>
#include <stdexcept>

namespace classes{

// fonction pour vérifier la validité des valeurs
static int clamp255(int v) {
    if (v < 0)   return 0;
    if (v > 255) return 255;
    return v;
}

// constructeur par défaut : noir
Color::Color() : r(0), g(0), b(0) {}

//Constructeur avec paramètres (valeurs tronquées)
Color::Color(int r_, int g_, int b_)
    : r(clamp255(r_)),
      g(clamp255(g_)),
      b(clamp255(b_)) {}


Pixel::Pixel() : x(0), y(0), c(Color()) {}
Pixel::Pixel(int x_, int y_, Color c_) : x(x_), y(y_), c(c_) {}


Vector4::Vector4() : x(0), y(0), z(0), d(0) {}

Vector4::Vector4(float x_, float y_, float z_)
    : x(x_), y(y_), z(z_), d(0.0f) {}

Vector4 Vector4::operator+(const Vector4& v) const {
    return Vector4(x + v.x, y + v.y, z + v.z);
}

Vector4 Vector4::operator-(const Vector4& v) const {
    return Vector4(x - v.x, y - v.y, z - v.z);
}

Vector4 Vector4::operator*(float s) const {
    return Vector4(x * s, y * s, z * s);
}

float Vector4::dot3(const Vector4& other) const {// produit scalaire
    return x*other.x + y*other.y + z*other.z;
}

std::ostream& operator<<(std::ostream& os, const Vector4& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.d << ")";
    return os;
}



Point4::Point4() : x(0), y(0), z(0), d(1) {}

Point4::Point4(float x_, float y_, float z_)
    : x(x_), y(y_), z(z_), d(1.0f) {}

Point4 Point4::operator+(const Vector4& v) const {
    return Point4(x + v.x, y + v.y, z + v.z);
}

Vector4 Point4::operator-(const Point4& p) const {
    return Vector4(x - p.x, y - p.y, z - p.z);
}

std::ostream& operator<<(std::ostream& os, const Point4& p) {
    os << "(" << p.x << ", " << p.y << ", " << p.z << ", " << p.d << ")";
    return os;
}

Image::Image() : h(0), w(0) {}
Image::Image(int h_,  int w_) : h(h_), w(w_) {
     pixels.resize(static_cast<size_t>(w) * static_cast<size_t>(h));//évite bugs 
}
void Image::set_pixel(const Pixel& p) {
    int x = static_cast<int>(p.x);
    int y = static_cast<int>(p.y);

    //Vérification
    if (x < 0 || x >= w || y < 0 || y >= h) {
        return; //cas des pixels hors de l'image
    }
    //conversion en 1D
    int index = y * w + x;
    pixels[index] = p;
}


void Image::savePPM(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("savePPM: impossible d'ouvrir le fichier");
    }

    // PPM P3 (ASCII)
    file << "P3\n";
    file << w << " " << h << "\n";
    file << "255\n";

    // on suppose que pixels[y*w + x] contient le Pixel du (x,y), et qu'il est bien rgb
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const Pixel& p = pixels[static_cast<size_t>(y) * static_cast<size_t>(w)
                                  + static_cast<size_t>(x)];

            file << clamp255(p.c.r) << " "
                 << clamp255(p.c.g) << " "
                 << clamp255(p.c.b) << "  ";
        }
        file << "\n";
    }
}



std::ostream& operator<<(std::ostream& os, const Image& img) {
    os << "Image(" << img.w << " x " << img.h
       << ", pixels=" << img.pixels.size() << ")";
    return os;
}



    Ray::Ray()  : origin(Point4()), direction(Vector4()) {}
    Ray::Ray(const Point4& o, const Vector4& d)  : origin(o), direction(d) {}


Ray::Ray() : origin(), direction() {}

Ray::Ray(const Point4& o, const Vector4& d)
    : origin(o), direction(d) {}


//Texture_Material::Texture_Material()
   // : kd(0.0f), ks(0.0f) {}




Uniform_texture::Uniform_texture(const Color& d, const Color& s)
{
    kd = d;
    ks = s;
}



PointLight::PointLight(const Point4& position, const Color& intensity)
    : pos(position), I(intensity) {}


Camera::Camera()
    : C(), P(), up(), alpha(0.0f), beta(0.0f), zmin(0.0f) {}

Camera::Camera(Point4 c, Point4 p,float alpha_, float beta_,float zmin_) :
    C(c), P(p), alpha(alpha_),beta(beta_),zmin(zmin_) {}



Scene::Scene()
    : objects(), lights(), cam() {}

Sphere::Sphere(Point4 c,float r) : centre(c),rayon(r) {}

Point4 Sphere::intersect(const Ray& ray) 
{
    // Convention: pas d'intersection => on renvoie un "point" avec d=0
    Point4 noHit(0.f, 0.f, 0.f);
    noHit.d = 0.f;

    const Vector4 oc = ray.origin - centre;          // origin - center
    const Vector4& dir = ray.direction;

    const float a = dir.dot3(dir);
    const float b = 2.0f * oc.dot3(dir);
    const float c = oc.dot3(oc) - rayon*rayon;

    // Cas dégénéré: direction nulle
    const float eps = 1e-8f;
    if (std::abs(a) < eps) return noHit;

    const float disc = b*b - 4.0f*a*c;
    if (disc < 0.0f) return noHit;

    const float sqrtDisc = std::sqrt(disc);

    // Deux solutions
    const float t1 = (-b - sqrtDisc) / (2.0f * a);
    const float t2 = (-b + sqrtDisc) / (2.0f * a);

    // Rayon => on garde le plus petit t >= 0
    float t = std::numeric_limits<float>::infinity();
    if (t1 >= 0.0f) t = t1;
    if (t2 >= 0.0f) t = std::min(t, t2);
    if (!std::isfinite(t)) return noHit;

    Point4 hit = ray.origin + (ray.direction * t);
    // hit.d restera 1 via ton constructeur Point4 + Vector4 (car Vector4.d=0)
    return hit;
    //résoudre selon t : origine rayon + t*vecteur = point du cerlce (à r du centre )
}

Vector4 Sphere::normal(const Point4& p){
    Vector4 norm = p - centre;
    return norm;
}

Texture_Material Sphere::TextureAt(const Point4& p){
    Color red(255, 0, 0);
    Uniform_texture Uni(red, red);
    return Uni;
}   

}



