#include <windows.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include "glaux.h"

#include "GL_movement.h"   

#include <locale.h>  
#include <stdio.h> 

#include "tiny_obj_loader.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>

#define PI 3.14159265

#pragma warning(disable : 4244)  
#pragma warning(disable : 4305)  

// �������� ����� extern ����������� �����������,
// ��� ��� ������� ���������� � �� �������� �������� ������ ������
// ������������ ������ GL_movement

extern GLfloat alX=0, alY=0; // ���� ��������

// ��������� ������ � ��������� � ����������� ����������� ����
extern int lx0=0,lx1=0,ly0=0,ly1=0; // ����� �������
extern int rx0=0,rx1=0,ry0=0,ry1=0; // ������ �������
extern int dveX0=0,dveY0=0,dveX1=0,dveY1=0; //��� ������� ������� ������

/*
 �������������� ���� ������ (FOV -Field of View) � ������������� ������
 ��� �������� �����������(� ���������� ��������) � ������������� ������
 ��� ����������� ������(���������� pzoom) ������ �������� zoom(+3);
 ��� ��������� ������(���������� pzoom) ������ �������� zoom(-3);
 */
extern int pzoom=60; 
 
// ��� ������������ ������ �� ���������
// 1 - ������������� 2-�������������                        
extern int view=1;

extern bool m1=false; // ������ �� ����� ������� ����
extern bool m2=false; // ������ �� ������ ������� ����

// �������������� ����������� ����������
extern GLdouble w=900,h=900,px=0,py=0;

extern MyPoint e(10,20,10);
extern MyPoint c(0,0,0);
extern MyPoint u(0,0,1);

MyPoint Light_pos(10,0,40);
bool plosk_smooth=true; // ���������� ����������� ���������?


const int  prec = 20;
MyPoint Bezie1[prec];
MyPoint Bezie2[prec];
MyPoint Bezie3[prec];
MyPoint Bezie4[prec];
MyPoint Bezie5[prec];
MyPoint Bezie6[prec];
MyPoint Bezie7[prec];
int total = 0;
int cur = 0;
float i = 0;
GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
GLfloat sh = 0.1f * 256;

static bool light = 0;

// ��������� �� ��������
GLubyte *resImage;
GLubyte *rImage;
GLubyte *resImage1;
GLubyte *resImage4;
// ����� ��������
GLuint texture1;
GLuint texture2;
GLuint texture3;
GLuint texture4;

// ���������� ��� ������ �����������
GLuint Item1;

int resImageWidth, resImageHeight;
int resImageWidth2, resImageHeight2;
int resImageWidth3, resImageHeight3;
int resImageWidth4, resImageHeight4;

static void CALLBACK Resize(int width, int height);
static void CALLBACK Draw(void);

void  DrawModel(void);

GLint windowW, windowH;

std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;


std::vector<tinyobj::shape_t> shapesF;
std::vector<tinyobj::material_t> materialsF;

std::vector<tinyobj::shape_t> shapes1;
std::vector<tinyobj::material_t> materials1;
// ������� ������� ������ ���������� ���������
void InitLight(void)
 {
 
  GLfloat amb[] = {0.1,0.1,0.1,1.};
  GLfloat dif[] = {1.,1.,1.,1.};
  GLfloat spec[] = {1.,1.,1.,1.};
  GLfloat pos[] = {Light_pos.x,Light_pos.y,Light_pos.z,1.};

// ��������� ��������� �����
// ������� ���������
  glLightfv(GL_LIGHT0, GL_POSITION, pos);



// �������������� ����������� �����
// ������� ��������� (���������� ����)
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
// ��������� ������������ �����
  glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
// ��������� ���������� ������������ �����
  glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
  
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glPointSize(10);
  glColor3ub(255,255,0);
  glBegin(GL_POINTS);
   glVertex3fv(Light_pos);
  glEnd();
  glEnable(GL_LIGHTING);

}


// ������� ��������� �������

//extern GLfloat alX=0, alY=0; // ���� ��������
int mx0,mx1,my0,my1;

void CALLBACK Key_DOWN(void)
{
  if(view==1)
    jamp(-3,0);
  else
    strate(-3,0);
}
void CALLBACK Key_UP(void)
{
  if(view==1)
    jamp(3,0);
  else
    strate(3,0);
}
void CALLBACK Key_LEFT(void)
{
  jamp(0,3);
}
void CALLBACK Key_RIGHT(void)
{
   jamp(0,-3);
}

// �������� �� ������� ����� ������ ����
void CALLBACK mouse_left_down(AUX_EVENTREC *event)
{   
   m1=true;                           // ���� �������, ��� ����� ������ ���� ������
   lx0 = event->data[AUX_MOUSEX];    // ��������� � ���������� �����
   ly0 = event->data[AUX_MOUSEY];   // ��������� y ���������� �����
   
   if ((m1) && (m2))                         // ���� ���(� ������ � �����) ������ ���� ���� ������
    {                                       // �� ��������� � � y ���������� ���� � ��������� ����������,
      dveX0 = event->data[AUX_MOUSEX];     //  ���������� �� �������� ����������� (� ���� ��������)  
      dveY0 = event->data[AUX_MOUSEY];    //  ���  ��������� ���� ������ � ������������� ��������  
                                         
    }

}
 // �������� �� ������� ���������� ����� ������ ����
void CALLBACK mouse_left_up(AUX_EVENTREC *event)
{
    m1=false;   
}


// �������� �� ������� ������� ������ ������
void CALLBACK mouse_right_down(AUX_EVENTREC *event)
{
    m2=1;                             // ���� �������, ��� ������ ������ ���� ������
    rx0 = event->data[AUX_MOUSEX];   // ��������� � ���������� �����
    ry0 = event->data[AUX_MOUSEY];  // ��������� y ���������� �����
    
    if ((m1) && (m2))
    {                                          // ���� ���(� ������ � �����) ������ ���� ���� ������
     dveX0 = event->data[AUX_MOUSEX];         // �� ��������� � � y ���������� ���� � ��������� ����������,
     dveY0 = event->data[AUX_MOUSEY];        //  ���������� �� �������� ����������� (� ���� ��������)  
    }                                       //  ���  ��������� ���� ������ � ������������� �������� 
}

// �������� �� ������� ���������� ������ ������ ����
void CALLBACK mouse_right_up(AUX_EVENTREC *event)
{m2=0;
}

