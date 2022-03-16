/*********************************
 * Ergasia 2 – Bonus Project – 25.02.2022
 * Choropanitis Paschalis - 2453
 *********************************/

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

enum MENU_TYPE {
    TOP_VIEW,
    TOP_VIEW_R,
    PERSP_VIEW,
    CLEAR_SCREEN,
    QUIT,
};

enum COLOR_TYPE {
    RED_COLOR,
    GREEN_COLOR,
    BLUE_COLOR,
};

int winID; // windows ID for destroying window

// Window's initial width and height
int width = 640;
int height = 480;
// x, y mouse coordinates of the clicked locations
int xm, ym;
int roofTopMode = 0;

float aspectRatio;

// XZ position of the camera
float x = 0.0f, y = 1.0f, z = 20.0f;

float red = 1.0f, green = 0.0f, blue = 0.0f;

GLuint woodTexture;
GLuint grassTexture;
GLuint roofTexture;
GLuint stoneTexture;
GLuint giocondaTexture;
GLuint marbleTexture;
GLuint ironTexture;

GLuint loadTexture(const char * filename,int texWidth, int texHeight) {
    GLuint texture;
    unsigned char * data;

    FILE * file;
    file = fopen(filename, "rb");

    if (file == NULL) return 0;
    data = (unsigned char *)malloc(texWidth * texHeight * 3);
    //int size = fseek(file,);
    fread(data, texWidth * texHeight * 3, 1, file);
    fclose(file);

    for (int i = 0; i < texWidth * texHeight; ++i) {
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];

        data[index] = R;
        data[index+2] = B;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
    free(data);

    return texture;
}

