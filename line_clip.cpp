#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <array>
#include <vector>
#include <algorithm>

using point_t = std::array<int, 2>;

constexpr int k_displayXMin = 0;
constexpr int k_displayXMax = 300;
constexpr int k_displayYMin = 0;
constexpr int k_displayYMax = 300;

double copySignum(double x, double y)
{
	return y ? std::copysign(x, y) : 0.0;
}

void lineBresenham(int x1, int y1, int x2, int y2)
{
	int deltaX = std::abs(x2 - x1), deltaY = std::abs(y2 - y1);
	int x = x1, y = y1;
	int *domainVar, *rangeVar, *domainDelta, *rangeDelta;
	int domainStep, rangeStep;
	if (deltaX > deltaY)
	{
		domainVar = &x, rangeVar = &y;
		domainDelta = &deltaX, rangeDelta = &deltaY;
		domainStep = static_cast<int>(copySignum(1.0, x2 - x1));
		rangeStep = static_cast<int>(copySignum(1.0, y2 - y1));
	}
	else
	{
		domainVar = &y, rangeVar = &x;
		domainDelta = &deltaY, rangeDelta = &deltaX;
		domainStep = static_cast<int>(copySignum(1.0, y2 - y1));
		rangeStep = static_cast<int>(copySignum(1.0, x2 - x1));
	}
	int p = 2*(*rangeDelta) - *domainDelta;
	// `d_lower` and `d_upper` are special cases of `d_same` and `d_changed` for when slope is positive.
	int same = 2*(*rangeDelta);
	int changed = same - 2*(*domainDelta);
	glBegin(GL_POINTS);
	for (int i = 0; i < *domainDelta; ++i)
	{
		glVertex2i(x, y);
		*domainVar += domainStep;
		if (p < 0)
			p += same;
		else
		{
			p += changed;
			*rangeVar += rangeStep;
		}
	}
	glEnd();
}

void clipLineLiangBarsky(const point_t& p1, const point_t& p2, const point_t& min, const point_t& max)
{
	// -deltaX, deltaX, -deltaY, deltaY
	int a[4]{p1[0] - p2[0], p2[0] - p1[0], p1[1] - p2[1], p2[1] - p1[1]};
	int b[4]{p1[0] - min[0], max[0] - p1[0], p1[1] - min[1], max[1] - p1[1]};
	std::vector<double> outToIn{0.0}, inToOut{1.0};
	for (int i = 0; i < 4; ++i)
	{
		if (a[i] < 0)
			outToIn.push_back(static_cast<double>(b[i]) / a[i]);
		else if (a[i] > 0)
			inToOut.push_back(static_cast<double>(b[i]) / a[i]);
		else if (a[i] == 0 && b[i] >= 0)
		{
			// If a line is parallel to one side of a rectangle, it's parallel to the opposite side as well, so `i` must
			// equal zero or two.
			if (i)
			{
				auto yVals(std::minmax(p1[1], p2[1]));
				return lineBresenham(min[0], yVals.first, max[0], yVals.second);
			}
			else
			{
				auto xVals(std::minmax(p1[0], p2[0]));
				return lineBresenham(xVals.first, min[1], xVals.second, max[1]);
			}
		}
		else
			return;
	}
	double u1 = *std::max_element(outToIn.begin(), outToIn.end()), u2 = *std::min_element(inToOut.begin(), inToOut.end());
	// If `u1 == u2`, the line need not be drawn, as it would have a length of zero.
	if (u1 < u2)
		lineBresenham(std::lround(p1[0] + u1*a[1]), std::lround(p1[1] + u1*a[3]), std::lround(p1[0] + u2*a[1]),
			std::lround(p1[1] + u2*a[3]));
}

// Display functions:

void builtinClip()
{
	gluOrtho2D(-51.0, 51.0, -5.0, 21.0);
	glViewport(0, 0, 200, 200);
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
		glVertex2i(-35, -50);
		glVertex2i(-20, 30);
		glVertex2i(-12, -10);
		glVertex2i(5, 50);
		glVertex2i(12, -10);
		glVertex2i(25, 30);
		glVertex2i(40, -50);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex2i(-25, 0);
		glVertex2i(-20, -10);
		glVertex2i(-15, 0);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex2i(-10, 0);
		glVertex2i(-5, -10);
		glVertex2i(5, 0);
		glVertex2i(7, -10);
		glVertex2i(10, 0);
	glEnd();
	glBegin(GL_LINE_STRIP);
		glVertex2i(15, 0);
		glVertex2i(20, -10);
		glVertex2i(30, 0);
	glEnd();
	glFlush();
}

void customClip()
{
	static constexpr point_t k_min{0, 45}, k_max{100, 100};
	static constexpr point_t mountain[7]{{15, 0}, {30, 80}, {38, 40}, {55, 100}, {62, 40}, {75, 80}, {90, 0}};
	static constexpr point_t snowLine1[3]{{25, 50}, {30, 40}, {35, 50}};
	static constexpr point_t snowLine2[5]{{40, 50}, {45, 40}, {55, 50}, {57, 40}, {60, 50}};
	static constexpr point_t snowLine3[3]{{65, 50}, {70, 40}, {80, 50}};
	
	gluOrtho2D(k_displayXMin, k_displayXMax, k_displayYMin, k_displayYMax);
	glColor3d(0.0, 0.0, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScaled(2.0, 2.0, 0.0);
	for (int i = 1; i < 7; ++i)
		clipLineLiangBarsky(mountain[i-1], mountain[i], k_min, k_max);
	clipLineLiangBarsky(mountain[6], mountain[0], k_min, k_max);
	for (int i = 1; i < 3; ++i)
		clipLineLiangBarsky(snowLine1[i-1], snowLine1[i], k_min, k_max);
	for (int i = 1; i < 5; ++i)
		clipLineLiangBarsky(snowLine2[i-1], snowLine2[i], k_min, k_max);
	for (int i = 1; i < 3; ++i)
		clipLineLiangBarsky(snowLine3[i-1], snowLine3[i], k_min, k_max);
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(k_displayXMax - k_displayXMin, k_displayYMax - k_displayYMin);
	glutCreateWindow("Line Clip");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
	glutDisplayFunc(customClip);
	glutMainLoop();
	return 0;
}