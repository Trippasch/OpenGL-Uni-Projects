/*********************************
 * Ergasia 1 – Askhsh 4 – 12.01.2022
 * Choropanitis Paschalis - 2453
 *********************************/

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600
#define MAXEDGES 10
#define MAXVER 10000

enum MENU_TYPE {
    POLYGON,
    FILL,
    CLEAR_SCREEN,
    QUIT,
};

enum COLOR_TYPE {
    RED_COLOR,
    GREEN_COLOR,
    BLUE_COLOR,
};

typedef struct edgelist {
    int ymax;       // max y-coord of edge
    float currentx; // x-coord of lowest edge point
    float slopeinverse;
} EdgeList;

typedef struct edgetabletup {
    int countEdgeList; // count of edgelists
    EdgeList lists[MAXVER];
} EdgeTableTuple;

EdgeTableTuple *EdgeTable, ActiveEdgeTuple;

typedef struct edgecoords {
    int x;
    int y;
} EdgeCoord;

EdgeCoord EdgeCoordinates[MAXEDGES];

int winID; // windows ID for destroying window
/* odd is the variable that measures the clicks of the user,
  depending at the mode selected by the menu,
  and do stuff accordingly in the mouse function */
int odd = 1, mode = 0;

int width = WIDTH;
int height = HEIGHT;
// x, y mouse coordinates of the clicked locations
int xm, ym;

float red = 1.0, green = 0.0, blue = 0.0;

void draw_pixel(int x, int y, float R, float G, float B, float e) {
    glBegin(GL_POINTS);
    glColor3f(e*R, e*G, e*B);
    glVertex2i(x, y);
    glEnd();
}

void initEdgeTable() {
    for (int i = 0; i < height; i++) {
        EdgeTable[i].countEdgeList = 0;
    }

    ActiveEdgeTuple.countEdgeList = 0;
}

// Function to sort an array using insertion sort
void insertionSort(EdgeTableTuple *ett) {
    int i,j;
    EdgeList temp;
 
    for (i = 1; i < ett->countEdgeList; i++)
    {
        temp.ymax = ett->lists[i].ymax;
        temp.currentx = ett->lists[i].currentx;
        temp.slopeinverse = ett->lists[i].slopeinverse;
        j = i - 1;
 
        while ((temp.currentx < ett->lists[j].currentx) && (j >= 0))
        {
            ett->lists[j + 1].ymax = ett->lists[j].ymax;
            ett->lists[j + 1].currentx = ett->lists[j].currentx;
            ett->lists[j + 1].slopeinverse = ett->lists[j].slopeinverse;
            j = j - 1;
        }
        ett->lists[j + 1].ymax = temp.ymax;
        ett->lists[j + 1].currentx = temp.currentx;
        ett->lists[j + 1].slopeinverse = temp.slopeinverse;
    }
}

void storeEdgeInTuple (EdgeTableTuple *receiver,int y,int x,float slopInv) {
    // both used for edgetable and active edge table..
    // The edge tuple sorted in increasing ymax and x of the lower end.
    (receiver->lists[(receiver)->countEdgeList]).ymax = y;
    (receiver->lists[(receiver)->countEdgeList]).currentx = (float)x;
    (receiver->lists[(receiver)->countEdgeList]).slopeinverse = slopInv;

    // sort the buckets
    insertionSort(receiver);

    (receiver->countEdgeList)++;
}

void storeEdgeInTable (int x1, int y1, int x2, int y2) {
    float m, minv;
    int ymaxTS, xwithyminTS, scanline; //ts stands for to store
     
    if (x2 == x1) {
        minv=0.000000;
    }
    else {
        m = ((float)(y2 - y1)) / ((float)(x2 - x1));
     
        // horizontal lines are not stored in edge table
        if (y2 == y1)
            return;
            
        minv = (float)1.0 / m;
    }
     
    if (y1 > y2) {
        scanline = y2;
        ymaxTS = y1;
        xwithyminTS = x2;
    }
    else {
        scanline = y1;
        ymaxTS = y2;
        xwithyminTS = x1;    
    }
    // the assignment part is done..now storage..
    storeEdgeInTuple(&EdgeTable[scanline],ymaxTS,xwithyminTS,minv);
}

void removeEdgeByYmax(EdgeTableTuple *Tup, int yy) {
    for (int i = 0; i < Tup->countEdgeList; i++) {
        if (Tup->lists[i].ymax == yy) {
            for (int j = i; j < Tup->countEdgeList - 1; j++) {
                Tup->lists[j].ymax =Tup->lists[j+1].ymax;
                Tup->lists[j].currentx =Tup->lists[j+1].currentx;
                Tup->lists[j].slopeinverse = Tup->lists[j+1].slopeinverse;
            }
            Tup->countEdgeList--;
            i--;
        }
    }
}  