// ����� ���������� ������������ ��� �������� ����
void CALLBACK mouse_move(AUX_EVENTREC *event)
{

 if((m1)&&(m2))                                  // � �� ���� �� �������� ����� � �������� ������ ��������?
    {dveX1=event->data[AUX_MOUSEX]-dveX0;
     dveY1 = event->data[AUX_MOUSEY]-dveY0;
     
     zoom((int)(dveX1+dveY1));                  // ���� �� �� ��������� �����������/��������� ���� ������

     dveX0 = event->data[AUX_MOUSEX];
     dveY0 = event->data[AUX_MOUSEY];  
     return;
    }
 else
    {
        if (m1)                                        // ����� ���� ������ ����� ������ ����?
            {  lx1 = event->data[AUX_MOUSEX]-lx0;
               ly1 = event->data[AUX_MOUSEY]-ly0;
                    
                    jamp(-ly1,lx1);                  // ����� ���������� ����� � �����(�� �����������) 
                    
               lx0 = event->data[AUX_MOUSEX];
               ly0 = event->data[AUX_MOUSEY];
            }
        else if(m2)                                  // ��� ����� ���� ������ ������ ������ ����?
        {
            
            rx1 = event->data[AUX_MOUSEX]-rx0;
            ry1 = event->data[AUX_MOUSEY]-ry0;
                
                look_around(-rx1/10.0,ry1/30.0);              // ����� ��������� ������
                
            rx0 = event->data[AUX_MOUSEX];
            ry0 = event->data[AUX_MOUSEY];
        }
    } 	
   
}

void CALLBACK view_select(void)
{view++;
if (view>2)
  view=1;
  
pzoom=80;
c.Set(0,0,0);
e.Set(10,20,10);
zoom(0);
}
  
void CALLBACK  Light_in_Camera(void)
{
    if(view==2) // ���� � �����������
    {
        Light_pos=e;
    }
}

void CALLBACK  plosk_select(void)
{
  plosk_smooth=!plosk_smooth;
}

void loadImageFountain()
{
	int i, j;
	AUX_RGBImageRec* srcImage;
	//srcImage = auxDIBImageLoad("list.bmp");
	srcImage = auxDIBImageLoad("mramor.bmp");
	resImageWidth2 = srcImage->sizeX;
	resImageHeight2 = srcImage->sizeY;

	//�������� ������ ��� ���� �����������
	rImage = new unsigned char[srcImage->sizeX * srcImage->sizeY * 4];

	//��������� ����������� (����� �� ������ � �� ��������)
	for (i = 0; i<srcImage->sizeY; i++)
	{
		for (j = 0; j<srcImage->sizeX; j++)
		{
			//��������������� � ����� ����������� �������� �������� ���������

			rImage[i*srcImage->sizeX * 4 + j * 4] = srcImage->data[i*srcImage->sizeX * 3 + j * 3];
			rImage[i*srcImage->sizeX * 4 + j * 4 + 1] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 1];
			rImage[i*srcImage->sizeX * 4 + j * 4 + 2] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 2];

			// ���� ���� �� ���� ����� 0, �� ������� ����� ��������� =0;
			if (rImage[i*srcImage->sizeX * 4 + j * 4] + rImage[i*srcImage->sizeX * 4 + j * 4 + 1] + rImage[i*srcImage->sizeX * 4 + j * 4 + 2] == 0)
				rImage[i*srcImage->sizeX * 4 + j * 4 + 3] = 0;
			else
				rImage[i*srcImage->sizeX * 4 + j * 4 + 3] = 255;
		}
	}
	delete srcImage;
}



void loadImage()
{
	int i,j;  
	AUX_RGBImageRec* srcImage;
	srcImage=auxDIBImageLoad("homo.bmp");
	
	resImageWidth=srcImage->sizeX;
	resImageHeight=srcImage->sizeY;

	//�������� ������ ��� ���� �����������
	resImage=new unsigned char[srcImage->sizeX * srcImage->sizeY * 4];

	//��������� ����������� (����� �� ������ � �� ��������)
	for (i=0;i<srcImage->sizeY;i++)
	{
		for(j=0;j<srcImage->sizeX;j++)
		{
			//��������������� � ����� ����������� �������� �������� ���������
			
			resImage[i*srcImage->sizeX*4+j*4]=srcImage->data[i*srcImage->sizeX*3+j*3];
			resImage[i*srcImage->sizeX*4+j*4+1]=srcImage->data[i*srcImage->sizeX*3+j*3+1];
			resImage[i*srcImage->sizeX*4+j*4+2]=srcImage->data[i*srcImage->sizeX*3+j*3+2];			
			
			// ���� ���� �� ���� ����� 0, �� ������� ����� ��������� =0;
                if (resImage[i*srcImage->sizeX*4+j*4]+resImage[i*srcImage->sizeX*4+j*4+1]+resImage[i*srcImage->sizeX*4+j*4+2]==0)
                  resImage[i*srcImage->sizeX*4+j*4+3]=0;
                else
                  resImage[i*srcImage->sizeX*4+j*4+3]=255;	
		}
	}
	delete srcImage;
}
void loadImage1()
{
	int i, j;
	AUX_RGBImageRec* srcImage;
	srcImage = auxDIBImageLoad("earth.bmp");

	resImageWidth3 = srcImage->sizeX;
	resImageHeight3 = srcImage->sizeY;

	//�������� ������ ��� ���� �����������
	resImage1 = new unsigned char[srcImage->sizeX * srcImage->sizeY * 4];

	//��������� ����������� (����� �� ������ � �� ��������)
	for (i = 0; i<srcImage->sizeY; i++)
	{
		for (j = 0; j<srcImage->sizeX; j++)
		{
			//��������������� � ����� ����������� �������� �������� ���������

			resImage1[i*srcImage->sizeX * 4 + j * 4] = srcImage->data[i*srcImage->sizeX * 3 + j * 3];
			resImage1[i*srcImage->sizeX * 4 + j * 4 + 1] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 1];
			resImage1[i*srcImage->sizeX * 4 + j * 4 + 2] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 2];

			// ���� ���� �� ���� ����� 0, �� ������� ����� ��������� =0;
			if (resImage1[i*srcImage->sizeX * 4 + j * 4] + resImage1[i*srcImage->sizeX * 4 + j * 4 + 1] + resImage1[i*srcImage->sizeX * 4 + j * 4 + 2] == 0)
				resImage1[i*srcImage->sizeX * 4 + j * 4 + 3] = 0;
			else
				resImage1[i*srcImage->sizeX * 4 + j * 4 + 3] = 255;
		}
	}
	delete srcImage;
}
void loadImage3()
{
	int i, j;
	AUX_RGBImageRec* srcImage;
	srcImage = auxDIBImageLoad("water.bmp");

	resImageWidth4 = srcImage->sizeX;
	resImageHeight4 = srcImage->sizeY;

	//�������� ������ ��� ���� �����������
	resImage4 = new unsigned char[srcImage->sizeX * srcImage->sizeY * 4];

	//��������� ����������� (����� �� ������ � �� ��������)
	for (i = 0; i<srcImage->sizeY; i++)
	{
		for (j = 0; j<srcImage->sizeX; j++)
		{
			//��������������� � ����� ����������� �������� �������� ���������

			resImage4[i*srcImage->sizeX * 4 + j * 4] = srcImage->data[i*srcImage->sizeX * 3 + j * 3];
			resImage4[i*srcImage->sizeX * 4 + j * 4 + 1] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 1];
			resImage4[i*srcImage->sizeX * 4 + j * 4 + 2] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 2];

			// ���� ���� �� ���� ����� 0, �� ������� ����� ��������� =0;
			if (resImage4[i*srcImage->sizeX * 4 + j * 4] + resImage4[i*srcImage->sizeX * 4 + j * 4 + 1] + resImage4[i*srcImage->sizeX * 4 + j * 4 + 2] == 0)
				resImage4[i*srcImage->sizeX * 4 + j * 4 + 3] = 0;
			else
				resImage4[i*srcImage->sizeX * 4 + j * 4 + 3] = 255;
		}
	}
	delete srcImage;
}


