#include "Smileyclass.h"

#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void draw_form(int npoints, float *tx, float *ty, GLenum mode)//draws the smiley
{
	glBegin(mode);
	for (int i = 0; i < npoints; ++i)
		glVertex2f(tx[i], ty[i]);
	glEnd();
}

smiley::smiley(float r, float x, float y): radius(r), xc(x), yc(y)
{
	happy = 1;
	set_numpoints();
}

void smiley::set_numpoints()
{
	npoints = int(radius)/4 + 14;
	if (npoints%8)
		npoints -= npoints%8; // npoints is multiple of 8
}

void smiley::resize(float r)
{
	if (r < 10.0f) r = 10.0f;
	else if (r > 200.0f) r = 200.0f;
	radius = r;
	set_numpoints();
}

void smiley::draw()
{
	float *ts = new float[npoints];
	float *tc = new float[npoints];
	float *tx = new float[npoints];
	float *ty = new float[npoints];
	float angle = 2.0*M_PI/npoints;
	float delta = angle;
	ts[0] = 0.0f;
	tc[0] = 1.0f;
	for (int i = 1; i < npoints; ++i)
	{
		ts[i] = sinf(angle);
		tc[i] = cosf(angle);
		angle += delta;
	}
	for (int i = 0; i < npoints; ++i)
	{
		tx[i] = tc[i]*radius + xc;
		ty[i] = ts[i]*radius + yc;
	}
	glColor3f(1.0f,1.0f,1.0f);
	draw_form(npoints, tx, ty, GL_POLYGON);
	if (happy)//change color
		glColor3f(1.0f,0.0f,0.0f);
	else
		glColor3f(0.0f,0.0f,1.0f);
	draw_form(npoints, tx, ty, GL_LINE_LOOP);
	int n = npoints/4;
	float r = 0.125f*radius;
	for (int i = 0; i < n; ++i)
	{
		tx[i] = tc[4*i]*r + xc - 0.4*radius;
		ty[i] = ts[4*i]*r + yc + 0.4*radius;
	}
	draw_form(n, tx, ty, GL_LINE_LOOP);
	for (int i = 0; i < n; ++i)
		tx[i] += 0.8*radius;
	draw_form(n, tx, ty, GL_LINE_LOOP);
	r = 0.6f*radius;
	int m = 3*n/2;
	if (happy)
		for (int i = 0; i <= n; ++i)
		{
			tx[i] = xc + ts[i + m]*r;
			ty[i] = yc + tc[i + m]*r;	
		}
		else
		for (int i = 0; i <= n; ++i)
		{
			tx[i] = xc - ts[i + m]*r;
			ty[i] = yc - 0.8*radius - tc[i + m]*r;	
		}
	draw_form(n + 1, tx, ty, GL_LINE_STRIP);
	delete[] ty;
	delete[] tx;
	delete[] tc;
	delete[] ts;
}


