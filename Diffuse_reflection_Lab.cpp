#include <GL/glut.h>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <GL/gl.h>
///*****************************************************
// Program: Lab_Diffuse_Refleciton	   Altemush Bhatti *
//  Professor Harry Li                                 *
// * Version: x1.0;    status: tested;                 *
// * Compile and build:                                *
// * g++ -ggdb `pkg-config --cflags opencv` -o `       *
//   basename main.cpp .cpp` main.cpp `pkg-config      *
//   --libs opencv` -lGL -lGLU -lglut -lm -lstdc++     *
//   -std=c++0x^C									   *
// * Date: December 17, 2018                           *
// * Purpose: Diffuse Reflection.                      * 
// *****************************************************/
#include <opencv2/opencv.hpp>   //for OpenCV 3.x  
#include <opencv/highgui.h>     //for OpenCV 3.x  
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>
#define UpperBD 100
#define PI      3.1415926
#define Num_pts 10
using namespace std;
float Xe = 200.0f;//100
float Ye = 300.0f;
float Ze = 450.0f;
float Rho = sqrt(pow(Xe,2) + pow(Ye,2) + pow(Ze,2));
float D_focal = 100.0f; 
GLuint texture[2];

struct Image {

    unsigned long sizeX;

    unsigned long sizeY;

    char *data;

};
typedef struct Image Image;


#define checkImageWidth 64

#define checkImageHeight 64

GLubyte checkImage[checkImageWidth][checkImageHeight][3];
void makeCheckImage(void){

    int i, j, c;

    for (i = 0; i < checkImageWidth; i++) {

        for (j = 0; j < checkImageHeight; j++) {

            c = ((((i&0x8)==0)^((j&0x8)==0)))*255;

            checkImage[i][j][0] = (GLubyte) c;

            checkImage[i][j][1] = (GLubyte) c;

            checkImage[i][j][2] = (GLubyte) c;

        }

    }

}
int ImageLoad(char *filename, Image *image) {

    FILE *file;

    unsigned long size; // size of the image in bytes.

    unsigned long i; // standard counter.

    unsigned short int planes; // number of planes in image (must be 1)

    unsigned short int bpp; // number of bits per pixel (must be 24)

    char temp; // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.

    if ((file = fopen(filename, "rb"))==NULL){

        printf("File Not Found : %s\n",filename);

        return 0;

    }

    // seek through the bmp header, up to the width/height:

    fseek(file, 18, SEEK_CUR);

    // read the width

    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {

        printf("Error reading width from %s.\n", filename);

        return 0;

    }

    //printf("Width of %s: %lu\n", filename, image->sizeX);

    // read the height

    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {

        printf("Error reading height from %s.\n", filename);

        return 0;

    }

    size = image->sizeX * image->sizeY * 3;

    if ((fread(&planes, 2, 1, file)) != 1) {

        printf("Error reading planes from %s.\n", filename);

        return 0;

    }

    if (planes != 1) {

        printf("Planes from %s is not 1: %u\n", filename, planes);

        return 0;

    }

    if ((i = fread(&bpp, 2, 1, file)) != 1) {

        printf("Error reading bpp from %s.\n", filename);

        return 0;

    }

    if (bpp != 24) {

        printf("Bpp from %s is not 24: %u\n", filename, bpp);

        return 0;

    }


    fseek(file, 24, SEEK_CUR);


    image->data = (char *) malloc(size);

    if (image->data == NULL) {

        printf("Error allocating memory for color-corrected image data");

        return 0;

    }

    if ((i = fread(image->data, size, 1, file)) != 1) {

        printf("Error reading image data from %s.\n", filename);

        return 0;

    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)

        temp = image->data[i];

        image->data[i] = image->data[i+2];

        image->data[i+2] = temp;

    }

    return 1;

}
Image * loadTexture(){

    Image *image1;


    image1 = (Image *) malloc(sizeof(Image));

    if (image1 == NULL) {

        printf("Error allocating space for image");

        exit(0);

    }

    if (!ImageLoad("g.bmp", image1)) {

        exit(1);

    }

    return image1;

}
double X1, Y1, X2, Y2;

float round_value(float v)
{
  return floor(v + 0.5);
}

void myinit(void)

{
glGenTextures(2, texture);

      int cx, cy, ch;
      stbi_uc *img = stbi_load("g.bmp", &cx, &cy, &ch, 3);
      if (img == nullptr)
        return;
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);     
      glBindTexture(GL_TEXTURE_2D, texture[0]);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
      glTexImage2D(GL_TEXTURE_2D, 0, 3, cx, cy, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
      stbi_image_free( img );
}
typedef struct {
    float X[UpperBD];
    float Y[UpperBD];
    float Z[UpperBD];
} pworld;

typedef struct {
    float X[UpperBD];
    float Y[UpperBD];
    float Z[UpperBD];
} pviewer;

typedef struct{
    float X[UpperBD];
    float Y[UpperBD];
} pperspective;

typedef struct{
    float X[UpperBD];
    float Y[UpperBD];
} pattern2DL;

typedef struct{
    float X[UpperBD];
    float Y[UpperBD];
} arrowpoint;

typedef struct {
    float r[UpperBD], g[UpperBD], b[UpperBD];
} pt_diffuse;