void Textura_use(void)
{

		loadImage();
		loadImageFountain();
		loadImage1();
		loadImage3();
		// ������������� ������ �������� ��������
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		//  ������� glGenTextures() � glBindTexture() ������� � ������� ���������� ������ ��� ����������� ��������.
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		// ������������� ��������� ���������� �� �����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// ��������� ����������� ������� ���� ���������� ���������� ��� ���������� ��������
		//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


		// ���� ������� �� ��������� �������� � ������ ����������
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth, resImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, resImage);


		// ������ ��������� �������� GL_MODULATE, GL_DECAL, and GL_BLEND.
		// GL_MODULATE - ��������� �� ���� �� ���������
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		//      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, texture1);


		//Fountain
		glGenTextures(1, &texture2);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, texture2);	// ������������� ��������� ���������� �� �����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //���� GL_LINEAR
		// ���� ������� �� ��������� �������� � ������ ����������
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth2, resImageHeight2, 0, GL_RGBA, GL_UNSIGNED_BYTE, rImage);
		// GL_MODULATE - ��������� �� ���� �� ���������

		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glGenTextures(1, &texture3);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, texture3);	// ������������� ��������� ���������� �� �����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //���� GL_LINEAR
		// ���� ������� �� ��������� �������� � ������ ����������
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth3, resImageHeight3, 0, GL_RGBA, GL_UNSIGNED_BYTE, resImage1);
		// GL_MODULATE - ��������� �� ���� �� ���������

		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, texture3);

		glGenTextures(1, &texture4);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, texture4);	// ������������� ��������� ���������� �� �����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //���� GL_LINEAR
		// ���� ������� �� ��������� �������� � ������ ����������
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth4, resImageHeight4, 0, GL_RGBA, GL_UNSIGNED_BYTE, resImage4);
		// GL_MODULATE - ��������� �� ���� �� ���������

		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, texture4);


}


void CALLBACK texture_mode(void)
{
static bool t=0;
if(t)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
else
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}
t=!t;
}




void loaddata()
{
  const char* filename="HOMO.obj";
  const char* basepath = NULL;

  std::cout << "Loading " << filename << std::endl;


  std::string err = tinyobj::LoadObj(shapes, materials, filename, basepath);

   if (!err.empty()) 
   {
		std::cerr << err << std::endl;
   }
   else
   {
	  std::cout << "# of shapes    : " << shapes.size() << std::endl;
	  std::cout << "# of materials : " << materials.size() << std::endl;
	  
	  //init 
	  for (size_t i = 0; i < 3; i++)
	  {
		  amb[i] = materials[0].ambient[i];
		  dif[i] = materials[0].diffuse[i];
		  spec[i] = materials[0].specular[i];
		  sh = materials[0].shininess;
	  }
	

      for (size_t i = 0; i < materials.size(); i++) {
        
        std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
        std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());

        printf("\n");
      }
   }



}

void loadFountain()
{
	const char* filename = "FON.obj";
	const char* basepath = NULL;

	std::cout << "Loading " << filename << std::endl;


	std::string err = tinyobj::LoadObj(shapesF, materialsF, filename, basepath);
	if (!err.empty())
	{
		std::cerr << err << std::endl;
	}
	else
	{
		std::cout << "# of shapes    : " << shapesF.size() << std::endl;
		std::cout << "# of materials : " << materialsF.size() << std::endl;
	}
}
void loadGround()
{
	const char* filename = "sky.obj";
	const char* basepath = NULL;

	std::cout << "Loading " << filename << std::endl;


	std::string err = tinyobj::LoadObj(shapes1, materials1, filename, basepath);
	if (!err.empty())
	{
		std::cerr << err << std::endl;
	}
	else
	{
		std::cout << "# of shapes    : " << shapes1.size() << std::endl;
		std::cout << "# of materials : " << materials1.size() << std::endl;
	}
}
void drawdataFountain()
{

	glColor3ub(255, 0, 0);

	for (unsigned int i = 0; i< shapesF.size(); i++)
	{
		if (shapesF[i].mesh.normals.size())
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);

		if (shapesF[i].mesh.texcoords.size())
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);


		glBegin(GL_TRIANGLES);
		for (int k = 0; k< shapesF[i].mesh.indices.size(); k += 1)
		{
			if (shapesF[i].mesh.normals.size())
			{
				glNormal3f(
					shapesF[i].mesh.normals[shapesF[i].mesh.indices[k] * 3 + 0],
					shapesF[i].mesh.normals[shapesF[i].mesh.indices[k] * 3 + 1],
					shapesF[i].mesh.normals[shapesF[i].mesh.indices[k] * 3 + 2]);
			}
			if (shapesF[i].mesh.texcoords.size())
			{
				glTexCoord2f(
					shapesF[i].mesh.texcoords[shapesF[i].mesh.indices[k] * 2 + 0],
					shapesF[i].mesh.texcoords[shapesF[i].mesh.indices[k] * 2 + 1]);


			}
			glVertex3f(
				shapesF[i].mesh.positions[shapesF[i].mesh.indices[k] * 3 + 0],
				shapesF[i].mesh.positions[shapesF[i].mesh.indices[k] * 3 + 1],
				shapesF[i].mesh.positions[shapesF[i].mesh.indices[k] * 3 + 2]);

		}
		glEnd();
	}

}
void drawdataGround()
{

	glColor3ub(255, 0, 0);

	for (unsigned int i = 0; i< shapes1.size(); i++)
	{
		if (shapes1[i].mesh.normals.size())
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);

		if (shapes1[i].mesh.texcoords.size())
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);


		glBegin(GL_TRIANGLES);
		for (int k = 0; k< shapes1[i].mesh.indices.size(); k += 1)
		{
			if (shapes1[i].mesh.normals.size())
			{
				glNormal3f(
					shapes1[i].mesh.normals[shapes1[i].mesh.indices[k] * 3 + 0],
					shapes1[i].mesh.normals[shapes1[i].mesh.indices[k] * 3 + 1],
					shapes1[i].mesh.normals[shapes1[i].mesh.indices[k] * 3 + 2]);
			}
			if (shapesF[i].mesh.texcoords.size())
			{
				glTexCoord2f(
					shapes1[i].mesh.texcoords[shapes1[i].mesh.indices[k] * 2 + 0],
					shapes1[i].mesh.texcoords[shapes1[i].mesh.indices[k] * 2 + 1]);


			}
			glVertex3f(
				shapes1[i].mesh.positions[shapes1[i].mesh.indices[k] * 3 + 0],
				shapes1[i].mesh.positions[shapes1[i].mesh.indices[k] * 3 + 1],
				shapes1[i].mesh.positions[shapes1[i].mesh.indices[k] * 3 + 2]);

		}
		glEnd();
	}

}
void drawdata()
{

	glColor3ub(255,0,0);


for(unsigned int i=0; i< shapes.size(); i++)
  {
    if(shapes[i].mesh.normals.size())    
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	if(shapes[i].mesh.texcoords.size())
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
    
  
    glBegin(GL_TRIANGLES);
    for(int k=0;k< shapes[i].mesh.indices.size();k+=1)
    {
        if(shapes[i].mesh.normals.size())
        {
            glNormal3f(   
                        shapes[i].mesh.normals[shapes[i].mesh.indices[k]*3+0],
                        shapes[i].mesh.normals[shapes[i].mesh.indices[k]*3+1],
                        shapes[i].mesh.normals[shapes[i].mesh.indices[k]*3+2]);
        }
		if(shapes[i].mesh.texcoords.size())
		{
			glTexCoord2f(
						shapes[i].mesh.texcoords[shapes[i].mesh.indices[k]*2+0],
                        shapes[i].mesh.texcoords[shapes[i].mesh.indices[k]*2+1]);
                        
						
		}
        glVertex3f(
            shapes[i].mesh.positions[shapes[i].mesh.indices[k]*3+0],
            shapes[i].mesh.positions[shapes[i].mesh.indices[k]*3+1],
            shapes[i].mesh.positions[shapes[i].mesh.indices[k]*3+2]);

    }
    glEnd();
  }

}

