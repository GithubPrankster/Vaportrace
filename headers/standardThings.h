#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

struct ImageTexture : Texture{
	u8* imageData;
	int imageWidth, imageHeight, imageDepth;
	ImageTexture(std::string imagePath){
		imageData = stbi_load(imagePath.c_str(), &imageWidth, &imageHeight, &imageDepth, 0);
	}
	glm::vec3 returnColor(float u, float v, glm::vec3 point){
		int i = (int)fabs((float)imageWidth * (v - ((int)v)));
		int j = (int)fabs((float)imageHeight * (u - ((int)u)));

		float R = imageData[imageDepth *(i + j * imageWidth)] / 255.0;
		float G = imageData[imageDepth *(i + j * imageWidth) + 1] / 255.0;
		float B = imageData[imageDepth *(i + j * imageWidth) + 2] / 255.0;
		return glm::vec3(R, G, B);
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
	glm::vec2 UV;
	Material *obtMat;
	hitHistory(float d, glm::vec3 hP, glm::vec3 n, Material &oM) : dist(d), hitPoint(hP), normal(n), obtMat(&oM) {}
	hitHistory() = default;
};

struct Light{
	glm::vec3 color;
	float intensity;
	Light(glm::vec3 c, float i) : color(c), intensity(i) {}
	virtual ~Light() = default;

	virtual glm::vec3 lightDirection(glm::vec3 point) = 0;
	virtual float lightDistance(glm::vec3 point) = 0;
	virtual float attenuation(float distance) = 0;
};

struct PointLight : Light{
	glm::vec3 origin;
	PointLight(glm::vec3 p, glm::vec3 c, float i) : origin(p), Light(c, i) {}
	glm::vec3 lightDirection(glm::vec3 point){
		return glm::normalize(origin - point);
	}

	float lightDistance(glm::vec3 point){
		return glm::length(origin - point);
	}
	float attenuation(float distance){
		return (1.0f + 0.09f * distance + 0.032f * (distance * distance));
	}
};

/*
struct SunLight : Light{
	glm::vec3 direction;
	SunLight(glm::vec3 d, glm::vec3 c, float i) : direction(d), Light(c, i) {}
	glm::vec3 lightDirection(glm::vec3 point){
		return direction;
	}

	float lightDistance(glm::vec3 point){
		return std::numeric_limits<float>::max();
	}

	float attenuation(float distance){
		return 1.0f;
	}
};
*/