void updatexbyslopeinv(EdgeTableTuple *Tup) {
    for (int i=0; i < Tup->countEdgeList; i++) {
        (Tup->lists[i]).currentx =(Tup->lists[i]).currentx + (Tup->lists[i]).slopeinverse;
    }
}

void scanlineFill() { 
    int j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;
     
    // we will start from scanline 0;
    // Repeat until last scanline:
    for (int i = 0; i < height; i++) { // Increment y by 1 (next scan line)
         
        // Move from ET bucket y to the
        // AET those edges whose ymin = y (entering edges)
        for (j=0; j<EdgeTable[i].countEdgeList; j++)
        {
            storeEdgeInTuple(&ActiveEdgeTuple,EdgeTable[i].lists[j].
                     ymax,EdgeTable[i].lists[j].currentx,
                    EdgeTable[i].lists[j].slopeinverse);
        }
         
        // Remove from AET those edges for
        // which y=ymax (not involved in next scan line)
        removeEdgeByYmax(&ActiveEdgeTuple, i);
         
        //sort AET (remember: ET is presorted)
        insertionSort(&ActiveEdgeTuple);
         
        // Fill lines on scan line y by using pairs of x-coords from AET
        j = 0;
        FillFlag = 0;
        coordCount = 0;
        x1 = 0;
        x2 = 0;
        ymax1 = 0;
        ymax2 = 0;

        while (j < ActiveEdgeTuple.countEdgeList) {
            if (coordCount%2==0) {
                x1 = (int)(ActiveEdgeTuple.lists[j].currentx);
                ymax1 = ActiveEdgeTuple.lists[j].ymax;

                if (x1==x2) {
                /* three cases can arrive-
                    1. lines are towards top of the intersection
                    2. lines are towards bottom
                    3. one line is towards top and other is towards bottom
                */
                    if (((x1 == ymax1) && (x2 != ymax2))||
                        ((x1 != ymax1) && (x2 == ymax2))) {
                        x2 = x1;
                        ymax2 = ymax1;
                    }
                    else {
                        coordCount++;
                    }
                }
                else {
                    coordCount++;
                }
            }
            else {
                x2 = (int)ActiveEdgeTuple.lists[j].currentx;
                ymax2 = ActiveEdgeTuple.lists[j].ymax;
             
                FillFlag = 0;
                 
                // checking for intersection...
                if (x1 == x2) {
                /*three cases can arrive-
                    1. lines are towards top of the intersection
                    2. lines are towards bottom
                    3. one line is towards top and other is towards bottom
                */
                    if (((x1 == ymax1) && (x2 != ymax2))||
                        ((x1 != ymax1) && (x2 == ymax2))) {
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else {
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else {
                    coordCount++;
                    FillFlag = 1;
                }
                if(FillFlag)
                {
                    //drawing actual lines...
                    glColor3f(red, green, blue);
                    
                    glBegin(GL_LINES);
                    glVertex2i(x1,i);
                    glVertex2i(x2,i);
                    glEnd();
                    glFlush();
                }
            }
            j++;
        }
        // For each nonvertical edge remaining in AET, update x for new y
        updatexbyslopeinv(&ActiveEdgeTuple);
    }
    printf("\nScanline filling complete\n");
    free(EdgeTable);
}

void drawPolygon() {
    EdgeTable = (EdgeTableTuple *) malloc (height * sizeof(EdgeTableTuple));
    initEdgeTable();

    glColor3f(red, green, blue);
    int count = 0, x0, y0, x1, y1, x2, y2;

    for (int i = 0; i < odd; i++) {

        if (i == 0) {
            x0 = EdgeCoordinates[i].x;
            y0 = EdgeCoordinates[i].y;
        }
        else if (i == odd - 1) {
            x1 = x2;
            y1 = y2;
            x2 = x0;
            y2 = y0;
            storeEdgeInTable(x1, y1, x2, y2); //storage of edges in edge table.
            break;
        }

        if (count > 1) {
            x1 = x2;
            y1 = y2;
            count = 1;
        }
        if (count == 0) {
            x1 = EdgeCoordinates[i].x;
            y1 = EdgeCoordinates[i].y;
        }
        else {
            x2 = EdgeCoordinates[i].x;
            y2 = EdgeCoordinates[i].y;
            glBegin(GL_LINES);
            glVertex2i(x1, y1);
            glVertex2i(x2, y2);
            glEnd();
            if (i == odd - 2) {
                glBegin(GL_LINES);
                glVertex2i(x2, y2);
                glVertex2i(x0, y0);
                glEnd();
            }
            storeEdgeInTable(x1, y1, x2, y2); //storage of edges in edge table.
            glFlush();
        }
        count++;
    }
}

void *font = GLUT_BITMAP_TIMES_ROMAN_10;

void show(int x, int y, char *string) {
    glColor3f(1.0, 1.0, 1.0);
    int len, i;
    glRasterPos2f(x, y);
    len = (int) strlen(string);
    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, string[i]);
}

// reset the display and draw the axis
void resetDisplay() {
    odd = 1;
    mode = 0;
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluOrtho2D(0, width, 0, height);
    // draw axis	
    for (int i = 0; i < width; i=i+10)
    {
        draw_pixel(i, height/2, 1.0, 1.0, 1.0, 1.0);
        draw_pixel(width/2, i, 1.0, 1.0, 1.0, 1.0);
    }

    show(0,(height/2),"- X-AXIS");
    show(width-40,(height/2),"X-AXIS");
    show((width/2),height - 10,"Y-AXIS");
    show((width/2),0,"- Y-AXIS");
    glutSwapBuffers();
    glFlush();
}

// update width and height values when on reshape
void myReshape(int w, int h) {
    width = w;
    height = h;
    resetDisplay();
}

void myDisplay() {
}

void keyEvent(unsigned char key, int x, int y) {
    switch (key) {
    // Quit
    case 'Q':
    case 'q':
        EdgeTable = NULL;
        free(EdgeTable);
        glutDestroyWindow(winID);
        break;
    default:
        break;
    }
}

void myMouse(int button, int state, GLint x, GLint y) {
    char coords[14];

    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN) {
        if (mode == 0) {
            if (odd <= MAXEDGES) {
                xm = x;
                ym = height - y;
                sprintf(coords, "X=%d,Y=%d", xm - (width/2), ym - (height/2));
                show(xm+5, ym, coords);
                draw_pixel(xm, ym, red, green, blue, 1.0);
                glFlush();
                EdgeCoordinates[odd - 1].x = xm;
                EdgeCoordinates[odd - 1].y = ym;
                odd++;
            }
        }
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        resetDisplay();
    }
}

void mySubMenu(int selection) {
    switch (selection) {
        case RED_COLOR:
            red = 1.0;
            green = 0.0;
            blue = 0.0;
            break;

        case GREEN_COLOR:
            red = 0.0;
            green = 1.0;
            blue = 0.0;
            break;

        case BLUE_COLOR:
            red = 0.0;
            green = 0.0;
            blue = 1.0;
            break;
    
        default:
            printf("Invalid menu selection\n");
            break;
    }
}

void myMenu(int selection) {
    switch(selection) {
        case POLYGON: // draw polygon if there are at least 3 edges
            if (odd > 3) {
                drawPolygon();
                mode = 1;
            }
            break;

        case FILL: // fill polygon if there are at least 3 edges drawed
            if (odd > 3 && mode == 1) {
                scanlineFill(); // actual calling of scanline filling..
                odd = 1;
                mode = 0;
            }
            break;

        case CLEAR_SCREEN:
            resetDisplay();
            break;

        case QUIT:
            EdgeTable = NULL;
            free(EdgeTable);
            glutDestroyWindow(winID);
            break;

        default:
            printf("Invalid menu selection\n");
            break;
	}
}

void createMenu() {
    int lineColorSubmenu;

    // Create a two-level pop-up menu
    lineColorSubmenu = glutCreateMenu(mySubMenu);
    glutAddMenuEntry("Red", RED_COLOR);
    glutAddMenuEntry("Green", GREEN_COLOR);
    glutAddMenuEntry("Blue", BLUE_COLOR);

    // menu creation
    glutCreateMenu(myMenu);
    glutAddMenuEntry("Draw Polygon", POLYGON);
    glutAddMenuEntry("Fill Polygon", FILL);
    glutAddSubMenu("Color", lineColorSubmenu);
    glutAddMenuEntry("Clear Screen", CLEAR_SCREEN);
    glutAddMenuEntry("Quit", QUIT);

    // register menu to right mouse button
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void init() {
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    winID = glutCreateWindow("Polygon Exercise");
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    init();
    createMenu();
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutMouseFunc(myMouse);
    glutKeyboardFunc(keyEvent);
    glutMainLoop();

    return 0;
}