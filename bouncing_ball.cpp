#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <array>

using point_t = std::array<double, 2>;

constexpr double k_twoPi = 6.283185307179586;
constexpr int k_xMin = -200, k_xMax = 400;
constexpr int k_yMin = 170, k_yMax = 470;

class Ball
{
	private:
		point_t m_start;
		point_t m_center;
		point_t m_triangle[3];
		point_t* m_endpoints;
		int m_sideCount;
		int m_theta = 0;
		
	public:
		// `sides` is a multiple of three, ensuring the triangle can be correctly aligned within the circle.
		Ball(double startX, double startY, int sides = 360):
			m_start{startX, startY}, m_center{startX, startY}, m_endpoints(new point_t[sides]), m_sideCount(sides)
		{
			double r = 25, c = k_twoPi / sides;
			for (int i = 0; i < sides; ++i)
			{
				double theta = c*i;
				m_endpoints[i][0] = startX + r*std::cos(theta);
				m_endpoints[i][1] = startY + r*std::sin(theta);
			}
			for (int i = 0; i < 3; ++i)
				m_triangle[i] = m_endpoints[sides*i / 3];
		}
		
		~Ball()
		{
			delete[] m_endpoints;
		}
		
		double getStartX() const {return m_start[0];}
		double getStartY() const {return m_start[1];}
		
		void setCenter(double x, double y)
		{
			double deltaX = x - m_center[0], deltaY = y - m_center[1];
			m_center[0] = x, m_center[1] = y;
			for (int i = 0; i < m_sideCount; ++i)
				m_endpoints[i][0] += deltaX, m_endpoints[i][1] += deltaY;
			for (int i = 0; i < 3; ++i)
				m_triangle[i][0] += deltaX, m_triangle[i][1] += deltaY;
		}
		
		// `theta` should be in the interval [0, 2pi).
		void setAngle(double theta)
		{
			int rho = std::lround((m_sideCount * theta) / (k_twoPi));
			for (int i = 0; i < 3; ++i)
				m_triangle[i] = m_endpoints[(rho + (i*m_sideCount) / 3) % m_sideCount];
			m_theta = rho;
		}
		
		void draw() const
		{
			glBegin(GL_POLYGON);
			glColor3d(1, 0, 0);
			for (int i = 0; i < m_sideCount; ++i)
				glVertex2d(m_endpoints[i][0], m_endpoints[i][1]);
			glEnd();
			glColor3d(0, 1, 0);
			glBegin(GL_TRIANGLES);
			for (int i = 0; i < 3; ++i)
				glVertex2d(m_triangle[i][0], m_triangle[i][1]);
			glEnd();
		}
};

// Globals:
static double g_deltaX = 0, g_deltaY = 0, g_theta = 0;
static bool g_paused = false;
static Ball g_ball(-150, 200);

double ballOffset(double deltaX)
{
	return g_ball.getStartY() * std::abs(std::sin(6.26*deltaX + k_twoPi)) * std::exp(-0.005*deltaX);
}

// actually returns absolute value of deltaTheta
double deltaTheta(double deltaX)
{
	double result = (k_twoPi / 180) * (1 - 0.75*deltaX / (k_xMax - k_xMin));
	return result > 0 ? result : 0;
}

void displayBall()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	
	g_deltaY = ballOffset(++g_deltaX);
	g_ball.setCenter(g_ball.getStartX() + g_deltaX, g_ball.getStartY() + g_deltaY);
	g_ball.setAngle(g_theta);
	g_ball.draw();
	
	glPopMatrix();
	glutSwapBuffers();
	glFlush();
	Sleep(3);
}


void rotateBall()
{
	g_theta -= deltaTheta(g_deltaX);
	g_theta = std::fmod(k_twoPi + g_theta, k_twoPi);
	glutPostRedisplay();
}

void winReshapeFunc(int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(k_xMin, k_xMax, k_yMin, k_yMax);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT);
}

void togglePauseKey(unsigned char key, int x, int y)
{
	if (key == 'p')
	{
		g_paused = !g_paused;
		glutIdleFunc(g_paused ? nullptr : rotateBall);
	}
}

void togglePauseMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		g_paused = !g_paused;
		glutIdleFunc(g_paused ? nullptr : rotateBall);
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(150, 150);
	glutInitWindowSize(k_xMax - k_xMin, k_yMax - k_yMin);
	glutCreateWindow("Bouncing Ball");
	glClearColor(1, 1, 1, 0);
	glutDisplayFunc(displayBall);
	glutReshapeFunc(winReshapeFunc);
	glutIdleFunc(rotateBall);
	glutKeyboardFunc(togglePauseKey);
	glutMouseFunc(togglePauseMouse);
	glutMainLoop();
	return 0;
}