#include <Windows.h>
#include <gl/glui.h>
#include <GL/glut.h>
#include <gl/GLU.h>
#include <stdlib.h>
#include<math.h>
#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<debugapi.h>
#include<SOIL.h>


#define checkImageWidth 128
#define checkImageHeight 128
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

static GLuint texName;

void checkertexturepreparation(void)
{
	int i, j, c;

	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
}

#pragma comment(lib, "glui32.lib")

using namespace std;

struct coordinate {
	float vertex1, vertex2, vertex3;
	coordinate(float v1, float v2, float v3) {
		this->vertex1 = v1;
		this->vertex2 = v2;
		this->vertex3 = v3;
	}

	float paramv1() {
		return vertex1;
	}
	float paramv2() {
		return vertex2;
	}
	float paramv3() {
		return vertex3;
	}
};

struct triangularface {
	int facevertex1, facevertex2, facevertex3;
	triangularface(int f1, int f2, int f3) {
		this->facevertex1 = f1;
		this->facevertex2 = f2;
		this->facevertex3 = f3;
	}

	int paramx() {
		return facevertex1;
	}
	int paramy() {
		return facevertex2;
	}
	int paramz() {

		return facevertex3;
	}

};

std::vector<coordinate*> myvertices;
std::vector<triangularface*> myfaces;
std::vector<std::string*> coord;

int color[3] = { 0,0,0 };
float object_position[] = { 0.0,0.0,0.0 };
float Translation_speed = 0.05;
float object_rotation[16] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
float object_rotation_y[4] = { 0,1,0,0 };


GLfloat spinx = 0.0;
GLfloat spiny = 0.0;
GLfloat spinz = 0.0;


float Camerax = 0.0;
float Cameray = -5.0;
float Cameraz = -15.0;

float camera_position[3] = { Camerax,Cameray,Cameraz };

//float camera_rotation[16] = { Camerax,0,0,0,0,Cameray,0,0,0,0,Cameraz,0,0,0,0,1 };

float orientationx = 0.0;
float orientationy = 1.0;
float orientationz = 0.0;

float zoomx = 1.0;
float zoomy = 1.0;
float zoomz = 1.0;

float lightsourcex = 0.0;
float lightsourcey = -20.0;
float lightsourcez = -15.0;

float materialspecularR = 1.0;
float materialspecularG = 1.0;
float materialspecularB = 0.0;
float materialspecularA = 1.0;
float material_properties[4] = { materialspecularR,materialspecularG,materialspecularB,materialspecularA };


float ambientR = 0.4;
float ambientG = 0.4;
float ambientB = 0.4;
float ambientA = 1.0;

// id for the glut window 
int window_id;
int wireframe = 0;
int colornumber = 0;
int rotationpatternnumber = 0;
int displaypatterntype = 0;
int lightswitchindicator = 0;
int shadingmodelindicator = 0;
int texturepatterntype = 0;


//near and far clipping planes 

float nearclippingplane = 0.5;
float farclippingplane = -20;

//filedofview 

float fieldofview = 60.0;
float verticalfieldofview;

//lighting parameters
GLfloat mat_specular[] = { materialspecularR,materialspecularG,materialspecularB,materialspecularA };
GLfloat mat_shininess[] = { 50.0 };
//GLfloat  light_position[] = { 0.0,0.0,1.0,0.0 };
GLfloat  light_position[] = { lightsourcex,lightsourcey,lightsourcez,1.0 };
GLfloat white_light[] = { 1.0,1.0,1.0,1.0 };
GLfloat lmodel_ambient[] = { ambientR,ambientG,ambientB,ambientA};
GLfloat model_twoside[] = { 0.0 };


//impart spin to the object through spin variables 

