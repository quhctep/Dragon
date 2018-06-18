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

// ключевое слово extern информирует компиллятор,
// что это внешние переменные и их значение доступно другим файлам
// используется внутри GL_movement

extern GLfloat alX=0, alY=0; // угол поворота

// начальные данные о положении и совершенном перемещении мыши
extern int lx0=0,lx1=0,ly0=0,ly1=0; // левая клавиша
extern int rx0=0,rx1=0,ry0=0,ry1=0; // правая клавиша
extern int dveX0=0,dveY0=0,dveX1=0,dveY1=0; //две клавиши нажатые вместе

/*
 предопредленый угол зрения (FOV -Field of View) в перспективном режиме
 или величина приближения(в абсолютных единицах) в ортогональном режиме
 для приближения камеры(увеличения pzoom) удобно вызывать zoom(+3);
 для отдаления камеры(уменьшения pzoom) удобно вызывать zoom(-3);
 */
extern int pzoom=60; 
 
// тип используемой камеры по умолчанию
// 1 - ортогональная 2-перспективная                        
extern int view=1;

extern bool m1=false; // нажата ли левая клавиша мыши
extern bool m2=false; // нажата ли правая клавиша мыши

// ининциализация глобавльных переменных
extern GLdouble w=900,h=900,px=0,py=0;

extern MyPoint e(10,20,10);
extern MyPoint c(0,0,0);
extern MyPoint u(0,0,1);

MyPoint Light_pos(10,0,40);
bool plosk_smooth=true; // сглаживать изображение плоскости?


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

// указатель на текстуру
GLubyte *resImage;
GLubyte *rImage;
GLubyte *resImage1;
GLubyte *resImage4;
// номер текстуры
GLuint texture1;
GLuint texture2;
GLuint texture3;
GLuint texture4;

