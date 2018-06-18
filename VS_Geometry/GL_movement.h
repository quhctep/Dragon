#ifndef GL_movement
#define GL_movement

// ���� �������� �������������� � ��������������� double �� float ��� ��� ����� ������������ ��� ��������������
#pragma warning(disable : 4244)     // MIPS

extern GLfloat alX, alY; // ���� ��������
extern int lx0,lx1,ly0,ly1,rx0,rx1,ry0,ry1,pzoom,dveX0,dveY0,dveX1,dveY1;
extern  int view,mouse_mode;
extern bool m2,m1;
extern GLdouble w,h,px,py;

class MyPoint{
public:
float x,y,z;

MyPoint ()
{x=0;
 y=0;
 z=0;
}


MyPoint (GLdouble XX,GLdouble YY,GLdouble ZZ)
{x=XX;
 y=YY;
 z=ZZ;
}

MyPoint(MyPoint &ss) // ����������� �����
{x=ss.x;
 y=ss.y;
 z=ss.z;
}

MyPoint & operator =(MyPoint  ss) //����� ������ ����� ������ (���� �� ����������� ���������)
{x=ss.x;
 y=ss.y;
 z=ss.z;
 return *this;
}

MyPoint & Set(float XX, float YY, float ZZ)
{x=XX;
 y=YY;
 z=ZZ;
 return *this;
}

double  Dlina()                      // ������ �������
{try{return sqrt(x*x+y*y+z*z);}
 catch(...)
 {}
}

MyPoint  operator+( const MyPoint & ss)     // ����� ��������
{return MyPoint(x+ss.x,y+ss.y,z+ss.z);
}

MyPoint  operator-( const MyPoint & ss)    // �������� ��������
{return MyPoint(x-ss.x,y-ss.y,z-ss.z);
}

double  operator*( MyPoint & s) //��������� ������������
{return (x*s.x+y*s.y+z*s.z);
}

MyPoint  operator *=(MyPoint &b) //��������� ������������
{return MyPoint(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);
}

MyPoint  operator*( float  s)  // ��������� �� ���������
{MyPoint b(*this);
 b.x=x*s;
 b.y=y*s;
 b.z=z*s;
 return b;
}

MyPoint  operator/( float  s)  // ������� �� ���������
{MyPoint b(*this);
 b.x=x/s;
 b.y=y/s;
 b.z=z/s;
 return b;

}

MyPoint  Normalize_Self()     // ������������� ���� �  ������� ��������������� ������
{try{
float sq=sqrt(x*x+y*y+z*z);
 x=x/sq;
 y=y/sq;
 z=z/sq;
 return *this;
 }
 catch(...)
 {}
}

MyPoint  Normalize()         // ������� ��������������� ������
{float a=this->Dlina();
 return MyPoint(x/a,y/a,z/a);
}

double Get_cos(MyPoint &ss)
{double aa=(*this)*ss;
 aa=aa/(this->Dlina()*ss.Dlina());
 return aa;
}

double Get_sin(MyPoint &ss)
{double aa= ((*this)*=ss).Dlina();
 aa=aa/(this->Dlina()*ss.Dlina());
 return aa;
}

 operator float * ()
{return &(this->x);
}

};


extern MyPoint e;
extern MyPoint c;
extern MyPoint u;


void strate(float vpered,float vlevo)
{
// ����������� ����� ������ �������
float a,b; //����������� � ���������� YOX
float L;   // ����� ����������� � ��������� XOY
float Lmov;  // ����� ����������� � ������������
float R3d;	 // ����� ������� ���������
float fi;    // ���� � ��������� XOY
float fi3d;  // ������ � ��������� XOY
Lmov=vpered;
R3d=sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y)+(c.z-e.z)*(c.z-e.z));
fi3d=asin((c.z-e.z) /R3d);
L=Lmov*cos(fi3d);

fi=atan((c.y-e.y)/(c.x-e.x));
if ((c.x-e.x)<0)
 fi+=(float)3.141592;

a=L*cos(fi);
b=L*sin(fi);

e.x+=a;
e.y+=b;
c.x+=a;
c.y+=b;

e.z+=Lmov*sin(fi3d);
c.z+=Lmov*sin(fi3d);
//printf("R3d=%3.3f  fi= %.f  fi3d= %f  L3d=%f\n",R3d,fi*180/3.141592,fi3d*180/3.141592,Lmov);
//printf("e %.0f %.0f %.0f   c %.0f %.0f %.0f  \n",e.x,e.y, e.z, c.x,c.y,c.z);

//����������� � ��� (������)
Lmov=vlevo;
float fi_perp;  //  ��� ������� ������ ���������������� �������(� ��������� XOY)
fi_perp=atan(-1/tan(fi));


	
a=Lmov*cos(fi_perp);
b=Lmov*sin(fi_perp);

if ((e.y-c.y)>0) 
{
e.x+=a;
e.y+=b;
c.x+=a;
c.y+=b;
}
else
{
e.x-=a;
e.y-=b;
c.x-=a;
c.y-=b;
}

glLoadIdentity();
gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );

}