void spindisplay(void) {

	switch (rotationpatternnumber)
	{
	case 1 :
		spinx = spinx + 0.5;
		if (spinx > 360.0) {
			spinx = spinx - 360;
			
		}
	
		break;
	case 2:
		spiny = spiny + 0.5;
		if (spiny > 360.0) {
			spiny = spiny - 360;
			
		}
		
		break;

	case 3:
		spinz = spinz + 0.5;

		if (spinz > 360.0) {
			spinz = spinz - 360;
		}
	
		break;

	}

	glutPostRedisplay();
	
}


//get the data from obj files into vectors 

void rotationpatternfineranndsetter(int rotationpatternnumber) {

	if (rotationpatternnumber != 0)

		glutIdleFunc(spindisplay);
}

void getdatafrom_objfileintovectors() {


	std::ifstream myfile("cow.obj");

	if (myfile.is_open())
	{
		std::string line;
		std::int32_t count = 0;
		//while (!myfile.eof) {
		//getline(myfile, line)

		while (!myfile.eof()) {
			getline(myfile, line);
			if (!line.empty()) {
				coord.push_back(new std::string(line));
				count++;
			}
		}
		printf("no of lines=%d \n ", count);

		for (unsigned int i = 0; i < coord.size(); i++) {
			if ((*coord[i])[0] == '	') { continue; }
			else if ((*coord[i])[0] == 'v' && (*coord[i])[1] == ' ') {
				float tmpx, tmpy, tmpz;
				sscanf_s((*coord[i]).c_str(), "v %f %f %f", &tmpx, &tmpy, &tmpz);
				myvertices.push_back(new coordinate(tmpx, tmpy, tmpz));

			/*	std::cout << *coord[i];
				std::cout << "\n";
*/
			}
			else if ((*coord[i])[0] == 'f' && (*coord[i])[1] == ' ') {
				int tmpv1, tmpv2, tmpv3;
				sscanf_s((*coord[i]).c_str(), "f %d %d %d", &tmpv1, &tmpv2, &tmpv3);
				myfaces.push_back(new triangularface(tmpv1, tmpv2, tmpv3));
/*
				std::cout << *coord[i];
				std::cout << "\n";
				*/
			}
			else if ((*coord[i])[0] == ' ') { continue; }

			//	std::cout << *coord[i];
		}

	}  //end of  file open  :: end of if(file open

	myfile.close();

} //end of function get getdatafrom_objfileintovectors

void determinecolorfromthecolornumber(int colorno) {

	switch (colorno)
	{
	case 0:  //red color
		color[0] = 255.0 / 255.0;
		color[1] = 0 / 255.0;
		color[2] = 0 / 255.0;
		
		break;
	case 1:  //white color
		color[0] = 255.0 / 255.0;
		color[1] = 255.0 / 255.0;
		color[2] = 255.0 / 255.0;
		break;
	case 2:  // yellow color
		color[0] = 255.0 / 255.0;
		color[1] = 255.0 / 255.0;
		color[2] = 0 / 255.0;
		break;
	case 3:  // blue color
		color[0] = 0 / 255.0;
		color[1] = 0 / 255.0;
		color[2] = 255.0 / 255.0;
		break;
	case 4:  //green color
		color[0] = 0 / 255.0;
		color[1] = 255.0 / 255.0;
		color[2] = 0 / 255.0;
		break;
	case 5:  //cyan color
		color[0] = 0 / 255.0;
		color[1] = 255.0 / 255.0;
		color[2] = 255.0 / 255.0;
		break;
	case 6:  //pink color
		color[0] = 255.0 / 255.0;
		color[1] = 0 / 255.0;
		color[2] = 255.0 / 255.0;
		break;
	default:  //red color 
		color[0] = 255.0 / 255.0;
		color[1] = 0 / 255.0;
		color[2] = 0 / 255.0;

		break;
	}

}

void init(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_FLAT);


	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_NORMALIZE);


}

