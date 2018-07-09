#include"stdafx.h"
#include"model.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<cstdio>
#include<vector>
#include<algorithm>
#include<GL/freeglut.h>
using namespace std;

const int halfx = 540;
const int halfy = 360;

void OBJ::Load_OBJ(const char* filename) {
	init_face_list();

	vector<point> ver;
	vector<norm> nor;
	model.faceNum = 0;

	ifstream in(filename);
	string line;
	double minx = 90000, maxx = -90000;

	while (getline(in, line)) {
		istringstream record(line);
		string info;
		
		record >> info;
		//导入顶点信息
		if (info[0] == 'v') {
			if (size(info) == 1) {
				point temp;
				double x, y, z;
				record >> x;
				record >> y;
				record >> z;
				if (x > maxx) {
					maxx = x;
				}
				if (x < minx) {
					minx = x;
				}
				temp.x = x;
				temp.y = y;
				temp.z = z;
				ver.push_back(temp);
			}
		}
		else if (info[0] == 'f') {
			//导入面片信息
			model.faceNum++;

			Face tempFace;
			int num = 0;
			while (record >> info) {
					int temp = 0;
					int len = size(info);
					int i = 0;
					while (info[i] != '/'&&i<len) {
						temp = temp * 10 + info[i] - '0';
						i++;
					}
					tempFace.v[num++] = ver[temp - 1];
			}
			tempFace.next = NULL;
			insert_face(&tempFace);
		}
	}

	//对过小的模型设置放大倍数，使模型正常显示
	double spanx = maxx - minx;
	model.scale = 1;
	if (spanx < 1)model.scale = 400;
	else if (spanx < 5)model.scale = 80;
	else if (spanx < 10)model.scale = 40;
	else if (spanx < 20)model.scale = 20;
	else if (spanx < 40)model.scale = 10;
}

//初始化面片链表
void OBJ::init_face_list() {
	model.face = (Face*)malloc(sizeof(Face));
	if (model.face == NULL)
	{
		printf("申请空间失败！");
		exit(0);
	}
	model.face->next = NULL;
}

//插入面片
void OBJ::insert_face(Face* face) {
	Face* temp = model.face;
	Face* f = new Face;
	*f = *face;
	while (temp->next != NULL )temp = temp->next;
	temp->next = f;
}


//初始化分类边表、分类多边形表以及活化边表和活化多边形表
//并且清除上次显示存留的链表
void zbuffer::init() {
	for (int i = 0; i < 1000; i++) {
		while (dis_edge[i] != NULL) {
				sort_edge_table* e = dis_edge[i];
				dis_edge[i] = dis_edge[i]->next;
				delete e;
		}
	}

	for (int i = 0; i < 1000; i++) {
		while (dis_pol[i] != NULL) {
			sort_pol_table* e = dis_pol[i];
			dis_pol[i] = dis_pol[i]->next;
			delete e;
		}
	}

	while (act_edge != NULL) {
		act_edge_table* ae = act_edge;
		act_edge = act_edge->next;
		delete ae;
	}

	while (act_pol != NULL) {
		act_pol_table* ae = act_pol;
		act_pol = act_pol->next;
		delete ae;
	}

	for (int i = 0; i < 1000; i++) {
		dis_edge[i] = (sort_edge_table*)malloc(sizeof(sort_edge_table));
		if (dis_edge[i] == NULL) {
			cout << "申请空间失败" << endl;
			exit(0);
		}
		dis_edge[i]->next = NULL;
	}

	for (int i = 0; i < 1000; i++) {
		dis_pol[i] = (sort_pol_table*)malloc(sizeof(sort_pol_table));
		if (dis_pol[i] == NULL) {
			cout << "申请空间失败" << endl;
			exit(0);
		}
		dis_pol[i]->next = NULL;
	}

	act_edge = (act_edge_table*)malloc(sizeof(act_edge_table));
	if (act_edge == NULL) {
		cout << "申请空间失败" << endl;
		exit(0);
	}
	act_edge->next = NULL;

	act_pol = (act_pol_table*)malloc(sizeof(act_pol_table));
	if (act_pol == NULL) {
		cout << "申请空间失败" << endl;
		exit(0);
	}
	act_pol->next = NULL;
}


