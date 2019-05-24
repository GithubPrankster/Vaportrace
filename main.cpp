#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <limits>
#include <string>
#include <cstdlib>
#include <random>
#include <chrono>
#include <thread>
#include <numeric>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/norm.hpp>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

auto seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937_64 ultraRNG(seed);

std::uniform_real_distribution<float> disty(0.0f, 1.0f);
std::uniform_real_distribution<float> distx(-5.0f, 5.0f);
std::uniform_real_distribution<float> distz(-5.0f, 5.0f);
std::uniform_int_distribution<int> distMat(0, 3);
//std::uniform_int_distribution<int> roulette(0, 1);

#include "headers/objects.h"
#include "headers/encoding.h"

int main(int argc, char** argv){
	std::cout << "V A P O R T R A C E" << std::endl;
	std::cout << "//// Version 0.96 //" << std::endl;
	std::cout << "Created by Uneven Prankster!" << std::endl;
	std::cout << std::endl << "There are disks. Hard to tell if they are floppy though." << std::endl;

	std::vector<Texture*> textures;
	//textures.push_back(new CheckerTexture(glm::vec3(0.4f, 0.2f, 0.2f), glm::vec3(0.1f), 10));
	textures.push_back(new PerlinTexture(3.0f, 0.6f, 5));
	textures.push_back(new PerlinTexture(2.8f, 0.4f, 9));
	textures.push_back(new SolidTexture(glm::vec3(0.1f, 0.6f, 0.1f)));
	textures.push_back(new SolidTexture(glm::vec3(0.1f, 0.2f, 0.7f)));
	
	std::vector<Material> mats;
	mats.push_back(Material(textures[0], 0.95f, Standard));
	mats.push_back(Material(textures[1], 0.0f, Standard));
	mats.push_back(Material(textures[2], 0.0f, Standard));
	mats.push_back(Material(textures[3], 0.0f, Standard));
	
	std::vector<Object*> objects;
	objects.push_back(new Disk(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 6.0f, mats[1]));
	//objects.push_back(new AxisBox(glm::vec3(-1.0f), glm::vec3(1.0f), mats[0]));
	
	for(int i = 0; i < 50; i++){
		float sphereSize = disty(ultraRNG);
		objects.push_back(new Sphere(glm::vec3(distx(ultraRNG), sphereSize, distz(ultraRNG)), sphereSize, mats[distMat(ultraRNG)]));
	}

	//objects.push_back(new Triangle(glm::vec3(0.5f, 0.0f, 0.0f), glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(-0.5f, 0.0f, 0.0f), mats[3]));
	//To remind your brain of what an object be push like
	
	std::vector<Light*> lights;
    lights.push_back(new DirectionalLight(glm::vec3(-0.3f), glm::vec3(0.9f, 0.5f, 0.6f), 1.0f));
	//lights.push_back(new PointLight(glm::vec3(4.2f, 4.3f, 2.0f), glm::vec3(0.4, 0.2, 0.8), 1.2f));
	
	Options userOpts(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));

	userOpts.camMan.position = glm::vec3(0.0f, 2.5f, 12.0f);
	userOpts.camMan.renderFov = glm::pi<float>() / 4.0f;
	userOpts.camMan.rotation = -15.0f;
	userOpts.camMan.rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
	
	PNGEncode(objects, lights, userOpts);
	
	std::cout << "A " << userOpts.encodeType << " has been written by the name of " << userOpts.renderName << std::endl;
	
	return 0;
}