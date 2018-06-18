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
  jamp(0,5);
}
void CALLBACK Key_RIGHT(void)
{
   jamp(0,-5);
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
    
	

// размер окна OpenGL
	windowW = 600;
	windowH = 600;

// расположение окна OpenGL на экране
	auxInitPosition(100, 100, windowW, windowH);

// установка основных параметров работы OpenGL
// цветовой режим RGB | включение Z-буфера для сортировки по глубине

	auxInitDisplayMode( AUX_RGB | AUX_DEPTH | AUX_DOUBLE );

// инициализация окна OpenGL с заголовком Title
	if(auxInitWindow("Laba1") ==GL_FALSE) auxQuit();

		const char* n;
	n=(char*)glGetString(GL_VERSION);
	printf("%s\r\n", n);


	loaddata();



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
	  glClearColor(0.75f,0.75f,0.75f,1.0f) ;

  // очистка всех буферов
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

//2.установка режимов рисования
  
  // нормализация нормалей : их длины будет равна 1
  glEnable(GL_NORMALIZE);

  // включение режима сортировки по глубине
  glEnable(GL_DEPTH_TEST) ;

  // тип рисования полигонов
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // первый параметр: рисовать все грани, или только внешние или внутренние
  // GL_FRONT_AND_BACK , GL_FRONT , GL_BACK  
  // второй: как рисовать 
  // GL_POINT (точки) GL_LINE(линии на границе), GL_FILL(заполнять)
  

  
// 3.установка видового преобразования

// выбор видовой матрицы в качестве текущей

  glMatrixMode( GL_MODELVIEW );

// Сохранение текущего значения матрицы в стеке
  //glPushMatrix();

// загрузка единичной матрицы
  glLoadIdentity();

// установка точки наблюдения

 gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );

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
//axes  Оси кооординат

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
// внутри glBegin можно использовать :
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







// созраняем матрицу в стек
glPushMatrix();
// поднимаем коондинаты на 3 по оси Z
glTranslatef(10,5,3);

// Рисуем бабочку
// тельце
 glColor3f( 0.f,0.f,0.f);
 glLineWidth(5);
 glBegin(GL_LINES);
 glVertex3f(0,-1,0);
 glVertex3f(0,8,0);
  glEnd();
 glLineWidth(1);

// повернем будущее крыло на УГОЛ вдоль вектора 0,1,0
glRotatef(Ugol,0,1,0);


// Левая сторона
glNormal3f(0,0,1);
// Вернеее крыло
 glColor3f( 0.9f,0.4f,0.0f);
 glBegin(GL_POLYGON);
 glVertex3f(0,0,0);
 glVertex3f(4,-2,0);
 glVertex3f(7,0,0);
 glVertex3f(5,4,0);
 glVertex3f(0,4,0);
 glEnd();
// нижнее крыло
  glBegin(GL_POLYGON);
  glColor3f( 0.8f,0.5f,0.0f);
 glVertex3f(0,5,0);
 glVertex3f(4,4,0);
 glVertex3f(5,7,0);
 glVertex3f(4,9,0);
 glVertex3f(2,9,0);
 glVertex3f(0,7,0);
  glEnd();

// Правая сторона

// повернем координаты в два раза больше назад
glRotatef((-Ugol)*2,0,1,0);
// Сохраняем мактрицу в стек
 glPushMatrix();

// применяем функцию растягивания в качестве отражения по оси Y
glScalef(-1,1,-1);


glColor3f( 0.9f,0.4f,0.0f);
// верхнее крыло
  glBegin(GL_POLYGON);
 glVertex3f(0,0,0);
 glVertex3f(4,-2,0);
 glVertex3f(7,0,0);
 glVertex3f(5,4,0);
 glVertex3f(0,4,0);
 glEnd();
// нижнее крыло
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