static void drawBox(GLfloat size, GLenum type) {
    static GLfloat n[6][3] = {
        {-1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {1.0, 0.0, 0.0},
        {0.0, -1.0, 0.0},
        {0.0, 0.0, 1.0},
        {0.0, 0.0, -1.0}
    };
    static GLint faces[6][4] = {
        {0, 1, 2, 3},
        {3, 2, 6, 7},
        {7, 6, 5, 4},
        {4, 5, 1, 0},
        {5, 6, 2, 1},
        {7, 4, 0, 3}
    };
    GLfloat v[8][3];
    GLint i;

    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2.0f;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2.0f;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2.0f;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2.0f;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2.0f;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2.0f;

    for (i = 5; i >= 0; i--) {
        glBegin(type);
        glNormal3fv(&n[i][0]);
        glTexCoord2f(0.0f, 0.0f); glVertex3fv(&v[faces[i][0]][0]);
        glTexCoord2f(0.0f, 1.0f); glVertex3fv(&v[faces[i][1]][0]);
        glTexCoord2f(1.0f, 1.0f); glVertex3fv(&v[faces[i][2]][0]);
        glTexCoord2f(1.0f, 0.0f); glVertex3fv(&v[faces[i][3]][0]);
        glEnd();
    }
}

/* Recreated my glutSolidCube from source because
 there aren't any generated texture coordinates */
void myGlutSolidCube(GLdouble size) {
    drawBox(size, GL_QUADS);
}

void drawScene() {
    // Ground
    glPushMatrix();
    glColor3f(0.0f, 1.0f, 0.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2d(0, 0);

    GLfloat GROUND_PLANE_WIDTH = 1000.0f;
    // repeat 1000 times for a plane 1000 times in width
    GLfloat textCoord = GROUND_PLANE_WIDTH;

    glVertex3f( -GROUND_PLANE_WIDTH, -0.1f, -GROUND_PLANE_WIDTH);
    // go beyond 1 for texture coordinate so it repeats
    glTexCoord2d(0.0f, textCoord);
    glVertex3f( -GROUND_PLANE_WIDTH, -0.1f, GROUND_PLANE_WIDTH);
    glTexCoord2d(textCoord, textCoord);
    glVertex3f( GROUND_PLANE_WIDTH, -0.1f, GROUND_PLANE_WIDTH);
    glTexCoord2d(textCoord, 0.0f);
    glVertex3f( GROUND_PLANE_WIDTH, -0.1f, -GROUND_PLANE_WIDTH);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Left Outer Wall
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(-3.85f, 1.5f, 0.0f);
    glScalef(0.25f, 3.0f, 10.7f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    //Right Outer Wall
    // First Right Outer Wall
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(3.85f, 1.5f, 2.925f);
    glScalef(0.25f, 3.0f, 4.85f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    // Middle Right Outer Wall
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(3.85f, 2.5f, 0.0f);
    glScalef(0.25f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, marbleTexture);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    // Second Right Outer Wall
    // First half for window
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(3.85f, 1.5f, -1.3375f);
    glScalef(0.25f, 3.0f, 1.675f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    // Second half for window
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(3.85f, 1.5f, -4.5125f);
    glScalef(0.25f, 3.0f, 1.675f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    // Top half for window
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(3.85f, 2.625f, -2.925f);
    glScalef(0.25f, 0.75f, 1.675f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    // Bottom half for window
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(3.85f, 0.375f, -2.925f);
    glScalef(0.25f, 0.75f, 1.675f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Bottom Outer Wall
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(0.0f, 1.5f, 5.225f);
    glScalef(7.7f, 3.0f, 0.25f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glRotatef(90.0f, 0, 0, 1);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Top Outer Wall
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(0.0f, 1.5f, -5.225f);
    glScalef(7.7f, 3.0f, 0.25f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glRotatef(90.0f, 0, 0, 1);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Bedroom - Bathroom Wall
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(0.0f, 1.5f, 3.15f);
    glScalef(0.1f, 3.0f, 4.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Hallroom Wall 1
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(0.0f, 1.5f, 1.1f);
    glScalef(7.7f, 3.0f, 0.1f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glRotatef(90.0f, 0, 0, 1);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Hallroom Wall 2
    glPushMatrix();
    glColor3f(0.93f, 0.9f, 0.66f);
    glTranslatef(0.0f, 1.5f, -1.1f);
    glScalef(7.7f, 3.0f, 0.1f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glRotatef(90.0f, 0, 0, 1);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Floor
    glPushMatrix();
    glColor3f(0.49f, 0.73f, 0.91f);
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(7.7f, 0.01f, 10.2f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, stoneTexture);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    
    // Bed
    glPushMatrix();
    glColor3f(0.8f, 0.52f, 0.24f);
    glTranslatef(-2.4f, 0.26, 3.4f);
    glScalef(1.5f, 0.5f, 3.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Commode
    glPushMatrix();
    glColor3f(1.0f, 0.38f, 0.27f);
    glTranslatef(-1.0f, 0.26f, 4.6f);
    glScalef(0.8f, 0.5f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Table
    GLfloat tableWidth = 1.5f;
    GLfloat tableHeight = 2.0f;
    glPushMatrix();
    glColor3f(0.7f, 0.3f, 0.0f);
    glTranslatef(-2.0f, 0.71f, -3.2f);
    glScalef(tableWidth, 0.05f, tableHeight);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Table Legs
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glColor3f(0.7f, 0.3f, 0.0f);
        if (i == 0)
            glTranslatef(-2.0f - tableWidth/2, 0.36f, -3.2f - tableHeight/2);
        else if (i == 1)
            glTranslatef(-2.0f + tableWidth/2, 0.36f, -3.2f + tableHeight/2);
        else if (i == 2)
            glTranslatef(-2.0f + tableWidth/2, 0.36f, -3.2f - tableHeight/2);
        else
            glTranslatef(-2.0f - tableWidth/2, 0.36f, -3.2f + tableHeight/2);
        glScalef(0.05f, 0.7f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // Chairs
    for (int i = 0; i < 4; i++) {
        // Chair Bottom
        GLfloat chairBottomWidth = 0.6f;
        GLfloat chairBottomHeight = 0.6f;
        glPushMatrix();
        glColor3f(0.7f, 0.3f, 0.0f);
        if (i == 0)
            glTranslatef(-2.75f, 0.5f, -3.2f);
        else if (i == 1)
            glTranslatef(-1.25f, 0.5f, -3.2f);
        else if (i == 2)
            glTranslatef(-2.0f, 0.5f, -4.2f);
        else
            glTranslatef(-2.0f, 0.5f, -2.2f);
        glScalef(chairBottomWidth, 0.05f, chairBottomHeight);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Chair Back
        glPushMatrix();
        glColor3f(0.7f, 0.3f, 0.0f);
        if (i == 0) {
            glTranslatef(-2.75f - 0.3f, 0.5f + 0.3f, -3.2f);
            glScalef(0.05f, 0.6f, 0.6f);
        }
        else if (i == 1) {
            glTranslatef(-1.25f + 0.3f, 0.5f + 0.3f, -3.2f);
            glScalef(0.05f, 0.6f, 0.6f);
        }
        else if (i == 2) {
            glTranslatef(-2.0f, 0.5f + 0.3f, -4.2f - 0.3f);
            glScalef(0.6f, 0.6f, 0.05f);
        }
        else {
            glTranslatef(-2.0f, 0.5f + 0.3f, -2.2f + 0.3f);
            glScalef(0.6f, 0.6f, 0.05f);
        }
        glutSolidCube(1.0f);
        glPopMatrix();

        // Chair Legs
        for (int j = 0; j < 4; j++) {
            glPushMatrix();
            glColor3f(0.7f, 0.3f, 0.0f);
            if (i == 0) {
                if (j == 0)
                    glTranslatef(-2.75f - chairBottomWidth/2, 0.26f, -3.2f - chairBottomHeight/2);
                else if (j == 1)
                    glTranslatef(-2.75f + chairBottomWidth/2, 0.26f, -3.2f + chairBottomHeight/2);
                else if (j == 2)
                    glTranslatef(-2.75f + chairBottomWidth/2, 0.26f, -3.2f - chairBottomHeight/2);
                else
                    glTranslatef(-2.75f - chairBottomWidth/2, 0.26f, -3.2f + chairBottomHeight/2);
            }
            else if (i == 1) {
                if (j == 0)
                    glTranslatef(-1.25f - chairBottomWidth/2, 0.26f, -3.2f - chairBottomHeight/2);
                else if (j == 1)
                    glTranslatef(-1.25f + chairBottomWidth/2, 0.26f, -3.2f + chairBottomHeight/2);
                else if (j == 2)
                    glTranslatef(-1.25f + chairBottomWidth/2, 0.26f, -3.2f - chairBottomHeight/2);
                else
                    glTranslatef(-1.25f - chairBottomWidth/2, 0.26f, -3.2f + chairBottomHeight/2);
            }
            else if (i == 2) {
                if (j == 0)
                    glTranslatef(-2.0f - chairBottomWidth/2, 0.26f, -4.2f - chairBottomHeight/2);
                else if (j == 1)
                    glTranslatef(-2.0f + chairBottomWidth/2, 0.26f, -4.2f + chairBottomHeight/2);
                else if (j == 2)
                    glTranslatef(-2.0f + chairBottomWidth/2, 0.26f, -4.2f - chairBottomHeight/2);
                else
                    glTranslatef(-2.0f - chairBottomWidth/2, 0.26f, -4.2f + chairBottomHeight/2);
            }
            else {
                if (j == 0)
                    glTranslatef(-2.0f - chairBottomWidth/2, 0.26f, -2.2f - chairBottomHeight/2);
                else if (j == 1)
                    glTranslatef(-2.0f + chairBottomWidth/2, 0.26f, -2.2f + chairBottomHeight/2);
                else if (j == 2)
                    glTranslatef(-2.0f + chairBottomWidth/2, 0.26f, -2.2f - chairBottomHeight/2);
                else
                    glTranslatef(-2.0f - chairBottomWidth/2, 0.26f, -2.2f + chairBottomHeight/2);
            }
            glScalef(0.05f, 0.5f, 0.05f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }

    // Mona Lisa
    glPushMatrix();
    glColor3f(0.82f, 0.7f, 0.54f);
    glTranslatef(-3.85f + 0.13f, 2.0f, 0.0f);
    glScalef(0.01f, 1.0f, 0.8f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, giocondaTexture);
    glRotatef(90.0f, 0, 1, 0);
    myGlutSolidCube(1.0f);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    if (roofTopMode) {
        // Rooftop first half
        glPushMatrix();
        glColor3f(0.9f, 0.0f, 0.0f);
        glTranslatef(-2.05f, 3.7f, 0.0f);
        glRotatef(20.0f, 0, 0, 1);
        glScalef(4.4f, 0.1f, 11.2f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, roofTexture);
        glRotatef(90.0f, 0, 1, 0);
        myGlutSolidCube(1.0f);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // Rooftop second half
        glPushMatrix();
        glColor3f(0.9f, 0.0f, 0.0f);
        glTranslatef(2.05f, 3.7f, 0.0f);
        glRotatef(-20.0f, 0, 0, 1);
        glScalef(4.4f, 0.1f, 11.2f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, roofTexture);
        glRotatef(-90.0f, 0, 1, 0);
        myGlutSolidCube(1.0f);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // Rooftop front triangle
        glPushMatrix();
        glColor3f(0.9f, 0.0f, 0.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, roofTexture);
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0);
        glVertex3f(-4.1f, 3.0f, 5.35f);
        glTexCoord2f(1, 0);
        glVertex3f(0.0f, 3.0f, 5.35f);
        glTexCoord2f(1, 1);
        glVertex3f(0.0, 4.5f, 5.35f);

        glTexCoord2f(1, 0);
        glVertex3f(0.0, 3.0f, 5.35f);
        glTexCoord2f(0, 1);
        glVertex3f(4.1f, 3.0f, 5.35f);
        glTexCoord2f(1, 1);
        glVertex3f(0.0, 4.5f, 5.35f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();

        // Rooftop back triangle
        glPushMatrix();
        glColor3f(0.9f, 0.0f, 0.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, roofTexture);
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0, 0);
        glVertex3f(-4.1f, 3.0f, -5.35f);
        glTexCoord2f(1, 0);
        glVertex3f(0.0f, 3.0f, -5.35f);
        glTexCoord2f(1, 1);
        glVertex3f(0.0, 4.5f, -5.35f);

        glTexCoord2f(1, 0);
        glVertex3f(0.0, 3.0f, -5.35f);
        glTexCoord2f(0, 1);
        glVertex3f(4.1f, 3.0f, -5.35f);
        glTexCoord2f(1, 1);
        glVertex3f(0.0, 4.5f, -5.35f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    // Stone Path
    for (int i = 0; i < 10; i++) {
        glPushMatrix();
        glColor3f(0.86f, 0.86f, 0.86f);
        glTranslatef(14.0f - i, 0.0f, 0.0f);
        glScalef(0.5f, 0.01f, 2.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ironTexture);
        myGlutSolidCube(1.0f);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }

    // Window
    glPushMatrix();
    glColor4f(0.23f, 0.78f, 0.32f, 0.1f);
    glTranslatef(3.85f, 1.5f, -2.925f);
    glScalef(0.01f, 1.5f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glutSwapBuffers();
}

void drawTopView() {
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);

    glOrtho(-20.0f*aspectRatio, 20.0f*aspectRatio, -20.0f, 20.0f, -20.0f, 20.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(90.0f, 1, 0, 0);

    drawScene();
}

void drawPerspectiveView() {
    glClearColor(0.0f, 0.74f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);

    gluPerspective(60.0, aspectRatio, 0.0001, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(20.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    drawScene();
}

void myReshape(int w, int h) {
    width = w;
    height = h;

    aspectRatio = (float) width / (float) height;

    // Clear Display
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glutSwapBuffers();
}

// reset the display
void resetDisplay() {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glutSwapBuffers();
}

void myDisplay() {
}

void keyEvent(unsigned char key, int delta_x, int delta_y) {
    switch (key) {
    // Quit (Escape key)
    case 27:
        glDeleteTextures(1, &woodTexture);
        glDeleteTextures(1, &grassTexture);
        glDeleteTextures(1, &roofTexture);
        glDeleteTextures(1, &stoneTexture);
        printf("Exiting...\n");
        glutDestroyWindow(winID);
        exit(0);
        break;
    default:
        break;
    }
}

void myMouse(int button, int state, GLint x, GLint y) {

    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN) {

    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        resetDisplay();
    }
}

void mySubMenu(int selection) {
    switch (selection) {
        case RED_COLOR:
            red = 1.0f;
            green = 0.0f;
            blue = 0.0f;
            break;

        case GREEN_COLOR:
            red = 0.0f;
            green = 1.0f;
            blue = 0.0f;
            break;

        case BLUE_COLOR:
            red = 0.0f;
            green = 0.0f;
            blue = 1.0f;
            break;
    
        default:
            printf("Invalid menu selection\n");
            break;
    }
}

void myMenu(int selection) {
    switch(selection) {
        case TOP_VIEW: // draw polygon if there are at least 3 edges
            roofTopMode = 0;
            drawTopView();
            break;

        case TOP_VIEW_R:
            roofTopMode = 1;
            drawTopView();
            break;

        case PERSP_VIEW:
            roofTopMode = 1;
            drawPerspectiveView();
            break;

        case CLEAR_SCREEN:
            resetDisplay();
            break;

        case QUIT:
            glDeleteTextures(1, &woodTexture);
            glDeleteTextures(1, &grassTexture);
            glDeleteTextures(1, &roofTexture);
            glDeleteTextures(1, &stoneTexture);
            printf("Exiting...\n");
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
    glutAddMenuEntry("Top View", TOP_VIEW);
    glutAddMenuEntry("Top View with rooftop", TOP_VIEW_R);
    glutAddMenuEntry("Perspective view", PERSP_VIEW);
    glutAddSubMenu("Color", lineColorSubmenu);
    glutAddMenuEntry("Clear Screen", CLEAR_SCREEN);
    glutAddMenuEntry("Quit", QUIT);

    // register menu to right mouse button
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void init() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    winID = glutCreateWindow("Camera Views");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glShadeModel(GL_SMOOTH);
    glutFullScreen();

    woodTexture = loadTexture("textures/wood02.bmp", 1920, 1229);
    grassTexture = loadTexture("textures/grass.raw", 256, 256);
    roofTexture = loadTexture("textures/ceramic1.bmp", 512, 256);
    stoneTexture = loadTexture("textures/stone.bmp", 256, 256);
    giocondaTexture = loadTexture("textures/LaGioconda.bmp", 256, 256);
    marbleTexture = loadTexture("textures/marble.bmp", 64, 64);
    ironTexture = loadTexture("textures/iron022.bmp", 64, 64);
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

    // Free our textures
    glDeleteTextures(1, &woodTexture);
    glDeleteTextures(1, &grassTexture);
    glDeleteTextures(1, &roofTexture);
    glDeleteTextures(1, &stoneTexture);

    return 0;
}