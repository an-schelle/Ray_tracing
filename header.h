#ifndef HEADER_H
#define HEADER_H
#include <iostream>
#include <vector>
#include <string>
#include <memory> // Ajouté pour unique_ptr

namespace classes {



struct Color {
    int r, g, b;
    Color();                              
    Color(int r_, int g_, int b_);       
};

class Vector4 {
public:
    float x, y, z, d;

    Vector4();
    Vector4(float x, float y, float z);

    Vector4 operator+(const Vector4& v) const;
    Vector4 operator-(const Vector4& v) const;
    Vector4 operator*(float s) const;

    float dot3(const Vector4& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Vector4& v);
};


class Point4 {
public:
    float x, y, z, d;

    Point4();
    Point4(float x, float y, float z);

    Point4 operator+(const Vector4& v) const;
    Vector4 operator-(const Point4& p) const;

    friend std::ostream& operator<<(std::ostream& os, const Point4& p);
};

class Pixel {
public: 
    int x,y;
    Color c;
    Pixel();
    Pixel(int x_, int y_, Color c);
};



class Image {
public:
    int h, w;
    std::vector<Pixel> pixels; // taille = w*h

    Image();
    Image(int h_, int w_);
    void set_pixel(const Pixel& p);
    void savePPM(const std::string& filename) const;

    friend std::ostream& operator<<(std::ostream& os, const Image& img);
};

class Ray {
public:
    Point4 origin;    // point de départ du rayon
    Vector4 direction; // un vecteur

    Ray();
    Ray(const Point4& o, const Vector4& d);
};

class Texture_Material{
    public:
    Color kd;
    Color ks;// coeff séculaire
    virtual ~Texture_Material() = default; // Ajouté pour la hiérarchie
    virtual Texture_Material Texturevirtu(const Point4& p);
};

class Uniform_texture : public Texture_Material{
    public:
    Color kd;
    Color ks;// coeff séculaire
    Uniform_texture(const Color& d, const Color& s);//constructeur
    Texture_Material Texturevirtu(const Point4& p) override;// ici va juste renvoyer la texture elle même
};

class Light {
public:
    virtual ~Light() = default;// destructeur

    virtual Point4 position() const = 0; //Position de la lumière
    virtual Color intensity() const = 0;   //Intensité  de la lumière
};

class PointLight : public Light {// héritage
public:
    Point4 pos;
    Color I;
    PointLight(const Point4& position, const Color& intensity);


    Point4 position() const override;
    Color intensity() const override;
};

class Camera {
    public:
    Point4 C; // le centre
    Point4 P; // le point de mire
    Vector4 up; 
    float alpha;
    float beta;
    float zmin;

    Camera();
    Camera (Point4 c, Point4 p,float alpha_, float beta_,float zmin_);
};






class Object {//pour la scène
public:
    Texture_Material texture;
    virtual ~Object() = default;

    
    virtual Point4 intersect(const Ray& ray) const = 0; 
    // Renvoie le point d'intersection, s'il n'existe pas on renvoie un point avec d=0

    
    virtual Vector4 normal(const Point4& p) const = 0; //normale en un point

    // Couleur/ matériau au point p (simplifié)
    virtual Texture_Material TextureAt(const Point4& p) const = 0;
};

class Sphere : public Object{
    public : 
        Point4 centre;
        float rayon;
        Sphere(Point4 c,float r);

        Point4 intersect(const Ray& ray) const override;
        Vector4 normal(const Point4& p) const override;
        Texture_Material TextureAt(const Point4& p) const override;


};

class Scene {
    public: 
    Scene();
    std::vector<std::unique_ptr<Object>> objects;//unique_ptr évite les bugs
    std::vector<std::unique_ptr<Light>>  lights;
    Camera cam;

};

}

#endif