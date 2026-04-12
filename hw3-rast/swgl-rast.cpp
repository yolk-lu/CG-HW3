#include "swgl.h"
#include <cmath>
#include <algorithm>

float *swdepthbuffer = NULL;
int zbuf_width = 0;
int zbuf_height = 0;

//---------------------------------------------------------------------------
//cghw-rast
//---------------------------------------------------------------------------

//You can change it to speed up
void writepixel(int x, int y, GLdouble r, GLdouble g, GLdouble b)
{
	/* 
	GLubyte map[1]={255};

	glColor3d(r, g, b);
	glRasterPos2i(x, y);
	glBitmap(1, 1, 0, 0, 0, 0, map);
	*/


	glColor3d(r, g, b);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}


void writepixel(GLdouble x, GLdouble y, GLdouble z, GLdouble r, GLdouble g, GLdouble b)
{
    int ix = (int)(x + 0.5);
    int iy = (int)(y + 0.5);
    if (ix < 0 || ix >= zbuf_width || iy < 0 || iy >= zbuf_height) return;
    
    int index = iy * zbuf_width + ix;
    if (z <= swdepthbuffer[index]) {
        swdepthbuffer[index] = z;
        glColor3d(r, g, b);
        glBegin(GL_POINTS);
        glVertex2i(ix, iy);
        glEnd();
    }
}

bool BresenhamLine(GLdouble x1, GLdouble y1, GLdouble z1, GLdouble x2, GLdouble y2, GLdouble z2, GLdouble r, GLdouble g, GLdouble b)
{
	/* 
    writepixel(x1, y1, r, g, b);
    writepixel(x2, y2, r, g, b);

    for(int i=0; i<100; i++) {
        float a = 0.01f*i;
        writepixel(a*x1+(1-a)*x2, a*y1+(1-a)*y2, r, g, b);

    }
	*/

    int ix1 = (int)(x1 + 0.5);
    int iy1 = (int)(y1 + 0.5);
    int ix2 = (int)(x2 + 0.5);
    int iy2 = (int)(y2 + 0.5);

    int dx = std::abs(ix2 - ix1), sx = ix1 < ix2 ? 1 : -1;
    int dy = -std::abs(iy2 - iy1), sy = iy1 < iy2 ? 1 : -1; 
    int err = dx + dy, e2; 
 
    int total_steps = (std::abs(dx) > std::abs(dy) ? std::abs(dx) : std::abs(dy));
    double dz = total_steps == 0 ? 0 : (z2 - z1) / total_steps;
    double current_z = z1;

    while (1) {
        writepixel(ix1, iy1, current_z, r, g, b);
        if (ix1 == ix2 && iy1 == iy2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; ix1 += sx; }
        if (e2 <= dx) { err += dx; iy1 += sy; }
        current_z += dz;
    }

	return true;
}

bool swTriangle(GLdouble x1, GLdouble y1, GLdouble z1,
			 GLdouble x2, GLdouble y2, GLdouble z2,
			 GLdouble x3, GLdouble y3, GLdouble z3,
			 GLdouble r, GLdouble g, GLdouble b)
{

    /*
    writepixel(x1, y1, r, g, b);
    writepixel(x2, y2, r, g, b);
    writepixel(x3, y3, r, g, b);

    BresenhamLine(x1, y1, z1, x2, y2, z2, r, g, b);
    BresenhamLine(x2, y2, z2, x3, y3, z3, r, g, b);
    BresenhamLine(x3, y3, z3, x1, y1, z1, r, g, b);
	*/

    int min_x = std::max(0, (int)std::floor(std::min({x1, x2, x3})));
    int max_x = std::min(zbuf_width - 1, (int)std::ceil(std::max({x1, x2, x3})));
    int min_y = std::max(0, (int)std::floor(std::min({y1, y2, y3})));
    int max_y = std::min(zbuf_height - 1, (int)std::ceil(std::max({y1, y2, y3})));

    auto edgeFunction = [](const GLdouble& a_x, const GLdouble& a_y, 
                           const GLdouble& b_x, const GLdouble& b_y, 
                           const GLdouble& c_x, const GLdouble& c_y) {
        return (c_x - a_x) * (b_y - a_y) - (c_y - a_y) * (b_x - a_x);
    };

    GLdouble area = edgeFunction(x1, y1, x2, y2, x3, y3);
    if (area == 0.0) return true;

    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            GLdouble w1 = edgeFunction(x2, y2, x3, y3, x, y);
            GLdouble w2 = edgeFunction(x3, y3, x1, y1, x, y);
            GLdouble w3 = edgeFunction(x1, y1, x2, y2, x, y);

            if (area > 0) {
                if (w1 >= 0 && w2 >= 0 && w3 >= 0) {
                    w1 /= area; w2 /= area; w3 /= area;
                    GLdouble z = w1 * z1 + w2 * z2 + w3 * z3;
                    writepixel(x, y, z, r, g, b);
                }
            } else {
                if (w1 <= 0 && w2 <= 0 && w3 <= 0) {
                    w1 /= area; w2 /= area; w3 /= area;
                    GLdouble z = w1 * z1 + w2 * z2 + w3 * z3;
                    writepixel(x, y, z, r, g, b);
                }
            }
        }
    }

	return true;
}


bool swInitZbuffer(int width, int height)
{
    //allocate/reallocate swdepthbuffer
    zbuf_width = width;
    zbuf_height = height;
    if (swdepthbuffer != NULL) {
        delete[] swdepthbuffer;
    }
    if (width > 0 && height > 0) {
        swdepthbuffer = new float[width * height];
    } else {
        swdepthbuffer = NULL;
    }
    return true;
}



bool swClearZbuffer()
{
    if (swdepthbuffer != NULL) {
        for (int i = 0; i < zbuf_width * zbuf_height; ++i) {
            swdepthbuffer[i] = 1.0f;
        }
    }
    return true;
}
