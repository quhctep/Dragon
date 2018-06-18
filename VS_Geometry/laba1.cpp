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

extern MyPoint e(6,20,10);
extern MyPoint c(0,0,0);
extern MyPoint u(0,0,1);



double Ugol=0;
bool up=0;
int resImageWidth, resImageHeight;


static void CALLBACK Resize(int width, int height);
static void CALLBACK Draw(void);

void  DrawModel(void);

GLint windowW, windowH;

std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;




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
  jamp(0,5);
}
void CALLBACK Key_RIGHT(void)
{
   jamp(0,-5);
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

void loaddata()
{
  const char* filename="ss.obj";
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
          printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3*f+0], shapes[i].mesh.indices[3*f+1], shapes[i].mesh.indices[3*f+2], shapes[i].mesh.material_ids[f]);
        }

        printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
        assert((shapes[i].mesh.positions.size() % 3) == 0);
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
          printf("  v[%ld] = (%f, %f, %f)\n", v,
            shapes[i].mesh.positions[3*v+0],
            shapes[i].mesh.positions[3*v+1],
            shapes[i].mesh.positions[3*v+2]);
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
//
//glEnableClientState(GL_VERTEX_ARRAY);
//  glEnableClientState(GL_INDEX_ARRAY);
//  //glEnableClientState(GL_NORMAL_ARRAY);
//
//  for(unsigned int i=0; i< shapes.size(); i++)
//  {
//	  glVertexPointer(3, GL_FLOAT, 0, &shapes[i].mesh.positions[0]);
//	  glIndexPointer(GL_UNSIGNED_INT,0,&shapes[i].mesh.indices[0]);
//	  glDrawArrays(GL_TRIANGLES,0,shapes[i].mesh.indices.size());
//	  
//	}
//	
	
	
	
