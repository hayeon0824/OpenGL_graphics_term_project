// graphics_swim_or_die.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <glut.h>
#include <stdio.h>
#include <math.h>

//Type definition
#define TYPE_BODY 0
#define TYPE_HEAD 1
#define TYPE_TOP_FIN 2
#define TYPE_LEFT_FIN 3
#define TYPE_RIGHT_FIN 4
#define TYPE_TAIL 5
#define TYPE_JOINT 6

void timer(int t);
static int delay = 10;

typedef struct _texture {
	GLubyte* rgb;
	int width;
	int height;
}Texture;

typedef struct _action {
	char type; //'R' or 'T' or 'N' or 'F'
	int duration; //how long 
	GLfloat param[3];
}Action;

typedef struct _object {
	GLfloat* vertices;
	GLfloat* colors;
	GLubyte* indices;
	int nums; //number of vertices

	Texture* tex;
	GLubyte* texflag;

	GLfloat theta[3]; //rotation parameter
	GLfloat trans[3]; //translate parameter

	Action* action;
	int action_idx;
	int action_counter;


	int type;
	struct _object* next;
	struct _object* child;

}Object;

typedef struct _viewer {
	GLfloat eye[3];
	GLfloat at[3];
	GLfloat up[3];
}Viewer;

//data
GLfloat mat_specular[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat mat_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat mat_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat mat_shininess = { 100.0 };

//action 처리해주기
Action body_action[] = {
	{ 'T', 30, -0.5,   0,   0 },
{ 'T', 30, +0.5,   0,   0 },
{ 'R', 60,    0, 720,   0 },
{ 'N',180,    0,   0,   0 },
{ 'R', 30,    0,   0,  30 },
{ 'R', 60,    0, 720,   0 },
{ 'F',  0,    0,   0,   0 }
};

Action tail_action[] = {
	{ 'T', 30, -0.5,   0,   0 },
{ 'T', 30, +0.5,   0,   0 },
{ 'R', 60,    0, 720,   0 },
{ 'N',180,    0,   0,   0 },
{ 'R', 30,    0,   0,  30 },
{ 'R', 60,    0, 720,   0 },
{ 'F',  0,    0,   0,   0 }
};

//objects
Object fish_body, fish_head, fish_top_fin, fish_left_fin, fish_right_fin, fish_tail;
Object fish_joint;

Viewer v;
GLUquadricObj *t;
int play = 0;

//functions
void init() {

	//light
	GLfloat light_ambient1[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular1[] = { 1.0, 1.0, 1.0 ,1.0 };
	GLfloat light_position1[] = { 10.0, 10.0, 10.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular1);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glOrtho(-3.0, 3.0, -3.0, 3.0, -10.0, 10.0);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT2);
	glEnable(GL_COLOR_MATERIAL);
}


//
void init_left_fin() {
	//계속해서 움직여주도록 움직이는 느낌나게 
}

void init_right_fin() {

}

void init_tail() {
	fish_tail.theta[0] = fish_tail.theta[1] = fish_tail.theta[2] = 0;
	fish_tail.trans[0] = -0.32;
	fish_tail.trans[1] = 0.1;
	fish_tail.trans[2] = 0;
	fish_tail.type = TYPE_TAIL;

	fish_tail.action = &tail_action[0];
	fish_tail.action_idx = 0;
	fish_tail.action_counter = 0;

	//fish_tail.next = &right_leg_joints;
	//fish_tail.child = &left_upper_leg;
}

void init_object() {
	//object initialization
	fish_body.nums = 24;
	fish_body.theta[0] = fish_body.theta[1] = fish_body.theta[2] = 0;
	fish_body.trans[0] = fish_body.trans[1] = fish_body.trans[2] = 0;

	fish_body.action = &body_action[0];
	fish_body.action_idx = 0;
	fish_body.action_counter = 0;
	fish_body.type = TYPE_BODY;

	fish_body.next = 0;
	fish_body.child = &fish_head;

	fish_head.nums = 24;
	fish_head.theta[0] = fish_head.theta[1] = fish_head.theta[2] = 0;
	fish_head.trans[0] = 0;
	fish_head.trans[1] = 1.55;
	fish_head.trans[2] = 0;
	fish_head.type = TYPE_HEAD;

	fish_head.action_idx = 0;
	fish_head.action_counter = 0;

	init_tail();
	init_left_fin();
	init_right_fin();

	//fish_head.next = &조인트?


	t = gluNewQuadric();
	gluQuadricDrawStyle(t, GLU_FILL);
	gluQuadricNormals(t, GLU_SMOOTH);

}

void trans() {

}

void spin() {
	glutPostRedisplay();
}

//i/o
void keyboard_handler(unsigned char key, int x, int y){
	if (key == 'x') v.eye[0] -= 0.1; if (key == 'X') v.eye[0] += 0.1;
	if (key == 'y') v.eye[1] -= 0.1; if (key == 'Y') v.eye[1] += 0.1;
	if (key == 'z') v.eye[2] -= 0.1; if (key == 'Z') v.eye[2] += 0.1;
	if (key == 'p') play = 1 - play;
	if (key == 'r'){
		play = 0;
		init_object();
	}
}

GLfloat clip(GLfloat x){
	if (x < 0) return x + 360.0;
	if (x > 360.0) return x - 360.0;
	return x;
}

void action(Object* p)
{
	if (p->action == 0) return;
	if (p->action[p->action_idx].type == 'F')
	{
		p->action_idx = 0;
		p->action_counter = 0;
		return;
	}
	if (p->action[p->action_idx].type == 'R')
	{
		for (int i = 0; i<3; i++)
			p->theta[i] = clip(p->theta[i] + p->action[p->action_idx].param[i] / p->action[p->action_idx].duration);
		p->action_counter++;
	}
	if (p->action[p->action_idx].type == 'T')
	{
		for (int i = 0; i<3; i++)
			p->trans[i] += p->action[p->action_idx].param[i] / p->action[p->action_idx].duration;
		p->action_counter++;
	}
	if (p->action[p->action_idx].type == 'N')
	{
		p->action_counter++;
	}
	if (p->action_counter >= p->action[p->action_idx].duration)
	{
		p->action_idx++;
		p->action_counter = 0;
	}
}

void draw(Object* p)
{
	// store matrix
	glPushMatrix();

	// transformation
	glTranslatef(p->trans[0], p->trans[1], p->trans[2]);
	glRotatef(p->theta[0], 1.0, 0.0, 0.0);
	glRotatef(p->theta[1], 0.0, 1.0, 0.0);
	glRotatef(p->theta[2], 0.0, 0.0, 1.0);

	// draw
	if (p->type == TYPE_BODY) {
		glPushMatrix();
		glRotatef(-90.0, 1, 0, 0);
		glColor3f(1, 0, 0);
		gluCylinder(t, 0.3, 0.4, 1.3, 12, 12);
		glPopMatrix();
	}
	else if (p->type == TYPE_HEAD) {
		glPushMatrix();
		glColor3f(1, 0, 0);
		glutSolidSphere(0.5, 10, 10);
		glColor3f(1, 1, 0);
		glRotatef(-90.0, 0, 0, 1);
		glutSolidCone(0.27, 0.8, 10, 10);
		glPopMatrix();
	}
	else if (p->type == TYPE_TAIL) {
		glPushMatrix();
		glColor3f(1, 0, 0);
		glRotatef(-90.0, 1, 0, 0);
		glutSolidCone(0.3, 0.8, 10, 10);
		glPopMatrix();
	}
	//그리고 더더더더 
	else {
		glVertexPointer(3, GL_FLOAT, 0, p->vertices);
		glColorPointer(3, GL_FLOAT, 0, p->colors);
		glDrawElements(GL_QUADS, p->nums, GL_UNSIGNED_BYTE, p->indices);
	}
	// apply action
	if (play) action(p);

	// draw children
	if (p->child) draw(p->child);

	// restore matrix
	glPopMatrix();

	// draw siblings
	if (p->next) draw(p->next);
}
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	glEnable(GL_LIGHT0);

	//viewer
	glLoadIdentity();
	gluLookAt(v.eye[0], v.eye[1], v.eye[2], v.at[0], v.at[1], v.at[2], v.up[0], v.up[1], v.up[2]);

	// draw
	draw(&fish_body);

	// flush & swap buffers
	glFlush();
	glutSwapBuffers();
}

int main(int argc, char* argv[]){
	// GLUT initialization
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Prog35: scripts");

	// call-back functions
	glutIdleFunc(spin);
	glutKeyboardFunc(keyboard_handler);
	glutDisplayFunc(display);

	// enable color/vertex array
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	// initialize object
	init_object();

	// initialize viewer
	v.eye[0] = 0.0; v.eye[1] = 0.0; v.eye[2] = 1.0;
	v.at[0] = 0.0; v.at[1] = 0.0; v.at[2] = 0.0;
	v.up[0] = 0.0; v.up[1] = 1.0; v.up[2] = 0.0;

	init();
	glutMainLoop();

	return 0;
}

