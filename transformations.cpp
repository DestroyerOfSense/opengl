#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <array>
#include <valarray>

constexpr double pi = 3.14159265358979;
constexpr int k_xMin = 0;
constexpr int k_xMax = 250;
constexpr int k_yMin = 0;
constexpr int k_yMax = 250;

using std::valarray;
using std::slice;
using point_t = std::array<double, 2>;
using polygon_t = std::vector<point_t>;

class Transformer
{
	/*
	 * The dot product of two valarrays `v` and `w` is computed via (`v` * `w`).sum(). This idiom is used in the methods
	 * `matMul` and `apply`.
	 */
	
	private:
		static valarray<double> matMul(const valarray<double>& a, const valarray<double>& b)
		{
			valarray<double> product(9);
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
					product[3*i + j] = (a[slice(3*i, 3, 1)] * b[slice(j, 3, 3)]).sum();
			}
			return product;
		}
	
		valarray<double> m_matrix{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
		
	public:
		Transformer& addTranslation(double deltaX, double deltaY)
		{
			m_matrix[2] += deltaX;
			m_matrix[5] += deltaY;
			return *this;
		}
		
		Transformer& addRotation(double theta, const point_t& pivot = point_t{0.0, 0.0})
		{
			double cosine = std::cos(theta), sine = std::sin(theta);
			double deltaX = pivot[0]*(1 - cosine) + pivot[1]*sine;
			double deltaY = pivot[1]*(1 - cosine) - pivot[0]*sine;
			m_matrix = matMul(valarray<double>{cosine, -sine, deltaX, sine, cosine, deltaY, 0, 0, 1}, m_matrix);
			return *this;
		}
		
		Transformer& addScaling(double scalarX, double scalarY, const point_t& fixed = point_t{0.0, 0.0})
		{
			double deltaX = fixed[0]*(1 - scalarX);
			double deltaY = fixed[1]*(1 - scalarY);
			m_matrix = matMul(valarray<double>{scalarX, 0, deltaX, 0, scalarY, deltaY, 0, 0, 1}, m_matrix);
			return *this;
		}
		
		void apply(polygon_t& poly) const
		{
			for (point_t& p: poly)
			{
				valarray<double> v{p[0], p[1], 1};
				p[0] = (m_matrix[slice(0, 3, 1)] * v).sum();
				p[1] = (m_matrix[slice(3, 3, 1)] * v).sum();
			}
		}
};

void drawPolygon(const polygon_t& poly)
{
	glBegin(GL_POLYGON);
	for (const point_t& p: poly)
		glVertex2d(p[0], p[1]);
	glEnd();
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

void customTransform()
{
	polygon_t poly{{0.0, 100.0}, {50.0, 150.0}, {100.0, 100.0}, {50.0, 25.0}};
	glColor3d(0.0, 0.0, 1.0);
	drawPolygon(poly);
	glFlush();
	Transformer()
		.addRotation(5*pi / 3, point_t{40.0, 10.0})
		.addTranslation(0.0, 100.0)
		.addScaling(0.5, 0.5, point_t{200.0, 200.0})
		.apply(poly);
	glColor3d(1.0, 0.0, 0.0);
	drawPolygon(poly);
	glFlush();
}

void builtinTransform()
{
	polygon_t poly{{0.0, 100.0}, {50.0, 150.0}, {100.0, 100.0}, {50.0, 25.0}};
	glColor3d(0.0, 0.0, 1.0);
	drawPolygon(poly);
	glFlush();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3d(1.0, 0.0, 0.0);
	// Scaling:
	glTranslated(200.0, 200.0, 0.0);
	glScaled(0.5, 0.5, 0.0);
	glTranslated(-200.0, -200.0, 0.0);
	// Translation:
	glTranslated(0.0, 100.0, 0.0);
	// Rotation:
	glTranslated(40.0, 10.0, 0.0);
	glRotated(300.0, 0.0, 0.0, 1.0);
	glTranslated(-40.0, -10.0, 0.0);
	drawPolygon(poly);
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(k_xMax - k_xMin, k_yMax - k_yMin);
	glutCreateWindow("Transformations");
	init();
	glutDisplayFunc(customTransform);
	glutMainLoop();
	return 0;
}