void mydisplay()
{
//glMatrixMode(GL_PROJECTION);
glLoadIdentity();

gluPerspective(45.0,0.5,1.0,60.0);
//gluOrtho2D(5, 10, 0.0, 10);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
glClear(GL_COLOR_BUFFER_BIT);
//gluOrtho2D(0 , 6400 , 0 , 4800);
// define x-y coordinate
float p1x=-1.0f,  p1y= 0.0f;   
float p2x= 1.0f,  p2y= 0.0f;
float p3x= 0.0f,  p3y= 1.0f;
float p4x= 0.0f,  p4y=-1.0f;


//glLoadIdentity();

pworld  world;
pviewer viewer;
pperspective perspective;
pattern2DL letterL;
arrowpoint arrow;

//define the x-y-z world coordinate
world.X[0] = 0.0;    world.Y[0] =  0.0;   world.Z[0] =  0.0;    // origin 
world.X[1] = 50.0;   world.Y[1] =  0.0;   world.Z[1] =  0.0;    // x-axis
world.X[2] = 0.0;    world.Y[2] =  50.0;  world.Z[2] =  0.0;    // y-axis   
world.X[3] = 0.0;    world.Y[3] =  0.0;   world.Z[3] =  50.0;   // y-axis

//define projection plane world coordinate , THIS IS THE SQUARE AROUND THE LETTERS
world.X[4] = 60.0;   world.Y[4] = -50.0;   world.Z[4] =  0.0;    
world.X[5] = 60.0;   world.Y[5] =  50.0;   world.Z[5] =  0.0;    // base line
world.X[7] = 60.0;   world.Y[7] = -50.0;   world.Z[7] = 100.0;   // side bar
world.X[6] = 60.0;   world.Y[6] =  50.0;   world.Z[6] =  100.0;    // side bar




//define 2D pattern letter A
letterL.X[0] = -10.0; letterL.Y[0] = 10.0;    
letterL.X[1] = -15.0; letterL.Y[1] = 10.0; 
letterL.X[2] = -20.0; letterL.Y[2] = 30.0;    
letterL.X[3] = -40.0; letterL.Y[3] = 30.0;
letterL.X[4] = -45.0; letterL.Y[4] = 10.0;  
letterL.X[5] = -50.0; letterL.Y[5] = 10.0;
letterL.X[6] = -37.0; letterL.Y[6] = 70.0;
letterL.X[7] = -23.0; letterL.Y[7] = 70.0;
letterL.X[8] = -25.0; letterL.Y[8] = 40.0;
letterL.X[9] = -35.0; letterL.Y[9] = 40.0;
letterL.X[10] = -30.0; letterL.Y[10] = 60.0;

//letter B
letterL.X[11] = 10.0; letterL.Y[11] = 10.0;    
letterL.X[12] = 10.0; letterL.Y[12] = 70.0; 
letterL.X[13] = 20.0; letterL.Y[13] = 10.0;    
letterL.X[14] = 20.0; letterL.Y[14] = 70.0;

letterL.X[15] = 20.0; letterL.Y[15] = 60.0;  
letterL.X[16] = 20.0; letterL.Y[16] = 45.0;
letterL.X[17] = 20.0; letterL.Y[17] = 35.0;
letterL.X[18] = 20.0; letterL.Y[18] = 20.0;

letterL.X[19] = 25.0; letterL.Y[19] = 58.0;
letterL.X[20] = 27.0; letterL.Y[20] = 56.0;
letterL.X[21] = 28.0; letterL.Y[21] = 52.0;
letterL.X[22] = 27.0; letterL.Y[22] = 49.0;
letterL.X[23] = 25.0; letterL.Y[23] = 47.0;

letterL.X[24] = 25.0; letterL.Y[24] = 33.0;
letterL.X[25] = 27.0; letterL.Y[25] = 31.0;
letterL.X[26] = 28.0; letterL.Y[26] = 27.0;
letterL.X[27] = 27.0; letterL.Y[27] = 24.0;
letterL.X[28] = 25.0; letterL.Y[28] = 22.0;

letterL.X[29] = 30.0; letterL.Y[29] = 65.0;
letterL.X[30] = 34.0; letterL.Y[30] = 60.0;
letterL.X[31] = 34.0; letterL.Y[31] = 50.0;
letterL.X[32] = 30.0; letterL.Y[32] = 45.0;

letterL.X[33] = 25.0; letterL.Y[33] = 40.0;

letterL.X[34] = 30.0; letterL.Y[34] = 38.0;
letterL.X[35] = 34.0; letterL.Y[35] = 30.0;
letterL.X[36] = 34.0; letterL.Y[36] = 20.0;
letterL.X[37] = 30.0; letterL.Y[37] = 15.0;

arrow.X[0] = 0.0; arrow.Y[0] = 25.0;    
arrow.X[1] = 0.0; arrow.Y[1] = 75.0; 
arrow.X[2] = 60.0; arrow.Y[2] = 75.0;    
arrow.X[3] = 60.0; arrow.Y[3] = 85.0;
arrow.X[4] = 90.0; arrow.Y[4] = 50.0;  
arrow.X[5] = 60.0; arrow.Y[5] = 15.0;
arrow.X[6] = 60.0; arrow.Y[6] = 25.0;

arrow.X[7] = 0.0; arrow.Y[7] = 25.0;    
arrow.X[8] = 0.0; arrow.Y[8] = 75.0; 
arrow.X[9] = 60.0; arrow.Y[9] = 75.0;    
arrow.X[10] = 60.0; arrow.Y[10] = 85.0;
arrow.X[11] = 90.0; arrow.Y[11] = 50.0;  
arrow.X[12] = 60.0; arrow.Y[12] = 15.0;
arrow.X[13] = 60.0; arrow.Y[13] = 25.0;

//decoration
for(int i = 0; i <= 37; i++)
{
  world.X[8+i] = 60.0;
  world.Y[8+i] = letterL.X[i];
  world.Z[8+i] = letterL.Y[i];
}
//arrow
for(int j = 0; j <= 6; j++)
{
  world.X[46+j] = arrow.X[j]-50;//-50
  world.Y[46+j] = arrow.Y[j];
  world.Z[46+j] = 100.0;//CHANGE TO 150?
}
for(int k = 0; k <= 6; k++)
{
  world.X[53+k] = arrow.X[k]-50;
  world.Y[53+k] = arrow.Y[k];
  world.Z[53+k] = 110.0;//CHANGE TO 150?
}//top part of arrow, 53 to 59

float sPheta = Ye / sqrt(pow(Xe,2) + pow(Ye,2));
float cPheta = Xe / sqrt(pow(Xe,2) + pow(Ye,2));
float sPhi = sqrt(pow(Xe,2) + pow(Ye,2)) / Rho;
float cPhi = Ze / Rho;

float xMin = 1000.0, xMax = -1000.0;
float yMin = 1000.0, yMax = -1000.0;
//47 is normal vector 46 is a, 45 is ps, 7 is top left box vertex
//COMUTER SHADE OF FLOATING ARROW DUE NEXT WEEK

world.X[60] = -200.0;   world.Y[60]=50.0;   world.Z[60]=200.0;//ps
world.X[61] = 0.0;   world.Y[61]=0.0;   world.Z[61]=0.0;//vector a
world.X[62] = 0.0;   world.Y[62]=0.0;   world.Z[62]=1.0;//VECTOR N

float tmp = (world.X[62]*(world.X[61]-world.X[60]))
            +(world.Y[62]*(world.Y[61]-world.Y[60]))
            +(world.Z[62]*(world.Z[61]-world.Z[60]));
            cout << tmp;
float lambda =  tmp/((world.X[62]*(world.X[60]-world.X[7]))
            +(world.Y[62]*(world.Y[60]-world.Y[7]))
            +(world.Z[62]*(world.Z[60]-world.Z[7])));
            cout << lambda;
float lambda_2 = tmp/((world.X[62]*(world.X[60]-world.X[6]))//MAKE ARROW HIGHER, ABOVE PROJECTION PLANE SQUARE
            +(world.Y[62]*(world.Y[60]-world.Y[6]))
            +(world.Z[62]*(world.Z[60]-world.Z[6])));
            cout << lambda_2;
world.X[63] = world.X[60]+lambda*(world.X[60]-world.X[7]);//interseciton point for p7, X COMP
world.Y[63] = world.Y[60]+lambda*(world.Y[60]-world.Y[7]);//Y COMP
world.Z[63] = 0.0;

world.X[64] = world.X[60]+lambda_2*(world.X[60]-world.X[6]);//interseciton point for p7, X COMP
world.Y[64] = world.Y[60]+lambda_2*(world.Y[60]-world.Y[6]);//Y COMP
world.Z[64] = 0.0;


//for arrow's shade, 46-52
float lambda_arrow1 =  tmp/((world.X[62]*(world.X[60]-world.X[46]))
            +(world.Y[62]*(world.Y[60]-world.Y[46]))
            +(world.Z[62]*(world.Z[60]-world.Z[46])));
         
float lambda_arrow2 = tmp/((world.X[62]*(world.X[60]-world.X[47]))//MAKE ARROW HIGHER, ABOVE PROJECTION PLANE SQUARE
            +(world.Y[62]*(world.Y[60]-world.Y[47]))
            +(world.Z[62]*(world.Z[60]-world.Z[47])));
           
float lambda_arrow3 =  tmp/((world.X[62]*(world.X[60]-world.X[48]))
                        +(world.Y[62]*(world.Y[60]-world.Y[48]))
                        +(world.Z[62]*(world.Z[60]-world.Z[48])));
                       
float lambda_arrow4 =  tmp/((world.X[62]*(world.X[60]-world.X[49]))
            +(world.Y[62]*(world.Y[60]-world.Y[49]))
            +(world.Z[62]*(world.Z[60]-world.Z[49])));
           
float lambda_arrow5 =  tmp/((world.X[62]*(world.X[60]-world.X[50]))
            +(world.Y[62]*(world.Y[60]-world.Y[50]))
            +(world.Z[62]*(world.Z[60]-world.Z[50])));
               
float lambda_arrow6 =  tmp/((world.X[62]*(world.X[60]-world.X[51]))
            +(world.Y[62]*(world.Y[60]-world.Y[51]))
            +(world.Z[62]*(world.Z[60]-world.Z[51])));
           
float lambda_arrow7 =  tmp/((world.X[62]*(world.X[60]-world.X[52]))
            +(world.Y[62]*(world.Y[60]-world.Y[52]))
            +(world.Z[62]*(world.Z[60]-world.Z[52])));
           
world.X[65] = world.X[60]+lambda_arrow1*(world.X[60]-world.X[46]);//interseciton point for p7, X COMP
world.Y[65] = world.Y[60]+lambda_arrow1*(world.Y[60]-world.Y[46]);//Y COMP
world.Z[65] = 0.0;

world.X[66] = world.X[60]+lambda_arrow2*(world.X[60]-world.X[47]);//interseciton point for p7, X COMP
world.Y[66] = world.Y[60]+lambda_arrow2*(world.Y[60]-world.Y[47]);//Y COMP
world.Z[66] = 0.0;

world.X[67] = world.X[60]+lambda_arrow3*(world.X[60]-world.X[48]);//interseciton point for p7, X COMP
world.Y[67] = world.Y[60]+lambda_arrow3*(world.Y[60]-world.Y[48]);//Y COMP
world.Z[67] = 0.0;

world.X[68] = world.X[60]+lambda_arrow4*(world.X[60]-world.X[49]);//interseciton point for p7, X COMP
world.Y[68] = world.Y[60]+lambda_arrow4*(world.Y[60]-world.Y[49]);//Y COMP
world.Z[68] = 0.0;

world.X[69] = world.X[60]+lambda_arrow5*(world.X[60]-world.X[50]);//interseciton point for p7, X COMP
world.Y[69] = world.Y[60]+lambda_arrow5*(world.Y[60]-world.Y[50]);//Y COMP
world.Z[69] = 0.0;

world.X[70] = world.X[60]+lambda_arrow6*(world.X[60]-world.X[51]);//interseciton point for p7, X COMP
world.Y[70] = world.Y[60]+lambda_arrow6*(world.Y[60]-world.Y[51]);//Y COMP
world.Z[70] = 0.0;


world.X[71] = world.X[60]+lambda_arrow7*(world.X[60]-world.X[52]);//interseciton point for p7, X COMP
world.Y[71] = world.Y[60]+lambda_arrow7*(world.Y[60]-world.Y[52]);//Y COMP
world.Z[71] = 0.0;
//NEW ADDED
// for(int k = 0; k <= 6; k++)
// {
//   world.X[72+k] = arrow.X[k]-50;
//   world.Y[72+k] = arrow.Y[k];
//   world.Z[72+k] = 110.0;//CHANGE TO 150?
// }//top part of arrow, 53 to 59

X1=world.X[53];
X2=world.X[57];
Y1=world.Y[53];
Y2=world.Y[57];
//void LineDDA(void)
//{
  double dx=(X2-X1);
  double dy=(Y2-Y1);
  double steps;
  float xInc,yInc,x=X1,y=Y1,x_2=X2,y_2=Y2;//DO IT FOR X2 AND Y2
  /* Find out whether to increment x or y */
  steps=(abs(dx)>abs(dy))?(abs(dx)):(abs(dy));
  xInc=dx/(float)steps;
  yInc=dy/(float)steps;

  int k;
  /* For every step, find an intermediate vertex */
  steps=10;
  for(k=0;k<steps;k++)//steps is too small, not even more than 1
  {
    cout<< "THE STEPSSSSSSSSS" << steps;
    x+=xInc;
    y+=yInc;
     printf("does this work!!!!!!!!!!!!!!!!"); 
     world.X[72+k]=x;
     world.Y[72+k]=y;
    //glVertex2d(round_value(x), round_value(y));//instead put x and y as world coordinates?
    cout << round_value(x) << "CHECK" << round_value(y);
  }//round x and y values are too big, need to fit in perspective plane

 // -----------diffuse reflection-----------*
    pt_diffuse  diffuse;   //diffuse.r[3]

    //-------reflectivity coefficient-----------* 
    #define     Kdr     0.8
    #define     Kdg     0.0
    #define     Kdb     0.0
 
    // define additional pts to find diffuse reflection
    //world.X[49] = world.X[45] + lambda_2*(world.X[45] - world.X[6]);

    //--------compute distance------------------*//change 45 to 60!!!!!!
    float distance[UpperBD];
    for (int i=63; i<=71; i++) {
    distance[i] = sqrt(pow((world.X[i]-world.X[60]),2)+         //intersect pt p7
                        pow((world.Y[i]-world.Y[60]),2)+
                        pow((world.X[i]-world.X[60]),2) );
    //std::cout << "distance[i]  " << distance[i] << std::endl;
    } 
 
    for (int i=53; i<=59; i++){
    distance[i] = sqrt(pow((world.X[i]-world.X[60]),2)+         //pt p4 of projection plane
                        pow((world.Y[i]-world.Y[60]),2)+
                        pow((world.X[i]-world.X[60]),2) );
    //std::cout << "distance[i]  " << distance[i] << std::endl;
    }
 //NEW ADDED
 for (int i=72; i<=78; i++){
    distance[i] = sqrt(pow((world.X[i]-world.X[60]),2)+         //pt p4 of projection plane
                        pow((world.Y[i]-world.Y[60]),2)+
                        pow((world.X[i]-world.X[60]),2) );
    //std::cout << "distance[i]  " << distance[i] << std::endl;
    }


    //--------compute angle---------------------*
    float angle[UpperBD], tmp_dotProd[UpperBD], tmp_mag_dotProd[UpperBD]; 

    for (int i=63; i<=71; i++){

    tmp_dotProd[i] = world.Z[i]-world.X[60]; 
    std::cout << " tmp_dotProd[i]  " <<  tmp_dotProd[i] << std::endl;

    tmp_mag_dotProd[i] = sqrt(pow((world.X[i]-world.X[60]),2)+         //[45] pt light source 
                        pow((world.Y[i]-world.Y[60]),2)+
                        pow((world.Z[i]-world.Z[60]),2) );
    std::cout << " tmp_mag_dotProd[i]  1 " <<  tmp_mag_dotProd[i] << std::endl;

    angle[i] = tmp_dotProd[i]/ tmp_mag_dotProd[i]; 
    std::cout << "angle[i]  " << angle[i] << std::endl;

    //compute color intensity
    diffuse.r[i] = Kdr *  angle[i] /  pow(distance[i],2) ;
    diffuse.g[i] = Kdg *  angle[i] /  pow(distance[i],2) ;
    diffuse.b[i] = Kdb *  angle[i] /  pow(distance[i],2) ;
    }

   for (int i=53; i<=59; i++){

    tmp_dotProd[i] = world.Z[i]-world.X[60]; 
    std::cout << " tmp_dotProd[i]  " <<  tmp_dotProd[i] << std::endl;

    tmp_mag_dotProd[i] = sqrt(pow((world.X[i]-world.X[60]),2)+         //[45] pt light source 
                        pow((world.Y[i]-world.Y[60]),2)+
                        pow((world.Z[i]-world.Z[60]),2) );
    std::cout << " tmp_mag_dotProd[i]  1 " <<  tmp_mag_dotProd[i] << std::endl;

    angle[i] = tmp_dotProd[i]/ tmp_mag_dotProd[i]; 
    std::cout << "angle[i]  " << angle[i] << std::endl;

    //compute color intensity
    diffuse.r[i] = Kdr *  angle[i] /  pow(distance[i],2) ;
   diffuse.g[i] = Kdg *  angle[i] /  pow(distance[i],2) ;
   diffuse.b[i] = Kdb *  angle[i] /  pow(distance[i],2) ;

    //std::cout << "diffuse.r[i]  " << diffuse.r[i] << std::endl;
    //std::cout << "diffuse.g[i]  " << diffuse.g[i] << std::endl;
    }

    //NEW ADDED
    for (int i=72; i<=78; i++){

    tmp_dotProd[i] = world.Z[i]-world.X[60]; 
    std::cout << " tmp_dotProd[i]  " <<  tmp_dotProd[i] << std::endl;

    tmp_mag_dotProd[i] = sqrt(pow((world.X[i]-world.X[60]),2)+         //[45] pt light source 
                        pow((world.Y[i]-world.Y[60]),2)+
                        pow((world.Z[i]-world.Z[60]),2) );
    std::cout << " tmp_mag_dotProd[i]  1 " <<  tmp_mag_dotProd[i] << std::endl;

    angle[i] = tmp_dotProd[i]/ tmp_mag_dotProd[i]; 
    std::cout << "angle[i]  " << angle[i] << std::endl;

    //compute color intensity
    diffuse.r[i] = Kdr *  angle[i] /  pow(distance[i],2) ;
   diffuse.g[i] = Kdg *  angle[i] /  pow(distance[i],2) ;
   diffuse.b[i] = Kdb *  angle[i] /  pow(distance[i],2) ;

    //std::cout << "diffuse.r[i]  " << diffuse.r[i] << std::endl;
    //std::cout << "diffuse.g[i]  " << diffuse.g[i] << std::endl;
    }
//viewer
for(int i = 0; i < UpperBD; i++)
{
  viewer.X[i] = -sPheta * world.X[i] + cPheta * world.Y[i];
  viewer.Y[i] = -cPheta * cPhi * world.X[i] 
           - cPhi * sPheta * world.Y[i]
           + sPhi * world.Z[i];
  viewer.Z[i] = -sPhi * cPheta * world.X[i]
           - sPhi * cPheta * world.Y[i]
           -cPheta * world.Z[i] + Rho;
          // cout << i;
}
//perspective
for(int i = 0; i < UpperBD; i++)
{
  perspective.X[i] = D_focal * viewer.X[i] / viewer.Z[i] ;
  perspective.Y[i] = D_focal * viewer.Y[i] / viewer.Z[i] ;
  if (perspective.X[i] > xMax) xMax = perspective.X[i];
  if (perspective.X[i] < xMin) xMin = perspective.X[i];
  if (perspective.Y[i] > yMax) yMax = perspective.Y[i];
  if (perspective.Y[i] < yMin) yMin = perspective.Y[i]; 

}
for(int i = 0; i < UpperBD; i++)
{
  if ((xMax-xMin) != 0) perspective.X[i] = perspective.X[i]/(xMax-xMin);
  if ((yMax-yMin) != 0) perspective.Y[i] = perspective.Y[i]/(yMax-yMin);
  std::cout << i << perspective.X[i] << perspective.Y[i] << std::endl;
}
glViewport(0,0,1550,1250);

 
 
glBegin(GL_LINES);
  //axis
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 0.0, 0.0);
  glVertex2f(perspective.X[0],perspective.Y[0]); 
  glVertex2f(perspective.X[1],perspective.Y[1]);
  glColor3f(0.0, 1.0, 0.0);
  glVertex2f(perspective.X[0],perspective.Y[0]); 
  glVertex2f(perspective.X[2],perspective.Y[2]);
  glColor3f(0.0, 0.0, 1.0);
  glVertex2f(perspective.X[0],perspective.Y[0]); 
  glVertex2f(perspective.X[3],perspective.Y[3]);

  glColor3f(1.0, 1.0, 0.0);  // projection plane , square
  glVertex2f(perspective.X[4],perspective.Y[4]); 
  glVertex2f(perspective.X[5],perspective.Y[5]);
  glVertex2f(perspective.X[4],perspective.Y[4]); 
  glVertex2f(perspective.X[7],perspective.Y[7]);
  glVertex2f(perspective.X[5],perspective.Y[5]); 
  glVertex2f(perspective.X[6],perspective.Y[6]);
  glVertex2f(perspective.X[6],perspective.Y[6]); 
  glVertex2f(perspective.X[7],perspective.Y[7]);
 glEnd();

  glColor3f(0.0, 1.0, 0.0);  // LETTER A STARTS HERE
