#include <GL/glut.h>
#include <stdio.h>
#include <math.h>

#define PI 3.1415926
#define P2 PI/2
#define P3 3*PI/2

#define WINDOW_HEIGHT 512
#define WINDOW_WIDTH 1024

#define NUM_OF_RAYS 120
#define RAY_ANGLE_INC 0.0074533
#define MAP_SIZE 210
#define MAP_ARRAY 15
#define MAP_CELL_SIZE MAP_SIZE/MAP_ARRAY
#define PLAYER_SIZE MAP_CELL_SIZE/3
#define VIEW_STRIP WINDOW_WIDTH/NUM_OF_RAYS
#define TURNING_ANGLE 0.07
#define SPEED PLAYER_SIZE/4

float playerX = MAP_CELL_SIZE * 1.5;
float playerY = MAP_CELL_SIZE * 1.5;
float playerAngle = 0.0;

int map[] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
    ,1,0,0,0,1,0,0,0,0,0,3,0,0,0,1
    ,1,0,0,0,1,0,0,0,0,0,3,0,0,0,1
    ,1,0,0,0,1,0,0,0,0,2,2,2,0,0,1
    ,1,1,0,1,1,1,1,0,0,0,0,0,0,0,1
    ,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1
    ,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1
    ,1,2,2,2,0,0,0,0,1,0,0,0,0,0,1
    ,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1
    ,1,0,3,0,0,0,0,0,0,0,0,0,0,0,1
    ,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1
    ,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1
    ,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1
    ,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1
    ,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

struct Ray 
{
    float distance;
    float angle;
    bool vertical;
    int wallType;
};

Ray rays[NUM_OF_RAYS];

void drawMinimap() 
{
    float translateY = WINDOW_HEIGHT - MAP_SIZE;

    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(0, translateY);
    glVertex2f(MAP_SIZE, translateY);
    glVertex2f(MAP_SIZE, translateY + MAP_SIZE);
    glVertex2f(0, translateY + MAP_SIZE);
    glEnd();

    for (int i = 0; i < MAP_ARRAY; i++) 
    {
        for (int j = 0; j < MAP_ARRAY; j++) 
        {
            switch (map[i * MAP_ARRAY + j]) 
            {
            case 0:
                glColor3f(0, 0, 0);
                break;
            case 1:
                glColor3f(255, 0, 0);
                break;
            case 2:
                glColor3f(0, 255, 0);
                break;
            case 3:
                glColor3f(0, 0, 255);
                break;
            }
       
            glBegin(GL_QUADS);
            glVertex2f(MAP_CELL_SIZE * j + 1, translateY + MAP_CELL_SIZE * i + 1);
            glVertex2f(MAP_CELL_SIZE * j + MAP_CELL_SIZE - 1, translateY + MAP_CELL_SIZE * i + 1);
            glVertex2f(MAP_CELL_SIZE * j + MAP_CELL_SIZE - 1, translateY + MAP_CELL_SIZE * i + MAP_CELL_SIZE - 1);
            glVertex2f(MAP_CELL_SIZE * j + 1, translateY + MAP_CELL_SIZE * i + MAP_CELL_SIZE - 1);
            glEnd();
        }
    }

    glPushMatrix();
    glTranslatef(playerX, translateY + playerY, 0);
    glRotatef(playerAngle * 180 / PI, 0, 0, 1);
    glTranslatef(-playerX, -(translateY + playerY), 0);

    glColor3f(0, 255, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(playerX - 4, translateY + playerY - 4);
    glVertex2f(playerX - 4, translateY + playerY + 4);
    glVertex2f(playerX + 6, translateY + playerY);
    glEnd();

    glPopMatrix();
}

void drawView() 
{
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(0, WINDOW_HEIGHT / 2);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT / 2);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    float lineHeight;
    float fixFishEye;
    for (int i = 0; i < NUM_OF_RAYS; i++) 
    {
        fixFishEye = playerAngle - rays[i].angle;
        if (fixFishEye < 0) 
        {
            fixFishEye += 2 * PI;
        }
        else if (fixFishEye > 2 * PI) 
        {
            fixFishEye -= 2 * PI;
        }
        rays[i].distance = rays[i].distance * cos(fixFishEye);

        switch (rays[i].wallType) 
        {
        case 1:
            if (rays[i].vertical) 
            {
                glColor3f(0.6, 0, 0);
            }
            else 
            {
                glColor3f(0.4, 0, 0);
            }
            break;
        case 2:
            if (rays[i].vertical) 
            {
                glColor3f(0, 0.6, 0);
            }
            else 
            {
                glColor3f(0, 0.4, 0);
            }
            break;
        case 3:
            if (rays[i].vertical) 
            {
                glColor3f(0, 0, 0.6);
            }
            else 
            {
                glColor3f(0, 0, 0.4);
            }
            break;
        }

        lineHeight = (MAP_CELL_SIZE * WINDOW_HEIGHT) / rays[i].distance;
        glBegin(GL_QUADS);
        glVertex2f(i * VIEW_STRIP, WINDOW_HEIGHT / 2 - lineHeight / 2);
        glVertex2f((i + 1) * VIEW_STRIP, WINDOW_HEIGHT / 2 - lineHeight / 2);
        glVertex2f((i + 1) * VIEW_STRIP, WINDOW_HEIGHT / 2 + lineHeight);
        glVertex2f(i * VIEW_STRIP, WINDOW_HEIGHT / 2 + lineHeight);
        glEnd();
    }
}

