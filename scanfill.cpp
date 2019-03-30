#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include <array>
#include <vector>
#include <algorithm>
#include <list>

constexpr int k_xMin = 0;
constexpr int k_yMin = 0;
constexpr int k_xMax = 400;
constexpr int k_yMax = 300;
constexpr int k_width = k_xMax - k_xMin;
constexpr int k_height = k_yMax - k_yMin;

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

class Side
{
	private:
		int m_intercept;
		int m_maxY;
		int m_deltaX;
		int m_deltaY;
		int m_counter = 0;
		
	public:
		Side(int startX, int maxY, int deltaX, int deltaY):
			m_intercept(startX), m_maxY(maxY), m_deltaX(deltaX), m_deltaY(deltaY) {}
			
		Side(const Side& other):
			m_intercept(other.m_intercept), m_maxY(other.m_maxY), m_deltaX(other.m_deltaX), m_deltaY(other.m_deltaY) {}
			
		Side(Side&& other):
			m_intercept(other.m_intercept), m_maxY(other.m_maxY), m_deltaX(other.m_deltaX), m_deltaY(other.m_deltaY) {}
			
		int getIntercept() const {return m_intercept;}
		int getMaxY() const {return m_maxY;}
		
		void incrY()
		{
			int div = m_counter / m_deltaY;
			if (div)
			{
				m_intercept += div;
				m_counter %= m_deltaY;
			}
			m_counter += m_deltaX;
		}
		
		friend bool operator<(const Side& first, const Side& second);
};

bool operator<(const Side& first, const Side& second)
{
	return first.m_intercept < second.m_intercept;
}

void initRows(std::list<Side> rows[k_height], std::vector<std::array<int, 4>>& sides)
{
	auto *sideOne = &sides.at(sides.size() - 2), *sideTwo = &sides.at(sides.size() - 1);
	for (int i = 0; i < sides.size(); ++i)
	{
		int &y1 = (*sideOne)[1], &y2 = (*sideOne)[3], &y3 = (*sideTwo)[3];
		if (y1 != y2)
		{
			if (y1 < y2)
				rows[y1 - k_yMin].emplace_back((*sideOne)[0], y2, (*sideOne)[2] - (*sideOne)[0], y2 - y1);
			else
				rows[y2 - k_yMin].emplace_back((*sideOne)[2], y1, (*sideOne)[0] - (*sideOne)[2], y1 - y2);
		}
		sideOne = sideTwo, sideTwo = &sides[i];
	}
	for (int i = 0; i < k_height; ++i)
		rows[i].sort();
}

void scanFill(std::vector<std::array<int, 4>>& sides)
{
	std::list<Side> rows[k_height];
	initRows(rows, sides);
	std::list<Side> activeSides;
	for (int i = 0, y = k_yMin; i < k_height; ++i, ++y)
	{
		activeSides.remove_if([=](Side& s){return y >= s.getMaxY();});
		activeSides.sort();
		activeSides.merge(rows[i]);
		auto orig(activeSides.begin()), dest(activeSides.begin());
		bool filling = true;
		while (++dest != activeSides.end())
		{
			if (filling)
				lineBresenham(orig->getIntercept(), y, dest->getIntercept(), y);
			orig->incrY();
			filling = !filling;
			++orig;
		}
		if (activeSides.size())
			activeSides.back().incrY();
	}
}

void init()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(k_xMin, k_xMax, k_yMin, k_yMax);
	glClear(GL_COLOR_BUFFER_BIT);
}

void fillPoly()
{
	std::vector<std::array<int, 4>> vertices
	{
		{300, 50, 50, 50},
		{50, 50, 55, 200},
		{55, 200, 70, 180},
		{70, 180, 80, 250},
		{80, 250, 98, 150},
		{98, 150, 200, 120},
		{200, 120, 205, 210},
		{205, 210, 220, 112},
		{220, 112, 250, 160},
		{250, 160, 300, 50}
	};
	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	scanFill(vertices);
	glEnd();
	glFlush();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(k_width, k_height);
	glutCreateWindow("Scan Fill");
	init();
	glutDisplayFunc(fillPoly);
	glutMainLoop();
	return 0;
}
	