glBegin(GL_POLYGON);
       glVertex2f(perspective.X[13],perspective.Y[13]);
    glVertex2f(perspective.X[12],perspective.Y[12]);
 
    glVertex2f(perspective.X[11],perspective.Y[11]);
    glVertex2f(perspective.X[12],perspective.Y[12]);
   
     glVertex2f(perspective.X[14],perspective.Y[14]);
      glVertex2f(perspective.X[13],perspective.Y[13]);
   
     glVertex2f(perspective.X[18],perspective.Y[18]);
      glVertex2f(perspective.X[17],perspective.Y[17]);
   
     glVertex2f(perspective.X[11],perspective.Y[11]);
      glVertex2f(perspective.X[17],perspective.Y[17]);
     
     
     glVertex2f(perspective.X[18],perspective.Y[18]);
      glVertex2f(perspective.X[14],perspective.Y[14]);
   
     
      glEnd();
  glColor3f(0.0, 1.0, 0.0); 
glBegin(GL_POLYGON);
   
   
   
     glVertex2f(perspective.X[8],perspective.Y[8]);
      glVertex2f(perspective.X[15],perspective.Y[15]);
   
     glVertex2f(perspective.X[14],perspective.Y[14]);
      glVertex2f(perspective.X[15],perspective.Y[15]);
     
      glVertex2f(perspective.X[14],perspective.Y[14]);
      glVertex2f(perspective.X[18],perspective.Y[18]);
  
     glVertex2f(perspective.X[16],perspective.Y[16]);
      glVertex2f(perspective.X[18],perspective.Y[18]);
  
     glVertex2f(perspective.X[16],perspective.Y[16]);
      glVertex2f(perspective.X[10],perspective.Y[10]);
  
    glVertex2f(perspective.X[9],perspective.Y[9]);
  glVertex2f(perspective.X[10],perspective.Y[10]);
 
   glVertex2f(perspective.X[8],perspective.Y[8]);
  glVertex2f(perspective.X[9],perspective.Y[9]);
 
  
     glEnd();
  glColor3f(0.0, 1.0, 0.0); 
