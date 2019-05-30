#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <limits>
#include <cstdint>

struct TrueColor{
    uint8_t R, G, B;
    TrueColor(uint8_t C, uint8_t Y, uint8_t M) : R(C), G(Y), B(M){}
    TrueColor() = default;

    //Luma weighted.
    double distCompare(TrueColor other)
    {
        double luma = (R * 299 + G * 587 + B * 114) / (255.0*1000);
        double luma1 = (other.R * 299 + other.G * 587 + other.B * 114) / (255.0*1000);
        double diffLuma = luma-luma1;

        double diffR = (R - other.R)/255.0; 
        double diffG = (G - other.G)/255.0; 
        double diffB = (B - other.B)/255.0;
        return (diffR*diffR * 0.299 + diffG*diffG * 0.587+ diffB*diffB * 0.114) * 0.75 + diffLuma * diffLuma;
    }

    void printSelf(){
        std::cout << (unsigned int)R << " " << (unsigned int)G << " " << (unsigned int)B << std::endl;
    }
};

struct Palette{
    std::vector<TrueColor> pal;
    
    uint32_t getNumColors(){
        return pal.size();
    }

    void loadPalFile(std::string filename){
        std::ifstream input(filename);
        unsigned int r, g, b;
        while (input >> r >> g >> b){
            pal.push_back(TrueColor(r, g, b));
        }
    }

    void loadBinFile(std::string filename){
        std::ifstream input(filename, std::ios::binary);
        uint8_t r, g, b;
        while (input >> r >> g >> b){
            pal.push_back(TrueColor(r * 4, g * 4, b * 4));
        }
    }

    TrueColor nearestFromPalette(TrueColor original){
        double nearestDistance = std::numeric_limits<double>::max();
        TrueColor nearestColor;
        for(uint32_t x = 0; x < getNumColors(); x++){
            TrueColor palColor = pal[x];
            double dist = original.distCompare(palColor);
            if(dist < nearestDistance){
                nearestDistance = dist;
                nearestColor = palColor;
            }
        }
        return nearestColor;
    }
};

struct Image{
    uint8_t *imageData;
    int imageWidth, imageHeight, imageDepth;

    Image(std::string filepath){
        imageData = stbi_load(filepath.c_str(), &imageWidth, &imageHeight, &imageDepth, 0);
    }
    void writePalettized(Palette pal){
        TrueColor *result = new TrueColor[imageWidth * imageHeight];

        #pragma omp parallel for
        for(int x = 0; x < imageWidth; x++){
            for(int y = 0; y < imageHeight; y++){
                uint8_t red = imageData[(x + imageWidth * y) * imageDepth];
                uint8_t green = imageData[(x + imageWidth * y) * imageDepth + 1];
                uint8_t blue = imageData[(x + imageWidth * y) * imageDepth + 2];
                TrueColor palettized = pal.nearestFromPalette(TrueColor(red, green, blue));

                result[(x + y * imageWidth)] = palettized;
            }
        }

        stbi_write_png("result.png", imageWidth, imageHeight, imageDepth, result, 0);
        delete result;
    }
};