void createmodelfrommeshpoints(int model_type) {

	switch (model_type) {
	case 0: glBegin(GL_TRIANGLES); break;
	case 1: glBegin(GL_LINES); break;
	case 2: glBegin(GL_POINTS); break;
	}

	//glBegin(GL_TRIANGLES); //draw a cube with 12 triangles 
	//glBegin(GL_LINE_LOOP);

	std::vector<triangularface*>::iterator iter;

	//get the correct color to show 

	determinecolorfromthecolornumber(colornumber);

	int i = 0;
	for (iter = myfaces.begin(); iter != myfaces.end(); iter++)
	{
		i = i + 1;
		//glColor3f(1.0, 0.0, 1.0);
		glColor3f(color[0], color[1], color[2]);

		int vertex1number = (*iter)->paramx();
		int vertex2number = (*iter)->paramy();
		int vertex3number = (*iter)->paramz();

		float x1 = myvertices[vertex1number - 1]->paramv1();
		float y1 = myvertices[vertex1number - 1]->paramv2();
		float z1 = myvertices[vertex1number - 1]->paramv3();

		float x2 = myvertices[vertex2number - 1]->paramv1();
		float y2 = myvertices[vertex2number - 1]->paramv2();
		float z2 = myvertices[vertex2number - 1]->paramv3();

		float x3 = myvertices[vertex3number - 1]->paramv1();
		float y3 = myvertices[vertex3number - 1]->paramv2();
		float z3 = myvertices[vertex3number - 1]->paramv3();

		if (displaypatterntype == 1 && texturepatterntype == 0) {

			if (i % 7 == 0) {
				glTexCoord2f(0.0, 0.0);
			}

		}
		else if (displaypatterntype == 1 && texturepatterntype == 1) {
			glTexCoord2f(0.0, 0.0);
		}
		
		
		glVertex3f(x1, y1, z1);
		if (displaypatterntype == 1 ) {
			glTexCoord2f(0.0, 1.0);
		}
		
		glVertex3f(x2, y2, z2);

		if (displaypatterntype == 1 && texturepatterntype == 0) {

			if (i*i*i % 13 == 0) {
				glTexCoord2f(1.0, 1.0);
			}

		}
		else if (displaypatterntype == 1 && texturepatterntype == 1) {
			glTexCoord2f(1.0, 1.0);
		}

		glVertex3f(x3, y3, z3);
		if (displaypatterntype == 1  ) {
			glTexCoord2f(1.0, 0.0);
		}
		//	glVertex3f(myvertices[((*iter)->paramx()) - 1]->paramv1(), myvertices[((*iter)->paramx())-1]->paramv2(), myvertices[((*iter)->paramx()) - 1]->paramv3());
		//	glVertex3f(myvertices[((*iter)->paramy()) - 1]->paramv1(), myvertices[((*iter)->paramy()) - 1]->paramv2(), myvertices[((*iter)->paramy()) - 1]->paramv3());
		//	glVertex3f(myvertices[((*iter)->paramz()) - 1]->paramv1(), myvertices[((*iter)->paramz()) - 1]->paramv2(), myvertices[((*iter)->paramz()) - 1]->paramv3());

	}

	glEnd();


	//include part of texture for initializing the texture

	checkertexturepreparation();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
		checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);

}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (displaypatterntype == 1) {
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glBindTexture(GL_TEXTURE_2D, texName);
	}
	else if (displaypatterntype == 0) {
		glDisable(GL_TEXTURE_2D);
	}
	glMatrixMode(GL_MODELVIEW);

	//add the lightning components here
	if (shadingmodelindicator == 0) {
		glShadeModel(GL_FLAT);
	}
	else if (shadingmodelindicator == 1) {
		glShadeModel(GL_SMOOTH);
	}

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lmodel_ambient);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	//glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, model_twoside);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_DEPTH_TEST);



	glLoadIdentity();


	glPushMatrix();
	glRotatef(spinx, spiny, spinz, 0);

	//gluLookAt(Camerax, Cameray, Cameraz, 0, 0, 0, orientationx, orientationy, orientationz);

	gluLookAt(camera_position[0], camera_position[1], camera_position[2], 0, 0, 0, orientationx, orientationy, orientationz);


	glScalef(zoomx, zoomy, zoomz);
	//gluLookAt(5, -5, -13, 0, 0, 0, 0, 1, 0);

	//glTranslatef(2.0f, 2.0f, -17.0f);

	glTranslatef(object_position[0], object_position[1], -object_position[2]);
	
	//glRotatef(-160, 0, 1, 0);
	glMultMatrixf(object_rotation);
	//glMultMatrixf(object_rotation_y);

	
	if (displaypatterntype == 0) {
		if (lightswitchindicator == 1) {
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
		}
		else if (lightswitchindicator == 0) {
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
		}

	}


	createmodelfrommeshpoints(wireframe);
	

	//glEnable(GL_DEPTH_TEST);


	//	glLoadIdentity();
	//glFlush();
	glPopMatrix();
	glutSwapBuffers();

}