void look_around(float left,float up)
{//�������� � 3� �� ��� �- ��� ��� ����������
	if(view==2)
	{
		static float R; 
		R =sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y));
		float vr01,vr1;    //��0 - ���� �� �������� ������� ������ �-E
			if ((c.x-e.x)>=0) //� ������ � ��������� �������� ��������� �������� ������� ����������
		  vr01=atan((c.y-e.y)/(c.x-e.x));  
		else             // �� 2-� � 3-� �������� � ����������� �������� ��
		  vr01=(float)3.141592+atan((c.y-e.y)/(c.x-e.x));

		vr1 = left/R;  // ������������� �������, ������������� ��0, �� ������������ vr01
		//printf("\n   R=%3.3f  vr1= %.f  vr01= %f  rx1=%d\n",R,vr1*180/3.141592,vr01*180/3.141592,left);
	   	
		c.x=e.x+R*cos(vr01+vr1); //
		c.y=e.y+R*sin(vr01+vr1);
		

		//������ �������� ��������
		//������ ����� � 3� �� ��� Y - ��� ��� ����������

		static double R3d;
		GLdouble	vr02,vr2;	//��0 - ���� �� �������� ������� ������ �-E �� ��� Z
		R3d=sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y)+(c.z-e.z)*(c.z-e.z));
		vr02=asin((c.z-e.z)/R3d);
		vr2=up/R3d;
		c.z=e.z+R3d*sin(vr02+vr2);
	}
	else if(view==1)
	{
		 static double R; 
		R =sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y));
		GLdouble vr01,vr1;    //��0 - ���� �� �������� ������� ������ �-E
		if ((c.x-e.x)>=0) //� ������ � ��������� �������� ��������� �������� ������� ����������
		  vr01=atan((c.y-e.y)/(c.x-e.x));  
		else             // �� 2-� � 3-� �������� � ����������� �������� ��
		  vr01=3.141592+atan((c.y-e.y)/(c.x-e.x));

		vr1 = -left/R;  // ������������� �������, ������������� ��0, �� ������������ vr01
		//printf("\n   R=%3.3f  vr1= %.f  vr01= %f  rx1=%d\n",R,vr1*180/3.141592,vr01*180/3.141592,left);
	   	
		e.x=c.x-R*cos(vr01+vr1); //
		e.y=c.y-R*sin(vr01+vr1);
		

		//������ �������� ��������
		//������ ����� � 3� �� ��� Y - ��� ��� ����������

		static double R3d;
		GLdouble	vr02,vr2;	//��0 - ���� �� �������� ������� ������ �-E �� ��� Z
		R3d=sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y)+(c.z-e.z)*(c.z-e.z));
		vr02=asin((c.z-e.z)/R3d);
		vr2=up/R3d;
		c.z=e.z+R3d*sin(vr02+vr2);

	}
	//printf("R3d=%3.3f  vr2= %.f  vr02= %f  ry1=%d\n",R3d,vr2*180/3.141592,vr02*180/3.141592,up);
 //	printf("e %.0f %.0f %.0f   c %.0f %.0f %.0f  \n",e.x,e.y, e.z, c.x,c.y,c.z);
    //printf("CPoint %.0f %.0f %.0f\n",(c.x-e.x)/R3d,(c.y-e.y)/R3d,(c.z-e.z)/R3d);
   // glLoadIdentity();
//gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );


    glLoadIdentity();
gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );
    }

void jamp(float up, float left)
{
if(view==1)
	{
		left=left*(double)px/w;
		up=up*(double)py/h;
	} 
else
{up=up/10.0;
 left=left/10.0;
}
	
strate(0,left); // ���������� � ���

float	vr02,R3d;	//��0 - ���� �� �������� ������� ������ �-E �� ��� Z
R3d=sqrt((e.x-c.x)*(e.x-c.x)+(e.y-c.y)*(e.y-c.y)+(c.z-e.z)*(c.z-e.z));
vr02=asin(abs(c.z-e.z)/R3d);

c.z-=up/cos(vr02);
e.z-=up/cos(vr02);
glLoadIdentity();

gluLookAt( e.x,e.y,e.z,c.x,c.y,c.z,u.x,u.y,u.z );
}

void inline zoom(int zoom)
{
 // ����� ������� �������������
 glMatrixMode( GL_PROJECTION );
 // �������� ��������� �������
 glLoadIdentity();
 // ��������� ����������  pzoom, ��� ������ ���� ������
 pzoom+=zoom;

 if (pzoom>200)
  pzoom=200;
if (pzoom<5)
  pzoom=5;
   if(w>=h) {px=w/h*pzoom; py=pzoom;}
   else     {py=h/w*pzoom; px=pzoom;}
   if (view==1)
 // ������������� ������������� ��������
 // ��� -px,px, -py,py ���������� ���������������
 // �����, ������, ������, � ������� ���������� ���������
 // � �������� -10000 � 10000 - ������� � ������� ���������� ���������
   glOrtho(-px/2,px/2, -py/2,py/2, -10000, 10000);
   else
    // ������������� ������������� ��������
  // pzoom - ���� ������, w/h - ����������� ����������� � ��������� ���� ������
  // 2 - ������� ��������� ��������� (������ ������������)
  // 10000 - ������� ��������� ��������� (������ ������������)
   gluPerspective (pzoom,w/h,2,10000);

 // ��� ���������� ������ - �������� � ������� ���������� �������������
 glMatrixMode(GL_MODELVIEW);

 // ���� ��������� �������
    glLoadIdentity();
    return ;
}

#endif
 