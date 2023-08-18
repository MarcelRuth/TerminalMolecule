/*
Script to make a 3D-Animation (Black&Wight) of Hydroxymethylene
Initial sphere and light tracing based on:
https://www.quora.com/How-do-I-make-animations-in-the-terminal-using-C++-without-graphics-h

Editor: Marcel Ruth
*/

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>

using namespace std;
namespace fs = std::__fs::filesystem;

struct vec3 { 
	float x; 
	float y; 
	float z; 
 
	vec3 operator+(const vec3& other) const { 
		return { x + other.x, y + other.y, z + other.z }; 
	} 
 
	vec3 operator-(const vec3& other) const { 
		return { x - other.x, y - other.y, z - other.z }; 
	} 
 
	vec3 operator-() const { 
		return { -x, -y, -z }; 
	} 
 
	vec3 operator*(const float& scalar) const { 
		return { x * scalar, y * scalar, z * scalar }; 
	} 
 
	vec3 operator/(const float& scalar) const { 
		return { x / scalar, y / scalar, z / scalar }; 
	} 
 
	float dot(const vec3& other) const { 
		return x * other.x + y * other.y + z * other.z; 
	} 
 
	float len() const { 
		return sqrtf(this->dot(*this)); 
	} 
 
	void normalize() { 
		float len = this->len(); 
		x /= len; 
		y /= len; 
		z /= len; 
	} 
}; 

float intersectSphere(vec3 origin, vec3 dir, vec3 center, float radius) { 
	vec3 oc = origin - center; 
	float a = dir.dot(dir); 
	float b = 2.0f * oc.dot(dir); 
	float c = oc.dot(oc) - radius * radius; 
	float discriminant = b * b - 4 * a * c; 
	if (discriminant < 0.0f) { 
		return -1.0f; 
	} 
	return (-b - sqrtf(discriminant)) / (2.0f * a); 
} 

// https://www.cs.helsinki.fi/group/goa/mallinnus/3dtransf/3drot.html 
// for xyz transformation
vec3 rotateCoordinates(vec3 initialCoordinates, float theta, int axis = 0)
{
    vec3 newCoordinates{};
    newCoordinates = initialCoordinates;
    if (axis == 0) // x-axis
    {
    newCoordinates.y = initialCoordinates.y * cos(theta) - initialCoordinates.z * sin(theta);
    newCoordinates.z = initialCoordinates.y * sin(theta) + initialCoordinates.z * cos(theta);
    }
    else if (axis == 1) // y-axis
    {
    newCoordinates.z = initialCoordinates.z * cos(theta) - initialCoordinates.x * sin(theta);
    newCoordinates.x = initialCoordinates.z * sin(theta) + initialCoordinates.x * cos(theta);
    }
    else if (axis == 2) // z-axis
    {
    newCoordinates.x = initialCoordinates.x * cos(theta) - initialCoordinates.y * sin(theta);
    newCoordinates.y = initialCoordinates.x * sin(theta) + initialCoordinates.y * cos(theta);
    }
    return newCoordinates;
}