//将原始顶点圆整为最近的整数坐标点
vertex zbuffer::roundVer(point x,int scale) {
	vertex v;
	
	v.x = (int)(x.x*scale + ((x.x < 0) ? -0.5 : 0.5));
	v.y = (int)(x.y*scale + ((x.y < 0) ? -0.5 : 0.5));
	v.z = (int)(x.z*scale + ((x.z < 0) ? -0.5 : 0.5));
	/*
	v.x = x.x + ((x.x < 0) ? -0.5 : 0.5);
	v.y = x.y + ((x.y < 0) ? -0.5 : 0.5);
	v.z = x.z + ((x.z < 0) ? -0.5 : 0.5);

	v.x = (int)(x.x*scale);
	v.y = (int)(x.y*scale);
	v.z = (int)(x.z*scale);*/
	return v;
}

//绕y轴旋转
void OBJ::rotatey(int theta) {

	double x = 3.14*theta / 180;
	double sinx = sin(x);
	double cosx = cos(x);

	Face* f = model.face->next;
	while (f) {
		for (int i = 0; i < 3; i++) {
			point* po = &f->v[i];
			float t;
			t = cosx * po->x - sinx * po->z;
			po->z = sinx * po->x + cosx * po->z;
			po->x = t;
		}
		f = f->next;
	}
}

//绕x轴旋转
void OBJ::rotatex(int theta) {

	double x = 3.14*theta / 180;
	double sinx = sin(x);
	double cosx = cos(x);

	Face* f = model.face->next;
	while (f) {
		for (int i = 0; i < 3; i++) {
			point* po = &f->v[i];
			float t;
			t = cosx * po->y - sinx * po->z;
			po->z = sinx * po->y + cosx * po->z;
			po->y = t;
		}
		f = f->next;
	}
}

//放大
void OBJ::m_plus(int scale) {

	double s = 1.0 + (double)scale / 100;

	Face* f = model.face->next;
	while (f) {
		for (int i = 0; i < 3; i++) {
			point* po = &f->v[i];
			po->x = po->x*s;
			po->y = po->y*s;
			po->z = po->z*s;
		}
		f = f->next;
	}
}

//缩小
void OBJ::m_minu( int scale) {

	double s = 1.0 + (double)scale / 100;

	Face* f = model.face->next;
	while (f) {
		for (int i = 0; i < 3; i++) {
			point* po = &f->v[i];
			po->x = po->x/s;
			po->y = po->y/s;
			po->z = po->z/s;
		}
		f = f->next;
	}
}

//向下平移
void OBJ::to_down(int step) {

	Face* f = model.face->next;
	while (f) {
		for (int i = 0; i < 3; i++) {
			point* po = &f->v[i];
			po->y = po->y - (double)step/model.scale;
		}
		f = f->next;
	}
}

//向上平移
void OBJ::to_up(int step) {

	Face* f = model.face->next;
	while (f) {
		for (int i = 0; i < 3; i++) {
			point* po = &f->v[i];
			po->y = po->y + (double)step/model.scale;
		}
		f = f->next;
	}
}

//向左平移
void OBJ::to_left(int step) {

	Face* f = model.face->next;
	while (f) {
		for (int i = 0; i < 3; i++) {
			point* po = &f->v[i];
			po->x = po->x - (double)step/model.scale;
		}
		f = f->next;
	}
}

//向右平移
void OBJ::to_right(int step) {

	Face* f = model.face->next;
	while (f) {
		for (int i = 0; i < 3; i++) {
			point* po = &f->v[i];
			po->x = po->x + (double)step/model.scale;
		}
		f = f->next;
	}
}

//控制模型运动
void OBJ::trans(unsigned char key,int scale) {

	switch (key) {
	case'y': {
		rotatey(scale);
		break;
	}
	case'x': {
		rotatex(scale);
		break;
	}
	case'+': {
		m_plus(scale);
		break;
	}
	case'-': {
		m_minu(scale);
		break;
	}
	case'8': {
		to_up(scale);
		break;
	}
	case'2': {
		to_down(scale);
		break;
	}
	case'4': {
		to_left(scale);
		break;
	}
	case'6': {
		to_right(scale);
		break;
	}
	}

}


bool cmp_p(vertex a, vertex b) {
	return a.y > b.y;
}

