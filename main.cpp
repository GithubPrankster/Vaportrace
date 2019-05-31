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

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

auto seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937_64 ultraRNG(seed);

std::uniform_real_distribution<float> disty(0.0f, 1.0f);
std::uniform_real_distribution<float> distx(-5.0f, 5.0f);
std::uniform_real_distribution<float> distz(-5.0f, 5.0f);
std::uniform_int_distribution<int> distMat(0, 3);
std::uniform_int_distribution<int> roulette(0, 1);

#include "headers/objects.h"
#include "headers/encoding.h"
#include "headers/INIReader.h"

int main(int argc, char** argv){
	std::cout << "V A P O R T R A C E" << std::endl;
	std::cout << "//// Version 0.995 //" << std::endl;
	std::cout << "Created by Uneven Prankster!" << std::endl;
	std::cout << std::endl << "Dithering like it's the 90's!" << std::endl;

	std::vector<Texture*> textures;
	//textures.push_back(new CheckerTexture(glm::vec3(0.4f, 0.2f, 0.2f), glm::vec3(0.1f), 10));
	textures.push_back(new PerlinTexture(3.0f, 0.6f, 5));
	textures.push_back(new ImageTexture("stdfloor.png"));
	textures.push_back(new SolidTexture(glm::vec3(0.1f, 0.6f, 0.1f)));
	textures.push_back(new SolidTexture(glm::vec3(0.1f, 0.2f, 0.7f)));
	
	std::vector<Material> mats;
	mats.push_back(Material(textures[0], 0.95f, Standard));
	mats.push_back(Material(textures[1], 0.0f, Standard));
	mats.push_back(Material(textures[2], 0.7f, Reflective));
	mats.push_back(Material(textures[3], 0.0f, Standard));
	
	std::vector<Object*> objects;
	objects.push_back(new Plane(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), mats[1]));

	for(int i = 0; i < 50; i++){
		float sphereSize = disty(ultraRNG);
		objects.push_back(new Sphere(glm::vec3(distx(ultraRNG), sphereSize, distz(ultraRNG)), sphereSize, mats[distMat(ultraRNG)]));
	}
	
	std::vector<Light*> lights;
	//lights.push_back(new SunLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.7f, 0.7f, 0.0f), 1.0f));
    lights.push_back(new PointLight(glm::vec3(0.6f, 4.0f, 5.0f), glm::vec3(0.9f, 0.2f, 0.3f), 2.0f));
	lights.push_back(new PointLight(glm::vec3(4.2f, 4.3f, 2.0f), glm::vec3(0.4, 0.2f, 0.7f), 2.4f));

	INIReader reader("options.ini");

    if (reader.ParseError() < 0) {
        std::cout << "Can't load ini stuff!" << std::endl;
        return EXIT_FAILURE;
    }
	std::string rName = reader.Get("MainSettings", "Name", "ERMAC.png");
	std::string Encode = reader.Get("MainSettings", "Encoding", "png");

	u16 rWidth = reader.GetInteger("MainSettings", "RenderWidth", 1280);
	u16 rHeight = reader.GetInteger("MainSettings", "RenderHeight", 720);

	u8 rChannels = reader.GetInteger("MainSettings", "Channels", 3);
	u8 rSamples = reader.GetInteger("MainSettings", "Samples", 4);

	initShadowSoftness(reader.GetInteger("MainSettings", "ShadowSamples", 4));
	
	Options userOpts(rName, Encode, rWidth, rHeight, rChannels, rSamples);

	if(reader.GetBoolean("Palette", "Palettized", false)){
		userOpts.pal = Palette(reader.Get("Palette", "Path", "goof.gpl"));
		userOpts.palette = true;
		userOpts.renderName = "result.png";
	}

	userOpts.camMan.position = glm::vec3(reader.GetReal("Camera", "PositionX", 0.0f), 
							   reader.GetReal("Camera", "PositionY", 0.0f), 
							   reader.GetReal("Camera", "PositionZ", 0.0f));

	userOpts.camMan.renderFov = glm::radians(reader.GetReal("Camera", "FOV", 90.0f));
	userOpts.camMan.rotation = reader.GetReal("Camera", "Rotation", 0.0f);

	userOpts.camMan.rotationAxis = glm::vec3(reader.GetReal("Camera", "RotationAxisX", 0.0f), 
							   reader.GetReal("Camera", "RotationAxisY", 0.0f), 
							   reader.GetReal("Camera", "RotationAxisZ", 0.0f));

	userOpts.camMan.background = glm::vec3(0.0f);
	
	PNGEncode(objects, lights, userOpts);
	
	std::cout << "A " << userOpts.encodeType << " has been written by the name of " << userOpts.renderName << std::endl;
	
	return 0;
}