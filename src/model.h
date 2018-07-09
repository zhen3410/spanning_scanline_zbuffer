#pragma once
#ifndef MODEL
#define MODEL
#include<vector>
#include<GL\freeglut.h>
using namespace std;

struct Color {
	int r, g, b;
};

struct point {
	double x, y, z;
};

struct vertex {
	int x, y, z;
};

struct edge
{
	vertex x, y;
};

struct norm {
	double x, y, z;
};

struct Face {
	point v[3];
	Face* next;
};

struct Model {
	int faceNum;
	int scale;
	Face* face;
};

class OBJ {
public:
	OBJ() = default;

	void Load_OBJ(const char* filename);
	void insert_face(Face* face);
	void init_face_list();

	Model model;

	void rotatey(int theta);
	void rotatex(int theta);
	void m_plus(int scale);
	void m_minu(int scale);
	void to_up(int step);
	void to_down(int step);
	void to_left(int step);
	void to_right(int step);

	void trans(unsigned char key, int scale);
};

struct sort_edge_table {
	int id;
	int ymax, dy;
	double dx, x;
	sort_edge_table* next;
};

struct sort_pol_table {
	int id;
	int dy;
	double a, b, c, d;
	Color color;
	sort_pol_table* next;
};

struct act_pol_table {
	int id;
	int dy;
	double a, b, c, d;
	Color color;
	bool flag;
	act_pol_table* next;
};

struct act_edge_table {
	int dy;
	int id;
	double x, dx;
	act_edge_table* next;
};

class zbuffer {
public:

	vertex roundVer(point x, int scale);
	void creatDS(Model model);
	void addAct(int scan_line);
	void exc_zbuffer(int scan_line);
	void insert_edge(int ind, sort_edge_table* p);
	void insert_pol(int ind, sort_pol_table*p);
	void insert_act_pol(act_pol_table* pol);
	void insert_act_edge(act_edge_table* edge);
	void update_act();
	void init();
	void sort_act_edge();

	GLubyte x_resolution[1080][3];

private:

	sort_pol_table * dis_pol[1000];
	sort_edge_table* dis_edge[1000];

	act_edge_table* act_edge;
	act_pol_table* act_pol;
};


#endif // ! MODEL