//建立分类边表和分类多边形表
void zbuffer::creatDS(Model model) {
	
	init();
	int length = model.faceNum;
	int scale = model.scale;
	Face* tempFace = model.face->next;
	int ind = 0;
	while(tempFace) {
		vertex ver[3];
		for (int j = 0; j < 3; j++) {
			ver[j] = roundVer(tempFace->v[j], scale);
		}

		int ymax = ver[0].y;
		int ymin = ver[0].y;
		for (int j = 1; j < 3; j++) {
			if (ver[j].y > ymax) {
				ymax = ver[j].y;
			}
			if (ver[j].y < ymin) {
				ymin = ver[j].y;
			}
		}
		int dy = ymax - ymin;

		double norm_face[3];

		double ax = ver[0].x - ver[1].x;
		double ay = ver[0].y - ver[1].y;
		double az = ver[0].z - ver[1].z;

		double bx = ver[1].x - ver[2].x;
		double by = ver[1].y - ver[2].y;
		double bz = ver[1].z - ver[2].z;

		norm_face[0] = (ay*bz - az * by);
		norm_face[1] = (az*bx - ax * bz);
		norm_face[2] = (ax*by - ay * bx);

		double a = norm_face[0];
		double b = norm_face[1];
		double c = norm_face[2];
		double d = -(a*ver[0].x + b * ver[0].y + c * ver[0].z);

		/*
		if (d != 0) {
			a = a / d;
			b = b / d;
			c = c / d;
			d = 1;
		}*/

		int id = ind++;

		sort_pol_table* pol=new sort_pol_table;

		sort(ver, ver+3, cmp_p);

		edge edge_i[3];
		edge_i[0].x = ver[0];
		edge_i[0].y = ver[1];
		edge_i[1].x = ver[1];
		edge_i[1].y = ver[2];
		edge_i[2].x = ver[0];
		edge_i[2].y = ver[2];


		for (int j = 0; j < 3; j++) {

			if (ind == 715) {
				int x = 0;
				int y = x * 5;
			}
			int ymax_edge, ymin_edge, dy;
			double dx;
			int x;
			//dx = -(edge_i[j].y.x[1] - edge_i[j].x.x[1]) / (edge_i[j].y.x[2] - edge_i[j].x.x[2]);
			 dx = (double)(edge_i[j].y.x - edge_i[j].x.x) / (edge_i[j].x.y - edge_i[j].y.y);
			//dx = -1 / k;

			if (edge_i[j].x.y > edge_i[j].y.y) {
				ymax_edge = edge_i[j].x.y;
				x = edge_i[j].x.x;
				ymin_edge = edge_i[j].y.y;
			}
			else {
				ymax_edge = edge_i[j].y.y;
				x = edge_i[j].y.x;
				ymin_edge = edge_i[j].x.y;
			}

			dy = ymax_edge - ymin_edge;

			sort_edge_table* temp = new sort_edge_table;
			temp->dx = dx;
			temp->dy = dy;
			temp->id = id;
			temp->x = x;
			temp->ymax = ymax_edge;
			temp->next = NULL;
			/*
			if (j == 1) {
				temp->dy--;
				temp->x += temp->dx;
				if (ymax_edge + halfy - 1 < 720 && (ymax_edge + halfy - 1 >= 0)) {
					insert_edge(ymax_edge + halfy - 1, temp);
				}
			}
			else {
				if (ymax_edge + halfy < 720 && (ymax_edge + halfy >= 0)) {
					insert_edge(ymax_edge + halfy, temp);
				}
			}
			*/

			temp->dy--;
			temp->x += temp->dx;
			if (ymax_edge + halfy - 1 < 720 && (ymax_edge + halfy - 1 >= 0)) {
				insert_edge(ymax_edge + halfy - 1, temp);
			}
		}
		
		pol->a = a;
		pol->b = b;
		pol->c = c;
		pol->d = d;
		pol->dy = dy;
		pol->id = id;

		GLfloat po = abs(norm_face[2]) / sqrt(norm_face[0]*norm_face[0]+norm_face[1]*norm_face[1]+norm_face[2]*norm_face[2]);
		pol->color.r = (GLubyte)(255 * po);
		pol->color.g = (GLubyte)(255 * po);
		pol->color.b = (GLubyte)(255 * po);

		pol->next = NULL;
		if (ymax + halfy < 720 && (ymax + halfy >= 0)) {
			insert_pol(ymax + halfy, pol);
		}
		tempFace = tempFace->next;
	}
}

void zbuffer::insert_edge(int ind, sort_edge_table* p) {
	sort_edge_table* temp = dis_edge[ind];
	//sort_edge_table* e = p;
	//*e = *p;
	while (temp->next != NULL)temp = temp->next;
	temp->next = p;
}

void zbuffer::insert_pol(int ind, sort_pol_table* p) {
	sort_pol_table* temp = dis_pol[ind];
	//sort_pol_table* pol = p;
	//*pol = *p;
	while (temp->next != NULL)temp = temp->next;
	temp->next = p;
}

