#include <windows.h>
#include <GL/glut.h>
#include <functional>
#include <cmath>

/*
 * Colors are represented by RGBA values, where each of the four components takes up one (unsigned) byte. These bytes are
 * packed into an unsigned int.
 */

constexpr int k_xMin = 0;
constexpr int k_xMax = 300;
constexpr int k_yMin = 0;
constexpr int k_yMax = 300;
constexpr int k_radius = 45;
constexpr int k_ringCenterOffsets[5][2]{{50, 200}, {150, 200}, {250, 200}, {100, 150}, {200, 150}};
constexpr unsigned int k_ringBoundaryColors[5]{0xFFFF0000, 0xFF000000, 0xFF0000FF, 0xFF00FFFF, 0xFF00FF00};
constexpr unsigned int k_ringFillColors[5]{0xFF000000, 0xFF0000FF, 0xFF00FFFF, 0xFF00FF00, 0xFFFF0000};

void setPixel(int x, int y, unsigned int color)
{
	glBegin(GL_POINTS);
	glColor4ub(color, color >> 8, color >> 16, color >> 24);
	glVertex2i(x, y);
	glEnd();
}

/*
 * Fills the region surrounding the point (x, y) with a given color while a condition (of the type described below) is true,
 * via a template for the family of algorithms that includes flood fill and boundary fill, specifically their 4-connected
 * variants. It is guaranteed to work properly for any convex region.
 *
 * A `BinaryColorPredicate` takes arguments `pixelColor` and `param`, where `pixelColor` is the color of the next pixel to
 * be set, and `param` is a color passed to `fillWhile` that serves as a parameter (in the mathematical sense) that is bound
 * to the predicate.
 */
template <typename BinaryColorPredicate>
void fillWhile(int x, int y, unsigned int fillColor, const BinaryColorPredicate& pred, unsigned int param)
{
	unsigned int pixelColor;
	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixelColor);
	if (pred(param, pixelColor))
	{
		setPixel(x, y, fillColor);
		glReadPixels(x+1, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixelColor);
		int rightOffset = 0;
		while (pred(param, pixelColor))
		{
			setPixel(x + ++rightOffset, y, fillColor);
			glReadPixels(x + rightOffset + 1, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixelColor);
		}
		glReadPixels(x-1, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixelColor);
		int leftOffset = 0;
		while (pred(param, pixelColor))
		{
			setPixel(x - ++leftOffset, y, fillColor);
			glReadPixels(x - leftOffset - 1, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixelColor);
		}
		fillWhile(x, y+1, fillColor, pred, param);
		fillWhile(x, y-1, fillColor, pred, param);
	}
}

void floodFill(int x, int y, unsigned int interiorColor, unsigned int fillColor)
{
	fillWhile(x, y, fillColor, std::equal_to<unsigned int>(), interiorColor);
}

void boundaryFill(int x, int y, unsigned int boundaryColor, unsigned int fillColor)
{
	fillWhile(x, y, fillColor,
		[=](unsigned int param, unsigned int pixelColor){return pixelColor != param && pixelColor != fillColor;},
		boundaryColor);
}

void drawInQuadrants(int centerX, int centerY, int x, int y)
{
	glBegin(GL_POINTS);
	glVertex2i(centerX + x, centerY + y);
	glVertex2i(centerX - x, centerY + y);
	glVertex2i(centerX - x, centerY - y);
	glVertex2i(centerX + x, centerY - y);
	glEnd();
}

// uses the ellipse midpoint algorithm
void drawEllipse(const int center[2], int radiusX, int radiusY, unsigned int color)
{
	glColor4ub(color, color >> 8, color >> 16, color >> 24);
	int rxSquared = radiusX*radiusX, rySquared = radiusY*radiusY;
	int rxSquaredTimesTwo = 2*rxSquared, rySquaredTimesTwo = 2*rySquared;
	int x = 0, y = radiusY;
	int p = static_cast<int>(std::round(rySquared - rxSquared*radiusY + 0.25*rxSquared));
	while (rySquaredTimesTwo*x <= rxSquaredTimesTwo*y)
	{
		drawInQuadrants(center[0], center[1], x, y);
		p += rySquaredTimesTwo*++x + rySquared;
		if (p >= 0)
			p -= rxSquaredTimesTwo*--y;
	}
	p = static_cast<int>(std::round(rySquared*(x*x + x + 0.25) + rxSquared*(y*y - 2*y + 1) - rxSquared*rySquared));
	while (y >= 0)
	{
		drawInQuadrants(center[0], center[1], x, y);
		p += rxSquared - rxSquaredTimesTwo*--y;
		if (p <= 0)
			p += rySquaredTimesTwo*++x;
	}
}

void init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(k_xMin, k_xMax, k_yMin, k_yMax);
	glClear(GL_COLOR_BUFFER_BIT);
}

// Display functions:

void useFloodFill()
{
	for (int i = 0; i < 5; ++i)
	{
		int center[2]{k_xMin + k_ringCenterOffsets[i][0], k_yMin + k_ringCenterOffsets[i][1]};
		drawEllipse(center, k_radius, k_radius, k_ringBoundaryColors[i]);
		floodFill(center[0], center[1], 0xFFFFFFFF, k_ringFillColors[i]);
		glFlush();
	}
}

void useBoundaryFill()
{
	for (int i = 0; i < 5; ++i)
	{
		int center[2]{k_xMin + k_ringCenterOffsets[i][0], k_yMin + k_ringCenterOffsets[i][1]};
		drawEllipse(center, k_radius, k_radius, k_ringBoundaryColors[i]);
		boundaryFill(center[0], center[1], k_ringBoundaryColors[i], k_ringFillColors[i]);
		glFlush();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(k_xMax - k_xMin, k_yMax - k_yMin);
	glutCreateWindow("Olympic Rings");
	init();
	glutDisplayFunc(useFloodFill);
	glutMainLoop();
	return 0;
}