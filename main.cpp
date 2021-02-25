#include <iostream>
#include <GL/glut.h>
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>


#define SLOW true
#define macsec 30000   //macsec is microseconds itll sleep per action if on SLOWmode

using namespace std;
int HEIGHT=22;
int LENGTH=10;
const int BUFFER=2;
const double SCALE=0.85;
typedef char i8;
i8* board;
//board array is of the form board[h*LENGTH+l] 
//where h is height of block and l is length of block

float** cols;
int (*peices[7])[2];	// array of 7, 1 per peice,
						//	thease point to a 2d array each of the form [4][2]
						// thease 2d arrays are of the form arr[x][y] where you are getting the position of the xth peice where y=0 is the x position and y=1 is the y position
int held=1;

#define IMG false
#if IMG
#include <fstream>
int fHeight=-1;
int* fboard;
#endif

void rotateTet(int (*peice)[2]){
	int t=0;
	for(int i=0;i<4;i++){
		t = peice[i][0];
		peice[i][0] = -peice[i][1];
		peice[i][1] = t;
	}
}

enum color {
	blank,
	cyan,	// |
	yellow, // o
	purple, // T
	green,	// S
	red,	// Z
	blue,	// J
	orange, // L
};




void drawSquare(int x1, int y1,float* rgb)
{
    glColor3f(rgb[0],rgb[1],rgb[2]);
    glBegin(GL_QUADS);
	float xd = 1.0 / (LENGTH+BUFFER);
	float yd = 1.0 / (HEIGHT+BUFFER);
	float xf=x1- (LENGTH+BUFFER/2.0)/2.0;
	float yf=y1- (HEIGHT+BUFFER/2.0)/2.0;
	xf = xf/(LENGTH/2.0 + BUFFER/2.0) +xd*(BUFFER)/2.0;
	yf = yf/(HEIGHT/2.0 + BUFFER/2.0) +yd*(BUFFER)/2.0;
	xf += xd;
	yf += yd;
	xd=xd*SCALE;
	yd=yd*SCALE;
    glVertex2d(xf + xd, yf + yd);
    glVertex2d(xf + xd, yf - yd);
    glVertex2d(xf - xd, yf - yd);
    glVertex2d(xf - xd, yf + yd);

    glEnd();
}

