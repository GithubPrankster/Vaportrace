#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <limits>
#include <cstdint>

//This header also implements the Knoll-Yilluoma dither algorithm
//as described in Biqswit's article here:
//https://bisqwit.iki.fi/story/howto/dither/jy/

struct TrueColor{
    uint8_t R, G, B;
    TrueColor(uint8_t C, uint8_t Y, uint8_t M) : R(C), G(Y), B(M){}
    TrueColor() = default;

    bool operator!=(TrueColor other){
        return (R != other.R) || (G != other.G) || (B != other.B);
    }

    unsigned LumaFunction(){
        return R * 299 + G * 587 + B * 114;
    }

    //Luma weighted.
    double distCompare(TrueColor other){
        double luma = LumaFunction() / (255.0 * 1000);
        double luma1 = other.LumaFunction() / (255.0 * 1000);
        double diffLuma = luma - luma1;

        double diffR = (R - other.R) / 255.0; 
        double diffG = (G - other.G) / 255.0; 
        double diffB = (B - other.B) / 255.0;
        return (diffR*diffR * 0.299 + diffG*diffG * 0.587+ diffB*diffB * 0.114) * 0.75 + diffLuma * diffLuma;
    }

    double mixingPenalty(TrueColor other, TrueColor color1, TrueColor color2, double ratio){
        return distCompare(other) + color1.distCompare(color2) * 0.1 * (fabs(ratio - 0.5) + 0.5);
    }

    void printSelf(){
        std::cout << (unsigned int)R << " " << (unsigned int)G << " " << (unsigned int)B << std::endl;
    }
};

struct MixPlan{
    TrueColor colors[4];
    double mixRatio;
};

struct Palette{
    std::vector<TrueColor> pal;
    
    Palette() = default;
    Palette(std::string filename){
        loadPalFile(filename);
    }

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

    MixPlan deviseColorPlan(TrueColor original){
        MixPlan result = {{TrueColor(), TrueColor()}, 0};
        double nearestPenalty = std::numeric_limits<double>::max();

        for(u8 index1 = 0; index1 < 16; index1++){
            for(u8 index2 = index1; index2 < 16; index2++){
                TrueColor color1 = pal[index1];
                TrueColor color2 = pal[index2];
                int ratio;

                if(color1 != color2){
                    ratio = ((color2.R != color1.R ? 299*64 * int(original.R - color1.R) / int(color2.R - color1.R) : 0)
                        +  (color2.G != color1.G ? 587*64 * int(original.G - color1.G) / int(color2.G  - color1.G) : 0)
                        +  (color1.B != color2.B ? 114*64 * int(original.B  - color1.B) / int(color2.B - color1.B) : 0))
                    / ((color2.R != color1.R ? 299 : 0)
                    + (color2.G  != color1.G ? 587 : 0)
                    + (color2.B != color1.B ? 114 : 0));
                    if(ratio < 0) ratio = 0; else if(ratio > 63) ratio = 63;
                }

                u8 r0 = color1.R + ratio * (int)(color2.R - color1.R) / 64;
                u8 g0 = color1.G + ratio * (int)(color2.G - color1.G) / 64;
                u8 b0 = color1.B + ratio * (int)(color2.B - color1.B) / 64;
                TrueColor rgb0(r0, g0, b0);

                double penalty = original.mixingPenalty(rgb0, color1, color2, ratio / 64.0);

                if(penalty < nearestPenalty){
                    nearestPenalty = penalty;
                    result.colors[0] = pal[index1];
                    result.colors[1] = pal[index2];
                    result.mixRatio = ratio / 64.0;
                }
                
                if(index1 != index2){
                    for(u8 index3 = 0; index3 < 16; index3++){
                        if(index3 == index2 || index3 == index1)
                            continue;

                        TrueColor color3 = pal[index3];

                        rgb0 = TrueColor((color1.R + color2.R + color3.R * 2) / 4, 
                                         (color1.G + color2.G + color3.G * 2) / 4,
                                         (color1.B + color2.B + color3.B * 2) / 4);
                        TrueColor triTone((color1.R + color1.G) / 2,(color1.G + color2.G) / 2, (color1.B + color2.B) / 2); 
                        penalty = original.distCompare(rgb0) + color1.distCompare(color2) * 0.025 +
                                  triTone.distCompare(color3) * 0.025;
                        if(penalty < nearestPenalty){
                            nearestPenalty = penalty;
                            result.colors[0] = pal[index3]; 
                            result.colors[1] = pal[index1]; 
                            result.colors[2] = pal[index2]; 
                            result.colors[3] = pal[index3];
                            result.mixRatio = 4.0;
                        }
                    }
                }
            }
        }
        return result;
    }

};

#define d(x) x/64.0
constexpr double matrix[8][8] = {
d( 0), d(48), d(12), d(60), d( 3), d(51), d(15), d(63),
d(32), d(16), d(44), d(28), d(35), d(19), d(47), d(31),
d( 8), d(56), d( 4), d(52), d(11), d(59), d( 7), d(55),
d(40), d(24), d(36), d(20), d(43), d(27), d(39), d(23),
d( 2), d(50), d(14), d(62), d( 1), d(49), d(13), d(61),
d(34), d(18), d(46), d(30), d(33), d(17), d(45), d(29),
d(10), d(58), d( 6), d(54), d( 9), d(57), d( 5), d(53),
d(42), d(26), d(38), d(22), d(41), d(25), d(37), d(21) };
#undef d

void writeRenderPalettized(Palette pal, u8* data, u16 imgWidth, u16 imgHeight, u8 imgDepth){
    TrueColor *result = new TrueColor[imgWidth * imgHeight];

    #pragma omp parallel for
    for(int x = 0; x < imgWidth; x++){
        for(int y = 0; y < imgHeight; y++){
            uint8_t red = data[(x + imgWidth * y) * imgDepth];
            uint8_t green = data[(x + imgWidth * y) * imgDepth + 1];
            uint8_t blue = data[(x + imgWidth * y) * imgDepth + 2];

            MixPlan paletteMix = pal.deviseColorPlan(TrueColor(red, green, blue));
            if(paletteMix.mixRatio = 4.0){
                result[(x + y * imgWidth)] = paletteMix.colors[((y & 1) * 2 + (x & 1))];
            }else{
                double factor = matrix[x % 8][y % 8];
                result[(x + y * imgWidth)] = paletteMix.colors[factor < paletteMix.mixRatio ? 1 : 0];
            } 
        }
    }

    stbi_write_png("result.png", imgWidth, imgHeight, imgDepth, result, 0);
    delete result;
}