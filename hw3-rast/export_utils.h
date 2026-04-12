#ifndef __EXPORT_UTILS_H__
#define __EXPORT_UTILS_H__

// Extract the OpenGL Z-buffer bounds and write it as raw binary
void exportOpenGLDepthBuffer(const char* filename, int x, int y, int width, int height);

// Dump the custom floating point Software Rasterizer buffer to raw binary
void exportSoftwareZBuffer(const char* filename, float* swdepthbuffer, int width, int height);

#endif // __EXPORT_UTILS_H__
