//#define _CRT_SECURE_NO_WARNINGS
//#include <iostream>
//#include <GL/glut.h>
//#include <stdio.h>
//#include <math.h>
//using namespace std;
//#define WIRE 0
//#define SHADE 1
//
//typedef struct {
//	float x, y, z;
//} Point;
//
//typedef struct {
//	unsigned int ip[3];
//} Face;
//
//int pnum;
//int fnum;
//Point* mpoint = NULL;
//Face* mface = NULL;
//
//GLfloat angle = 0;
//
//int moving;
//int mousebegin;
//int light_moving;
//float scalefactor = 1.0f;
//int scaling = 0;
//int status = 0;
//
//string fname = "c:\\data\\CrystalFace.dat";
//
//void ReadModel() 
//{
//	FILE* f1; char s[81]; int i;
//	if (mpoint != NULL) delete[] mpoint;
//	if (mface != NULL) delete[] mface;
//
//	if((f1 = fopen(fname.c_str(), "rt")) == NULL ) 	{	cout << "파일 열기 오류" << endl; exit(0);}
//	fscanf(f1, "%s", s); printf("%s", s); fscanf(f1, "%s", s); printf("%s\n", s);
//	fscanf(f1, "%d", &pnum); printf("점의 개수: %d\n", pnum);
//	mpoint = new Point[pnum];
//
//	for (i = 0; i < pnum; i++) {
//		fscanf(f1, "%f %f %f", &mpoint[i].x, &mpoint[i].y, &mpoint[i].z);
//		printf("점 %d : %f %f %f\n", i, mpoint[i].x, mpoint[i].y, mpoint[i].z);
//	}
//
//
//	fscanf(f1, "%s", s); printf("%s", s); fscanf(f1, "%s", s); printf("%s\n", s);
//	fscanf(f1, "%d", &fnum); printf("면의 개수: %d\n", fnum);
//	mface = new Face[fnum];
//	
//	for(i = 0; i < fnum; i++) {
//		fscanf(f1, "%d %d %d", &mface[i].ip[0], &mface[i].ip[1], &mface[i].ip[2]);
//		printf("면 %d : %d %d %d\n", i, mface[i].ip[0], mface[i].ip[1], mface[i].ip[2]);
//	}
//	fclose(f1);
//
//}
//
//void DrawWire(void)
//{
//	glClearColor(1.0, 1.0, 1.0, 1.0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glEnable(GL_DEPTH_TEST);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glCallList(1);
//	glutSwapBuffers();
//}
//
//void DrawShade(void)
//{
//	glClearColor(1.0, 1.0, 1.0, 1.0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glEnable(GL_DEPTH_TEST);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glCallList(1);
//	glutSwapBuffers();
//}
//
//void GLSetupRC(void)
//{
//	glEnable(GL_DEPTH_TEST);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluPerspective(40.0, 1.0, 1.0, 2000.0);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	gluLookAt(400.0, 400.0, 400.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
//}
//
//Point cnormal(Point a, Point b, Point c)
//{
//	Point p, q, r;
//	double val;
//	p.x = b.x - a.x; p.y = b.y - a.y; p.z = b.z - a.z;
//	q.x = c.x - a.x; q.y = c.y - a.y; q.z = c.z - a.z;
//	r.x = p.y * q.z - p.z * q.y;
//	r.y = p.z * q.x - p.x * q.z;
//	r.z = p.x * q.y - p.y * q.x;
//	val = sqrt(r.x * r.x + r.y * r.y + r.z * r.z);
//	r.x = r.x / val; r.y = r.y / val; r.z = r.z / val;
//	return r;
//}
//
//void MakeGL_Model(void)
//{
//	int i;
//	glShadeModel(GL_SMOOTH);
//	if (glIsList(1)) glDeleteLists(1, 1);
//	glNewList(1, GL_COMPILE);
//	glPushMatrix();
//	glRotatef(angle, 0.0f, 1.0f, 0.0f);
//	glScalef(scalefactor, scalefactor, scalefactor);
//	glColor3f(1, 0, 0);
//	for (i = 0; i < fnum; i++) {
//		Point norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]], mpoint[mface[i].ip[0]]);
//		glBegin(GL_TRIANGLES);
//		glNormal3f(norm.x, norm.y, norm.z);
//		glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
//		glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
//		glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
//		glEnd();
//	}
//	glPopMatrix();
//	glEndList();
//}
//
//void display(void)
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	MakeGL_Model();
//	if (status == WIRE)
//		DrawWire();
//	else
//		DrawShade();
//}
//
//void keyboard(unsigned char key, int x, int y)
//{
//	printf("key = %c\n", key);
//	switch (key) {
//	case 'w':
//		status = WIRE; glutPostRedisplay(); break;
//	case 's':
//		status = SHADE; glutPostRedisplay(); break;
//
//	case '1':
//		fname = "c:\\data\\CrystalFace.dat";
//		ReadModel(); glutPostRedisplay(); break;
//	//case '2':
//	//	fname = "c:\\data\\sphere.dat";
//	//	ReadModel(); glutPostRedisplay(); break;
//	//case '3':
//	//	fname = "c:\\data\\ellipsoid.dat";
//	//	ReadModel(); glutPostRedisplay(); break;
//	//case '4':
//	//	fname = "c:\\data\\torus.dat";
//	//	ReadModel(); glutPostRedisplay(); break;
//	//case '5':
//	//	fname = "c:\\data\\bspline.dat";
//	//	ReadModel(); glutPostRedisplay(); break;
//	}
//}
//
//void mouse(int button, int state, int x, int y)
//{
//	if (button == GLUT_LEFT_BUTTON) {
//		if (state == GLUT_DOWN) {
//			moving = 1;
//			mousebegin = x;
//		}
//		else if (state == GLUT_UP) {
//			moving = 0;
//		}
//	}
//	else if (button == GLUT_RIGHT_BUTTON) {
//		if (state == GLUT_DOWN) {
//			scaling = 1;
//			mousebegin = x;
//		}
//		else if (state == GLUT_UP) {
//			scaling = 0;
//		}
//	}
//}
//
//void motion (int x, int y)
//{
//	if (moving) {
//		angle += (x - mousebegin);
//		mousebegin = x;
//		glutPostRedisplay();
//	}
//	else if (scaling) {
//		scalefactor = scalefactor*(1.0 - mousebegin - x) * 0.0001f;
//		glutPostRedisplay();
//	}
//}
//
//
//void InitLight() {
//	GLfloat mat_diffuse[] = { 0.5f, 0.4f, 0.3f, 1.0f };
//	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	GLfloat mat_ambient[] = { 0.5f, 0.4f, 0.3f, 1.0f };
//	GLfloat mat_shininess[] = { 15.0f };	
//	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//	GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
//	GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
//	GLfloat light_position[] = { -3.0f, 6.0f, 3.0f, 0.0f };
//	glShadeModel(GL_SMOOTH);
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_LIGHTING);
//	glEnable(GL_LIGHT0);
//	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
//	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
//}
//
//
//
//int main(int argc, char** argv)
//{
//	glutInit(&argc, argv);
//	glutInitWindowSize(500, 500);
//	glutInitWindowPosition(100, 100);
//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//	glutCreateWindow("20213960 SOOKIM Simple Modeling Practice 03");
//	glutDisplayFunc(display);
//	glutKeyboardFunc(keyboard);
//	glutMouseFunc(mouse);
//	glutMotionFunc(motion);
//	ReadModel();
//	GLSetupRC();
//	InitLight();
//	glutMainLoop();
//	return 0;
//}