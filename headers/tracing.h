bool sceneIntersection(Ray ray, std::vector<Object*> stuff, hitHistory &history){
	float stuff_dist = std::numeric_limits<float>::max();
	for(auto &object : stuff){
		float dist_i = 0.0f;
		if(object->intersect(ray, dist_i) && dist_i < stuff_dist){
			stuff_dist = dist_i;
			glm::vec3 hitPoint = ray.origin + ray.direction * dist_i;
			hitHistory gotHist(dist_i, hitPoint, object->getNormal(hitPoint), object->material);
			history = gotHist;
		}
	}
    return stuff_dist < std::numeric_limits<float>::max();
}

glm::vec3 clampRay(glm::vec3 col){
	glm::vec3 res = col;
	res.x = col.x < 0 ? 0 : col.x > 1 ? 1 : col.x;
	res.y = col.y < 0 ? 0 : col.y > 1 ? 1 : col.y;
	res.z = col.z < 0 ? 0 : col.z > 1 ? 1 : col.z;
	return res;
}

glm::vec3 cast_ray(Ray ray, std::vector<Object*> stuff, std::vector<Light> lights, u8 depth = 0) {
	float numericalMinimum = 1e-4f;
	glm::vec3 finalColor;
	hitHistory rayHist;
    if (depth > 8 || !sceneIntersection(ray, stuff, rayHist)) {
        return glm::vec3(0.0f); // Nothing, you dummy.
    }

	for(Light light : lights){
		glm::vec3 lightDir = light.lightDirection(rayHist.hitPoint);
		float lightDist = light.lightDistance(rayHist.hitPoint);
		
		Ray shadowRay(glm::dot(lightDir ,rayHist.normal) < 0 ? rayHist.hitPoint - rayHist.normal * numericalMinimum : rayHist.hitPoint + rayHist.normal * numericalMinimum, lightDir);
		hitHistory shadowHist;
		
		if (sceneIntersection(shadowRay, stuff, shadowHist) && glm::length(shadowHist.hitPoint - shadowRay.origin) < lightDist){
			continue;
		}

		glm::vec3 obtainedColor = rayHist.obtMat->diffuse->returnColor(0.0f, 0.0f, rayHist.hitPoint);

		switch(rayHist.obtMat->type){
			case Standard:{
				float brightness = light.intensity * std::max(0.f, glm::dot(lightDir, rayHist.normal));
				finalColor += obtainedColor * light.color * brightness;
				break;
			}
			case Reflective:{
				float brightness = light.intensity * std::max(0.f, glm::dot(lightDir, rayHist.normal));
				glm::vec3 reflect_dir = glm::normalize(glm::reflect(ray.direction, rayHist.normal));
    			glm::vec3 reflect_orig = glm::dot(reflect_dir, rayHist.normal) < 0 ? rayHist.hitPoint - rayHist.normal * numericalMinimum : 
										rayHist.hitPoint + rayHist.normal * numericalMinimum;
    			glm::vec3 reflect_color = cast_ray(Ray(reflect_orig, reflect_dir), stuff, lights, depth + 1);

				finalColor += (obtainedColor * rayHist.obtMat->reflectiveness )* (reflect_color * rayHist.obtMat->reflectiveness) * light.color 
							* brightness;
				break;
			}
			default:
				break;
		}
		
	}
	return clampRay(finalColor);
}