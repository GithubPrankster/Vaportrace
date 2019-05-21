#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include "tracing.h"

struct Camera{
	glm::vec3 position, rotationAxis;
	float rotation, renderFov;
};

struct Options{
	std::string renderName, encodeType;
	u16 renderWidth, renderHeight;
	u8 renderChannels, renderSamples;
	Camera camMan;
	Options(std::string renderN, std::string encodeT,u16 renderW, u16 renderH, u8 renderC, u8 renderS): renderName(renderN), encodeType(encodeT),renderWidth(renderW), 
	renderHeight(renderH), renderChannels(renderC), renderSamples(renderS){}
};

u8 convertVec(float d){
	return fmax(0.0f, fmin(255.0f, d * 255.0f));
}

glm::vec3 calculateWin(float fov, float x, float y, u16 w, u16 h){
	float i =  (2*(x + 0.5f)/(float)w  - 1)*tan(fov/2.0f)*w/(float)h;
    float j = -(2*(y + 0.5f)/(float)h - 1)*tan(fov/2.0f);
	return glm::vec3(i, j, -1);
}

void PNGEncode(std::vector<Object*> objects, std::vector<Light> lights, Options opts){
	u8* render = new u8[opts.renderWidth * opts.renderHeight * opts.renderChannels];
	glm::mat3 rotMat = glm::rotate(glm::radians(opts.camMan.rotation), opts.camMan.rotationAxis);
	
	std::vector<std::thread> daThreads;
	
	auto timeThen = std::chrono::system_clock::now(), timeNow = std::chrono::system_clock::now();
	float elapsedTime = 0.0f;
	for(int z = 0; z < std::thread::hardware_concurrency(); z++){
		daThreads.emplace_back(std::thread([objects, lights, opts, render, rotMat, &timeThen, &timeNow, &elapsedTime](){
			for(int x = 0; x < opts.renderWidth; x++){
				for(int y = 0; y < opts.renderHeight; y++){
					timeNow = std::chrono::system_clock::now();
					std::chrono::duration<float> deltaChrono = timeNow - timeThen;
					timeThen = timeNow;
				   
					glm::vec3 finalResult;
					for(int sample = 0; sample < opts.renderSamples; sample++){
						float sampleX = (x + 0.5f + ((sample < 2) ? -0.25f : 0.25f)); 
						float sampleY = (y + 0.5f + ((sample >= 2) ? -0.25f : 0.25f));
						
						glm::vec3 dir = rotMat * glm::normalize(calculateWin(opts.camMan.renderFov, sampleX, sampleY, opts.renderWidth, opts.renderHeight));
						Ray currentRay(opts.camMan.position, dir);
						
						finalResult += cast_ray(currentRay, objects, lights);
					}
					finalResult /= opts.renderSamples;
					
					render[opts.renderChannels *(x + y * opts.renderWidth)] = convertVec(finalResult.x);
					render[opts.renderChannels *(x + y * opts.renderWidth)+ 1] = convertVec(finalResult.y);
					render[opts.renderChannels *(x + y * opts.renderWidth) + 2] = convertVec(finalResult.z);
					
					elapsedTime += deltaChrono.count();
				}
			}
		}));
	}
	
	for(auto& thread: daThreads){
		thread.join();
	}
	stbi_write_png(opts.renderName.c_str(), opts.renderWidth, opts.renderHeight, opts.renderChannels, render, 0);
	delete[] render;
	std::cout << "Time rendered: " << elapsedTime << std::endl;
}