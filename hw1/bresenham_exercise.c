/*********************************
 * Ergasia 1 – Askhsh 2 – 10.01.2022
 * Choropanitis Paschalis - 2453
 *********************************/

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

// comment the line below for switching antialiasing off
#define ANTIALIASING_ON

enum MENU_TYPE {
    LINE,
    ELLIPSE,
    CLEAR_SCREEN,
    QUIT,
};

enum COLOR_TYPE {
    RED_COLOR,
    GREEN_COLOR,
    BLUE_COLOR,
};

int winID; // windows ID for destroying window
/* odd is the variable that measures the clicks of the user,
  depending at the mode selected by the menu,
  and do stuff accordingly in the mouse function */
int odd = 1, mode = 1;

int width = WIDTH;
int height = HEIGHT;
// x, y mouse coordinates of the clicked locations
int xm, ym, xm1, ym1, xm2, ym2;
int a, b;

float red = 1.0, green = 0.0, blue = 0.0;

// draw pixel with intensity by its distance to the line
void draw_pixel_antialiased(int x, int y, float R, float G, float B, float A) {    
    float r, g, b;

    r = (R * (1 - A / 1.5));
    g = (G * (1 - A / 1.5));
    b = (B * (1 - A / 1.5));
    
    glBegin(GL_POINTS);
    glColor3f(r, g, b);
    glVertex2i(x, y);
    glEnd();
}

void draw_pixel(int x, int y, float R, float G, float B, float A) {
    glBegin(GL_POINTS);
    glColor3f(A*R, A*G, A*B);
    glVertex2i(x, y);
    glEnd();
}

void draw_line(int x1, int x2, int y1, int y2) {
    int i, p;
    int stepx, stepy, inc1, inc2;

    float d = 0, sin_a, cos_a, sin_cos_a, denom;

    int x = x1;
    int y = y1;

    int dx = x2-x1;
    int dy = y2-y1;

    if (dy < 0) {
        dy = -dy;
        stepy = -1;
    } else stepy = 1;
    if (dx < 0) {
        dx = -dx;
        stepx = -1;
    } else stepx = 1;

    if (dx > dy) {
        draw_pixel(x, y, red, green, blue, 1.0); // start pixel
        p = 2*dy - dx; // initial value of the decision parameter
        inc1 = 2*(dy - dx);
        inc2 = 2*dy;

        denom = (float) sqrt((double) (dx * dx + dy * dy));
        sin_a = dy / denom;
        cos_a = dx / denom;
        sin_cos_a = sin_a - cos_a;

        for (i=0; i < dx; i++) {
            if (p >= 0) {
                y += stepy;
                p += inc1;
                d += sin_cos_a;
            }
            else {
                p += inc2;
                d += sin_a;
            }
            x += stepx;

            #ifdef ANTIALIASING_ON
            draw_pixel_antialiased(x, y, red, green, blue, fabs(d));
            draw_pixel_antialiased(x, y + 1, red, green, blue, fabs(d - cos_a));
            draw_pixel_antialiased(x, y - 1, red, green, blue, fabs(d + cos_a));
            #else
            draw_pixel(x, y, red, green, blue, 1.0);
            #endif
        }
    } else {
        draw_pixel(x, y, red, green, blue, 1.0); // start pixel
        p = 2*dx - dy;
        inc1 = 2*(dx - dy);
        inc2 = 2*dx;

        denom = (float) sqrt((double) (dx * dx + dy * dy));
        sin_a = dx / denom;
        cos_a = dy / denom;
        sin_cos_a = sin_a - cos_a;

        for (i=0; i < dy; i++) {
            if (p >= 0) {
                x += stepx;
                p += inc1;
                d += sin_cos_a;
            }
            else {
                p += inc2;
                d += sin_a;
            }
            y += stepy;

            #ifdef ANTIALIASING_ON
            draw_pixel_antialiased(x, y, red, green, blue, fabs(d));
            draw_pixel_antialiased(x + 1, y, red, green, blue, fabs(d - cos_a));
            draw_pixel_antialiased(x - 1, y, red, green, blue, fabs(d + cos_a));
            #else
            draw_pixel(x, y, red, green, blue, 1.0);
            #endif
        }
    }
}