glBegin(GL_POLYGON);
  
    glVertex2f(perspective.X[16],perspective.Y[16]);
      glVertex2f(perspective.X[17],perspective.Y[17]);
   
     glVertex2f(perspective.X[11],perspective.Y[11]);
    glVertex2f(perspective.X[10],perspective.Y[10]);
   
      glVertex2f(perspective.X[11],perspective.Y[11]);
    glVertex2f(perspective.X[17],perspective.Y[17]);
   
    glVertex2f(perspective.X[16],perspective.Y[16]);
      glVertex2f(perspective.X[10],perspective.Y[10]);
  
  
     glEnd();
  glColor3f(0.0, 1.0, 0.0);  //LETTER B STARTS HERE
glBegin(GL_POLYGON);
  
    glVertex2f(perspective.X[19],perspective.Y[19]);
      glVertex2f(perspective.X[20],perspective.Y[20]);
   
    glVertex2f(perspective.X[21],perspective.Y[21]);
      glVertex2f(perspective.X[19],perspective.Y[19]);
   
     glVertex2f(perspective.X[20],perspective.Y[20]);
      glVertex2f(perspective.X[22],perspective.Y[22]);

 glVertex2f(perspective.X[23],perspective.Y[23]);
      glVertex2f(perspective.X[22],perspective.Y[22]);
     
      glVertex2f(perspective.X[21],perspective.Y[21]);
      glVertex2f(perspective.X[26],perspective.Y[26]);
     
    glVertex2f(perspective.X[25],perspective.Y[25]);
      glVertex2f(perspective.X[24],perspective.Y[24]);

 glEnd();
  glColor3f(0.0, 1.0, 0.0);
