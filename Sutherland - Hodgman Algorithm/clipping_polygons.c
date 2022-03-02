/*********************************
 * Ergasia 2 – Askhsh 6 – 23.02.2022
 * Choropanitis Paschalis - 2453
 *********************************/

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600
#define MAXEDGES 20

typedef struct coords {
    float x;
    float y;
} PointCoord;

PointCoord EdgeCoords[MAXEDGES];
PointCoord IntersectionCoords[MAXEDGES];
// start and end coordinates of the clipping plane
PointCoord start, end;
// x, y mouse coordinates of the clicked locations
PointCoord mouseCoords;

int winID; // windows ID for destroying window
/* odd is the variable that measures the clicks of the user,
  depending at the mode selected by the menu,
  and do stuff accordingly in the mouse function */
int odd = 0, mode = 0;
/* variable that controls if the clipping plane
should be toggled or not */
int toggle = 0;

int width = WIDTH;
int height = HEIGHT;

float red = 1.0, green = 0.0, blue = 0.0;

void leftClipper() {
    int i = 0, j = 0;

    for (i = 0; i < odd; i++) {
        // Case-1:	outside to inside
        if (EdgeCoords[i].x < start.x && EdgeCoords[i + 1].x >= start.x) {
            if (EdgeCoords[i + 1].x - EdgeCoords[i].x != 0) {
                // save point of intersection
                IntersectionCoords[j].y = (EdgeCoords[i + 1].y - EdgeCoords[i].y) /
                (EdgeCoords[i + 1].x - EdgeCoords[i].x) *
                (start.x - EdgeCoords[i].x) + EdgeCoords[i].y;
            }
            else {
                IntersectionCoords[j].y = EdgeCoords[i].y;
            }
            IntersectionCoords[j].x = start.x;
            j++;
            // save the point that lie inside the clipping plane
            IntersectionCoords[j].x = EdgeCoords[i + 1].x;
            IntersectionCoords[j].y = EdgeCoords[i + 1].y;
            j++;
        }

        // Case-2: inside to inside
        if (EdgeCoords[i].x >= start.x && EdgeCoords[i + 1].x >= start.x) {
            // only save second point that lie inside our clipping plane
            IntersectionCoords[j].y = EdgeCoords[i + 1].y;
            IntersectionCoords[j].x = EdgeCoords[i + 1].x;
            j++;
        }

        // Case-3: inside to outside
        if (EdgeCoords[i].x >= start.x && EdgeCoords[i + 1].x < start.x) {
            if (EdgeCoords[i + 1].x - EdgeCoords[i].x != 0) {
                IntersectionCoords[j].y = (EdgeCoords[i + 1].y - EdgeCoords[i].y) /
                (EdgeCoords[i + 1].x - EdgeCoords[i].x) *
                (start.x - EdgeCoords[i].x) + EdgeCoords[i].y;
            }
            else {
                IntersectionCoords[j].y = EdgeCoords[i].y;
            }
            IntersectionCoords[j].x = start.x;
            j++;
        }
    }

    for (i = 0; i < j; i++) {
        EdgeCoords[i].x = IntersectionCoords[i].x;
        EdgeCoords[i].y = IntersectionCoords[i].y;
    }
    EdgeCoords[i].x = IntersectionCoords[0].x;
    EdgeCoords[i].y = IntersectionCoords[0].y;
    odd = j;
}

void rightClipper() {
    int i = 0, j = 0;

    for (i = 0; i < odd; i++) {
        // Case-1:	outside to inside
        if (EdgeCoords[i].x > end.x && EdgeCoords[i + 1].x <= end.x) {
            if (EdgeCoords[i + 1].x - EdgeCoords[i].x != 0) {
                // save point of intersection
                IntersectionCoords[j].y = (EdgeCoords[i + 1].y - EdgeCoords[i].y) /
                (EdgeCoords[i + 1].x - EdgeCoords[i].x) *
                (end.x - EdgeCoords[i].x) + EdgeCoords[i].y;
            }
            else {
                IntersectionCoords[j].y = EdgeCoords[i].y;
            }
            IntersectionCoords[j].x = end.x;
            j++;
            // save the point that lie inside the clipping plane
            IntersectionCoords[j].x = EdgeCoords[i + 1].x;
            IntersectionCoords[j].y = EdgeCoords[i + 1].y;
            j++;
        }

        // Case-2: inside to inside
        if (EdgeCoords[i].x <= end.x && EdgeCoords[i + 1].x <= end.x) {
            // only save second point that lie inside our clipping plane
            IntersectionCoords[j].y = EdgeCoords[i + 1].y;
            IntersectionCoords[j].x = EdgeCoords[i + 1].x;
            j++;
        }

        // Case-3: inside to outside
        if (EdgeCoords[i].x <= end.x && EdgeCoords[i + 1].x > end.x) {
            if (EdgeCoords[i + 1].x - EdgeCoords[i].x != 0) {
                IntersectionCoords[j].y = (EdgeCoords[i + 1].y - EdgeCoords[i].y) /
                (EdgeCoords[i + 1].x - EdgeCoords[i].x) *
                (end.x - EdgeCoords[i].x) + EdgeCoords[i].y;
            }
            else {
                IntersectionCoords[j].y = EdgeCoords[i].y;
            }
            IntersectionCoords[j].x = end.x;
            j++;
        }
    }

    for (i = 0; i < j; i++) {
        EdgeCoords[i].x = IntersectionCoords[i].x;
        EdgeCoords[i].y = IntersectionCoords[i].y;
    }
    EdgeCoords[i].x = IntersectionCoords[0].x;
    EdgeCoords[i].y = IntersectionCoords[0].y;
    odd = j;
}

