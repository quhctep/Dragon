#include <windows.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

#include "GL_movement.h"   // ������ ������� ����� 3� ����� � �������� � �������

#include <locale.h>  // ��� �������� �����   �� main
#include <stdio.h> 

#include "tiny_obj_loader.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>



// ���� �������� �������������� � ��������������� double �� float ��� ��� ����� ������������ ��� ��������������

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

MyPoint Light_pos(10,0,25);
bool plosk_smooth=true; // ���������� ����������� ���������?



// ��������� �� ��������
GLubyte *resImage;
// ����� ��������
GLuint texture1;

// ���������� ��� ������ �����������
GLuint Item1;

int resImageWidth, resImageHeight;

int mode=0;

static void CALLBACK Resize(int width, int height);
static void CALLBACK Draw(void);

void  DrawModel(void);

GLint windowW, windowH;

std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;


// ������� ������� ������ ���������� ���������
void InitLight(void)
 {
 
  GLfloat amb[] = {0.1,0.1,0.1,1.};
  GLfloat dif[] = {1.,1.,1.,1.};
  GLfloat spec[] = {3.,3.,3.,3.};
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
  glColor3ub(255, 255, 0);
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


void loadImage()
{
	int i,j;  
	AUX_RGBImageRec* srcImage;
	srcImage=auxDIBImageLoad("mramor.bmp");
	
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

void CALLBACK Textura_use(void)
{
    loadImage();

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
      glBindTexture(GL_TEXTURE_2D,texture1);

// �������� ��� ����� ������ �����������
Item1=glGenLists(2);

glNewList(Item1,GL_COMPILE);
glBegin(GL_QUADS);
glNormal3f(0,0,1);

    glTexCoord2f(0,0);
    glVertex3f(-30,-30,-3);


    glTexCoord2f(1,0);
    glVertex3f(30,-30,-3);

    glTexCoord2f(1,1);
    glVertex3f(30,30,-3);

    glTexCoord2f(0,1);
    glVertex3f(-30,30,-3);

glEnd();
glEndList();

MyPoint A(-10,-10,0),B(-10,10,0),C(10,-10,0),D(10,10,0),S(0,0,20),N1,N2,N3,N4, T1,T2;

T1=B-A;
T2= S-A;
// ����������� *= � ������������� ��������� ��������� ��������
N1=T2*=T1;
N1.Normalize_Self();

T1= (D-B);
T2=(S-B);
N2=T2*=T1;
N2.Normalize_Self();

T1=(A-C);
T2=(S-C);
N3=T2*=T1;
N3.Normalize_Self();

T1=(C-D);
T2=(S-D);
N4=T2*=T1;
N4.Normalize_Self();

/*glNewList(Item1+1,GL_COMPILE);

//������ ���������
glBegin(GL_TRIANGLES);

glNormal3fv(N1);
glTexCoord2f(1,1);glVertex3fv(B);
glTexCoord2f(0,1);glVertex3fv(A);
glTexCoord2f(0.5,0);glVertex3fv(S);

glNormal3fv(N2);
glTexCoord2f(1,1);glVertex3fv(D);
glTexCoord2f(0,1);glVertex3fv(B);
glTexCoord2f(0.5,0); glVertex3fv(S);

glNormal3fv(N3);
glTexCoord2f(1,1); glVertex3fv(C);
glTexCoord2f(0,1); glVertex3fv(D);
glTexCoord2f(0.5,0);glVertex3fv(S);

glNormal3fv(N4);
glTexCoord2f(1,1);glVertex3fv(A);
glTexCoord2f(0,1);glVertex3fv(C);
glTexCoord2f(0.5,0);glVertex3fv(S);  

glEnd();
    
glEndList();
mode=1;
*/
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
  const char* filename="FONTAN.obj";
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

	   for (size_t i = 0; i < shapes.size(); i++) {
		   printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
		   printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
		   printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
		   assert((shapes[i].mesh.indices.size() % 3) == 0);
		   for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
			   printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3 * f + 0], shapes[i].mesh.indices[3 * f + 1], shapes[i].mesh.indices[3 * f + 2], shapes[i].mesh.material_ids[f]);
		   }

		   printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
		   assert((shapes[i].mesh.positions.size() % 3) == 0);
		   for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			   printf("  v[%ld] = (%f, %f, %f)\n", v,
				   shapes[i].mesh.positions[3 * v + 0],
				   shapes[i].mesh.positions[3 * v + 1],
				   shapes[i].mesh.positions[3 * v + 2]);
		   }
	   }

	   for (size_t i = 0; i < materials.size(); i++) {
		   printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
		   printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
		   printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
		   printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
		   printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
		   printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
		   printf("  material.Ns = %f\n", materials[i].shininess);
		   printf("  material.Ni = %f\n", materials[i].ior);
		   printf("  material.dissolve = %f\n", materials[i].dissolve);
		   printf("  material.illum = %d\n", materials[i].illum);
		   printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
		   printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
		   printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
		   printf("  material.map_Ns = %s\n", materials[i].normal_texname.c_str());
		   std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
		   std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
		   for (; it != itEnd; it++) {
			   printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
		   }
		   printf("\n");
	   }
   }



}

