bool sceneIntersection(Ray ray, std::vector<Object*> stuff, hitHistory &history){
	float stuff_dist = std::numeric_limits<float>::max();
	for(auto &object : stuff){
		float dist_i = 0.0f;
		if(object->intersect(ray, dist_i) && dist_i < stuff_dist){
			stuff_dist = dist_i;
			glm::vec3 hitPoint = ray.origin + ray.direction * dist_i;
			hitHistory gotHist(dist_i, hitPoint, object->getNormal(hitPoint), object->material);
			gotHist.UV = object->getUV(hitPoint);
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

glm::vec3 cast_ray(Ray ray, std::vector<Object*> stuff, std::vector<Light*> lights, glm::vec3 background, u8 depth = 0) {
	float numericalMinimum = 1e-4f;
	glm::vec3 finalColor;
	hitHistory rayHist;
    if (depth > 8 || !sceneIntersection(ray, stuff, rayHist)) {
        return background; // Nothing, you dummy.
    }

	switch(rayHist.obtMat->type){
			case Standard:{
				for(u32 i = 0; i < lights.size(); i++){
					glm::vec3 lightDir = lights[i]->lightDirection(rayHist.hitPoint);
					float lightDist = lights[i]->lightDistance(rayHist.hitPoint);
					
					Ray shadowRay(glm::dot(lightDir ,rayHist.normal) < 0 ? rayHist.hitPoint - rayHist.normal * numericalMinimum : rayHist.hitPoint + rayHist.normal * numericalMinimum, lightDir);
					hitHistory shadowHist;
					
					if (sceneIntersection(shadowRay, stuff, shadowHist) && glm::length(shadowHist.hitPoint - shadowRay.origin) < lightDist){
						continue;
					}

					glm::vec3 obtainedColor = rayHist.obtMat->diffuse->returnColor(rayHist.UV.x, rayHist.UV.y, rayHist.hitPoint);
					float brightness = lights[i]->intensity * std::max(0.f, glm::dot(lightDir, rayHist.normal));
					finalColor += (obtainedColor * lights[i]->color * brightness) / lights[i]->attenuation(lightDist);
				}
				break;
			}
			case Reflective:{
				glm::vec3 reflect_dir = glm::normalize(glm::reflect(ray.direction, rayHist.normal));
    			glm::vec3 reflect_orig = glm::dot(reflect_dir, rayHist.normal) < 0 ? rayHist.hitPoint - rayHist.normal * numericalMinimum : 
										rayHist.hitPoint + rayHist.normal * numericalMinimum;
    			glm::vec3 reflect_color = cast_ray(Ray(reflect_orig, reflect_dir), stuff, lights, background, depth + 1);

				finalColor += (reflect_color * rayHist.obtMat->reflectiveness);
				break;
			}
			default:
				break;
	}

	return clampRay(finalColor);
}