int main(int argc, char* argv[])
{
	// Image width and height. 
	const int width = 40; 
	const int height = 20; 
    int animationSpeed = 17; // time for a frame update in ms 60 fps are 16.66 
    int loopNumber = 628; // 628 would be two turns
    int rotAxis = 1;

	// For square pixels, e.g. PPM file, set to 1.0f 
	const float pixelAspect = 2.0f; 
 
    /*
    Hydroxymethylene has the following xyz
    6 –0.739089 –0.122224 0.000000
    8 0.563790 0.083238 0.000000
    1 –1.136948 0.918588 0.000000
    1 0.989394 –0.784332 0.000000

    https://www.nature.com/articles/s41557-018-0128-2
    */

	// Initial Sphere position and radius. 
	const vec3 center_carbon{ -7.39089f, -1.22224f, 0.0f }; 
	const float radius_carbon = 7.7f; 
    vec3 carbon{};

    const vec3 center_oxygen{ 5.63790f, 0.83238f, 0.0f }; 
	const float radius_oxygen = 6.6f; 
    vec3 oxygen{};

    const vec3 center_h1{ -11.36948f, 9.18588f, 0.0f }; 
	const float radius_h1 = 3.2f; 
    vec3 hyd1{};

    const vec3 center_h2{ 9.89394f, -7.84332f, 0.0f }; 
	const float radius_h2 = 3.2f; 
    vec3 hyd2{};

	// Light shines towards left and bottom, to inside screen. 
	vec3 light{ -1.0f, 1.0f, 1.0f }; 
	light.normalize(); 
 
	// Characters with different brightness. 
	const char colors[] = { '.', '-', ':', '=', '+', '*', '#', '@' }; 
	const size_t numColors = sizeof(colors); 
 
	// Ray direction - all parallel, down Z axis. 
	const vec3 dir{ 0.0f, 0.0f, 1.0f }; 

    float theta;
    //loop through all thetas
    for (int radians = 0; radians < loopNumber; radians += 1) // 628 = 3.14 * 100 * 2
    {
        system("clear");
        theta = radians/100.0;
        // update coordinates
        carbon = rotateCoordinates(center_carbon, theta, rotAxis);
        oxygen = rotateCoordinates(center_oxygen, theta, rotAxis);
        hyd1 = rotateCoordinates(center_h1, theta, rotAxis);
        hyd2 = rotateCoordinates(center_h2, theta, rotAxis);

        // For each pixel... 
        for (int y = 0; y < height; y++)
        { 
            for (int x = 0; x < width; x++)
            { 
                float xWorld = x - (width / 2) + 0.5f; 
                float yWorld = (y - (height / 2) + 0.5f) * pixelAspect; 
                vec3 origin = vec3{ xWorld, yWorld, -30.0f }; 
    
                float t_carbon = intersectSphere(origin, dir, carbon, radius_carbon); 
                float t_oxygen = intersectSphere(origin, dir, oxygen, radius_oxygen); 
                float t_h1 = intersectSphere(origin, dir, hyd1, radius_h1); 
                float t_h2 = intersectSphere(origin, dir, hyd2, radius_h2); 

                if (t_carbon > 0.0f)
                { // Ray hit carbon. 
                    vec3 intersection_carb = origin + (dir * t_carbon); 
                    vec3 normal_carb = intersection_carb - carbon; 
                    normal_carb.normalize(); 
    
                    float lum_carb = normal_carb.dot(-light); 
    
                    if (lum_carb < 0)
                    { 
                        // Give some brightness even if pointing 
                        // completely away from the light. 
                        std::cout << colors[0]; 
                    } 
                    else { 
                        // Pick character based on light value. 
                        std::cout << colors[(int)(numColors * lum_carb)]; 
                    } 
                } 

                
                else if (t_oxygen > 0.0f)
                { // Ray hit oxygen. 
                    vec3 intersection_ox = origin + (dir * t_oxygen); 
                    vec3 normal_ox = intersection_ox - oxygen; 
                    normal_ox.normalize(); 
    
                    float lum_ox = normal_ox.dot(-light); 
    
                    if (lum_ox < 0)
                    { 
                        // Give some brightness even if pointing 
                        // completely away from the light. 
                        std::cout << colors[0]; 
                    } 
                    else
                    { 
                        // Pick character based on light value. 
                        std::cout << colors[(int)(numColors * lum_ox)]; 
                    } 
                }
                else if (t_h1 > 0.0f)
                { // Ray hit hydrogen 1. 
                    vec3 intersection_h1 = origin + (dir * t_h1); 
                    vec3 normal_h1 = intersection_h1 - hyd1; 
                    normal_h1.normalize(); 
    
                    float lum_h1 = normal_h1.dot(-light); 
    
                    if (lum_h1 < 0)
                    { 
                        // Give some brightness even if pointing 
                        // completely away from the light. 
                        std::cout << colors[0]; 
                    } 
                    else
                    { 
                        // Pick character based on light value. 
                        std::cout << colors[(int)(numColors * lum_h1)]; 
                    } 
                }
                else if (t_h2 > 0.0f)
                { // Ray hit hydrogen 2. 
                    vec3 intersection_h2 = origin + (dir * t_h2); 
                    vec3 normal_h2 = intersection_h2 - hyd2; 
                    normal_h2.normalize(); 
    
                    float lum_h2 = normal_h2.dot(-light); 
    
                    if (lum_h2 < 0)
                    { 
                        // Give some brightness even if pointing 
                        // completely away from the light. 
                        std::cout << colors[0]; 
                    } 
                    else
                    { 
                        // Pick character based on light value. 
                        std::cout << colors[(int)(numColors * lum_h2)]; 
                    } 
                }  
                else
                { // Ray miss 
                    std::cout << " "; 
                }
            } 
            std::cout << "\n"; 
        }
        // Flush terminal
        std::this_thread::sleep_for(std::chrono::milliseconds(animationSpeed));
        system("clear");
    }
    return 0; 
};