for(unsigned int i=0; i< shapes.size(); i++)
  {
  if(shapes[i].mesh.normals.size())
  {
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
  }
  
  glBegin(GL_TRIANGLES);
   for(int k=0;k< shapes[i].mesh.indices.size();k+=1)
   {
        if(shapes[i].mesh.normals.size())
            glNormal3f(   
                        shapes[i].mesh.normals[shapes[i].mesh.indices[k]*3+0],
                        shapes[i].mesh.normals[shapes[i].mesh.indices[k]*3+0],
                        shapes[i].mesh.normals[shapes[i].mesh.indices[k]*3+0]);
   
   
   
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
    
	

// ������ ���� OpenGL
	windowW = 600;
	windowH = 600;

// ������������ ���� OpenGL �� ������
	auxInitPosition(100, 100, windowW, windowH);

// ��������� �������� ���������� ������ OpenGL
// �������� ����� RGB | ��������� Z-������ ��� ���������� �� �������

	auxInitDisplayMode( AUX_RGB | AUX_DEPTH | AUX_DOUBLE );

// ������������� ���� OpenGL � ���������� Title
	if(auxInitWindow("Laba1") ==GL_FALSE) auxQuit();

		const char* n;
	n=(char*)glGetString(GL_VERSION);
	printf("%s\r\n", n);


	loaddata();



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
	  glClearColor(0.75f,0.75f,0.75f,1.0f) ;

  // ������� ���� �������
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//2.��������� ������� ���������
  
  // ������������ �������� : �� ����� ����� ����� 1
  glEnable(GL_NORMALIZE);

  // ��������� ������ ���������� �� �������
  glEnable(GL_DEPTH_TEST) ;

  // ��� ��������� ���������
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // ������ ��������: �������� ��� �����, ��� ������ ������� ��� ����������
  // GL_FRONT_AND_BACK , GL_FRONT , GL_BACK  
  // ������: ��� �������� 
  // GL_POINT (�����) GL_LINE(����� �� �������), GL_FILL(���������)
  

  
// 3.��������� �������� ��������������

// ����� ������� ������� � �������� �������

  glMatrixMode( GL_MODELVIEW );

// ���������� �������� �������� ������� � �����
  //glPushMatrix();

// �������� ��������� �������
  glLoadIdentity();

// ��������� ����� ����������

 gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );

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

void Ugol_update()
{
	if(up)
	  Ugol+=6;
	else
	  Ugol-=6;
	if (Ugol>=30)
	  up=false;
	if (Ugol<=-70)
	  up=true;
}

void DrawModel (void)
{

	Ugol_update();
//axes  ��� ����������

//OX RED
  glBegin(GL_LINES);
  glColor3f( 1.0f,0.0f,0.0f);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(30.0f,0.0f,0.0f);
  glEnd();

// OY GREEN
  glBegin(GL_LINES);
  glColor3f( 0.0f,1.0f,0.0f);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(0.0f,30.0f,0.0f);
  glEnd();

// OZ BLUE
  glBegin(GL_LINES);
  glColor3f( 0.0f,0.0f,1.0f);
  glVertex3f(0.0f,0.0f,0.0f);
  glVertex3f(0.0f,0.0f,30.0f);
  glEnd();


drawdata();

//Ground


/*
 glNormal3f(0,0,1);
 glBegin(GL_QUADS);
// ������ glBegin ����� ������������ :
//  GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_LINE_LOOP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS, GL_QUAD_STRIP, and GL_POLYGON.
//  glBegin(GL_LINE_STRIP);





  glVertex3d(-9.5,-9.5,-0.05);
  glVertex3d(20.5,-9.5,-0.05);
  glVertex3d(20.5,20.5,-0.05);
  glVertex3d(-9.5,20.5,-0.05);

  glEnd();



 glNormal3f(0,0,1);
 glBegin(GL_LINE_STRIP);
 glColor3f( 0.6f,0.8f,0.8f);

int i,j;
for(i=-10; i<20;i++)
 for(j=-10;j<20;j++)
 {
  glVertex3d(i+0.5,j+0.5,0.05);
  glVertex3d(i+1+0.5,j+0.5,0.05);
  glVertex3d(i+1+0.5,j+1+0.5,0.05);
   }
  glEnd();







// ��������� ������� � ����
glPushMatrix();
// ��������� ���������� �� 3 �� ��� Z
glTranslatef(10,5,3);

// ������ �������
// ������
 glColor3f( 0.f,0.f,0.f);
 glLineWidth(5);
 glBegin(GL_LINES);
 glVertex3f(0,-1,0);
 glVertex3f(0,8,0);
  glEnd();
 glLineWidth(1);

// �������� ������� ����� �� ���� ����� ������� 0,1,0
glRotatef(Ugol,0,1,0);


// ����� �������
glNormal3f(0,0,1);
// ������� �����
 glColor3f( 0.9f,0.4f,0.0f);
 glBegin(GL_POLYGON);
 glVertex3f(0,0,0);
 glVertex3f(4,-2,0);
 glVertex3f(7,0,0);
 glVertex3f(5,4,0);
 glVertex3f(0,4,0);
 glEnd();
// ������ �����
  glBegin(GL_POLYGON);
  glColor3f( 0.8f,0.5f,0.0f);
 glVertex3f(0,5,0);
 glVertex3f(4,4,0);
 glVertex3f(5,7,0);
 glVertex3f(4,9,0);
 glVertex3f(2,9,0);
 glVertex3f(0,7,0);
  glEnd();

// ������ �������

// �������� ���������� � ��� ���� ������ �����
glRotatef((-Ugol)*2,0,1,0);
// ��������� �������� � ����
 glPushMatrix();

// ��������� ������� ������������ � �������� ��������� �� ��� Y
glScalef(-1,1,-1);


glColor3f( 0.9f,0.4f,0.0f);
// ������� �����
  glBegin(GL_POLYGON);
 glVertex3f(0,0,0);
 glVertex3f(4,-2,0);
 glVertex3f(7,0,0);
 glVertex3f(5,4,0);
 glVertex3f(0,4,0);
 glEnd();
// ������ �����
  glBegin(GL_POLYGON);
  glColor3f( 0.8f,0.5f,0.0f);
 glVertex3f(0,5,0);
 glVertex3f(4,4,0);
 glVertex3f(5,7,0);
 glVertex3f(4,9,0);
 glVertex3f(2,9,0);
 glVertex3f(0,7,0);
  glEnd();

  glPopMatrix();
  glPopMatrix();
*/
}