// переменная для списка отображения
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
// функция задания свойст источников освещения
void InitLight(void)
 {
 
  GLfloat amb[] = {0.1,0.1,0.1,1.};
  GLfloat dif[] = {1.,1.,1.,1.};
  GLfloat spec[] = {1.,1.,1.,1.};
  GLfloat pos[] = {Light_pos.x,Light_pos.y,Light_pos.z,1.};

// параметры источника света
// позиция источника
  glLightfv(GL_LIGHT0, GL_POSITION, pos);



// характеристики излучаемого света
// фоновое освещение (рассеянный свет)
  glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
// диффузная составляющая света
  glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
// зеркально отражаемая составляющая света
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


// функции обработки событий

//extern GLfloat alX=0, alY=0; // угол поворота
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

// действие по нажатию левой кнопки мыши
void CALLBACK mouse_left_down(AUX_EVENTREC *event)
{   
   m1=true;                           // дать пометку, что левая кнопка была нажата
   lx0 = event->data[AUX_MOUSEX];    // сохранить х координату мышки
   ly0 = event->data[AUX_MOUSEY];   // сохранить y координату мышки
   
   if ((m1) && (m2))                         // если обе(и правая и левая) кнопки мыши были нажаты
    {                                       // то сохранить х и y координаты мыши в отдельные переменные,
      dveX0 = event->data[AUX_MOUSEX];     //  отвечающие за действие приближения (в орто проекции)  
      dveY0 = event->data[AUX_MOUSEY];    //  или  изменения угла зрения в перспективной проекции  
                                         
    }

}
 // действие по событию отпускания левой кнопки мыши
void CALLBACK mouse_left_up(AUX_EVENTREC *event)
{
    m1=false;   
}


// действие по событию нажатия правой кнопки
void CALLBACK mouse_right_down(AUX_EVENTREC *event)
{
    m2=1;                             // дать пометку, что правая кнопка была нажата
    rx0 = event->data[AUX_MOUSEX];   // сохранить х координату мышки
    ry0 = event->data[AUX_MOUSEY];  // сохранить y координату мышки
    
    if ((m1) && (m2))
    {                                          // если обе(и правая и левая) кнопки мыши были нажаты
     dveX0 = event->data[AUX_MOUSEX];         // то сохранить х и y координаты мыши в отдельные переменные,
     dveY0 = event->data[AUX_MOUSEY];        //  отвечающие за действие приближения (в орто проекции)  
    }                                       //  или  изменения угла зрения в перспективной проекции 
}

// действие по событию отпускания правой кнопки мыши
void CALLBACK mouse_right_up(AUX_EVENTREC *event)
{m2=0;
}

// общий обработчик вызывающийся при движении мыши
void CALLBACK mouse_move(AUX_EVENTREC *event)
{

 if((m1)&&(m2))                                  // а не было ли жвижения мышки с нажатыми обоими кнопками?
    {dveX1=event->data[AUX_MOUSEX]-dveX0;
     dveY1 = event->data[AUX_MOUSEY]-dveY0;
     
     zoom((int)(dveX1+dveY1));                  // если да то применить приближение/изменения угла зрения

     dveX0 = event->data[AUX_MOUSEX];
     dveY0 = event->data[AUX_MOUSEY];  
     return;
    }
 else
    {
        if (m1)                                        // может была нажата левая кнопка мыши?
            {  lx1 = event->data[AUX_MOUSEX]-lx0;
               ly1 = event->data[AUX_MOUSEY]-ly0;
                    
                    jamp(-ly1,lx1);                  // тогда сдвинуться вверх и влево(по перемещению) 
                    
               lx0 = event->data[AUX_MOUSEX];
               ly0 = event->data[AUX_MOUSEY];
            }
        else if(m2)                                  // или может была нажата правая кнопка мыши?
        {
            
            rx1 = event->data[AUX_MOUSEX]-rx0;
            ry1 = event->data[AUX_MOUSEY]-ry0;
                
                look_around(-rx1/10.0,ry1/30.0);              // тогда повернуть камеру
                
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
    if(view==2) // если в перспективе
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

	//Выделяем память под наше изображение
	rImage = new unsigned char[srcImage->sizeX * srcImage->sizeY * 4];

	//Процессим изображение (циклы по линиям и по столбцам)
	for (i = 0; i<srcImage->sizeY; i++)
	{
		for (j = 0; j<srcImage->sizeX; j++)
		{
			//Переприсваиваем в нашем изображении цветовые значения исходного

			rImage[i*srcImage->sizeX * 4 + j * 4] = srcImage->data[i*srcImage->sizeX * 3 + j * 3];
			rImage[i*srcImage->sizeX * 4 + j * 4 + 1] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 1];
			rImage[i*srcImage->sizeX * 4 + j * 4 + 2] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 2];

			// если цвет их всех равен 0, то сделать альфа компонент =0;
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

	//Выделяем память под наше изображение
	resImage=new unsigned char[srcImage->sizeX * srcImage->sizeY * 4];

	//Процессим изображение (циклы по линиям и по столбцам)
	for (i=0;i<srcImage->sizeY;i++)
	{
		for(j=0;j<srcImage->sizeX;j++)
		{
			//Переприсваиваем в нашем изображении цветовые значения исходного
			
			resImage[i*srcImage->sizeX*4+j*4]=srcImage->data[i*srcImage->sizeX*3+j*3];
			resImage[i*srcImage->sizeX*4+j*4+1]=srcImage->data[i*srcImage->sizeX*3+j*3+1];
			resImage[i*srcImage->sizeX*4+j*4+2]=srcImage->data[i*srcImage->sizeX*3+j*3+2];			
			
			// если цвет их всех равен 0, то сделать альфа компонент =0;
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

	//Выделяем память под наше изображение
	resImage1 = new unsigned char[srcImage->sizeX * srcImage->sizeY * 4];

	//Процессим изображение (циклы по линиям и по столбцам)
	for (i = 0; i<srcImage->sizeY; i++)
	{
		for (j = 0; j<srcImage->sizeX; j++)
		{
			//Переприсваиваем в нашем изображении цветовые значения исходного

			resImage1[i*srcImage->sizeX * 4 + j * 4] = srcImage->data[i*srcImage->sizeX * 3 + j * 3];
			resImage1[i*srcImage->sizeX * 4 + j * 4 + 1] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 1];
			resImage1[i*srcImage->sizeX * 4 + j * 4 + 2] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 2];

			// если цвет их всех равен 0, то сделать альфа компонент =0;
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

	//Выделяем память под наше изображение
	resImage4 = new unsigned char[srcImage->sizeX * srcImage->sizeY * 4];

	//Процессим изображение (циклы по линиям и по столбцам)
	for (i = 0; i<srcImage->sizeY; i++)
	{
		for (j = 0; j<srcImage->sizeX; j++)
		{
			//Переприсваиваем в нашем изображении цветовые значения исходного

			resImage4[i*srcImage->sizeX * 4 + j * 4] = srcImage->data[i*srcImage->sizeX * 3 + j * 3];
			resImage4[i*srcImage->sizeX * 4 + j * 4 + 1] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 1];
			resImage4[i*srcImage->sizeX * 4 + j * 4 + 2] = srcImage->data[i*srcImage->sizeX * 3 + j * 3 + 2];

			// если цвет их всех равен 0, то сделать альфа компонент =0;
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
		// устанавливаем формат хранения пикселей
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		//  Команды glGenTextures() и glBindTexture() именуют и создают текстурный объект для изображения текстуры.
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		// устанавливаем параметры повторения на краях
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// параметры отображения текстур если происходит уменьшение или увеличение текстуры
		//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


		// даем команду на пересылку текстуры в память видеокарты
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth, resImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, resImage);


		// способ наложения текстуры GL_MODULATE, GL_DECAL, and GL_BLEND.
		// GL_MODULATE - умножение на цвет от освещения
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		//      glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
		glBindTexture(GL_TEXTURE_2D, texture1);


		//Fountain
		glGenTextures(1, &texture2);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, texture2);	// устанавливаем параметры повторения на краях
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //было GL_LINEAR
		// даем команду на пересылку текстуры в память видеокарты
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth2, resImageHeight2, 0, GL_RGBA, GL_UNSIGNED_BYTE, rImage);
		// GL_MODULATE - умножение на цвет от освещения

		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glGenTextures(1, &texture3);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, texture3);	// устанавливаем параметры повторения на краях
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //было GL_LINEAR
		// даем команду на пересылку текстуры в память видеокарты
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth3, resImageHeight3, 0, GL_RGBA, GL_UNSIGNED_BYTE, resImage1);
		// GL_MODULATE - умножение на цвет от освещения

		//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, texture3);

		glGenTextures(1, &texture4);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, texture4);	// устанавливаем параметры повторения на краях
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //было GL_LINEAR
		// даем команду на пересылку текстуры в память видеокарты
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resImageWidth4, resImageHeight4, 0, GL_RGBA, GL_UNSIGNED_BYTE, resImage4);
		// GL_MODULATE - умножение на цвет от освещения

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