void drawdata()
{

	
glColor3ub(255,0,0);

glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_INDEX_ARRAY);
  //glEnableClientState(GL_NORMAL_ARRAY);

  for(unsigned int i=0; i< shapes.size(); i++)
  {
	  glVertexPointer(3, GL_FLOAT, 0, &shapes[i].mesh.positions[0]);
	  glIndexPointer(GL_UNSIGNED_INT,0,&shapes[i].mesh.indices[0]);
	  glDrawArrays(GL_TRIANGLES,0,shapes[i].mesh.indices.size());
	  
	}
	
	
	
	
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
    printf("[3] ��� [t] ��� [T] - ���������������� ����\r\n" );
    printf("[4] ��������/��������� ���������� ����������\r\n" );
    
    
    
	//auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSELOC, mouse_left_move);   // ����� ������������ ����� ���������� �� ����
	auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEDOWN, mouse_left_down);
    auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEUP, mouse_left_up);
           
    auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEDOWN, mouse_right_down);	
	auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEUP, mouse_right_up);
	
	// ��� ������� �������� - ����� ���������� � ��� ��������� �������, � ��� �������.
	// ���� ��� ������� ������, ����� � ������������ ����� ������� ������
	auxMouseFunc(NULL, AUX_MOUSELOC, mouse_move);         
	
	
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
    
    auxKeyFunc('t', Textura_use);
	auxKeyFunc('T', Textura_use);
    auxKeyFunc('3', Textura_use);
    auxKeyFunc('4', texture_mode);


// ������ ���� OpenGL
	windowW = 600;
	windowH = 600;

// ������������ ���� OpenGL �� ������
	auxInitPosition(100, 100, windowW, windowH);

// ��������� �������� ���������� ������ OpenGL
// �������� ����� RGB | ��������� Z-������ ��� ���������� �� �������

	auxInitDisplayMode( AUX_RGB | AUX_DEPTH | AUX_DOUBLE );

// ������������� ���� OpenGL � ���������� Title
	if(auxInitWindow("LABA2") ==GL_FALSE) auxQuit();
	
	
		const char* n;
	n=(char*)glGetString(GL_VERSION);
	printf("%s\r\n", n);


	loaddata();


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
	  glClearColor(0.70f,0.70f,1.0f,1.0f) ;

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

void DrawModel (void)
{
// �������������� ������ �������� ��� �
// 3 ������� ����� ������������ ����


  glColor3f( 0.5f,0.5f,0.5f); 

  GLfloat amb[] = {0.2,0.2,0.1,1.}; 
  GLfloat dif[] = {0.4,0.65,0.5,1.};
  GLfloat spec[] = {0.9,0.8,0.3,1.};
  GLfloat sh= 0.1f*256; 

  
  glMaterialfv(GL_FRONT,GL_AMBIENT,amb);
  glMaterialfv(GL_FRONT,GL_DIFFUSE,dif);
  glMaterialfv(GL_FRONT,GL_SPECULAR,spec);
  glMaterialf(GL_FRONT,GL_SHININESS,sh);
  
glDisable(GL_BLEND);
  glShadeModel(GL_SMOOTH);
  auxSolidSphere(1.0);
  glShadeModel(GL_FLAT) ; 
  auxSolidTorus (0.5,1.5);


if(plosk_smooth)
 glShadeModel(GL_SMOOTH);
else
 glShadeModel(GL_FLAT) ; 
  //Ground
glNormal3f(0,0,1);
glBegin(GL_QUADS);
int i,j;
for(i=-10; i<20;i++)
 for(j=-10;j<20;j++)
 {
  glVertex3f(i,j,15);
  glVertex3f(i+1,j,15);
   glVertex3f(i+1,j+1,15);
  glVertex3f(i,j+1,15);

  }
 glEnd();


  if(mode)
 { glEnable(GL_TEXTURE_2D);
   // ������ �������������
   // ������� ���������� ��������� ������� �� � �������
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   glCallList(Item1);
 glTranslatef(-15,-15,0);
   // ������ ���������
   // ���� �������� �������� ������������
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glCallList(Item1+1);

   glEnable(GL_BLEND);
// ��������� ��������� � ���������
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
 glPushMatrix();
int i;
 glTranslatef(30,0,0);

// ���� �������� ���������� �������������� �������������� ���������
glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
 //������ �������� 6 ��� �������� ���������
   for(i=0;i<=6;i++)
   {
   glScalef(1.1-i*0.1,1.1-i*0.1,0.8);
   glTranslatef(0,0,5*i);
   glCallList(Item1+1);
    }
   glPopMatrix();
   
  }
  
  {
  glPushMatrix();
  glTranslatef(0,0,15);
  glRotatef(90,0,-1,0);
    drawdata();
  glPopMatrix();
  }
  
/*// ��������� ����� ����� ������������, ����� ���������� ��� ����������� �����
     glDisable(GL_LIGHTING); 

//OX RED
  glBegin(GL_LINES);

  glColor3f( 1.0f,0.0f,0.0f);
  glNormal3f(0., 0., 1.);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(3.0f,0.0f,0.0f); 
  glEnd();

// OY GREEN
  glBegin(GL_LINES);
  glColor3f( 0.0f,1.0f,0.0f);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(0.0f,3.0f,0.0f); 
  glEnd();

// OZ BLUE
  glBegin(GL_LINES);
  glColor3f( 0.0f,0.0f,1.0f);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(0.0f,0.0f,3.0f); 
  glEnd();
  */
}