void main()
{


// ����������� ������� ��������� ������� �� ���� � �� ����������
    setlocale( LC_CTYPE, "Russian" );
    printf("����� ������ �� ������� ����� ��������\r\n" );
    printf("���������� WASD - ������ � ���������� ���������\r\n" );
    
    
    printf("����������:\r\n" );
    printf("[������] - ������������ ������:\r\n" );
    printf("[������� �����] ��� [A] ��� [a] - ��������� �����\r\n" );
    printf("[������� ����]  ��� [D] ��� [D] - ��������� ������\r\n" );
    
    printf("[������� ����] ��� [W] ��� [w] - ��������� ������(�����������) ��� �����(����)\r\n" );
    printf("[������� ����] ��� [S] ��� [s] - ��������� �����(�����������) ��� ����(����)\r\n\n\n" );
    
    
    
    printf("[1] ��� [l] ��� [L] - � ������ �����������\n - ���������� �������� ��������� � ����� ����\r\n" );
    printf("[2] ��� [p] ��� [P] - �������� ��� ������ ������� ���������\r\n" );
    printf("[4] ��������/��������� ���������� ����������\r\n" );
    

    
	//auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSELOC, mouse_left_move);   // ����� ������������ ����� ���������� �� ����
	auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEDOWN, mouse_left_down);
    auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEUP, mouse_left_up);
           
    auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEDOWN, mouse_right_down);	
	auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEUP, mouse_right_up);
	
	// ��� ������� �������� - ����� ���������� � ��� ��������� �������, � ��� �������.
	// ���� ��� ������� ������, ����� � ������������ ����� ������� ������
	auxMouseFunc(NULL, AUX_MOUSELOC, mouse_move);         
	
	view_select();
	auxKeyFunc(AUX_SPACE, view_select);
	
	auxKeyFunc(AUX_DOWN, Key_DOWN);
    auxKeyFunc(AUX_LEFT, Key_LEFT); 
	auxKeyFunc(AUX_RIGHT, Key_RIGHT);
    auxKeyFunc(AUX_UP, Key_UP);    
    
    auxKeyFunc('w', Key_UP); 
    auxKeyFunc('W', Key_UP); 
    auxKeyFunc('s', Key_DOWN);
    auxKeyFunc('S', Key_DOWN);
    auxKeyFunc('a', Key_LEFT);
    auxKeyFunc('A', Key_LEFT);
    auxKeyFunc('d', Key_RIGHT);
    auxKeyFunc('D', Key_RIGHT);
    
	auxKeyFunc('L', Light_in_Camera);
	auxKeyFunc('l', Light_in_Camera);
    auxKeyFunc('1', Light_in_Camera);

    auxKeyFunc('p', plosk_select);
	auxKeyFunc('P', plosk_select);
    auxKeyFunc('2', plosk_select);
    
    auxKeyFunc('4', texture_mode);

// ������ ���� OpenGL
	windowW = 700;
	windowH = 700;

// ������������ ���� OpenGL �� ������
	auxInitPosition(100, 100, windowW, windowH);

// ��������� �������� ���������� ������ OpenGL
// �������� ����� RGB | ��������� Z-������ ��� ���������� �� �������

	auxInitDisplayMode( AUX_RGB | AUX_DEPTH | AUX_DOUBLE );

// ������������� ���� OpenGL � ���������� Title
	if(auxInitWindow("�������� ������. ������� ��������� �������������. ���-13-11") ==GL_FALSE) auxQuit();
	
	


	Textura_use();

	loaddata();
	loadFountain();
	loadGround();

// ������������� ���������� �����
// ���� ��� �������� ��������������, �� �������� ������������ ������������ 
// �����������, �.�. �� ����� ���������� � ������� ���������, 
// XOY ������� ������� � �������. 
// � ����� ������� �������� � pos[] = {0.,0.,1.,0.}; ������ ��������������� ������
   InitLight();

// ����������� �������, ������� ���������� ��� ��������� �������� ����
//  Resize() - ������� ������������
	auxReshapeFunc((AUXRESHAPEPROC)Resize);