void reshape(int w, int h) {

	if (w == 0 || h == 0) return; 

	//calculate verticalfieldofview 

	   verticalfieldofview=2 * (atan(tan(fieldofview / 2)*((GLdouble)w / (GLdouble)h)));
	
	//start with projection 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fieldofview, (GLdouble)w / (GLdouble)h, nearclippingplane, farclippingplane);
	glMatrixMode(GL_MODELVIEW);


//code to give space to subwindow in the main glui window 

	int tx, ty;
	GLUI_Master.get_viewport_area(&tx, &ty, &w, &h);

	glViewport(tx, ty, w, h);  //Use the whole window for rendering


}

void callback_func(int control) {
	if (control !=0)
	{
		glutIdleFunc(spindisplay);
	}
}

void myglutIdle(void) {

		if (glutGetWindow() != window_id)  glutSetWindow(window_id);

		//rotationpatternfineranndsetter(rotationpatternnumber);
		
	glutPostRedisplay();

}

void addgluicomponents(void) {

	//code to add glui window 

	//GLUI_Master.set_glutIdleFunc(NULL);

	//GLUI *glui = GLUI_Master.create_glui("cowgui");

	GLUI *glui_subwin = GLUI_Master.create_glui_subwindow(window_id, GLUI_SUBWINDOW_BOTTOM);
	GLUI *gluI_subwin_right = GLUI_Master.create_glui_subwindow(window_id, GLUI_SUBWINDOW_RIGHT);

	glui_subwin->set_main_gfx_window(window_id);
	gluI_subwin_right->set_main_gfx_window(window_id);

	//add the components of the subwindow 


	GLUI_Panel *obj_panel = glui_subwin->add_panel("Object Type");

	//add pattern change radiobuttons
	GLUI_RadioGroup *patternchanegroup = glui_subwin->add_radiogroup_to_panel(obj_panel, &wireframe, 3, NULL);
	glui_subwin->add_radiobutton_to_group(patternchanegroup, "Solid");
	glui_subwin->add_radiobutton_to_group(patternchanegroup, "Wireframe");
	glui_subwin->add_radiobutton_to_group(patternchanegroup, "Points");
	glui_subwin->add_column(true);


	//add color change radio buttons 

	GLUI_Panel *color_panel = glui_subwin->add_panel("Color");

	GLUI_RadioGroup *colorchangegroup = glui_subwin->add_radiogroup_to_panel(color_panel, &colornumber, 7, NULL);

	glui_subwin->add_radiobutton_to_group(colorchangegroup, "RED");
	glui_subwin->add_radiobutton_to_group(colorchangegroup, "WHITE");
	glui_subwin->add_radiobutton_to_group(colorchangegroup, "YELLOW");
	glui_subwin->add_column_to_panel(color_panel);
	glui_subwin->add_radiobutton_to_group(colorchangegroup, "BLUE");
	glui_subwin->add_radiobutton_to_group(colorchangegroup, "GREEN");
	glui_subwin->add_radiobutton_to_group(colorchangegroup, "CYAN");
	//	glui_subwin->add_radiobutton_to_group(colorchangegroup, "PINK");

	//rotating panel to subwindow bottom 
	//glui_subwin->add_column(true);

//	GLUI_Panel *rotate_panel = glui_subwin->add_panel("Rotations");

	/*
	GLUI_RadioGroup *rotationchangegroup = glui_subwin->add_radiogroup_to_panel(rotate_panel, &rotationpatternnumber, 8, callback_func);   //rotationpatternfineranndsetter);
	glui_subwin->add_radiobutton_to_group(rotationchangegroup, "No Rotation");
	glui_subwin->add_radiobutton_to_group(rotationchangegroup, "Aroung X axis");
	glui_subwin->add_radiobutton_to_group(rotationchangegroup, "Aroung Y axis");
	glui_subwin->add_radiobutton_to_group(rotationchangegroup, "Aroung Z axis");

	*/

	//place camera in the desired posiiton 
	glui_subwin->add_column(true);

	GLUI_Panel *camera_panel = glui_subwin->add_panel("Camera Position");

	glui_subwin->add_edittext_to_panel(camera_panel, "X Position", GLUI_EDITTEXT_FLOAT, &camera_position[0]);
	glui_subwin->add_edittext_to_panel(camera_panel, "Y Position", GLUI_EDITTEXT_FLOAT, &camera_position[1]);
	glui_subwin->add_edittext_to_panel(camera_panel, "Z Position", GLUI_EDITTEXT_FLOAT, &camera_position[2]);


//commenting the camera rotation as it is not required now 
/*
	//place the orientation of the camera in the desired position
	glui_subwin->add_column(true);

	GLUI_Panel *orientation_panel = glui_subwin->add_panel("Camera Orientation");

	glui_subwin->add_edittext_to_panel(orientation_panel, "X axis", GLUI_EDITTEXT_FLOAT, &orientationx);
	glui_subwin->add_edittext_to_panel(orientation_panel, "Y axis", GLUI_EDITTEXT_FLOAT, &orientationy);
	glui_subwin->add_edittext_to_panel(orientation_panel, "Z axis", GLUI_EDITTEXT_FLOAT, &orientationz);

	*/
	//place light in the desired position 

	//place camera in the desired posiiton 
	glui_subwin->add_column(true);

	GLUI_Panel *lightposition_panel = glui_subwin->add_panel("Light Position");

	glui_subwin->add_edittext_to_panel(lightposition_panel, "X ", GLUI_EDITTEXT_FLOAT, &light_position[0]);
	glui_subwin->add_edittext_to_panel(lightposition_panel, "Y ", GLUI_EDITTEXT_FLOAT, &light_position[1]);
	glui_subwin->add_edittext_to_panel(lightposition_panel, "Z ", GLUI_EDITTEXT_FLOAT, &light_position[2]);

   //CODE TO SAY IF THe LIGHT IS AT INFINITY OR A FIXED POSITION 


	//add components for choosing lighting and texture
	glui_subwin->add_column();

	GLUI_Panel *assignment_panel = glui_subwin->add_panel("The assignment");

	//add pattern change radiobuttons
	GLUI_RadioGroup *assignmentchangegroup = glui_subwin->add_radiogroup_to_panel(assignment_panel, &displaypatterntype, 2, NULL);
	glui_subwin->add_radiobutton_to_group(assignmentchangegroup, "NoTexture");
	glui_subwin->add_radiobutton_to_group(assignmentchangegroup, "Texture");

	glui_subwin->add_separator();

	GLUI_Panel *texturetype_panel = glui_subwin->add_panel("Texture Type");

	//add pattern change radiobuttons
	GLUI_RadioGroup *texturetypegroup = glui_subwin->add_radiogroup_to_panel(texturetype_panel, &texturepatterntype, 2, NULL);
	glui_subwin->add_radiobutton_to_group(texturetypegroup, "Cow");
	glui_subwin->add_radiobutton_to_group(texturetypegroup, "Checker");


	glui_subwin->add_column();

//panel to turn lights on or off
	GLUI_Panel *lightsswitchpanel = glui_subwin->add_panel("LightSwitch");

	//add pattern change radiobuttons
	GLUI_RadioGroup *lightswitchgroup = glui_subwin->add_radiogroup_to_panel(lightsswitchpanel, &lightswitchindicator, 2, NULL);
	glui_subwin->add_radiobutton_to_group(lightswitchgroup, "OFF");
	glui_subwin->add_radiobutton_to_group(lightswitchgroup, "ON");

	glui_subwin->add_separator();
    //choose the shading model whether goround or  flat
	GLUI_Panel *shadingmodelpanel = glui_subwin->add_panel("ShadingModel");

	//add pattern change radiobuttons
	GLUI_RadioGroup *shadingmodelgroup = glui_subwin->add_radiogroup_to_panel(shadingmodelpanel, &shadingmodelindicator, 2, NULL);
	glui_subwin->add_radiobutton_to_group(shadingmodelgroup, "FLAT");
	glui_subwin->add_radiobutton_to_group(shadingmodelgroup, "GOURAND");


	//choose the specular material properties for the objects 
	glui_subwin->add_column(true);

	GLUI_Panel *material_panel = glui_subwin->add_panel("Material Properties");

	glui_subwin->add_edittext_to_panel(material_panel, "R ", GLUI_EDITTEXT_FLOAT, &materialspecularR);
	glui_subwin->add_edittext_to_panel(material_panel, "G ", GLUI_EDITTEXT_FLOAT, &materialspecularG);
	glui_subwin->add_edittext_to_panel(material_panel, "B ", GLUI_EDITTEXT_FLOAT, &materialspecularB);
	glui_subwin->add_edittext_to_panel(material_panel, "A ", GLUI_EDITTEXT_FLOAT, &materialspecularA);

	//choose the ambient RGBA for the global ambient light
	glui_subwin->add_column(true);

	GLUI_Panel *ambient_panel = glui_subwin->add_panel("ambient specularlight");

	glui_subwin->add_edittext_to_panel(ambient_panel, "R ", GLUI_EDITTEXT_FLOAT, &ambientR);
	glui_subwin->add_edittext_to_panel(ambient_panel, "G ", GLUI_EDITTEXT_FLOAT, &ambientG);
	glui_subwin->add_edittext_to_panel(ambient_panel, "B ", GLUI_EDITTEXT_FLOAT, &ambientB);
	glui_subwin->add_edittext_to_panel(ambient_panel, "A ", GLUI_EDITTEXT_FLOAT, &ambientA);

	/*   commmenting the    near and far cliiping plane alterations as not required in this assignment*/
/*
	//near and far clipping planes
	glui_subwin->add_column(true);

	GLUI_Panel *clipping_panel = glui_subwin->add_panel("Clipping Plane");

	glui_subwin->add_edittext_to_panel(clipping_panel, "Near", GLUI_EDITTEXT_FLOAT, &nearclippingplane);
	glui_subwin->add_edittext_to_panel(clipping_panel, "Far", GLUI_EDITTEXT_FLOAT, &farclippingplane);

	//change the field of view

	glui_subwin->add_column(true);

	GLUI_Panel *fieldofview_panel = glui_subwin->add_panel("Field Of View");

	glui_subwin->add_edittext_to_panel(fieldofview_panel, "Horizontal", GLUI_EDITTEXT_FLOAT, &fieldofview);
	glui_subwin->add_edittext_to_panel(fieldofview_panel, "Vertical", GLUI_EDITTEXT_FLOAT, &verticalfieldofview);

*/
	//moving panel in the subwindow right 


	GLUI_Panel *movingpanel = gluI_subwin_right->add_panel("Moving Objects");

	//translating along the x direction 
	GLUI_Translation *translate_x;
	translate_x = gluI_subwin_right->add_translation_to_panel(movingpanel, "TRANSLATE X", GLUI_TRANSLATION_X, &object_position[0], 1);
	translate_x->set_speed(Translation_speed);
	//translating along the y direction 
	GLUI_Translation *translate_y;
	translate_y = gluI_subwin_right->add_translation_to_panel(movingpanel,
		"TRANSLATE Y", GLUI_TRANSLATION_Y, &object_position[1], 2);
	translate_y->set_speed(Translation_speed);
	//translating along the z direction 
	GLUI_Translation *translate_z;
	translate_z = gluI_subwin_right->add_translation_to_panel(movingpanel,
		"TRANSLATE Z", GLUI_TRANSLATION_Z, &object_position[2], 3);
	translate_z->set_speed(Translation_speed);


	//rotating the object  across x direction 

	GLUI_Rotation *rotate_x;
	rotate_x = gluI_subwin_right->add_rotation_to_panel(movingpanel, "ROTATE X", object_rotation);
	rotate_x->set_spin(0.5);

	
	
	/* commenting the translation and zooming of the cameras
	
	//add the zoom in and zoom out view translation 
	//zoom in x direction 
	GLUI_Translation *zoom_x;
	zoom_x = gluI_subwin_right->add_translation_to_panel(movingpanel,
		"ZOOM X", GLUI_TRANSLATION_Z, &zoomx, 2);
	zoom_x->set_speed(Translation_speed);

	glui_subwin->add_column_to_panel(movingpanel);
	//translating the camera along all the directions

	//translating the camrea along x direction

	GLUI_Translation *translate__camera_x;
	translate__camera_x = gluI_subwin_right->add_translation_to_panel(movingpanel, "TRANSLATE CAMERA X", GLUI_TRANSLATION_X, &camera_position[0], 1);
	translate__camera_x->set_speed(Translation_speed);
	//translating the camera along the y direction 
	GLUI_Translation *translate_camera_y;
	translate_camera_y = gluI_subwin_right->add_translation_to_panel(movingpanel,
		"TRANSLATE CAMERA Y", GLUI_TRANSLATION_Y, &camera_position[1], 2);
	translate_camera_y->set_speed(Translation_speed);
	//translating the camera along the z direction 
	GLUI_Translation *translate_camera_z;
	translate_camera_z = gluI_subwin_right->add_translation_to_panel(movingpanel,
		"TRANSLATE CAMERA Z", GLUI_TRANSLATION_Z, &camera_position[2], 3);
	translate_camera_z->set_speed(Translation_speed);


	//rotating the camera across x direction 

	/*	GLUI_Rotation *rotate_camera_x;
	rotate_camera_x = gluI_subwin_right->add_rotation_to_panel(movingpanel, "ROTATE CAMERA ", camera_position);
	*/


	//zoom in z direction 

	/*
	GLUI_Translation *zoom_z;
	zoom_z = gluI_subwin_right->add_translation_to_panel(movingpanel,
		"ZOOM Z", GLUI_TRANSLATION_Z, &zoomz, 2);
	zoom_z->set_speed(Translation_speed);



	//zoom in y direction 


	GLUI_Translation *zoom_y;
	zoom_y = gluI_subwin_right->add_translation_to_panel(movingpanel,
		"ZOOM Y", GLUI_TRANSLATION_Y, &zoomy, 2);
	zoom_y->set_speed(Translation_speed);

	
	*/
	glui_subwin->add_button("QUIT", 0, (GLUI_Update_CB)exit);

	//end of gui code 


}

void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			glutIdleFunc(spindisplay);
		break;
	case GLUT_MIDDLE_BUTTON:
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			glutIdleFunc(NULL);
		break;
	default:
		break;
	}
}


void loadjpgtextureimagesfromoutside() {

	

}


int main(int argc, char** argv)
{

	loadjpgtextureimagesfromoutside();
	getdatafrom_objfileintovectors();

	//create the window with its parameters 

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1300, 650);
	glutInitWindowPosition(20, 20);
	window_id = glutCreateWindow("DRAWCube");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);


//	glutMouseFunc(mouse);
	//glutIdleFunc(spindisplay);

	addgluicomponents();
	
	glutMainLoop();

	//

	return 0;
}   //end of main 
