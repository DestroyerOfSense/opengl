#include <complex>
#include <windows.h>

#include "GL/glut.h"

using std::complex;

const int k_defaultIterations = 1000;
const double k_defaultThreshold = 2.0;
const double k_xMax = 4.0;
const double k_yMax = 3.0;
const double k_multiplier = 100.0;

/*
 * Performs a given number of iterations of the function x_(n+1) = (x_n)^2 + z. If the magnitude of x_n exceeds threshold
 * during iteration, n is returned. Otherwise, -1 is returned.
 */
int findEscapeTime(complex<double> x, complex<double> z, int iterations, double threshold)
{
	threshold *= threshold;
	for (int i = 0; i < iterations; ++i)
	{
		if (norm(x) > threshold)
			return i;
		x = x*x + z;
	}
	return -1;
}

void drawPoint(double x, double y, int escapeTime, int iterations)
{
	double shade = static_cast<double>(escapeTime == -1 ? 0 : iterations - escapeTime) / iterations;
	glColor3d(shade, shade, shade);
	glVertex2d(k_multiplier * x, k_multiplier * y);
}

void mandelbrot(int iterations, double threshold)
{
	double delta = 1 / k_multiplier;
	for (double x = -k_xMax; x <= k_xMax; x += delta)
	{
		for (double y = -k_yMax; y <= k_yMax; y += delta)
		{
			int escapeTime = findEscapeTime(complex<double>{}, complex<double>{x, y}, iterations, threshold);
			drawPoint(x, y, escapeTime, iterations);
		}
	}
}

void julia(complex<double> z, int iterations, double threshold)
{
	double delta = 1 / k_multiplier;
	for (double x = -k_xMax; x <= k_xMax; x += delta)
	{
		for (double y = -k_yMax; y <= k_yMax; y += delta)
		{
			int escapeTime = findEscapeTime(complex<double>{x, y}, z, iterations, threshold);
			drawPoint(x, y, escapeTime, iterations);
		}
	}
}

void init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-k_multiplier * k_xMax, k_multiplier * k_xMax, -k_multiplier * k_yMax, k_multiplier * k_yMax);
	glClear(GL_COLOR_BUFFER_BIT);
}

// Display functions:

void drawMandelbrot()
{
	glBegin(GL_POINTS);
	mandelbrot(k_defaultIterations, k_defaultThreshold);
	glEnd();
	glFlush();
}

void drawJuliaA()
{
	glBegin(GL_POINTS);
	julia(complex<double>{-0.74543, 0.11301}, k_defaultIterations, k_defaultThreshold);
	glEnd();
	glFlush();
}

void drawJuliaB()
{
	glBegin(GL_POINTS);
	julia(complex<double>{-0.835, -0.2321}, 64, k_defaultThreshold);
	glEnd();
	glFlush();
}

void drawJuliaC()
{
	glBegin(GL_POINTS);
	julia(complex<double>{-0.8, 0.156}, 512, k_defaultThreshold);
	glEnd();
	glFlush();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(k_xMax * k_multiplier * 2, k_yMax * k_multiplier * 2);
	glutCreateWindow("MandelGen");
	init();
	glutDisplayFunc(drawMandelbrot);
	glutMainLoop();
	return 0;
}