glBegin(GL_LINES);
     
       glVertex2f(perspective.X[23],perspective.Y[23]);
      glVertex2f(perspective.X[24],perspective.Y[24]);
   
   
     glVertex2f(perspective.X[25],perspective.Y[25]);
      glVertex2f(perspective.X[26],perspective.Y[26]);
  
      glEnd();
  glColor3f(0.0, 1.0, 0.0); 
glBegin(GL_POLYGON);
 
      glVertex2f(perspective.X[27],perspective.Y[27]);
      glVertex2f(perspective.X[28],perspective.Y[28]);
     
        glVertex2f(perspective.X[27],perspective.Y[27]);
      glVertex2f(perspective.X[23],perspective.Y[23]);
     
      glVertex2f(perspective.X[29],perspective.Y[29]);
      glVertex2f(perspective.X[28],perspective.Y[28]);
     
       glVertex2f(perspective.X[22],perspective.Y[22]);
      glVertex2f(perspective.X[37],perspective.Y[37]);
     
        glVertex2f(perspective.X[38],perspective.Y[38]);
      glVertex2f(perspective.X[37],perspective.Y[37]);
     
       glVertex2f(perspective.X[38],perspective.Y[38]);
      glVertex2f(perspective.X[39],perspective.Y[39]);
     
        glVertex2f(perspective.X[29],perspective.Y[29]);
      glVertex2f(perspective.X[39],perspective.Y[39]);
  
          glEnd();
  glColor3f(0.0, 1.0, 0.0); 