void topClipper() {
    int i = 0, j = 0;

    for (i = 0; i < odd; i++) {
        // Case-1:	outside to inside
        if (EdgeCoords[i].y > end.y && EdgeCoords[i + 1].y <= end.y) {
            if (EdgeCoords[i + 1].y - EdgeCoords[i].y != 0) {
                // save point of intersection
                IntersectionCoords[j].x = (EdgeCoords[i + 1].x - EdgeCoords[i].x) /
                (EdgeCoords[i + 1].y - EdgeCoords[i].y) *
                (end.y - EdgeCoords[i].y) + EdgeCoords[i].x;
            }
            else {
                IntersectionCoords[j].x = EdgeCoords[i].x;
            }
            IntersectionCoords[j].y = end.y;
            j++;
            // save the point that lie inside the clipping plane
            IntersectionCoords[j].x = EdgeCoords[i + 1].x;
            IntersectionCoords[j].y = EdgeCoords[i + 1].y;
            j++;
        }

        // Case-2: inside to inside
        if (EdgeCoords[i].y <= end.y && EdgeCoords[i + 1].y <= end.y) {
            // only save second point that lie inside our clipping plane
            IntersectionCoords[j].y = EdgeCoords[i + 1].y;
            IntersectionCoords[j].x = EdgeCoords[i + 1].x;
            j++;
        }

        // Case-3: inside to outside
        if (EdgeCoords[i].y <= end.y && EdgeCoords[i + 1].y > end.y) {
            if (EdgeCoords[i + 1].y - EdgeCoords[i].y != 0) {
                IntersectionCoords[j].x = (EdgeCoords[i + 1].x - EdgeCoords[i].x) /
                (EdgeCoords[i + 1].y - EdgeCoords[i].y) *
                (end.y - EdgeCoords[i].y) + EdgeCoords[i].x;
            }
            else {
                IntersectionCoords[j].x = EdgeCoords[i].x;
            }
            IntersectionCoords[j].y = end.y;
            j++;
        }
    }

    for (i = 0; i < j; i++) {
        EdgeCoords[i].x = IntersectionCoords[i].x;
        EdgeCoords[i].y = IntersectionCoords[i].y;
    }
    EdgeCoords[i].x = IntersectionCoords[0].x;
    EdgeCoords[i].y = IntersectionCoords[0].y;
    odd = j;
}

void bottomClipper() {
    int i = 0, j = 0;

    for (i = 0; i < odd; i++) {
        // Case-1:	outside to inside
        if (EdgeCoords[i].y < start.y && EdgeCoords[i + 1].y >= start.y) {
            if (EdgeCoords[i + 1].y - EdgeCoords[i].y != 0) {
                // save point of intersection
                IntersectionCoords[j].x = (EdgeCoords[i + 1].x - EdgeCoords[i].x) /
                (EdgeCoords[i + 1].y - EdgeCoords[i].y) *
                (start.y - EdgeCoords[i].y) + EdgeCoords[i].x;
            }
            else {
                IntersectionCoords[j].x = EdgeCoords[i].x;
            }
            IntersectionCoords[j].y = start.y;
            j++;
            // save the point that lie inside the clipping plane
            IntersectionCoords[j].x = EdgeCoords[i + 1].x;
            IntersectionCoords[j].y = EdgeCoords[i + 1].y;
            j++;
        }

        // Case-2: inside to inside
        if (EdgeCoords[i].y >= start.y && EdgeCoords[i + 1].y >= start.y) {
            // only save second point that lie inside our clipping plane
            IntersectionCoords[j].x = EdgeCoords[i + 1].x;
            IntersectionCoords[j].y = EdgeCoords[i + 1].y;
            j++;
        }

        // Case-3: inside to outside
        if (EdgeCoords[i].y >= start.y && EdgeCoords[i + 1].y < start.y) {
            if (EdgeCoords[i + 1].y - EdgeCoords[i].y != 0) {
                IntersectionCoords[j].x = (EdgeCoords[i + 1].x - EdgeCoords[i].x) /
                (EdgeCoords[i + 1].y - EdgeCoords[i].y) *
                (start.y - EdgeCoords[i].y) + EdgeCoords[i].x;
            }
            else {
                IntersectionCoords[j].x = EdgeCoords[i].x;
            }
            IntersectionCoords[j].y = start.y;
            j++;
        }
    }

    for (i = 0; i < j; i++) {
        EdgeCoords[i].x = IntersectionCoords[i].x;
        EdgeCoords[i].y = IntersectionCoords[i].y;
    }
    EdgeCoords[i].x = IntersectionCoords[0].x;
    EdgeCoords[i].y = IntersectionCoords[0].y;
    odd = j;
}

