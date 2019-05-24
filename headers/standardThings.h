#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

struct Ray{
	glm::vec3 origin, direction;
	glm::vec3 invDirection;
	int sign[3];
	Ray(glm::vec3 o, glm::vec3 d) : origin(o), direction(d) {
		invDirection = glm::vec3(1.0f) / direction;
		sign[0] = (invDirection.x < 0); 
        sign[1] = (invDirection.y < 0); 
        sign[2] = (invDirection.z < 0); 
	}
	Ray() = default;
};

enum MaterialType{Standard, Reflective};

struct Texture{
	virtual ~Texture() = default;
	virtual glm::vec3 returnColor(float u, float v, glm::vec3 point) = 0;
};

struct SolidTexture : Texture{
	glm::vec3 color;
	SolidTexture(glm::vec3 c) : color(c){}
	glm::vec3 returnColor(float u, float v, glm::vec3 point){
		return color;
	}
};

struct CheckerTexture : Texture{
	glm::vec3 color, secondColor;
	int checkerScale;
	CheckerTexture(glm::vec3 c, glm::vec3 c2, int scale) : color(c), secondColor(c2), checkerScale(scale) {}
	glm::vec3 returnColor(float u, float v, glm::vec3 point){
		glm::vec2 uv = glm::vec2(glm::atan(point.x, point.z) / (2.0f * glm::pi<float>()) + 0.5f, glm::asin(point.y) / glm::pi<float>() + 0.5f); 
		
		return (int)(floor(16.0f * uv.x) + floor(10.0f * uv.y)) % 2 ? secondColor : color;
	}
};

struct PerlinTexture : Texture{
	float lac, gain;
	int octaves;
	PerlinTexture(float la, float ga, int oc) : lac(la), gain(ga), octaves(oc) {}
	glm::vec3 returnColor(float u, float v, glm::vec3 point){
		return glm::vec3(stb_perlin_turbulence_noise3(point.x, point.y, point.z, lac, gain, octaves));
	}
};

struct Material{
	Texture *diffuse;
	float reflectiveness;
	MaterialType type;
	Material(Texture *diff, float ref, MaterialType typ) : diffuse(diff), reflectiveness(ref), type(typ) {}
	Material() = default;
};

struct hitHistory{
	float dist;
	glm::vec3 hitPoint, normal;
	Material *obtMat;
	hitHistory(float d, glm::vec3 hP, glm::vec3 n, Material &oM) : dist(d), hitPoint(hP), normal(n), obtMat(&oM) {}
	hitHistory() = default;
};

struct Light{
	glm::vec3 color;
	float intensity;
	Light(glm::vec3 c, float i) : color(c), intensity(i) {}
	Light() = default;

	virtual void illuminate(glm::vec3 hitPoint, glm::vec3 &lightDirection, glm::vec3 &fullLight, float &lightDistance) = 0;
};

struct DirectionalLight : Light{
	glm::vec3 direction;
	DirectionalLight(glm::vec3 dir, glm::vec3 col, float intense) : direction(dir), Light(col, intense){}

	void illuminate(glm::vec3 hitPoint, glm::vec3 &lightDirection, glm::vec3 &fullLight, float &lightDistance){
		lightDirection = direction; 
        fullLight = color * intensity; 
        lightDistance = std::numeric_limits<float>::max();
	}
};

struct PointLight : Light{
	glm::vec3 pos;
	PointLight(glm::vec3 p, glm::vec3 col, float intense) : pos(p), Light(col, intense){}

	void illuminate(glm::vec3 hitPoint, glm::vec3 &lightDirection, glm::vec3 &fullLight, float &lightDistance){
		lightDirection = (hitPoint - pos); 
        float r2 = glm::length2(lightDirection); 
        lightDistance = sqrt(r2); 
        lightDirection.x /= lightDistance, lightDirection.y /= lightDistance, lightDirection.z /= lightDistance; 
        // avoid division by 0
        fullLight = color * intensity / (4 * glm::pi<float>() * r2);  
	}
};