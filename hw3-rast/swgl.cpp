#include "swgl.h"


GLdouble CTM_MV[16];	//Current Transformation Matrix: ModelView
GLdouble CTM_P[16];		//Current Transformation Matrix: Projection
GLdouble *CTM;			//Pointer to Current Transformation Matrix


bool swTransformation(const GLdouble h[4], GLdouble w[4])
{
	//p = CTM_P*CTM_MV*h
	GLdouble p_mv[4] = {0, 0, 0, 0};
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			p_mv[i] += CTM_MV[j*4 + i] * h[j];
		}
	}

	GLdouble p[4] = {0, 0, 0, 0};
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			p[i] += CTM_P[j*4 + i] * p_mv[j];
		}
	}

	//prespective division
	w[0] = p[0];
	w[1] = p[1];
	w[2] = p[2];
	w[3] = p[3];
	if (p[3] != 0.0) {
		w[0] /= p[3];
		w[1] /= p[3];
		w[2] /= p[3];
	}

	//viewport transformation
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	w[0] = viewport[0] + (w[0] + 1.0) * viewport[2] / 2.0;
	w[1] = viewport[1] + (w[1] + 1.0) * viewport[3] / 2.0;
	w[2] = (w[2] + 1.0) / 2.0;

	return true;
}