void drawpolygon()
{
    glColor3f(red, green, blue);
    glBegin(GL_POLYGON);
    for (int i = 0; i < odd; i++) {
        glVertex3d(EdgeCoords[i].x, EdgeCoords[i].y, 1.0);
    }
    glEnd();
    glFlush();
}

void drawClippingPlane() {
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex3f(start.x, start.y, 1.0);
    glVertex3f(end.x, start.y, 1.0);
    glVertex3f(end.x, end.y, 1.0);
    glVertex3f(start.x, end.y, 1.0);
    glEnd();
    glFlush();
}

void toggleClippingPlane() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawpolygon();
    if (toggle % 2 == 0) {
        drawClippingPlane();
    }
}

void redraw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawpolygon();
    drawClippingPlane();
    toggle = 1;
}

void clipPolygon() {
    if (mode == 1) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        leftClipper();
        rightClipper();
        topClipper();
        bottomClipper();
        redraw();
    }
}

void draw_pixel(int x, int y, float R, float G, float B, float e) {
    glBegin(GL_POINTS);
    glColor3f(e*R, e*G, e*B);
    glVertex3i(x, y, 1);
    glEnd();
}

void *font = GLUT_BITMAP_TIMES_ROMAN_10;

void show(int x, int y, char *string) {
    glColor3f(1.0, 1.0, 1.0);
    int len, i;
    glRasterPos3f(x, y, 1.0f);
    len = (int) strlen(string);
    for (i = 0; i < len; i++)
        glutBitmapCharacter(font, string[i]);
}

// reset the display and draw the axis
void resetDisplay() {
    odd = 0;
    mode = 0;
    toggle = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
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

void mySpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_F1:
        if (odd >= 3) {
            drawpolygon();
            mode = 1;
        }
        break;
    default:
        break;
    }
}

void keyEvent(unsigned char key, int x, int y) {
    switch (key) {
    // Toggle Clipping Plane
    case ' ':
        if (mode == 1) {
            toggleClippingPlane();
            toggle++;
        }
        break;
    // Clip
    case 'C':
    case 'c':
        if (toggle == 1) {
            clipPolygon();
        }
        break;
    // Quit
    case 'Q':
    case 'q':
        glutDestroyWindow(winID);
        break;
    default:
        break;
    }
}

// this function takes the mouse position while moving mouse
void myMotion(GLint x, GLint y) {
    if (mode == 1) {
        end.x = x;
        end.y = height - y;
        redraw();
    }
}

void myMouse(int button, int state, GLint x, GLint y) {
    char coords[14];

    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN) {
        if (mode == 0) {
            if (odd < MAXEDGES) {
                mouseCoords.x = x;
                mouseCoords.y = height - y;
                sprintf(coords, "X=%.0f,Y=%.0f", mouseCoords.x - (width/2), mouseCoords.y - (height/2));
                show(mouseCoords.x+5, mouseCoords.y, coords);
                draw_pixel(mouseCoords.x, mouseCoords.y, red, green, blue, 1.0);
                EdgeCoords[odd].x = mouseCoords.x;
                EdgeCoords[odd].y = mouseCoords.y;
                odd++;
            }

            EdgeCoords[odd].x = EdgeCoords[0].x;
            EdgeCoords[odd].y = EdgeCoords[0].y;
        }
        else if (mode == 1) {
            start.x = x;
            start.y = height - y;
        }
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        resetDisplay();
    }

    glFlush();
}

void init() {
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    winID = glutCreateWindow("Clipping Polygons");
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    init();
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutSpecialFunc(mySpecialKeys);
    glutMouseFunc(myMouse);
    glutMotionFunc(myMotion);
    glutKeyboardFunc(keyEvent);
    glutMainLoop();

    return 0;
}