void draw(){
#if IMG
	for (int i =0;i<=fHeight;i++){
		for (int j =0;j<LENGTH;j++){
			drawSquare(j,i,cols[fboard[(i*LENGTH) +j]]);
		}
	}
	for (int i =fHeight+1;i<HEIGHT-2;i++){
#endif
	for (int i =0;i<HEIGHT;i++){
		for (int j =0;j<LENGTH;j++){
			drawSquare(j,i,cols[board[(i*LENGTH) +j]]);
		}
	}
}

int abs(int x){
	if(x<0){return (-x);}
	else{return x;}
}

double vs[4] = {0.510066,-0.760666,0.35663,0.184483};

double cost(){
	double cst=0;
	int i{HEIGHT},j{0};
	int ph;
	bool in;
	for(j=0;j<LENGTH;j++){
		in = false;
		int h=0;
		i=HEIGHT;
		while(i>0){
			i--;
			if(!in && board[i*LENGTH +j]!=0){
				h=i+1;
			}
			if(in&&board[i*LENGTH +j]==0){cst+=vs[2];}
			in |= board[i*LENGTH +j]!=0;
		}
		cst+=vs[0]+h;
		if(j!=0){cst+=vs[3]*abs(ph-h);}
		ph=h;
	}
	for(i=0;i<HEIGHT;i++){
		bool fullR=true;
		for(j=0;j<LENGTH;j++){
			fullR &= board[(i*LENGTH) +j]!=0;
		}
		if (fullR){cst+=vs[1];}
	}
	
	return cst;
}

void disInit(){
	glClearColor(0.0f, 0.0f, 0.0f, 1); // Set background color to black and opaque
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	draw();
	glFlush();
}

void run() {
	int y{HEIGHT-2},x{LENGTH/2},pec{rand()%orange};
#if SLOW
	int temps[4]={0,0,0,0};
	for(int i=0;i<4;i++){
		temps[i] = board[(y+peices[pec][i][1])*LENGTH +(x+peices[pec][i][0])]; 
		*(board+((y+peices[pec][i][1])*LENGTH)+x+peices[pec][i][0]) = pec+1;
		drawSquare(x+peices[pec][i][0],y+peices[pec][i][1],cols[pec+1]);
	}
	glFlush();
	usleep(macsec);
	for(int i=0;i<4;i++){
		board[(y+peices[pec][i][1])*LENGTH+(x+peices[pec][i][0])] = temps[i];
		drawSquare(x+peices[pec][i][0],y+peices[pec][i][1],cols[temps[i]]);
	}
#endif
	int bx{5},by{10},br{2},bcul{1}; //best x best y, best rotation (rotation stored as r where toation is r*pi/2)
	double bc=9000000; //best cost
	int culs[2] = {held,pec};
	int c;
	for(int a=0;a<2;a++){
		c = culs[a];	
		for(int i=0;i<HEIGHT;i++){
			for (int j=0;j<LENGTH;j++){
				for (int r=0;r<4;r++){
					bool fits = true;
					bool floating=true;
					for(int k =0;k<4;k++){
						fits &= peices[c][k][0]+j >=0 && peices[c][k][1]+i >=0 && peices[c][k][0]+j<LENGTH && peices[c][k][1]+i<HEIGHT;
						if(!fits){break;}
						if((peices[c][k][1]+i-1)<0){floating=false;}else{
							floating &= board[peices[c][k][0]+j + (peices[c][k][1]+i-1)*LENGTH]==0;
						}
						fits = board[peices[c][k][0]+j + (peices[c][k][1]+i)*LENGTH]==0;
					}
					if(fits and !floating){
						//if it fits, it sits
						for(int k=0;k<4;k++){
							board[peices[c][k][0]+j + (peices[c][k][1]+i)*LENGTH]=c+1;
						}

						double col = cost();
						if (col <= bc){
							bx = j;
							by = i;
							br = r;
							bc = col;
							bcul =c;
						}

						for(int k=0;k<4;k++){
							board[peices[c][k][0]+j + (peices[c][k][1]+i)*LENGTH]=0;
						}
					}

					rotateTet(peices[c]);
				}
			}
		}
	}
	if(bcul==held){
		held=pec;
	}
	c=bcul;
	for(int i=0;i<br;i++){
		rotateTet(peices[c]);
	}
#if SLOW
	for(int k=0;k<4;k++){
		temps[k]=board[peices[c][k][0]+bx + (peices[c][k][1]+HEIGHT-2)*LENGTH];
		board[peices[c][k][0]+bx + (peices[c][k][1]+HEIGHT-2)*LENGTH]=c+1;
		drawSquare(bx+peices[c][k][0],HEIGHT-2+peices[c][k][1],cols[c+1]);
	}
	glFlush();
	usleep(macsec);
	for(int k=0;k<4;k++){
		board[(HEIGHT-2+peices[c][k][1])*LENGTH+(bx+peices[c][k][0])] = temps[k];
		drawSquare(bx+peices[c][k][0],HEIGHT-2+peices[c][k][1],cols[temps[k]]);
		board[peices[c][k][0]+bx + (peices[c][k][1]+by)*LENGTH]=c+1;
    } 
	glFlush();
	usleep(macsec);
#else
	for(int k=0;k<4;k++){
		board[peices[c][k][0]+bx + (peices[c][k][1]+by)*LENGTH]=c+1;
    }
#endif
	int killR=0;
	i8 t;
#if IMG
	for(int i=fHeight+1;i<HEIGHT;i++){
#else
	for(int i=0;i<HEIGHT;i++){
#endif
		bool fullR=true;
		for(int j=0;j<LENGTH;j++){
			fullR&=board[i*LENGTH +j]!=0;
			t = board[i*LENGTH+j];
			board[i*LENGTH +j]=0;
			board[(i-killR)*LENGTH +j]=t;
		}
		if(fullR){
#if IMG
			fHeight+=1;
			for(int j=0;j<LENGTH;j++){
				fboard[i*LENGTH+j]=(fboard[i*LENGTH+j]!=0) * board[i*LENGTH+j]+(board[i*LENGTH+j]==0);
			}
#else
			killR++;
#endif
		}

	}
	
	draw();
#if SLOW
	usleep(macsec);
#endif
	glFlush();
}

int main(int argc, char** argv) {
#if IMG	
	ifstream f;
	f.open("base.txt",ios::in);
	f.seekg (0, f.end);
    int l = f.tellg();
    f.seekg (0, f.beg);
	char inp;
	int lines=0;
	fboard = new int[l]{0};
	int i=0;
	while(not f.eof()){
		f.read(&inp,1);	
		if(inp=='\n'){lines++;}
		if(inp=='o'){fboard[i]=1;i++;}
		if(inp==' '){i++;}
	}
	f.close();
	HEIGHT=lines+3;
	LENGTH=l / lines
             + (((l < 0) ^ (lines > 0)) && (l%lines))-1;
	cout << "the psudeo-constants LENGTH and HEIGHT have changed"<<endl<<"LENGTH: "<<LENGTH<<", HEIGHT:" <<HEIGHT<<endl;
#endif


	cols = new float*[8];
	cols[blank] = new float[3]{0,0,0};
	cols[cyan] = new float[3]{0,1,1};
	cols[yellow] = new float[3]{1,1,0};
	cols[purple] = new float[3]{1,0,1};
	cols[green] = new float[3]{0,1,0};
	cols[red] = new float[3]{1,0,0};
	cols[blue] = new float[3]{0,0,1};
	cols[orange] = new float[3]{1,0.5,0};
	
	board = new i8[HEIGHT*LENGTH]{0};
    
	peices[0] = new int[4][2]{ //line
		{-1,0},
		{0,0},
		{1,0},
		{2,0}
	};
	peices[1] = new int[4][2]{ //sqaure
		{0,0},
		{0,1},
		{1,0},
		{1,1}
	};
	peices[2] = new int[4][2]{ //T
		{0,0},
		{-1,0},
		{1,0},
		{0,1}
	};
	peices[3] = new int[4][2]{ //S
		{-1,0},
		{0,0},
		{0,1},
		{1,1}
	};
	peices[4] = new int[4][2]{ //Z
		{1,0},
		{0,0},
		{0,1},
		{-1,1}
	};
	peices[5] = new int[4][2]{ //L
		{0,-1},
		{0,0},
		{1,1},
		{0,1}
	};
	peices[6] = new int[4][2]{ //J
		{0,-1},
		{0,0},
		{-1,1},
		{0,1}
	};

	srand (time(NULL));
	glutInit(&argc, argv);		// Initialize GLUT
    glutInitWindowSize((LENGTH*20) + 20*BUFFER, (HEIGHT*20)+20*BUFFER);   // Set the window's initial width & height
    glutCreateWindow("Tetris"); // Create a window with the given title
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(disInit); // Register display callback handler for window re-paint
	glutIdleFunc(run);
	glutMainLoop();
	return 0;
}
