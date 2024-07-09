#include <iostream>
#include <GL/glut.h>
#include <time.h> 
#include <stdlib.h>
using namespace std;

const int HEIGHT=22;
const int LENGTH=10;
const int BUFFER=2;
typedef char i8;
i8* board;
//board array is of the form board[h*LENGTH+l] 
//where h is height of block and l is length of block

float** cols;
int (*peices[7])[2];	// array of 7, 1 per peice,
						//	thease point to a 2d array each of the form [4][2]
						// thease 2d arrays are of the form arr[x][y] where you are getting the position of the xth peice where y=0 is the x position and y=1 is the y position

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
	xf = xf/(LENGTH/2.0 + BUFFER/2.0) +xd*BUFFER/2.0;
	yf = yf/(HEIGHT/2.0 + BUFFER/2.0) +yd*BUFFER/2.0;
    glVertex2d(xf + xd, yf + yd);
    glVertex2d(xf + xd, yf - yd);
    glVertex2d(xf - xd, yf - yd);
    glVertex2d(xf - xd, yf + yd);

    glEnd();
}

void draw(){
	for (int i =0;i<HEIGHT;i++){
		for (int j =0;j<LENGTH;j++){
			drawSquare(j,i,cols[board[(i*LENGTH) +j]]);
		}
	}
	
	
}

void disInit(){
	glClearColor(0.0f, 0.0f, 0.0f, 1); // Set background color to black and opaque
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	glFlush();
}

void run() {
	//draw();
    //glFlush();  // Render now
	int y{10},x{LENGTH/2},c{rand()%orange};
	for(int i=0;i<4;i++){
		*(board+((y+peices[c][i][1])*LENGTH)+x+peices[c][i][0]) = c+1;
		drawSquare(x+peices[c][i][0],y+peices[c][i][1],cols[c+1]);
	}
	glFlush();
	int bx,by,br; //best x best y and best rotation (rotation stored as r where toation is r*pi/2)
	for(int i=0;i<HEIGHT;i++){
		for (int j=0;j<LENGTH;j++){
			for (int r=0;r<4;r++){
				bool fits = true;
				for(int k =0;k<4;k++){
					fits &= peices[c][k][0]+j >0 && peices[c][k][1]+i >0 && peices[c][k][0]+j<LENGTH && peices[c][k][1]+i<HEIGHT;
					if(!fits){break;}
					fits = board[peices[c][k][0]+j + (peices[c][k][1]+i)*LENGTH]==0;
				}
				rotateTet(peices[c]);
			}
		}
	}

}

int main(int argc, char** argv) {
	cols = new float*[8];
	cols[blank] = new float[3]{0,0,0};
	cols[cyan] = new float[3]{0,1,1};
	cols[yellow] = new float[3]{1,1,0};
	cols[purple] = new float[3]{1,0,1};
	cols[green] = new float[3]{0,1,0};
	cols[red] = new float[3]{1,0,0};
	cols[blue] = new float[3]{0,0,1};
	cols[orange] = new float[3]{1,0.5,0};
	
	board = new i8[HEIGHT*LENGTH];
    
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
	//for(int i=0;i<LENGTH;i++){
	//	board[i]=(i+1)%8;
	//	board[i*LENGTH]= (i+1)%8;
	//}
	//int x{5},y{5},c{blue};
	//rotateTet(peices[c-1]);
	//for(int i=0;i<4;i++){
	//	*(board+((y+peices[c-1][i][1])*LENGTH)+x+peices[c-1][i][0]) = c;
//		cout << peices[c-1][i][0] + x << "," << peices[c-1][i][1] + y << endl;
	//}
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