// ����������� �������, ������� ���������� ��� ����������� 
// � ������ ����� ��������� �������
// Draw() - ������� ������������
	auxIdleFunc(Draw);
	auxMainLoop(Draw);

}



static void CALLBACK Resize(int width, int height) // ��������� �������������
{ 
//1. ��������� ������� ��������� ���� 


	w = width;
	h = height;
// ��������� ������� ���� ������ w,h  
    if(h == 0)
    	h = 1;
// ��������� ����� �������� ���� ������
glViewport(0,0,w,h);


// � ��� �� ��������� ��� �������� (������������� ��� �������������)
    zoom(0);

}



static void CALLBACK Draw(void) // ��������� �������������
{
// 1. ������� ������� (�����, ������� � �������� ��.)

  // ��������� ����� ����
	  glClearColor(0.0f,0.0f,0.0f,1.0f) ;


  // ������� ���� �������
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//2.��������� ������� ���������
  
  // ������������ �������� : �� ����� ����� ����� 1
  glEnable(GL_NORMALIZE);

  // ��������� ������ ���������� �� �������
  glEnable(GL_DEPTH_TEST) ;


  
  glEnable(GL_LINE_SMOOTH); // ���������� ������������� ��� �����
  glEnable(GL_POINT_SMOOTH);
  
  // ��� ������������� ���������� ������������� 
  // ��� ���� �������� ����� �����-���������(����� �������� BLEND - ���������)
  glEnable(GL_BLEND);
  
  // ��������� ����� �����������:
  glBlendFunc (GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);

  // ��� ��������� ���������
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // ������ ��������: �������� ��� �����, ��� ������ ������� ��� ����������
  // GL_FRONT_AND_BACK , GL_FRONT , GL_BACK  
  // ������: ��� �������� 
  // GL_POINT (�����) GL_LINE(����� �� �������), GL_FILL(���������)
  

  //
  // ��� �������� ���������
    glShadeModel(GL_FLAT) ; 
  // GL_FLAT ��� ������� �������� ������ ���������� ���� ( �� ���� ����������� 
  // ���� ������ �������������� �������, ���� �������� ���������, 
  // �� ���� ���� �������������� � ������ ���������� � ������� � �������)
  // GL_SMOOTH ���� ������� ������� �������������� ������������� ������ ������ 
  // � ������ ���������� �����. ���� ��� ��������
  
  //  �������� ����� ����� ���������
     glEnable(GL_LIGHTING); 

  //   ������ ��������� ���������
  //  �������� GL_LIGHT_MODEL_TWO_SIDE - 
  //                0 -  ������� � ���������� �������� ���������(�� ���������), 
  //                1 - ������� � ���������� �������������� ������� ��������       
  //                �������������� ������� � ���������� ��������� ����������.    
  //  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
  //                �� ��������� �� ���������
  // �� ��������� (0.2, 0.2, 0.2, 1.0)
      
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0 ); 

  
  //  �������� ������ ���������� ���������� 
   glEnable(GL_LIGHT0);

  // new!  ��������������� ������� ������� ��������� 
  // �������� glColor() �� ����������!!!
  // glEnable(GL_COLOR_MATERIAL);
  // glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
 
// 3.��������� �������� ��������������

// ����� ������� ������� � �������� �������

  glMatrixMode( GL_MODELVIEW );

// ���������� �������� �������� ������� � �����
  //glPushMatrix();

// �������� ��������� �������
  glLoadIdentity();

// ��������� ����� ����������

 gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );

 InitLight();
// ���� ����� ���������������� �������� �����
// �� �� ����� ������������ � ������� �����������, �.�.
// ��� poition(0.,0.,1.,0.) ��  ����� ������� �����
//  ������� ��� Z ( � ����� ������� ����� ���)

// �������������� ��������
// (���� ��������, �������� ����� ��� �����������)
// glRotatef(alX,1.0f,0.0f, 0.0f);
// glRotatef(alY,0.0f,1.0f, 0.0f);

//new!  InitLight();
// ���� ����� ���������������� ��������, �� �� ����� ��������� 
//������ � ��������. � ����� ������ ��� ������� ��������� 
// pos[] = {0.,0.,1.,0.} ��� ����� ��������� ���� �������� ����� 
//������ ����� �����

// 4. ����� ������
  DrawModel() ;

// 5. ���������� �������� ��������������
  // Restore the view matrix
//  glPopMatrix();

//6. ��������� ��� ��������� �� ������ ������ �������� OpenGL
  glFinish();
  auxSwapBuffers() ;
  if((!m1)&&(!m2))
    Sleep(100);

}



void DrawStuff()
{
	glBindTexture(GL_TEXTURE_2D, texture4);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUAD_STRIP);
	MyPoint a(0.2, 0.2, 0.2), b(0.0, 0.0, 0.0), c(0.2, 0.2, 0.0), d(0.2, 0.0, 0.2);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(1, 0);
	glVertex3f(a.x, a.y, a.z);
	glTexCoord2f(0, 1);
	glVertex3f(b.x, b.y, b.z);
	glTexCoord2f(0, 0);
	glVertex3f(c.x, c.y, c.z);

	glTexCoord2f(1, 0);
	glVertex3f(a.x, a.y, a.z);
	glTexCoord2f(0, 1);
	glVertex3f(b.x, b.y, b.z);
	glTexCoord2f(0, 0);
	glVertex3f(d.x, d.y, d.z);

	glTexCoord2f(1, 0);
	glVertex3f(a.x, a.y, a.z);
	glTexCoord2f(0, 1);
	glVertex3f(c.x, c.y, c.z);
	glTexCoord2f(0, 0);
	glVertex3f(d.x, d.y, d.z);

	glTexCoord2f(1, 0);
	glVertex3f(b.x, b.y, b.z);
	glTexCoord2f(0, 1);
	glVertex3f(c.x, c.y, c.z);
	glTexCoord2f(0, 0);
	glVertex3f(d.x, d.y, d.z);


	glEnd();
	glDisable(GL_LIGHTING);
}

float GetVecLength(MyPoint p)
{
	return sqrt(p.x * p.x + p.y*p.y + p.z*p.z);
}