// регистрация функций обработки событий от мыши и от клавиатуры
    setlocale( LC_CTYPE, "Russian" );
    printf("Вывод текста на русском языке возможен\r\n" );
    printf("Управление WASD - только в английской раскладке\r\n" );
    
    
    printf("Управление:\r\n" );
    printf("[Пробел] - переключение камеры:\r\n" );
    printf("[Клавиша Влево] или [A] или [a] - Двигаться влево\r\n" );
    printf("[Клавиша Вниз]  или [D] или [D] - Двигаться вправо\r\n" );
    
    printf("[Клавиша Верх] или [W] или [w] - Двигаться вперед(перспектива) или вверх(орто)\r\n" );
    printf("[Клавиша Вниз] или [S] или [s] - Двигаться назад(перспектива) или вниз(орто)\r\n\n\n" );
    
    
    
    printf("[1] или [l] или [L] - В режиме перспектива\n - установить источник освещения в месте глаз\r\n" );
    printf("[2] или [p] или [P] - Поменять мат модель расчета освещения\r\n" );
    printf("[4] включить/выключить билинейную фильтрацию\r\n" );
    

    
	//auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSELOC, mouse_left_move);   // будем использовать общий обработчик см ниже
	auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEDOWN, mouse_left_down);
    auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEUP, mouse_left_up);
           
    auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEDOWN, mouse_right_down);	
	auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEUP, mouse_right_up);
	
	// для события смещения - общий обработчик и при ненажатых кнопках, и при нажатых.
	// один для нажатия правой, левой и одновременно обоих нажатых кнопок
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

// размер окна OpenGL
	windowW = 700;
	windowH = 700;

// расположение окна OpenGL на экране
	auxInitPosition(100, 100, windowW, windowH);

// установка основных параметров работы OpenGL
// цветовой режим RGB | включение Z-буфера для сортировки по глубине

	auxInitDisplayMode( AUX_RGB | AUX_DEPTH | AUX_DOUBLE );

// инициализация окна OpenGL с заголовком Title
	if(auxInitWindow("Курсовая работа. Кузьмин Александр Александрович. ИДБ-13-11") ==GL_FALSE) auxQuit();
	
	


	Textura_use();

	loaddata();
	loadFountain();
	loadGround();

// Инициализация источников света
// если вне видового преобразования, то источник ориентирован относительно 
// наблюдателя, т.е. он будет фиксирован в системе координат, 
// XOY которой связана с экраном. 
// В нашем примере источник с pos[] = {0.,0.,1.,0.}; светит перпендикулярно экрану
   InitLight();

// регистрация функции, которая вызывается при изменении размеров окна
//  Resize() - функция пользователя
	auxReshapeFunc((AUXRESHAPEPROC)Resize);

