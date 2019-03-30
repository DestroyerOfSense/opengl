#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <algorithm>

constexpr int k_width = 300;
constexpr int k_height = 300;
constexpr int k_windowPosX = 100;
constexpr int k_windowPosY = 100;
constexpr double k_ringColors[5][3]{{0.0, 0.0, 1.0}, {1.0, 1.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0}};

void drawInQuadrants(int centerX, int centerY, int x, int y)
{
	glVertex2i(centerX + x, centerY + y);
	glVertex2i(centerX - x, centerY + y);
	glVertex2i(centerX - x, centerY - y);
	glVertex2i(centerX + x, centerY - y);
}

void drawCircle(const int center[2], int radius, const double color[3])
{
	glColor3d(color[0], color[1], color[2]);
	int x = 0, y = radius, p = 1 - radius;
	while (x < y)
	{
		drawInQuadrants(center[0], center[1], x, y);
		drawInQuadrants(center[0], center[1], y, x);
		p += 2*++x + 1;
		if (p >= 0)
			p -= 2*--y;
	}
}

void drawEllipse(const int center[2], int radiusX, int radiusY, const double color[3])
{
	glColor3d(color[0], color[1], color[2]);
	int rxSquared = radiusX*radiusX, rySquared = radiusY*radiusY;
	int rxSquaredTimesTwo = 2*rxSquared, rySquaredTimesTwo = 2*rySquared;
	int x = 0, y = radiusY;
	int p = static_cast<int>(std::round(rySquared - rxSquared*radiusY + 0.25*rxSquared));
	while (rySquaredTimesTwo*x < rxSquaredTimesTwo*y)
	{
		drawInQuadrants(center[0], center[1], x, y);
		p += rySquaredTimesTwo*++x + rySquared;
		if (p >= 0)
			p -= rxSquaredTimesTwo*--y;
	}
	p = static_cast<int>(std::round(rySquared*(x*x + x + 0.25) + rxSquared*(y*y - 2*y + 1) - rxSquared*rySquared));
	while (y > 0)
	{
		drawInQuadrants(center[0], center[1], x, y);
		p += rxSquared - rxSquaredTimesTwo*--y;
		if (p <= 0)
			p += rySquaredTimesTwo*++x;
	}
}

void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, k_width, 0, k_height);
	glClear(GL_COLOR_BUFFER_BIT);
}

// Display functions:

void drawOlympicRingsAsCircles()
{
	glBegin(GL_POINTS);
	int radius = std::min(k_width, k_height) / 6;
	int center[2];
	for (int i = 0; i < 5; ++i)
	{
		// `center[1]` = 3`k_height`/4 when `i` is even, `k_height`/2 otherwise.
		center[0] = (i + 1)*radius, center[1] = 3*k_height/4 - (i % 2)*k_height/4;
		drawCircle(center, radius, k_ringColors[i]);
	}
	glEnd();
	glFlush();
}

void drawOlympicRingsAsEllipses()
{
	glBegin(GL_POINTS);
	int scale = std::min(k_width, k_height);
	int radiusX = scale / 6, radiusY = scale / 8;
	int center[2];
	for (int i = 0; i < 5; ++i)
	{
		// `center[1]` = 3`k_height`/4 when `i` is even, 11`k_height`/20 otherwise.
		center[0] = (i + 1)*radiusX, center[1] = 3*k_height/4 - (i % 2)*k_height/5;
		drawEllipse(center, radiusX, radiusY, k_ringColors[i]);
	}
	glEnd();
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(k_windowPosX, k_windowPosY);
	glutInitWindowSize(k_width, k_height);
	glutCreateWindow("Olympic Rings");
	init();
	glutDisplayFunc(drawOlympicRingsAsEllipses);
	glutMainLoop();
	return 0;
}