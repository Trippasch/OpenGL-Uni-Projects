/*********************************
 * Ergasia 2 – Bonus Project – 25.02.2022
 * Choropanitis Paschalis - 2453
 *********************************/

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define DEG2RAD(a) (a * 0.0174532925)

int winID; // windows ID for destroying window

// Window's initial width and height
int width = 640;
int height = 480;

float aspectRatio;

GLuint woodTexture;
GLuint grassTexture;
GLuint roofTexture;
GLuint stoneTexture;
GLuint giocondaTexture;
GLuint marbleTexture;
GLuint ironTexture;

class Vector3f {
public:
    float x, y, z;

    Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f operator+(Vector3f const &v) {
        return Vector3f(x + v.x, y + v.y, z + v.z);
    }

    Vector3f operator-(Vector3f &v) {
        return Vector3f(x - v.x, y - v.y, z - v.z);
    }

    Vector3f operator*(float n) {
        return Vector3f(x * n, y * n, z * n);
    }

    Vector3f operator/(float n) {
        return Vector3f(x / n, y / n, z / n);
    }

    Vector3f unit() {
        return *this / sqrt(x * x + y * y + z * z);
    }

    Vector3f cross(Vector3f v) {
        return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};

class Camera {
public:
    Vector3f eye, center, up;

    Camera(float eyeX = 20.0f, float eyeY = 1.0f, float eyeZ = 0.0f, float centerX = 0.0f, float centerY = 1.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
        eye = Vector3f(eyeX, eyeY, eyeZ);
        center = Vector3f(centerX, centerY, centerZ);
        up = Vector3f(upX, upY, upZ);
        Vector3f view = (center - eye).unit();
        eye = eye + view * -0.7;
        center = center + view * -0.7;
    }

    void moveX(float d) {
        Vector3f right = up.cross(center - eye).unit();
        eye = eye + right * d;
        center = center + right * d;
    }

    void moveY(float d) {
        eye = eye + up.unit() * d;
        center = center + up.unit() * d;
    }

    void moveZ(float d) {
        Vector3f view = (center - eye).unit();
        eye = eye + view * d;
        center = center + view * d;
    }

    void rotateX(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
        up = view.cross(right);
        center = eye + view;
    }

    void rotateY(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
        right = view.cross(up);
        center = eye + view;
    }

    void look() {
        gluLookAt(
                eye.x, eye.y, eye.z,
                center.x, center.y, center.z,
                up.x, up.y, up.z);
    }
};

Camera camera;

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
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, data);
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

    // Bed
    glPushMatrix();
    glColor3f(0.8f, 0.52f, 0.24f);
    GLfloat ambientBed[] = {0.8f, 0.52f, 0.24f, 1.0f};
    GLfloat specularBed[] = {0.1f, 0.1f, 0.1f, 0.1f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambientBed);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularBed);
    glMaterialf(GL_FRONT, GL_SHININESS, 10);
    glTranslatef(-2.4f, 0.26, 3.4f);
    glScalef(1.5f, 0.5f, 3.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Commode
    glPushMatrix();
    glColor3f(1.0f, 0.38f, 0.27f);
    GLfloat ambientCommode[] = {1.0f, 0.38f, 0.27f, 1.0f};
    GLfloat specularCommode[] = {0.1f, 0.1f, 0.1f, 0.1f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambientCommode);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularCommode);
    glMaterialf(GL_FRONT, GL_SHININESS, 10);
    glTranslatef(-1.0f, 0.26f, 4.6f);
    glScalef(0.8f, 0.5f, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Table
    GLfloat ambientTable[] = {0.7f, 0.3f, 0.0f, 1.0f};
    GLfloat specularTable[] = {0.1f, 0.1f, 0.1f, 0.1f};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambientTable);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularTable);
    glMaterialf(GL_FRONT, GL_SHININESS, 10);
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

    // Window
    glPushMatrix();
    glColor4f(0.23f, 0.78f, 0.32f, 0.1f);
    glTranslatef(3.85f, 1.5f, -2.925f);
    glScalef(0.01f, 1.5f, 1.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void setUpLights() {
    GLfloat ambient[] = {0.2f, 0.2f, 0.2, 1.0f};
    GLfloat diffuse[] = {0.8f, 0.8f, 0.8, 1.0f};
    GLfloat specular[] = {1.0f, 1.0f, 1.0, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

void setUpCamera() {
    glClearColor(0.0f, 0.74f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);

    gluPerspective(60.0, aspectRatio, 0.0001, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.look();
}

void myDisplay() {
    setUpCamera();
    setUpLights();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene();
    
    glutSwapBuffers();
}

void myReshape(int w, int h) {
    width = w;
    height = h;

    aspectRatio = (float) width / (float) height;
}

void specialKeysEvent(int key, int delta_x, int delta_y) {
    float angle = 1.0f;
    switch (key) {
    case GLUT_KEY_LEFT:
        camera.rotateY(angle);
        break;
    case GLUT_KEY_RIGHT:
        camera.rotateY(-angle);
        break;
    case GLUT_KEY_UP:
        camera.rotateX(angle);
        break;
    case GLUT_KEY_DOWN:
        camera.rotateX(-angle);
        break;
    }

    glutPostRedisplay();
}

void keyEvent(unsigned char key, int delta_x, int delta_y) {
    float fraction = 0.1f;

    switch (key) {
    case 'A':
    case 'a':
        camera.moveX(fraction);
        break;
    case 'D':
    case 'd':
        camera.moveX(-fraction);
        break;
    case 'W':
    case 'w':
        camera.moveZ(fraction);
        break;
    case 'S':
    case 's':
        camera.moveZ(-fraction);
        break;
    case 'Q':
    case 'q':
        camera.moveY(-fraction);
        break;
    case 'E':
    case 'e':
        camera.moveY(fraction);
        break;
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
    }

    glutPostRedisplay();
}

void init() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    winID = glutCreateWindow("Visiting House");
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
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
    glutDisplayFunc(myDisplay);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(keyEvent);
    glutSpecialFunc(specialKeysEvent);
    glutMainLoop();

    // Free our textures
    glDeleteTextures(1, &woodTexture);
    glDeleteTextures(1, &grassTexture);
    glDeleteTextures(1, &roofTexture);
    glDeleteTextures(1, &stoneTexture);

    return 0;
}