void draw_ellipse(int cx, int cy, int xradius, int yradius) {
    int x, y;
    int xchange, ychange;
    int error;
    int twoasquare, twobsquare;
    int stopx, stopy;

    twoasquare = 2*xradius*xradius;
    twobsquare = 2*yradius*yradius;

    x = xradius;
    y = 0;
    xchange = yradius*yradius*(1 - 2*xradius);
    ychange = xradius*xradius;
    error = 0;
    stopx = twobsquare*xradius;
    stopy = 0;

    while (stopx >= stopy) { // first set of points, y' > -1
        draw_pixel(cx+x, cy+y, red, green, blue, 1.0);
        draw_pixel(cx-x, cy+y, red, green, blue, 1.0);
        draw_pixel(cx-x, cy-y, red, green, blue, 1.0);
        draw_pixel(cx+x, cy-y, red, green, blue, 1.0);
        y++;
        stopy = stopy + twoasquare;
        error = error + ychange;
        ychange = ychange + twoasquare;
        if ((2*error + xchange) > 0) {
            x--;
            stopx = stopx - twobsquare;
            error = error + xchange;
            xchange = xchange + twobsquare;
        }
    }

    y = yradius;
    x = 0;
    ychange = xradius*xradius*(1 - 2*yradius);
    xchange = yradius*yradius;
    error = 0;
    stopy = twoasquare*yradius;
    stopx = 0;

    while (stopy >= stopx) { // second set of points, y' < -1
        draw_pixel(cx+x, cy+y, red, green, blue, 1.0);
        draw_pixel(cx-x, cy+y, red, green, blue, 1.0);
        draw_pixel(cx-x, cy-y, red, green, blue, 1.0);
        draw_pixel(cx+x, cy-y, red, green, blue, 1.0);
        x++;
        stopx = stopx + twobsquare;
        error = error + xchange;
        xchange = xchange + twobsquare;
        if ((2*error + ychange) > 0) {
            y--;
            stopy = stopy - twoasquare;
            error = error + ychange;
            ychange = ychange + twoasquare;
        }
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
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluOrtho2D(-(width/2), (width/2), -(height/2), (height/2));
    // draw axis	
    for (int i =- (width/2); i < (width/2); i=i+10)
    {
        draw_pixel(i, 0, 1.0, 1.0, 1.0, 1.0);
        draw_pixel(0, i, 1.0, 1.0, 1.0, 1.0);
    }

    show(-(width/2),0,"- X-AXIS");
    show((width/2)-50,0,"X-AXIS");
    show(0,(height/2)-10,"Y-AXIS");
    show(0,-(height/2)+5,"- Y-AXIS");    
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
        glutDestroyWindow(winID);
        break;
    default:
        break;
    }
}

void myMouse(int button, int state, GLint x, GLint y) {
    char coords[14];

    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN) {
        if (mode == 1) {
            // first mode is the line
            // if we are at the second click of the user we draw the line
            if (odd % 2 == 0) {
                xm1 = x - (width/2);
                ym1 = (height/2) - y;
                sprintf(coords, "X=%d,Y=%d", xm1, ym1);
                show(xm1+5, ym1, coords);
                draw_pixel(xm, ym, red, green, blue, 1.0);
                draw_line(xm, xm1, ym, ym1);
                glFlush();
            }
            else {
                xm = x - (width/2);
                ym = (height/2) - y;
                sprintf(coords, "X=%d,Y=%d", xm, ym);
                show(xm+5, ym, coords);
                draw_pixel(xm, ym, red, green, blue, 1.0);
                glFlush();
            }
            odd++;
        }
        else {
            // second mode is the ellipse
            // if we are at the third click of the user we draw the ellipse
            if (odd == 3) {
                // get b location (y-axis of the ellipse)
                xm2 = x - (width/2);
                ym2 = (height/2) - y;

                // calculate b disance from the center
                b = sqrt(pow(xm2 - xm, 2) + pow(ym2 - ym, 2) * 1.0);
                sprintf(coords, "B=%d", b);
                show(xm2+5, ym2, coords);
                draw_pixel(xm2, ym2, red, green, blue, 1.0);
                draw_ellipse(xm, ym, a, b);
                glFlush();
                odd = 1;
                return;
            }
            else if (odd == 2) {
                // get a location (x-axis of the ellipse)
                xm1 = x - (width/2);
                ym1 = (height/2) - y;

                // calculate a disance from the center
                a = sqrt(pow(xm1 - xm, 2) + pow(ym1 - ym, 2) * 1.0);
                sprintf(coords, "A=%d", a);
                show(xm1+5, ym1, coords);
                draw_pixel(xm1, ym1, red, green, blue, 1.0);
                glFlush();
            }
            else {
                // get the center of the eclipse
                xm = x - (width/2);
                ym = (height/2) - y;
                sprintf(coords, "X=%d,Y=%d", xm, ym);
                show(xm+5, ym, coords);
                draw_pixel(xm, ym, red, green, blue, 1.0);
                glFlush();
            }
            odd++;
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
        case LINE: // drawing  line
            mode = 1;
            odd = 1;
            break;

        case ELLIPSE: // drawing ellipse
            mode = 2;
            odd = 1;
            break;

        case CLEAR_SCREEN:
            resetDisplay();
            break;

        case QUIT:
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
    glutAddMenuEntry("Line", LINE);
    glutAddMenuEntry("Ellipse", ELLIPSE);
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
    winID = glutCreateWindow("Bresenham Exercise");
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