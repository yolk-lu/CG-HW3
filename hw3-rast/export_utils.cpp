#include "export_utils.h"
#include <GL/glew.h>
#include <fstream>
#include <iostream>

void exportOpenGLDepthBuffer(const char* filename, int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) return;
    
    // Allocate space for the OpenGL float depth values [0, 1]
    float* depthData = new float[width * height];
    
    // Read the depth buffer directly from OpenGL's internal state
    glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depthData);
    
    // Write out dimensions and array data natively
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open " << filename << " for writing." << std::endl;
        delete[] depthData;
        return;
    }
    int hdr[2] = {width, height};
    out.write(reinterpret_cast<char*>(hdr), sizeof(hdr));
    out.write(reinterpret_cast<char*>(depthData), width * height * sizeof(float));
    out.close();
    
    delete[] depthData;
    std::cout << "Successfully exported OpenGL Depth Map to binary: " << filename << std::endl;
}

void exportSoftwareZBuffer(const char* filename, float* swdepthbuffer, int width, int height) {
    if (!swdepthbuffer || width <= 0 || height <= 0) return;
    
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open " << filename << " for writing." << std::endl;
        return;
    }
    int hdr[2] = {width, height};
    out.write(reinterpret_cast<char*>(hdr), sizeof(hdr));
    out.write(reinterpret_cast<char*>(swdepthbuffer), width * height * sizeof(float));
    out.close();
    
    std::cout << "Successfully exported Software Rasterized Depth Map to binary: " << filename << std::endl;
}
