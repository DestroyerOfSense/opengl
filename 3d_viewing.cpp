#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glut.h>
#include <cmath>

constexpr double pi = 3.14159265358979;
constexpr double viewX = 0, viewY = 100, viewZ = 100;
constexpr int circleSides = 360;
constexpr int partitionSize = 24;

enum class Axis
{
	X, Y, Z
};

void drawCircle(Axis a, Axis b, double x0, double y0, double z0, double r)
{
	int addCosTo = static_cast<int>(a), addSinTo = static_cast<int>(b);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < circleSides; ++i)
	{
		double coords[3]{x0, y0, z0};
		double theta = 2*pi*i / circleSides;
		coords[addCosTo] += r*std::cos(theta), coords[addSinTo] += r*std::sin(theta);
		glVertex3dv(coords);
	}
	glEnd();
}

void drawCube()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	// back side
	glColor3d(0, 1, 1);
	glVertex3i(-60, -150, -70);
	glVertex3i(-60, 0, -70);
	glVertex3i(40, 0, -50);
	glVertex3i(40, -150, -50);
	// left side
	glColor3d(0, 0, 1);
	glVertex3i(-60, -150, -70);
	glVertex3i(-60, 0, -70);
	glVertex3i(-100, 0, -10);
	glVertex3i(-100, -150, -10);
	// right side
	glColor3d(1, 0, 0);
	glVertex3i(40, -150, -50);
	glVertex3i(40, 0, -50);
	glVertex3i(0, 0, 10);
	glVertex3i(0, -150, 10);
	// front side
	glColor3d(0, 1, 0);
	glVertex3i(0, 0, 10);
	glVertex3i(0, -150, 10);
	glVertex3i(-100, -150, -10);
	glVertex3i(-100, 0, -10);
	glEnd();
}

void drawSphere()
{
	static constexpr double x0 = 110, y0 = 25, z0 = 75, r = 60;
	
	glColor3d(0, 1, 0);
	for (int i = 0; i < partitionSize; ++i)
	{
		double phi = pi*i / partitionSize;
		double c = r*std::cos(phi), s = r*std::sin(phi);
		drawCircle(Axis::X, Axis::Z, x0, y0 + c, z0, s);
		drawCircle(Axis::Y, Axis::Z, x0 + c, y0, z0, s);
	}
}

void drawCone()
{
	static constexpr double x0 = -80, y0 = 50, z0 = -125;
	static constexpr int rMax = 60;
	
	glColor3d(0.4, 0.2, 0);
	for (int r = 1; r <= rMax; r += 2)
		drawCircle(Axis::X, Axis::Z, x0, y0 - r, z0, r);
	glBegin(GL_LINES);
	for (int i = 0; i < partitionSize; ++i)
	{
		double theta = 2*pi*i / partitionSize;
		glVertex3d(x0, y0, z0);
		glVertex3d(x0 + rMax*std::cos(theta), y0 - rMax, z0 + rMax*std::sin(theta));
	}
	glEnd();
}

void init()
{
	glClearColor(1, 1, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(viewX, viewY, viewZ, 0, 0, 0, 0, 1, 0);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-300, 300, -300, 300, -300, 300);
	glClear(GL_COLOR_BUFFER_BIT);
}

void reshapeFunc(int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
}

// Display functions:

void drawShapesCustom()
{
	drawCube();
	drawSphere();
	drawCone();
	glFlush();
}

void drawShapesBuiltin()
{
	static constexpr int sliceCount = 32;
	
	// plotting cube
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(-100, -100, 0);
	glRotated(90, viewX, viewY, viewZ);
	glColor3d(0, 1, 0);
	glutSolidCube(150);
	// plotting sphere
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(100, -100, 0);
	glRotated(90, viewX, viewY, viewZ);
	glColor3d(0.8, 0.4, 0);
	glutWireSphere(90, sliceCount, sliceCount);
	// plotting cone
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(-100, 125, 0);
	glRotated(90, viewX, -viewY, viewZ);
	glColor3d(0, 0.3, 0.6);
	glutWireCone(75, 75, sliceCount, sliceCount);
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(600, 600);
	glutCreateWindow("3D Shapes");
	init();
	glutDisplayFunc(drawShapesBuiltin);
	glutReshapeFunc(reshapeFunc);
	glutMainLoop();
	return 0;
}