glBegin(GL_POLYGON);
   
        glVertex2f(perspective.X[29],perspective.Y[29]);
      glVertex2f(perspective.X[30],perspective.Y[30]);
     
        glVertex2f(perspective.X[24],perspective.Y[24]);
      glVertex2f(perspective.X[30],perspective.Y[30]);
   
       glVertex2f(perspective.X[40],perspective.Y[40]);
      glVertex2f(perspective.X[39],perspective.Y[39]);
     
        glVertex2f(perspective.X[40],perspective.Y[40]);
      glVertex2f(perspective.X[41],perspective.Y[41]);
     
       glVertex2f(perspective.X[24],perspective.Y[24]);
      glVertex2f(perspective.X[41],perspective.Y[41]);
     
     
     
      glEnd();
  glColor3f(0.0, 1.0, 0.0); 
glBegin(GL_POLYGON);
     
       glVertex2f(perspective.X[24],perspective.Y[24]);
      glVertex2f(perspective.X[41],perspective.Y[41]);
     
        glVertex2f(perspective.X[25],perspective.Y[25]);
      glVertex2f(perspective.X[32],perspective.Y[32]);
     
       glVertex2f(perspective.X[33],perspective.Y[33]);
      glVertex2f(perspective.X[32],perspective.Y[32]);
     
          glVertex2f(perspective.X[33],perspective.Y[33]);
      glVertex2f(perspective.X[34],perspective.Y[34]);
     
        glVertex2f(perspective.X[42],perspective.Y[42]);
      glVertex2f(perspective.X[41],perspective.Y[41]);
     
       glVertex2f(perspective.X[42],perspective.Y[42]);
      glVertex2f(perspective.X[43],perspective.Y[43]);
     
       glEnd();
  glColor3f(0.0, 1.0, 0.0);
glBegin(GL_POLYGON);
     
     
       glVertex2f(perspective.X[35],perspective.Y[35]);
      glVertex2f(perspective.X[34],perspective.Y[34]);
     
       glVertex2f(perspective.X[35],perspective.Y[35]);
      glVertex2f(perspective.X[36],perspective.Y[36]);
     
        glVertex2f(perspective.X[26],perspective.Y[26]);
      glVertex2f(perspective.X[36],perspective.Y[36]);

     
        glVertex2f(perspective.X[44],perspective.Y[44]);
      glVertex2f(perspective.X[43],perspective.Y[43]);
     
        glVertex2f(perspective.X[44],perspective.Y[44]);
      glVertex2f(perspective.X[45],perspective.Y[45]);
     
      glVertex2f(perspective.X[21],perspective.Y[21]);
      glVertex2f(perspective.X[45],perspective.Y[45]);



glEnd();

glColor3f(1.0, 0.0, 0.0);
glBegin(GL_LINES);//3D arrow starts here

glVertex2f(perspective.X[46],perspective.Y[46]);
      glVertex2f(perspective.X[47],perspective.Y[47]);
     
      glVertex2f(perspective.X[48],perspective.Y[48]);
      glVertex2f(perspective.X[49],perspective.Y[49]);


glVertex2f(perspective.X[50],perspective.Y[50]);
      glVertex2f(perspective.X[51],perspective.Y[51]);
     
      glVertex2f(perspective.X[52],perspective.Y[52]);
      glVertex2f(perspective.X[51],perspective.Y[51]);
     
        glVertex2f(perspective.X[49],perspective.Y[49]);
      glVertex2f(perspective.X[50],perspective.Y[50]);
     
        glVertex2f(perspective.X[48],perspective.Y[48]);
      glVertex2f(perspective.X[47],perspective.Y[47]);
