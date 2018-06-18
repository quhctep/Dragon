#include <windows.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

#include "GL_movement.h"   // здесть удобный класс 3д точки и действия с камерой

#include <locale.h>  // для русского языка   см main
#include <stdio.h> 

#include "tiny_obj_loader.h"

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <fstream>



// меня замучали предупреждения о преобразованиях double во float так что будем игнорировать эти предупреждения

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

MyPoint Light_pos(10,0,25);
bool plosk_smooth=true; // сглаживать изображение плоскости?



// указатель на текстуру
GLubyte *resImage;
// номер текстуры
GLuint texture1;

// переменная для списка отображения
GLuint Item1;

int resImageWidth, resImageHeight;

int mode=0;

static void CALLBACK Resize(int width, int height);
static void CALLBACK Draw(void);

void  DrawModel(void);

GLint windowW, windowH;

std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;


// функция задания свойст источников освещения
void InitLight(void)
 {
 
  GLfloat amb[] = {0.1,0.1,0.1,1.};
  GLfloat dif[] = {1.,1.,1.,1.};
  GLfloat spec[] = {3.,3.,3.,3.};
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
  glColor3ub(255, 255, 0);
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


void loadImage()
{
	int i,j;  
	AUX_RGBImageRec* srcImage;
	srcImage=auxDIBImageLoad("mramor.bmp");
	
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

void CALLBACK Textura_use(void)
{
    loadImage();

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
      glBindTexture(GL_TEXTURE_2D,texture1);

// создадим два новых списка отображения
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
// опереатором *= я переопределил векторное умнежение векторов
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

//Рисуем пирамидку
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
    printf("[3] или [t] или [T] - Текстурированная Елка\r\n" );
    printf("[4] включить/выключить билинейную фильтрацию\r\n" );
    
    
    
	//auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSELOC, mouse_left_move);   // будем использовать общий обработчик см ниже
	auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEDOWN, mouse_left_down);
    auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEUP, mouse_left_up);
           
    auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEDOWN, mouse_right_down);	
	auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEUP, mouse_right_up);
	
	// для события смещения - общий обработчик и при ненажатых кнопках, и при нажатых.
	// один для нажатия правой, левой и одновременно обоих нажатых кнопок
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


// размер окна OpenGL
	windowW = 600;
	windowH = 600;

// расположение окна OpenGL на экране
	auxInitPosition(100, 100, windowW, windowH);

// установка основных параметров работы OpenGL
// цветовой режим RGB | включение Z-буфера для сортировки по глубине

	auxInitDisplayMode( AUX_RGB | AUX_DEPTH | AUX_DOUBLE );

// инициализация окна OpenGL с заголовком Title
	if(auxInitWindow("LABA2") ==GL_FALSE) auxQuit();
	
	
		const char* n;
	n=(char*)glGetString(GL_VERSION);
	printf("%s\r\n", n);


	loaddata();


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
	  glClearColor(0.70f,0.70f,1.0f,1.0f) ;

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

void DrawModel (void)
{
// геометрическая модель включает шар и
// 3 отрезка вдоль координатных осей


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
   // рисуем прямоугольник
   // формулу вычисления освещения пикселя см в справке
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   glCallList(Item1);
 glTranslatef(-15,-15,0);
   // рисуем пирамидку
   // цвет текстуры заменяет освещенность
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glCallList(Item1+1);

   glEnable(GL_BLEND);
// настройти приемника и источника
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
 glPushMatrix();
int i;
 glTranslatef(30,0,0);

// цвет текстуры умножается модифицируется интенсивностью освещения
glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
 //елочка рисуется 6 раз вывается пирамидка
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
  
/*// отключаем режим учета освещенности, чтобы нарисовать оси неизменного цвета
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