//执行zbuffer算法
void zbuffer::exc_zbuffer(int scan_line) {
	memset(x_resolution, 0, sizeof(GLubyte) * 3 * 1080);
	addAct(scan_line);


	int x_left = 0;
	int x_right;
	int num_pol = 0;

	act_edge_table* edge = act_edge->next;
	while (edge != NULL) {
		x_right = edge->x + halfx;
		if (num_pol == 0) {
			/*
			for (int i = x_left; i < x_right; i++) {
				x_resolution[i][0] = 0;
				x_resolution[i][1] = 0;
				x_resolution[i][2] = 0;
			}*/
		}
		else if (num_pol > 1) {
			double z_max = -1000000;
			act_pol_table* pol = act_pol->next;
			act_pol_table* p = new act_pol_table;
			while (pol) {
				if (pol->flag) {
					double z = (-pol->d - pol->a*(x_left - halfx + 1) - pol->b*(scan_line - halfy)) / pol->c;
					if (z > z_max) {
						z_max = z;
						*p = *pol;
					}
				}
				pol = pol->next;
			}
			for (int i = x_left; i < x_right&&i<1080; i++) {
				x_resolution[i][0] = p->color.r;
				x_resolution[i][1] = p->color.g;
				x_resolution[i][2] = p->color.b;
			}
			delete p;
		}
		else {
			act_pol_table* p = act_pol->next;
			while (p) {
				if (p->flag) {
					break;
				}
				else {
					p = p->next;
				}
			}

			for (int i = x_left; i < x_right&&i<1080; i++) {
				x_resolution[i][0] = p->color.r;
				x_resolution[i][1] = p->color.g;
				x_resolution[i][2] = p->color.b;
			}
		}

		act_pol_table* a = act_pol->next;
		while (a != NULL) {
			if (a->id == edge->id) {
				a->flag = !a->flag;
				if (a->flag == 0) {
					num_pol--;
				}
				else {
					num_pol++;
				}
				break;
			}
			a = a->next;
		}

		x_left = x_right;
		edge = edge->next;
	}

	update_act();
	sort_act_edge();
}

//更新活化边表和活化多边形表
void zbuffer::update_act() {
	act_pol_table* p = act_pol;
	act_pol_table* q = p -> next;
	while (q != NULL) {
		q->dy--;
		if (q->dy < 0) {
			p->next = q->next;
			delete q;
			q = p->next;
		}
		else {
			p = p->next;
			q = p->next;
		}
	}

	act_edge_table* x = act_edge;
	act_edge_table* y = x->next;
	while (y != NULL) {
		y->dy--;
		if (y->dy < 0) {
			x->next = y->next;
			delete y;
			y = x->next;
		}
		else {
			y->x = y->x + y->dx;
			x = x->next;
			y = x->next;
		}
	}
}


//插入活化边
void zbuffer::insert_act_edge(act_edge_table* e) {
	act_edge_table* p;
	act_edge_table* q = act_edge;
	p = q->next;
	while (p != NULL) {
		if (e->x < p->x) {
			p = NULL;
		}
		else {
			q = p;
			p = p->next;
		}
	}
	e->next = q->next;
	q->next = e;
}

//插入活化多边形
void zbuffer::insert_act_pol(act_pol_table* p) {
	act_pol_table* temp = act_pol;
	//act_pol_table* pol = p;
	//*pol = *p;
	while (temp->next != NULL)temp = temp->next;
	temp->next = p;
}

//对活化边表进行分类
void zbuffer::sort_act_edge() {
	act_edge_table* q;
	act_edge_table* p = act_edge->next;
	act_edge->next = NULL;
	while (p) {
		q = p->next;
		insert_act_edge(p);
		p = q;
	}
}

void zbuffer::addAct(int scan_line) {
	sort_pol_table* temp = dis_pol[scan_line]->next;
	sort_edge_table* tempEdge = dis_edge[scan_line]->next;
	scan_line -= halfy;
	while (temp != NULL) {
		act_pol_table* pol = new act_pol_table;
		pol->a = temp->a;
		pol->b = temp->b;
		pol->c = temp->c;
		pol->d = temp->d;
		pol->dy = temp->dy;
		pol->id = temp->id;
		pol->flag = false;
		pol->color = temp->color;
		pol->next = NULL;
		
		insert_act_pol(pol);

		temp = temp->next;
	}
	while (tempEdge != NULL) {
		if (tempEdge->dy >= 0) {
			act_edge_table* edge = new act_edge_table;
			edge->dx = tempEdge->dx;
			edge->dy = tempEdge->dy;
			edge->id = tempEdge->id;
			//edge_flag[edge->id] = !edge_flag[edge->id];
			edge->x = tempEdge->x;
			//tempEdge->flag = true;
			edge->next = NULL;
			//edge->flag = edge_flag[edge->id];
			insert_act_edge(edge);
		}

		tempEdge = tempEdge->next;
	}
}