//      
      glVertex2f(perspective.X[52],perspective.Y[52]);
      glVertex2f(perspective.X[46],perspective.Y[46]);
     
     
     
     
     
     
      glVertex2f(perspective.X[53],perspective.Y[53]);
      glVertex2f(perspective.X[54],perspective.Y[54]);
     
      glVertex2f(perspective.X[55],perspective.Y[55]);
      glVertex2f(perspective.X[56],perspective.Y[56]);


glVertex2f(perspective.X[57],perspective.Y[57]);
      glVertex2f(perspective.X[58],perspective.Y[58]);
     
      glVertex2f(perspective.X[59],perspective.Y[59]);
      glVertex2f(perspective.X[58],perspective.Y[58]);
//     
        glVertex2f(perspective.X[56],perspective.Y[56]);
      glVertex2f(perspective.X[57],perspective.Y[57]);
//     
 glVertex2f(perspective.X[55],perspective.Y[55]);
      glVertex2f(perspective.X[54],perspective.Y[54]);
     
       glVertex2f(perspective.X[59],perspective.Y[59]);
      glVertex2f(perspective.X[53],perspective.Y[53]);
     
      glVertex2f(perspective.X[46],perspective.Y[46]);
      glVertex2f(perspective.X[46+7],perspective.Y[46+7]);
     
      glVertex2f(perspective.X[47],perspective.Y[47]);
      glVertex2f(perspective.X[47+7],perspective.Y[47+7]);
     
       glVertex2f(perspective.X[48],perspective.Y[48]);
      glVertex2f(perspective.X[48+7],perspective.Y[48+7]);
     
       glVertex2f(perspective.X[49],perspective.Y[49]);
      glVertex2f(perspective.X[49+7],perspective.Y[49+7]);
     
       glVertex2f(perspective.X[50],perspective.Y[50]);
      glVertex2f(perspective.X[50+7],perspective.Y[50+7]);
     
        glVertex2f(perspective.X[51],perspective.Y[51]);
      glVertex2f(perspective.X[51+7],perspective.Y[51+7]);
     
   glEnd();    //end arrow
     
     
      glColor3f(1.0, 1.0, 0.0);
      glBegin(GL_LINES);
  
   glVertex2f(perspective.X[63],perspective.Y[63]);
      glVertex2f(perspective.X[64],perspective.Y[64]);
  
   glVertex2f(perspective.X[63],perspective.Y[63]);
      glVertex2f(perspective.X[4],perspective.Y[4]);
     
         glVertex2f(perspective.X[64],perspective.Y[64]);
      glVertex2f(perspective.X[5],perspective.Y[5]);
 
   glEnd();    //end arrow
     
     
      glColor3f(1.0, 0.0, 0.0);
      glBegin(GL_LINES);
 //arrow shadow
     
         glVertex2f(perspective.X[65],perspective.Y[65]);
      glVertex2f(perspective.X[66],perspective.Y[66]);

         glVertex2f(perspective.X[67],perspective.Y[67]);
      glVertex2f(perspective.X[66],perspective.Y[66]);
     
        glVertex2f(perspective.X[67],perspective.Y[67]);
      glVertex2f(perspective.X[68],perspective.Y[68]);
     
     
      glVertex2f(perspective.X[69],perspective.Y[69]);
      glVertex2f(perspective.X[68],perspective.Y[68]);
        
      glVertex2f(perspective.X[69],perspective.Y[69]);
      glVertex2f(perspective.X[70],perspective.Y[70]);
     
       glVertex2f(perspective.X[71],perspective.Y[71]);
      glVertex2f(perspective.X[70],perspective.Y[70]);
     
        glVertex2f(perspective.X[71],perspective.Y[71]);
      glVertex2f(perspective.X[65],perspective.Y[65]);


glEnd();
//Image on projeciton plane
glBindTexture(GL_TEXTURE_2D, texture[1]);


    glBindTexture(GL_TEXTURE_2D, texture[0]);
glEnable( GL_TEXTURE_2D );
glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glBegin(GL_QUADS);
glTexCoord2f(0.0, 0.0);
 glVertex2f(perspective.X[4],perspective.Y[4]); 
 glTexCoord2f(1.0, 0.0);
  glVertex2f(perspective.X[5],perspective.Y[5]);

glTexCoord2f(1.0, 1.0);
  glVertex2f(perspective.X[6],perspective.Y[6]);

glTexCoord2f(0.0, 1.0);
  glVertex2f(perspective.X[7],perspective.Y[7]);
    
    glEnd();
//diffuse reflection
    #define     display_scaling        200000.0
    #define     display_shifting    0.2
    for (int i=63; i<=64; i++) {
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    //r = display_scaling*diffuse.r[i];
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);//make a square at each vertex
    glVertex2f(perspective.X[i],perspective.Y[i]);
    glVertex2f(perspective.X[i]+0.1,perspective.Y[i]);
    glVertex2f(perspective.X[i]+0.1,perspective.Y[i]+0.1);
    glVertex2f(perspective.X[i],perspective.Y[i]+0.1);
    glEnd();
    }
     for (int i=66; i<=71; i++) {
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    //r = display_scaling*diffuse.r[i];
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);//make a square at each vertex
    glVertex2f(perspective.X[i],perspective.Y[i]);
    glVertex2f(perspective.X[i]+0.1,perspective.Y[i]);
    glVertex2f(perspective.X[i]+0.1,perspective.Y[i]+0.1);
    glVertex2f(perspective.X[i],perspective.Y[i]+0.1);
    glEnd();
    }