float distance(float ax, float bx, float ay, float by) 
{
    return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

void castRays() 
{
    float rayY, rayX, xOffset, yOffset;
    int mapX, mapY, mapIndex, hWallType, vWallType;
    int depth, maxDepth = MAP_ARRAY;

    float rayAngle = playerAngle - NUM_OF_RAYS / 2 * RAY_ANGLE_INC;
    if (rayAngle < 0) 
    {
        rayAngle += 2 * PI;
    }
    else if (rayAngle > 2 * PI) 
    {
        rayAngle -= 2 * PI;
    }


    for (int i = 0; i < NUM_OF_RAYS; i++) 
    {
        float hX, hY, hDistance = 9999999999;
        depth = 0;
        float aTan = -1 / tan(rayAngle);

        if (rayAngle == PI || rayAngle == 0) 
        {
            rayX = playerX;
            rayY = playerY;
            depth = 8;
        }
        else {
            if (rayAngle > PI) 
            {
                rayY = ((int)playerY) / ((int)MAP_CELL_SIZE) * ((int)MAP_CELL_SIZE) - 0.0001;
                yOffset = -MAP_CELL_SIZE;
            }
            else 
            {
                rayY = (((int)playerY) / ((int)MAP_CELL_SIZE) * ((int)MAP_CELL_SIZE)) + MAP_CELL_SIZE;
                yOffset = MAP_CELL_SIZE;

            }
            rayX = (playerY - rayY) * aTan + playerX;
            xOffset = -yOffset * aTan;
        }

        while (depth < maxDepth) 
        {
            mapX = ((int)rayX) / ((int)MAP_CELL_SIZE);
            mapY = rayY / ((int)MAP_CELL_SIZE);
            mapIndex = mapY * MAP_ARRAY + mapX;

            if (mapIndex >= 0 && mapIndex < MAP_ARRAY * MAP_ARRAY && map[mapIndex] != 0) 
            {
                depth = maxDepth;
                hWallType = map[mapIndex];
                hX = rayX;
                hY = rayY;
                hDistance = distance(playerX, hX, playerY, hY);
            }
            else 
            {
                rayX += xOffset;
                rayY += yOffset;
                depth++;
            }
        }

        float vX, vY, vDistance = 9999999999;
        depth = 0;
        float nTan = -tan(rayAngle);

        if (rayAngle == P2 || rayAngle == P3) 
        {
            rayX = playerX;
            rayY = playerY;
            depth = 8;
        }
        else 
        {
            if (rayAngle > P2 && rayAngle < P3) 
            {
                rayX = ((int)playerX) / ((int)MAP_CELL_SIZE) * ((int)MAP_CELL_SIZE) - 0.0001;
                xOffset = -MAP_CELL_SIZE;
            }
            else 
            {
                rayX = (((int)playerX) / ((int)MAP_CELL_SIZE) * ((int)MAP_CELL_SIZE)) + MAP_CELL_SIZE;
                xOffset = MAP_CELL_SIZE;

            }
            rayY = (playerX - rayX) * nTan + playerY;
            yOffset = -xOffset * nTan;
        }

        while (depth < maxDepth) 
        {
            mapX = ((int)rayX) / ((int)MAP_CELL_SIZE);
            mapY = rayY / ((int)MAP_CELL_SIZE);
            mapIndex = mapY * MAP_ARRAY + mapX;

            if (mapIndex >= 0 && mapIndex < MAP_ARRAY * MAP_ARRAY && map[mapIndex] != 0) 
            {
                depth = maxDepth;
                vWallType = map[mapIndex];
                vX = rayX;
                vY = rayY;
                vDistance = distance(playerX, vX, playerY, vY);
            }
            else {
                rayX += xOffset;
                rayY += yOffset;
                depth++;
            }
        }

        if (hDistance < vDistance) 
        {
            rayX = hX;
            rayY = hY;
            rays[i].distance = hDistance;
            rays[i].vertical = false;
            rays[i].wallType = hWallType;
            rays[i].angle = rayAngle;
        }
        else {
            rays[i].distance = vDistance;
            rays[i].vertical = true;
            rays[i].wallType = vWallType;
            rays[i].angle = rayAngle;
        }

        rayAngle += RAY_ANGLE_INC;
        if (rayAngle < 0) 
        {
            rayAngle += 2 * PI;
        }
        else if (rayAngle > 2 * PI) 
        {
            rayAngle -= 2 * PI;
        }
    }
}

void buttons(unsigned char key, int x, int y) 
{
    switch (key) 
    {
    case 'w':
        playerX += cos(playerAngle) * SPEED;
        playerY += sin(playerAngle) * SPEED;
        break;
    case 's':
        playerX -= cos(playerAngle) * SPEED;
        playerY -= sin(playerAngle) * SPEED;
        break;
    case 'd':
        playerAngle += TURNING_ANGLE;
        if (playerAngle >= 2 * PI) { playerAngle = 0; }
        break;
    case 'a':
        playerAngle -= TURNING_ANGLE;
        if (playerAngle <= 0) { playerAngle = 2 * PI; }
        break;
    }

    glutPostRedisplay();
}

void display() 
{
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();

    castRays();
    drawView();
    drawMinimap();

    glFlush();
}

void init() 
{
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    glutCreateWindow("Racasting Game Engine");

    glClearColor(0.4, 0.4, 0.4, 0);
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
}

int main(int argc, char** argv) 
{
    glutInit(&argc, argv);

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(buttons);

    glutMainLoop();

    return 0;
}