// регистрация функции, которая вызывается при перерисовке 
// и запуск цикла обработки событий
// Draw() - функция пользователя
	auxIdleFunc(Draw);
	auxMainLoop(Draw);

}



static void CALLBACK Resize(int width, int height) // создается пользователем
{ 
//1. получение текущих координат окна 


	w = width;
	h = height;
// сохраняем размеры окна вывода w,h  
    if(h == 0)
    	h = 1;
// установка новых размеров окна вывода
glViewport(0,0,w,h);


// в ней мы установим тип проекции (ортогональная или перспективная)
    zoom(0);

}



static void CALLBACK Draw(void) // создается пользователем
{
// 1. очистка буферов (цвета, глубины и возможно др.)

  // установка цвета фона
	  glClearColor(0.0f,0.0f,0.0f,1.0f) ;


  // очистка всех буферов
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//2.установка режимов рисования
  
  // нормализация нормалей : их длины будет равна 1
  glEnable(GL_NORMALIZE);

  // включение режима сортировки по глубине
  glEnable(GL_DEPTH_TEST) ;


  
  glEnable(GL_LINE_SMOOTH); // устранение ступенчатости для линий
  glEnable(GL_POINT_SMOOTH);
  
  // для качественного устранения ступенчатости 
  // нам надо включить режим альфа-наложения(альфа смешения BLEND - смешивать)
  glEnable(GL_BLEND);
  
  // Настройка альфа сглаживания:
  glBlendFunc (GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);

  // тип рисования полигонов
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // первый параметр: рисовать все грани, или только внешние или внутренние
  // GL_FRONT_AND_BACK , GL_FRONT , GL_BACK  
  // второй: как рисовать 
  // GL_POINT (точки) GL_LINE(линии на границе), GL_FILL(заполнять)
  

  //
  // тип закраски полигонов
    glShadeModel(GL_FLAT) ; 
  // GL_FLAT все пикселы полигона имеюют одинаковый цвет ( за цвет принимается 
  // цвет первой обрабатываемой вершины, если включены источники, 
  // то этот цвет модифицируется с учетом источников и нормали в вершине)
  // GL_SMOOTH цвет каждого пиксела рассчитывается интерполяцией цветов вершин 
  // с учетом источников света. если они включены
  
  //  включить режим учета освещения
     glEnable(GL_LIGHTING); 

  //   задать параметры освещения
  //  параметр GL_LIGHT_MODEL_TWO_SIDE - 
  //                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
  //                1 - лицевые и изнаночные обрабатываются разными режимами       
  //                соответственно лицевым и изнаночным свойствам материалов.    
  //  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
  //                не зависящее от сточников
  // по умолчанию (0.2, 0.2, 0.2, 1.0)
      
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0 ); 

  
  //  включить нужное количество источников 
   glEnable(GL_LIGHT0);

  // new!  автоматического задания свойств материала 
  // функцией glColor() не используем!!!
  // glEnable(GL_COLOR_MATERIAL);
  // glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
 
// 3.установка видового преобразования

// выбор видовой матрицы в качестве текущей

  glMatrixMode( GL_MODELVIEW );

// Сохранение текущего значения матрицы в стеке
  //glPushMatrix();

// загрузка единичной матрицы
  glLoadIdentity();

// установка точки наблюдения

 gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );

 InitLight();
// если здесь инициализировать источник света
// то он будет зафиксирован в мировых координатах, т.е.
// при poition(0.,0.,1.,0.) он  будет светить вдоль
//  мировой оси Z ( в нашем примере синяя ось)

// преобразования объектов
// (угол поворота, меняется машью или клавиатурой)
// glRotatef(alX,1.0f,0.0f, 0.0f);
// glRotatef(alY,0.0f,1.0f, 0.0f);

//new!  InitLight();
// если здесь инициализировать источник, то он будет вращается 
//вместе с объектом. В нашем случае при позиции источника 
// pos[] = {0.,0.,1.,0.} при любых вращениях ярко освещена будет 
//только синяя грань

// 4. вызов модели
  DrawModel() ;

// 5. завершение видового преобразования
  // Restore the view matrix
//  glPopMatrix();

//6. Завершить все вызванные на данный момент операции OpenGL
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
		// настройти приемника и источника
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

		//кривые Безье

		float division = 1 / (float)prec;
		int j = 0;
		CalcBezie1();
		CalcBezie2();
		CalcBezie3();
		CalcBezie4();
		CalcBezie5();
		CalcBezie6();
		CalcBezie7();

		//Капли

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

		//Вода
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