#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

struct Ray{
	glm::vec3 origin, direction;
	Ray(glm::vec3 o, glm::vec3 d) : origin(o), direction(d) {}
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
	glm::vec3 origin, color;
	float intensity;
	Light(glm::vec3 p, glm::vec3 c, float i) : origin(p), color(c), intensity(i) {}
	Light() = default;

	glm::vec3 lightDirection(glm::vec3 point){
		return glm::normalize(origin - point);
	}

	float lightDistance(glm::vec3 point){
		return glm::length(origin - point);
	}
};