void CalcBezie1(){
	MyPoint P1 = { 18, 0, 14 };
	MyPoint P2 = { 10, 0, 14 };
	MyPoint P3 = { 9.8, 0, 10 };
	MyPoint P4 = { 9.8, 0, 8 };

	float division = 1 / (float)prec;
	float t = 0;
	int i = 0;
	for (; t <= 1 && i < prec; t += division, i++)
	{
		Bezie1[i] = P1*(1 - t)*(1 - t)*(1 - t)
			+ P2 * 3 * t*(1 - t)*(1 - t)
			+ P3 * 3 * t*t*(1 - t)
			+ P4 * t*t*t;
	}total = --i;
}
void CalcBezie2(){
	MyPoint P1 = { 18, 0, 14 };
	MyPoint P2 = { 10, 0, 14 };
	MyPoint P3 = { 9.5, 1.1f, 10 };
	MyPoint P4 = { 9.5, 0.8f, 6.4f };

	float division = 1 / (float)prec;
	float t = 0;
	int i = 0;
	for (; t <= 1 && i < prec; t += division, i++)
	{
		Bezie2[i] = P1*(1 - t)*(1 - t)*(1 - t)
			+ P2 * 3 * t*(1 - t)*(1 - t)
			+ P3 * 3 * t*t*(1 - t)
			+ P4 * t*t*t;
	}total = --i;
}
void CalcBezie3(){
	MyPoint P1 = { 18, 0, 14 };
	MyPoint P2 = { 10, 0, 14 };
	MyPoint P3 = { 9.5, -1.1f, 10 };
	MyPoint P4 = { 9.9, -1.3f, 6.4f };

	float division = 1 / (float)prec;
	float t = 0;
	int i = 0;
	for (; t <= 1 && i < prec; t += division, i++)
	{
		Bezie3[i] = P1*(1 - t)*(1 - t)*(1 - t)
			+ P2 * 3 * t*(1 - t)*(1 - t)
			+ P3 * 3 * t*t*(1 - t)
			+ P4 * t*t*t;
	}total = --i;
}
void CalcBezie4(){
	MyPoint P1 = { 10., -0.8f, 7.f };
	MyPoint P2 = { 12.8, 2.5f, 7.f };
	MyPoint P3 = { 12.8, 2.5f, 7.f };
	MyPoint P4 = { 12.2, 0.8f, -2.5f };

	float division = 1 / (float)prec;
	float t = 0;
	int i = 0;
	for (; t <= 1 && i < prec; t += division, i++)
	{
		Bezie4[i] = P1*(1 - t)*(1 - t)*(1 - t)
			+ P2 * 3 * t*(1 - t)*(1 - t)
			+ P3 * 3 * t*t*(1 - t)
			+ P4 * t*t*t;
	}total = --i;
}

void CalcBezie5(){
	MyPoint P1 = { 9.6, -0.2f, 7.f };
	MyPoint P2 = { 12.2, 2.8f, 7.f };
	MyPoint P3 = { 12.3, 2.8f, 7.f };
	MyPoint P4 = { 11.2, 2.2f, -2.5f };

	float division = 1 / (float)prec;
	float t = 0;
	int i = 0;
	for (; t <= 1 && i < prec; t += division, i++)
	{
		Bezie5[i] = P1*(1 - t)*(1 - t)*(1 - t)
			+ P2 * 3 * t*(1 - t)*(1 - t)
			+ P3 * 3 * t*t*(1 - t)
			+ P4 * t*t*t;
	}total = --i;
}
void CalcBezie6(){
	MyPoint P1 = { 9., -0.7f, 7.f };
	MyPoint P2 = { 6.8, -2.3f, 6.7f };
	MyPoint P3 = { 6.8, -2.3f, 6.7f };
	MyPoint P4 = { 7.7, -1.7f, -2.5f };

	float division = 1 / (float)prec;
	float t = 0;
	int i = 0;
	for (; t <= 1 && i < prec; t += division, i++)
	{
		Bezie6[i] = P1*(1 - t)*(1 - t)*(1 - t)
			+ P2 * 3 * t*(1 - t)*(1 - t)
			+ P3 * 3 * t*t*(1 - t)
			+ P4 * t*t*t;
	}total = --i;
}


void CalcBezie7(){
	MyPoint P1 = { 9.5, -0.6f, 7.f };
	MyPoint P2 = { 7.4, -2.9f, 6.7f };
	MyPoint P3 = { 7.4, -2.9f, 6.7f };
	MyPoint P4 = { 8.7, -2.9f, -2.5f };

	float division = 1 / (float)prec;
	float t = 0;
	int i = 0;
	for (; t <= 1 && i < prec; t += division, i++)
	{
		Bezie7[i] = P1*(1 - t)*(1 - t)*(1 - t)
			+ P2 * 3 * t*(1 - t)*(1 - t)
			+ P3 * 3 * t*t*(1 - t)
			+ P4 * t*t*t;
	}total = --i;
}

