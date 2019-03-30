#include <windows.h>
#include <GL/glut.h>
#include <cmath>

using lineAlgorithm_t = void(*)(int, int, int, int);

constexpr int k_width = 300;
constexpr int k_height = 300;
constexpr int k_windowPosX = 100;
constexpr int k_windowPosY = 100;

/* 
 * For both the DDA and Bresenham's algorithm, the line function is from `x` to `y` if `deltaX` > `deltaY`, and from `y` to
 * `x` otherwise. The implementations below take advantage of the fact that neither algorithm requires substantial
 * modification if the line function is instead defined from `d` to `r`, where `d` is an element of its domain and `r` is an
 * element of its range, and where the domain and range depend on whether `deltaX` is smaller or larger than `deltaY`.
 */
 
double signum(double x, double y)
{
	return static_cast<bool>(y) * std::copysign(x, y);
}

void lineDDA(int x1, int y1, int x2, int y2)
{
	double deltaX = x2 - x1, deltaY = y2 - y1;
	double x = x1, y = y1;
	double stop, domainStep, rangeStep, m;
	double *domainVar, *rangeVar;
	if (std::abs(deltaX) > std::abs(deltaY))
	{
		m = deltaY / deltaX, stop = x2;
		domainVar = &x, rangeVar = &y;
		domainStep = signum(1.0, deltaX);
		rangeStep = signum(m, deltaY);
	}
	else
	{
		m = deltaX / deltaY, stop = y2;
		domainVar = &y, rangeVar = &x;
		domainStep = signum(1.0, deltaY);
		rangeStep = signum(m, deltaX);
	}
	while (std::abs(*domainVar - stop))
	{
		glVertex2i(std::round(x), std::round(y));
		*domainVar += domainStep, *rangeVar += rangeStep;
	}
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
		domainStep = static_cast<int>(signum(1.0, x2 - x1));
		rangeStep = static_cast<int>(signum(1.0, y2 - y1));
	}
	else
	{
		domainVar = &y, rangeVar = &x;
		domainDelta = &deltaY, rangeDelta = &deltaX;
		domainStep = static_cast<int>(signum(1.0, y2 - y1));
		rangeStep = static_cast<int>(signum(1.0, x2 - x1));
	}
	int p = 2*(*rangeDelta) - *domainDelta;
	// `d_lower` and `d_upper` are special cases of `d_same` and `d_changed` for when slope is positive.
	int same = 2*(*rangeDelta);
	int changed = same - 2*(*domainDelta);
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
}

void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, k_width, 0, k_height);
	glClear(GL_COLOR_BUFFER_BIT);
}

void drawScene(lineAlgorithm_t alg)
{
	glBegin(GL_POINTS);
	// house 1
	glColor3f(0.0, 0.0, 0.0);
	alg(5, 5, 5, 20);
	alg(5, 20, 47, 20);
	alg(47, 5, 47, 20);
	alg(5, 5, 47, 5);
	alg(5, 20, 26, 41);
	alg(26, 41, 47, 20);
	// house 2
	glColor3f(0.6, 0.4, 0.3);
	alg(55, 5, 55, 20);
	alg(55, 20, 97, 20);
	alg(97, 5, 97, 20);
	alg(55, 5, 97, 5);
	alg(55, 20, 76, 100);
	alg(76, 100, 97, 20);
	// house 3
	glColor3f(0.6, 0.0, 0.3);
	alg(105, 5, 105, 20);
	alg(105, 20, 147, 20);
	alg(147, 5, 147, 20);
	alg(105, 5, 147, 5);
	alg(105, 20, 126, 25);
	alg(126, 25, 147, 20);
	// star
	glColor3f(1.0, 1.0, 0.0);
	alg(105, 100, 147, 100);
	alg(126, 76, 126, 124);
	alg(147, 124, 105, 76);
	alg(105, 124, 147, 76);
	alg(137, 76, 116, 124);
	alg(137, 124, 116, 76);
	alg(147, 88, 105, 112);
	alg(147, 112, 105, 88);
	glEnd();
	glFlush();
}

// Display functions:
void drawWithDDA() {drawScene(lineDDA);}
void drawWithBresenham() {drawScene(lineBresenham);}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(k_windowPosX, k_windowPosY);
	glutInitWindowSize(k_width, k_height);
	glutCreateWindow("Line Algorithms");
	init();
	glutDisplayFunc(drawWithBresenham);
	glutMainLoop();
	return 0;
}