//#define TINYOBJLOADER_IMPLEMENTATION
//#include "tiny_obj_loader.h"

#include "standardThings.h"

constexpr float EPSILION = 1e-6f;

struct Object{
	glm::vec3 pos;
	Material material;
	Object(glm::vec3 p, Material mat) : pos(p), material(mat) {}
	Object() = default;
	virtual bool intersect(Ray ray, float &dist) = 0;
	virtual glm::vec3 getNormal(glm::vec3 hitPoint) = 0;
	virtual glm::vec2 getUV(glm::vec3 hitPoint);
};

struct Sphere : Object{
	float radius;
	Sphere (glm::vec3 c, float r, Material mat) : Object(c, mat), radius(r)  {}
	
	bool intersect(Ray ray, float &t2){
		float radius2 = radius * radius;
		glm::vec3 L = pos - ray.origin;

		float tca = glm::dot(L, ray.direction), d2 = glm::dot(L, L) - tca * tca;

		if (d2 > radius2) return false;

		float thc = sqrtf(radius2 - d2);

		float t0 = tca - thc, t1 = tca + thc;

		if (t0 > t1) std::swap(t0, t1);
		if (t0 < 0) {
			t0 = t1;
			if(t0 < 0) return false;
		}
		t2 = t0;
		return true;
	}
	
	glm::vec3 getNormal(glm::vec3 hitPoint){
		return glm::normalize(hitPoint - pos);
	}

	glm::vec2 getUV(glm::vec3 hitPoint){
		return glm::vec2(glm::atan(hitPoint.x, hitPoint.z) / (2.0f * glm::pi<float>()) + 0.5f, 
						 glm::asin(hitPoint.y) / glm::pi<float>() + 0.5f); 
	};
};

struct Plane : Object{
	glm::vec3 normal;
	Plane(glm::vec3 p, glm::vec3 n, Material mat) : Object(p, mat), normal(n) {}
	
	bool intersect(Ray ray, float &dist){
		float denom = glm::dot(normal, ray.direction);

		if(abs(denom) > EPSILION){
			dist = glm::dot(pos - ray.origin, normal) / denom;
			return (dist >= EPSILION);
		}
		return false;
	}

	glm::vec3 getNormal(glm::vec3 hitPoint){
		return normal;
	}

	glm::vec2 getUV(glm::vec3 hitPoint){
		float u = 0.5 + hitPoint.x;
  		float v = 0.5 + hitPoint.z;

		return glm::vec2(u, v);
	};
};

struct Disk : Object{
	glm::vec3 normal;
	float radius;
	Disk(glm::vec3 p, glm::vec3 n, float rad,Material mat) : Object(p, mat), radius(rad), normal(n) {}
	

	bool planeIntersect(Ray ray, float &dist){
		float denom = glm::dot(normal, ray.direction);

		if(abs(denom) > EPSILION){
			dist = glm::dot(pos - ray.origin, normal) / denom;
			return (dist >= EPSILION);
		}
		return false;
	}

	//One thing to note is that you'd need to do a floating point
	//square root for checking the hit with the disk's radius. 
	//Instead, an easy optimization is done similar to
	//what is done with the sphere intersection.
	bool intersect(Ray ray, float &dist){
		float radius2 = radius * radius;
		if (planeIntersect(ray, dist)) { 
			glm::vec3 p = ray.origin + ray.direction * dist; 
			glm::vec3 v = p - ray.origin; 
			float d2 = glm::length(v);
			return d2 <= radius2; 
		} 
 
     	return false;
	}

	glm::vec3 getNormal(glm::vec3 hitPoint){
		return normal;
	}

	glm::vec2 getUV(glm::vec3 hitPoint){
		glm::vec3 u = glm::normalize(glm::vec3( normal.y, -normal.x, 0));
		glm::vec3 v = glm::cross(u, normal);

		return glm::vec2(glm::dot(u, hitPoint), glm::dot(v, hitPoint));
	};
};
/*
struct Triangle : Object{
	glm::vec3 vertex1, vertex2, vertex3;
	Triangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, Material mat) : Object(glm::vec3(0.0f), mat), vertex1(p1), vertex2(p2), vertex3(p3) {}

	
	//thx to Ashley
	bool intersect(Ray ray, float &dist){

		glm::vec3 v2v1 = vertex2 - vertex1; 
    	glm::vec3 v3v1 = vertex3 - vertex1; 
    	glm::vec3 pvec = glm::cross(ray.direction,v3v1); 
    	float det = glm::dot(v2v1,pvec); 

		if (det < EPSILION) return false;

		float invDet = 1 / det; 
 
		glm::vec3 tvec = ray.origin - vertex1; 
		float u = glm::dot(tvec,pvec) * invDet; 
		if (u < 0 || u > 1) return false; 
	
		glm::vec3 qvec = glm::cross(tvec, v2v1); 
		float v = glm::dot(ray.direction,qvec) * invDet; 
		if (v < 0 || u + v > 1) return false; 
	
		dist = glm::dot(v3v1,qvec) * invDet; 
	
		return true; 
	}

	//Pretty simple to be fair.
	glm::vec3 getNormal(glm::vec3 hitPoint){
		glm::vec3 edge1 = vertex2 - vertex1;
		glm::vec3 edge2 = vertex3 - vertex1;

		return glm::cross(edge1, edge2);
	}
};
*/