for (int i=69; i<70; i++) {//67,69,71
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    //r = display_scaling*diffuse.r[i];
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);//make a square at each vertex
    glVertex2f(perspective.X[i]+0.05,perspective.Y[i]+0.05);
    glVertex2f(perspective.X[i]+0.1+0.05,perspective.Y[i]+0.05);
    glVertex2f(perspective.X[i]+0.1+0.05,perspective.Y[i]+0.1+0.05);
    glVertex2f(perspective.X[i]+0.05,perspective.Y[i]+0.1+0.05);
    glEnd();
    }
    for (int i=67; i<68; i++) {//67,69,71
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    //r = display_scaling*diffuse.r[i];
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);//make a square at each vertex
    glVertex2f(perspective.X[69]+0.1,perspective.Y[69]+0.1);
    glVertex2f(perspective.X[69]+0.1+0.1,perspective.Y[69]+0.1);
    glVertex2f(perspective.X[69]+0.1+0.1,perspective.Y[69]+0.1+0.1);
    glVertex2f(perspective.X[69]+0.1,perspective.Y[69]+0.1+0.1);
    glEnd();
    }
for (int i=67; i<68; i++) {//67,69,71
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    //r = display_scaling*diffuse.r[i];
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);//make a square at each vertex
    glVertex2f(perspective.X[69]+0.15,perspective.Y[69]+0.15);
    glVertex2f(perspective.X[69]+0.1+0.15,perspective.Y[69]+0.15);
    glVertex2f(perspective.X[69]+0.1+0.15,perspective.Y[69]+0.1+0.15);
    glVertex2f(perspective.X[69]+0.15,perspective.Y[69]+0.1+0.15);
    glEnd();
    }
for (int i=65; i<66; i++) {//67,69,71
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    //r = display_scaling*diffuse.r[i];
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);//make a square at each vertex
    glVertex2f(perspective.X[69]+0.2,perspective.Y[69]+0.2);
    glVertex2f(perspective.X[69]+0.1+0.2,perspective.Y[69]+0.2);
    glVertex2f(perspective.X[69]+0.1+0.2,perspective.Y[69]+0.1+0.2);
    glVertex2f(perspective.X[69]+0.2,perspective.Y[69]+0.1+0.2);
    glEnd();
    }
for (int i=65; i<66; i++) {//67,69,71
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    //r = display_scaling*diffuse.r[i];
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);//make a square at each vertex
    glVertex2f(perspective.X[69]+0.25,perspective.Y[69]+0.25);
    glVertex2f(perspective.X[69]+0.1+0.25,perspective.Y[69]+0.25);
    glVertex2f(perspective.X[69]+0.1+0.25,perspective.Y[69]+0.1+0.25);
    glVertex2f(perspective.X[69]+0.25,perspective.Y[69]+0.1+0.25);
    glEnd();
    }


for (int i=65; i<=66; i++) {
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    //r = display_scaling*diffuse.r[i];
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);//make a square at each vertex
    glVertex2f(perspective.X[i]-0.05,perspective.Y[i]-0.05);
    glVertex2f(perspective.X[i]-0.1+0.05,perspective.Y[i]-0.05);
    glVertex2f(perspective.X[i]-0.1+0.05,perspective.Y[i]-0.1+0.05);
    glVertex2f(perspective.X[i]-0.05,perspective.Y[i]-0.1+0.05);
    glEnd();
    }



    glColor3f(1.0,1.0,0.0);
  glBegin(GL_POINTS);
  /* Plot the first point */
  glVertex2f(perspective.X[53],perspective.Y[53]);
glVertex2f(perspective.X[72],perspective.Y[72]);


  glEnd();
//for arrow:
    for (int i=65; i<=66; i++) {//53 to 59
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);

 float x1mid=(perspective.X[53]+perspective.X[59])/2;
  float y1mid=(perspective.Y[53]+perspective.Y[59])/2;

float x2mid=(perspective.X[54]+perspective.X[55])/2;
  float y2mid=(perspective.Y[54]+perspective.Y[55])/2;


  glVertex2f(perspective.X[59],perspective.Y[59]);
       glVertex2f(perspective.X[55],perspective.Y[55]);

glVertex2f(x1mid,y1mid);
      glVertex2f(x2mid,y2mid);
   

glVertex2f(x2mid,y2mid);
      glVertex2f(perspective.X[55],perspective.Y[55]);


 glEnd();



     }
for (int i=53; i<=59; i++) {//53 to 59
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);
  
 
      glVertex2f(perspective.X[53],perspective.Y[53]);
      glVertex2f(perspective.X[54],perspective.Y[54]);
     float x1mid=(perspective.X[53]+perspective.X[59])/2;
  float y1mid=(perspective.Y[53]+perspective.Y[59])/2;

float x2mid=(perspective.X[54]+perspective.X[55])/2;
  float y2mid=(perspective.Y[54]+perspective.Y[55])/2;

glVertex2f(x1mid,y1mid);
      glVertex2f(x2mid,y2mid);

 glVertex2f(x2mid,y2mid);
      glVertex2f(perspective.X[54],perspective.Y[54]);

 glEnd();



     }
     for (int i=63; i<=71; i++) {
    float r, g, b;
    r = display_scaling*diffuse.r[i]+display_shifting;
    //r = display_scaling*diffuse.r[i];
    g = diffuse.g[i]; b = diffuse.b[i] ;
    glColor3f(r, g, b);
    std::cout << "display_scaling*diffuse.r[i]  " << r << std::endl;
    glBegin(GL_POLYGON);//make a square at each vertex
   glVertex2f(perspective.X[57],perspective.Y[57]);
       glVertex2f(perspective.X[58],perspective.Y[58]);

 glVertex2f(perspective.X[59],perspective.Y[59]);
       glVertex2f(perspective.X[58],perspective.Y[58]);

      glVertex2f(perspective.X[55],perspective.Y[55]);
       glVertex2f(perspective.X[56],perspective.Y[56]);
    glEnd();
	 }
  


glutSwapBuffers();
glFlush();
}

int main(int argc, char** argv)
{
cv::Mat image = cv::imread("b.jpg", CV_LOAD_IMAGE_COLOR);

glutInit(&argc,argv);
glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
glutInitWindowSize(900, 1000);
glutCreateWindow("lab");

glutDisplayFunc(mydisplay);
myinit();
glutMainLoop();
}