void Walk1()
{
	float zz = 0.0, yy = 0.0, xx = 0.0;

	xx = acos((Bezie1[cur + 1].x - Bezie1[cur].x) / (GetVecLength(Bezie1[cur + 1] - Bezie1[cur]))) * 180.0f / (double)PI;
	yy = acos((Bezie1[cur + 1].y - Bezie1[cur].y) / (GetVecLength(Bezie1[cur + 1] - Bezie1[cur]))) * 180.0f / (double)PI;
	zz = acos((Bezie1[cur + 1].z - Bezie1[cur].z) / (GetVecLength(Bezie1[cur + 1] - Bezie1[cur]))) * 180.0f / (double)PI;

	glPushMatrix();
	glTranslatef(Bezie1[cur].x, Bezie1[cur].y, Bezie1[cur].z);
	glRotatef(xx, 0.0f, 0.0f, -1.0f);
	glRotatef(zz, 0.0f, 0.0f, -1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawStuff();
	glPopMatrix();
	cur++;
	if (cur > total){
		cur = 0;
	}
}
void Walk2()
{
	float zz = 0.0, yy = 0.0, xx = 0.0;

	xx = acos((Bezie2[cur + 1].x - Bezie2[cur].x) / (GetVecLength(Bezie2[cur + 1] - Bezie2[cur]))) * 180.0f / (double)PI;
	yy = acos((Bezie2[cur + 1].y - Bezie2[cur].y) / (GetVecLength(Bezie2[cur + 1] - Bezie2[cur]))) * 180.0f / (double)PI;
	zz = acos((Bezie2[cur + 1].z - Bezie2[cur].z) / (GetVecLength(Bezie2[cur + 1] - Bezie2[cur]))) * 180.0f / (double)PI;

	glPushMatrix();
	glTranslatef(Bezie2[cur].x, Bezie2[cur].y, Bezie2[cur].z);
	glRotatef(xx, 0.0f, 0.0f, -1.0f);
	glRotatef(zz, 0.0f, 0.0f, -1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawStuff();
	glPopMatrix();
	cur++;
	if (cur > total){
		cur = 0;
	}
}
void Walk3()
{
	float zz = 0.0, yy = 0.0, xx = 0.0;

	xx = acos((Bezie3[cur + 1].x - Bezie3[cur].x) / (GetVecLength(Bezie3[cur + 1] - Bezie3[cur]))) * 180.0f / (double)PI;
	yy = acos((Bezie3[cur + 1].y - Bezie3[cur].y) / (GetVecLength(Bezie3[cur + 1] - Bezie3[cur]))) * 180.0f / (double)PI;
	zz = acos((Bezie3[cur + 1].z - Bezie3[cur].z) / (GetVecLength(Bezie3[cur + 1] - Bezie3[cur]))) * 180.0f / (double)PI;

	glPushMatrix();
	glTranslatef(Bezie3[cur].x, Bezie3[cur].y, Bezie3[cur].z);
	glRotatef(xx, 0.0f, 0.0f, -1.0f);
	glRotatef(zz, 0.0f, 0.0f, -1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawStuff();
	glPopMatrix();
	cur++;
	if (cur > total){
		cur = 0;
	}
}
void Walk4()
{
	float zz = 0.0, yy = 0.0, xx = 0.0;

	xx = acos((Bezie4[cur + 1].x - Bezie4[cur].x) / (GetVecLength(Bezie4[cur + 1] - Bezie4[cur]))) * 180.0f / (double)PI;
	yy = acos((Bezie4[cur + 1].y - Bezie4[cur].y) / (GetVecLength(Bezie4[cur + 1] - Bezie4[cur]))) * 180.0f / (double)PI;
	zz = acos((Bezie4[cur + 1].z - Bezie4[cur].z) / (GetVecLength(Bezie4[cur + 1] - Bezie4[cur]))) * 180.0f / (double)PI;

	glPushMatrix();
	glTranslatef(Bezie4[cur].x, Bezie4[cur].y, Bezie4[cur].z);
	glRotatef(xx, 0.0f, 0.0f, -1.0f);
	glRotatef(zz, 0.0f, 0.0f, -1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawStuff();
	glPopMatrix();
	cur++;
	if (cur > total){
		cur = 0;
	}
}
void Walk5()
{
	float zz = 0.0, yy = 0.0, xx = 0.0;

	xx = acos((Bezie5[cur + 1].x - Bezie5[cur].x) / (GetVecLength(Bezie5[cur + 1] - Bezie5[cur]))) * 180.0f / (double)PI;
	yy = acos((Bezie5[cur + 1].y - Bezie5[cur].y) / (GetVecLength(Bezie5[cur + 1] - Bezie5[cur]))) * 180.0f / (double)PI;
	zz = acos((Bezie5[cur + 1].z - Bezie5[cur].z) / (GetVecLength(Bezie5[cur + 1] - Bezie5[cur]))) * 180.0f / (double)PI;

	glPushMatrix();
	glTranslatef(Bezie5[cur].x, Bezie5[cur].y, Bezie5[cur].z);
	glRotatef(xx, 0.0f, 0.0f, -1.0f);
	glRotatef(zz, 0.0f, 0.0f, -1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawStuff();
	glPopMatrix();
	cur++;
	if (cur > total){
		cur = 0;
	}
}
void Walk6()
{
	float zz = 0.0, yy = 0.0, xx = 0.0;

	xx = acos((Bezie6[cur + 1].x - Bezie6[cur].x) / (GetVecLength(Bezie6[cur + 1] - Bezie6[cur]))) * 180.0f / (double)PI;
	yy = acos((Bezie6[cur + 1].y - Bezie6[cur].y) / (GetVecLength(Bezie6[cur + 1] - Bezie6[cur]))) * 180.0f / (double)PI;
	zz = acos((Bezie6[cur + 1].z - Bezie6[cur].z) / (GetVecLength(Bezie6[cur + 1] - Bezie6[cur]))) * 180.0f / (double)PI;

	glPushMatrix();
	glTranslatef(Bezie6[cur].x, Bezie6[cur].y, Bezie6[cur].z);
	glRotatef(xx, 0.0f, 0.0f, -1.0f);
	glRotatef(zz, 0.0f, 0.0f, -1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawStuff();
	glPopMatrix();
	cur++;
	if (cur > total){
		cur = 0;
	}
}
void Walk7()
{
	float zz = 0.0, yy = 0.0, xx = 0.0;

	xx = acos((Bezie7[cur + 1].x - Bezie7[cur].x) / (GetVecLength(Bezie7[cur + 1] - Bezie7[cur]))) * 180.0f / (double)PI;
	yy = acos((Bezie7[cur + 1].y - Bezie7[cur].y) / (GetVecLength(Bezie7[cur + 1] - Bezie7[cur]))) * 180.0f / (double)PI;
	zz = acos((Bezie7[cur + 1].z - Bezie7[cur].z) / (GetVecLength(Bezie7[cur + 1] - Bezie7[cur]))) * 180.0f / (double)PI;

	glPushMatrix();
	glTranslatef(Bezie7[cur].x, Bezie7[cur].y, Bezie7[cur].z);
	glRotatef(xx, 0.0f, 0.0f, -1.0f);
	glRotatef(zz, 0.0f, 0.0f, -1.0f);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	DrawStuff();
	glPopMatrix();
	cur++;
	if (cur > total){
		cur = 0;
	}
}



void Ugol_update()
{
	i += 0.05;
	if (i > 1)
		i -= 0;
}

void DrawModel(void)
{
	Ugol_update();
	float t = 0;
	MyPoint dot;
	MyPoint dot1;


	glColor3f(0.5f, 0.5f, 0.5f);


	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT, GL_SHININESS, sh);



	if (plosk_smooth)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);
	


		glEnable(GL_TEXTURE_2D);


		glEnable(GL_BLEND);
		// ��������� ��������� � ���������
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPushMatrix();
	  

		glEnable(GL_LIGHTING);
		glPushMatrix();
		glTranslatef(21, 0, 14.3f);
		glBindTexture(GL_TEXTURE_2D, texture1);
		drawdata();
		glPopMatrix();
		glPushMatrix();
		glTranslatef(10, 0, 0);
		glBindTexture(GL_TEXTURE_2D, texture2);
		drawdataFountain();
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, texture3);
		drawdataGround();
		glPopMatrix();
		glDisable(GL_LIGHTING);

		//������ �����

		float division = 1 / (float)prec;
		int j = 0;
		CalcBezie1();
		CalcBezie2();
		CalcBezie3();
		CalcBezie4();
		CalcBezie5();
		CalcBezie6();
		CalcBezie7();

		//�����

		for (j = 0; j < 5; j++)
		{
			Walk1();
			Walk2();
			Walk3();
			Walk4();
			Walk5();
			Walk6();
			Walk7();
		}

		//����
			MyPoint P1 = { 10., -0.8f, 7.f };
			MyPoint P2 = { 12.8, 2.5f, 7.f };
			MyPoint P3 = { 12.8, 2.5f, 7.f };
			MyPoint P4 = { 12.2, 0.8f, -2.5f };
			MyPoint P5 = { 9.6, -0.2f, 7.f };
			MyPoint P6 = { 12.2, 2.8f, 7.f };
			MyPoint P7 = { 12.3, 2.8f, 7.f };
			MyPoint P8 = { 11.2, 2.2f, -2.5f };
			glBindTexture(GL_TEXTURE_2D, texture4);
			glEnable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_QUAD_STRIP);
			t = 0.95; glNormal3f(1, 0, 1);
			Bezie4[1] = P1*(1 - t)*(1 - t)*(1 - t)
				+ P2 * 3 * t*(1 - t)*(1 - t)
				+ P3 * 3 * t*t*(1 - t)
				+ P4 * t*t*t;
			Bezie5[1] = P5*(1 - t)*(1 - t)*(1 - t)
				+ P6 * 3 * t*(1 - t)*(1 - t)
				+ P7 * 3 * t*t*(1 - t)
				+ P8 * t*t*t;

			for (t = 0.95; t >= 0; t -= 1 / (float)prec)
			{
				glTexCoord2f(t - i, 0);
				glVertex3fv(Bezie4[1]);
				glTexCoord2f(t - i, 0.25);
				glVertex3fv(Bezie5[1]);

				Bezie4[1] = P1*(1 - t)*(1 - t)*(1 - t)
					+ P2 * 3 * t*(1 - t)*(1 - t)
					+ P3 * 3 * t*t*(1 - t)
					+ P4 * t*t*t;
				Bezie5[1] = P5*(1 - t)*(1 - t)*(1 - t)
					+ P6 * 3 * t*(1 - t)*(1 - t)
					+ P7 * 3 * t*t*(1 - t)
					+ P8 * t*t*t;
				glTexCoord2f(t - i - 0.05, 0 );
				glVertex3fv(Bezie4[1]);
				glTexCoord2f(t - i - 0.05, 0.25);
				glVertex3fv(Bezie5[1]);


			}
			glVertex3fv(P4);
			glEnd();


			 P1 = { 18, 0, 14 };
			 P2 = { 10, 0, 14 };
			 P3 = { 9.5, 1.1f, 10 };
			 P4 = { 9.5, 0.8f, 6.4f };
			 P5 = { 18, 0, 14 };
			 P6 = { 10, 0, 14 };
			 P7 = { 9.5, -1.1f, 10 };
			 P8 = { 9.9, -1.3f, 6.4f };

			 glBegin(GL_QUAD_STRIP);
			 t = 0.95; glNormal3f(-1, 0, 1);
			 Bezie4[1] = P1*(1 - t)*(1 - t)*(1 - t)
				 + P2 * 3 * t*(1 - t)*(1 - t)
				 + P3 * 3 * t*t*(1 - t)
				 + P4 * t*t*t;
			 Bezie5[1] = P5*(1 - t)*(1 - t)*(1 - t)
				 + P6 * 3 * t*(1 - t)*(1 - t)
				 + P7 * 3 * t*t*(1 - t)
				 + P8 * t*t*t;

			 for (t = 0.95; t >= 0; t -= 1 / (float)prec)
			 {
				 glTexCoord2f(t - i, 0);
				 glVertex3fv(Bezie4[1]);
				 glTexCoord2f(t - i, 0.25);
				 glVertex3fv(Bezie5[1]);

				 Bezie4[1] = P1*(1 - t)*(1 - t)*(1 - t)
					 + P2 * 3 * t*(1 - t)*(1 - t)
					 + P3 * 3 * t*t*(1 - t)
					 + P4 * t*t*t;
				 Bezie5[1] = P5*(1 - t)*(1 - t)*(1 - t)
					 + P6 * 3 * t*(1 - t)*(1 - t)
					 + P7 * 3 * t*t*(1 - t)
					 + P8 * t*t*t;
				 glTexCoord2f(t - i - 0.05, 0);
				 glVertex3fv(Bezie4[1]);
				 glTexCoord2f(t - i - 0.05, 0.25);
				 glVertex3fv(Bezie5[1]);


			 }
			 glVertex3fv(P4);
			 glEnd();

			P1 = { 9., -0.7f, 7.f };
			P2 = { 6.8, -2.3f, 6.7f };
			P3 = { 6.8, -2.3f, 6.7f };
			P4 = { 7.7, -1.7f, -2.5f };
			P5 = { 9.5, -0.6f, 7.f };
			P6 = { 7.4, -2.9f, 6.7f };
			P7 = { 7.4, -2.9f, 6.7f };
			P8 = { 8.7, -2.9f, -2.5f };

			glBegin(GL_QUAD_STRIP);
			t = 0.95; glNormal3f(-1, 0, 1);
			Bezie4[1] = P1*(1 - t)*(1 - t)*(1 - t)
				+ P2 * 3 * t*(1 - t)*(1 - t)
				+ P3 * 3 * t*t*(1 - t)
				+ P4 * t*t*t;
			Bezie5[1] = P5*(1 - t)*(1 - t)*(1 - t)
				+ P6 * 3 * t*(1 - t)*(1 - t)
				+ P7 * 3 * t*t*(1 - t)
				+ P8 * t*t*t;

			for (t = 0.95; t >= 0; t -= 1 / (float)prec)
			{
				glTexCoord2f(t - i, 0);
				glVertex3fv(Bezie4[1]);
				glTexCoord2f(t - i, 0.25);
				glVertex3fv(Bezie5[1]);

				Bezie4[1] = P1*(1 - t)*(1 - t)*(1 - t)
					+ P2 * 3 * t*(1 - t)*(1 - t)
					+ P3 * 3 * t*t*(1 - t)
					+ P4 * t*t*t;
				Bezie5[1] = P5*(1 - t)*(1 - t)*(1 - t)
					+ P6 * 3 * t*(1 - t)*(1 - t)
					+ P7 * 3 * t*t*(1 - t)
					+ P8 * t*t*t;
				glTexCoord2f(t - i - 0.05, 0);
				glVertex3fv(Bezie4[1]);
				glTexCoord2f(t - i - 0.05, 0.25);
				glVertex3fv(Bezie5[1]);


			}
			glVertex3fv(P4);
			glEnd();



			glDisable(GL_LIGHTING);





		/*
		//OX RED
		glBegin(GL_LINES);

		glColor3f(1.0f, 0.0f, 0.0f);
		glNormal3f(0., 0., 1.);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(3.0f, 0.0f, 0.0f);
		glEnd();

		// OY GREEN
		glBegin(GL_LINES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 3.0f, 0.0f);
		glEnd();

		// OZ BLUE
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 3.0f);
		glEnd();
		*/
}