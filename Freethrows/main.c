/* ICT289 Assignment 2 - Computer Graphics Principles and Programming
    Author - Vladislav Kennebury
    Date - 11/06/2020
    Description -
    Freethrows: Basketball game where users are able to throw balls with various
    attribute such as elasticity and gravity to score. Interactions are enabled
    with mouse and keyboard, through camera movement and aiming of the ball.
*/

#ifdef _WIN32
	#include <gl/freeglut.h>
#ifdef _WIN64
#endif
#elif __APPLE__
	#include <GLUT/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define seconds 10

typedef GLfloat point3[3];
static GLdouble viewer[]= {100.0, 100.0, 50.0, //Camera location
                           0.0, 50.0, 0.0, //Direction of camera
                           0.0, 1.0, 0.0  //Up vector
                           };

//Time values for animation
float startTime, prevTime, timeControl = 1000.0f;

//Radius of basketball
float ballSize = 10;
//Variable check if ball is held to perform actions
bool ballHeld = true;
//Colour of basketball
float ballColour[3] = {1.0, 0.5, 0.0};
//Bounds of the axis aligned environment
int courtBounds = 250;

float dropOff = 0.8; //Loss of energy for wall collision
float hoopDropOff = 0.5;//Loss of energy for hoop collision

point3 playerThrow = {0, 0, 0};	//Player throw
point3 g = {0, 0, 0}; //Gravity
point3 startPos = {0, 50, 50}; //Starting position
point3 currPos, prevPos; //Position of center of basketball
point3 currVel, prevVel; //Velocities of basketball

//Camera direction rotation
float angle = 0.0f;
float deltaAngle = 0.0f;
//Camera direction
float camX = 0.0f, camZ = -1.0f;
// XZ position of the camera
float x = 0.0f, z = 75.0f;
//Determines degree of movement forwards or backwards
float deltaMove = 0;
//Used in calculating deltaAngle
int xOrigin = -1;

void myinit(void)
{
    glEnable(GL_DEPTH_TEST);
    //Projection mode
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLdouble fov = 80; //Field of view
    GLdouble aspect	= 1; //Aspect ratio
    GLdouble nearVal = 0.1;
    GLdouble farVal = 1000;
    gluPerspective(fov, aspect, nearVal, farVal);
}

void computePosition(float deltaMove) {
    //Camera movement speed
    float speed = 5.0f;
    //Checks that camera is within bounds before adjusting location
    if((x + deltaMove * camX * speed) <= courtBounds - ballSize && (x + deltaMove * camX * speed) >= -courtBounds + ballSize){
        x += deltaMove * camX * speed;
    }
    else{
        deltaMove = 0;
    }
    if((z + deltaMove * camZ * speed) <= courtBounds - ballSize && (z + deltaMove * camZ * speed) >= -courtBounds + ballSize){
        z += deltaMove * camZ * speed;
    }
    else{
        deltaMove = 0;
    }
}

void display(void)
{
    //Updates position if change in camera occurs
    if(deltaMove){
        computePosition(deltaMove);
    }

    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity(); //Identity transformation
    gluLookAt(	x, 50.0f, z,
			x+camX, 50.0f,  z+camZ,
			0.0f, 1.0f,  0.0f);

    //Displays room, backboard and hoop
    drawRoom();
    drawBackboard();
    drawHoop();

    // Basketball
    glColor3f(ballColour[0], ballColour[1], ballColour[2]);
    //Assigning ball to currPos point3
    glTranslatef(currPos[0], currPos[1], currPos[2]);
    glutSolidSphere(ballSize,20,20);
    glutSwapBuffers();
}

void drawRoom()
{
    glBegin(GL_QUADS);
// Floor /
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f(-courtBounds,0,-courtBounds); //top left
        glVertex3f(courtBounds,0,-courtBounds); //top right
        glVertex3f(courtBounds,0,courtBounds); //bot right
        glVertex3f(-courtBounds,0,courtBounds); //bot left
// Ceiling /
        glColor3f(0.0, 1.0, 1.0);
        glVertex3f(-courtBounds,courtBounds,-courtBounds); //top left
        glVertex3f(courtBounds,courtBounds,-courtBounds); //top right
        glVertex3f(courtBounds,courtBounds,courtBounds); //bot right
        glVertex3f(-courtBounds,courtBounds,courtBounds); //bot left
// Walls
//BACK WALL
        glColor3f(1.0, 0.0, 1.0);
        glVertex3f(-courtBounds,courtBounds,-courtBounds); //top left
        glVertex3f(courtBounds,courtBounds,-courtBounds); //top right
        glVertex3f(courtBounds,0,-courtBounds); //bot right
        glVertex3f(-courtBounds,0,-courtBounds); //bot left

//LEFT WALL
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(-courtBounds,0,-courtBounds); //top left
        glVertex3f(-courtBounds,courtBounds,-courtBounds); //top right
        glVertex3f(-courtBounds,courtBounds,courtBounds); //bot right
        glVertex3f(-courtBounds,0,courtBounds); //bot left
//RIGHT WALL
        glColor3f(1.0, 1.0, 0.0);
        glVertex3f(courtBounds,0,-courtBounds); //top left
        glVertex3f(courtBounds,courtBounds,-courtBounds); //top right
        glVertex3f(courtBounds,courtBounds,courtBounds); //bot right
        glVertex3f(courtBounds,0,courtBounds); //bot left
//FRONT WALL
        glColor3f(0.0, 0.5, 0.5);
        glVertex3f(-courtBounds,courtBounds,courtBounds); //top left
        glVertex3f(courtBounds,courtBounds,courtBounds); //top right
        glVertex3f(courtBounds,0,courtBounds); //bot right
        glVertex3f(-courtBounds,0,courtBounds); //bot left
    glEnd();
}

void drawBackboard()
{
    glBegin(GL_QUADS);
// Floor /
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(-75,100,-250); //top left
        glVertex3f(75,100,-250); //top right
        glVertex3f(75,100,-240); //bot right
        glVertex3f(-75,100,-240); //bot left
// Ceiling /
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(-75,200,-250); //top left
        glVertex3f(75,200,-250); //top right
        glVertex3f(75,200,-240); //bot right
        glVertex3f(-75,200,-240); //bot left
// Walls
//BACK WALL
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(-75,200,-250); //top left
        glVertex3f(75,200,-250); //top right
        glVertex3f(75,100,-250); //bot right
        glVertex3f(-75,100,-250); //bot left
//LEFT WALL
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(-75,100,-250); //top left
        glVertex3f(-75,200,-250); //top right
        glVertex3f(-75,200,-240); //bot right
        glVertex3f(-75,100,-240); //bot left
//RIGHT WALL
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(75,100,-250); //top left
        glVertex3f(75,200,-250); //top right
        glVertex3f(75,200,-240); //bot right
        glVertex3f(75,100,-240); //bot left
//FRONT WALL
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(-75,200,-240); //top left
        glVertex3f(75,200,-240); //top right
        glVertex3f(75,100,-240); //bot right
        glVertex3f(-75,100,-240); //bot left
    glEnd();
}

void drawHoop(){
    glBegin(GL_QUADS);
// Floor /
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-5,115,-240); //top left
        glVertex3f(5,115,-240); //top right
        glVertex3f(5,115,-230); //bot right
        glVertex3f(-5,115,-230); //bot left
// Ceiling /
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-5,120,-240); //top left
        glVertex3f(5,120,-240); //top right
        glVertex3f(5,120,-230); //bot right
        glVertex3f(-5,120,-230); //bot left
// Walls
//BACK WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-5,120,-240); //top left
        glVertex3f(5,120,-240); //top right
        glVertex3f(5,115,-240); //bot right
        glVertex3f(-5,115,-240); //bot left
//LEFT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-5,115,-240); //top left
        glVertex3f(-5,120,-240); //top right
        glVertex3f(-5,120,-230); //bot right
        glVertex3f(-5,115,-230); //bot left
//RIGHT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(5,115,-240); //top left
        glVertex3f(5,120,-240); //top right
        glVertex3f(5,120,-230); //bot right
        glVertex3f(5,115,-230); //bot left
//FRONT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-5,120,-230); //top left
        glVertex3f(5,120,-230); //top right
        glVertex3f(5,115,-230); //bot right
        glVertex3f(-5,115,-230); //bot left

    glEnd();
//Back Ring
    glBegin(GL_QUADS);
// Floor /
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,115,-230); //top left
        glVertex3f(20,115,-230); //top right
        glVertex3f(20,115,-225); //bot right
        glVertex3f(-20,115,-225); //bot left
// Ceiling /
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,120,-230); //top left
        glVertex3f(20,120,-230); //top right
        glVertex3f(20,120,-225); //bot right
        glVertex3f(-20,120,-225); //bot left
// Walls
//BACK WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,120,-230); //top left
        glVertex3f(20,120,-230); //top right
        glVertex3f(20,115,-225); //bot right
        glVertex3f(-20,115,-225); //bot left
//LEFT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,115,-230); //top left
        glVertex3f(-20,120,-230); //top right
        glVertex3f(-20,120,-225); //bot right
        glVertex3f(-20,115,-225); //bot left
//RIGHT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(20,115,-230); //top left
        glVertex3f(20,120,-230); //top right
        glVertex3f(20,120,-225); //bot right
        glVertex3f(20,115,-225); //bot left
//FRONT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,120,-225); //top left
        glVertex3f(20,120,-225); //top right
        glVertex3f(20,115,-225); //bot right
        glVertex3f(-20,115,-225); //bot left
    glEnd();
//Front Ring
    glBegin(GL_QUADS);
// Floor /
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,115,-180); //top left
        glVertex3f(20,115,-180); //top right
        glVertex3f(20,115,-175); //bot right
        glVertex3f(-20,115,-175); //bot left
// Ceiling /
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,120,-180); //top left
        glVertex3f(20,120,-180); //top right
        glVertex3f(20,120,-175); //bot right
        glVertex3f(-20,120,-175); //bot left
// Walls
//BACK WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,120,-180); //top left
        glVertex3f(20,120,-180); //top right
        glVertex3f(20,115,-175); //bot right
        glVertex3f(-20,115,-175); //bot left
//LEFT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,115,-180); //top left
        glVertex3f(-20,120,-180); //top right
        glVertex3f(-20,120,-175); //bot right
        glVertex3f(-20,115,-175); //bot left
//RIGHT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(20,115,-180); //top left
        glVertex3f(20,120,-180); //top right
        glVertex3f(20,120,-175); //bot right
        glVertex3f(20,115,-175); //bot left
//FRONT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,120,-175); //top left
        glVertex3f(20,120,-175); //top right
        glVertex3f(20,115,-175); //bot right
        glVertex3f(-20,115,-175); //bot left
    glEnd();
//Left Ring
    glBegin(GL_QUADS);
// Floor /
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-25,115,-230); //top left
        glVertex3f(-20,115,-230); //top right
        glVertex3f(-20,115,-175); //bot right
        glVertex3f(-25,115,-175); //bot left
// Ceiling /
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-25,120,-230); //top left
        glVertex3f(-20,120,-230); //top right
        glVertex3f(-20,120,-175); //bot right
        glVertex3f(-25,120,-175); //bot left
// Walls
//BACK WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-25,120,-230); //top left
        glVertex3f(-20,120,-230); //top right
        glVertex3f(-20,115,-230); //bot right
        glVertex3f(-25,115,-230); //bot left
//LEFT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-25,120,-230); //top left
        glVertex3f(-25,120,-175); //top right
        glVertex3f(-25,115,-175); //bot right
        glVertex3f(-25,115,-230); //bot left
//RIGHT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-20,120,-230); //top left
        glVertex3f(-20,120,-175); //top right
        glVertex3f(-20,115,-175); //bot right
        glVertex3f(-20,115,-230); //bot left
//FRONT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(-25,120,-175); //top left
        glVertex3f(-20,120,-175); //top right
        glVertex3f(-20,115,-175); //bot right
        glVertex3f(-25,115,-175); //bot left
    glEnd();
//Right Ring
    glBegin(GL_QUADS);
// Floor /
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(25,115,-230); //top left
        glVertex3f(20,115,-230); //top right
        glVertex3f(20,115,-175); //bot right
        glVertex3f(25,115,-175); //bot left
// Ceiling /
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(25,120,-230); //top left
        glVertex3f(20,120,-230); //top right
        glVertex3f(20,120,-175); //bot right
        glVertex3f(25,120,-175); //bot left
// Walls
//BACK WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(25,120,-230); //top left
        glVertex3f(20,120,-230); //top right
        glVertex3f(20,115,-230); //bot right
        glVertex3f(25,115,-230); //bot left
//LEFT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(25,120,-230); //top left
        glVertex3f(25,120,-175); //top right
        glVertex3f(25,115,-175); //bot right
        glVertex3f(25,115,-230); //bot left
//RIGHT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(20,120,-230); //top left
        glVertex3f(20,120,-175); //top right
        glVertex3f(20,115,-175); //bot right
        glVertex3f(20,115,-230); //bot left
//FRONT WALL
        glColor3f(0.5, 0.5, 0.5);
        glVertex3f(25,120,-175); //top left
        glVertex3f(20,120,-175); //top right
        glVertex3f(20,115,-175); //bot right
        glVertex3f(25,115,-175); //bot left
    glEnd();
}

void animate()
{
    glutTimerFunc(seconds, animate, 0);

    //Calculates current time and time between frames
    float currTime = glutGet(GLUT_ELAPSED_TIME) / timeControl ;
    float timeSincePrevFrame = currTime - prevTime;

    //Calculate position influenced by throw and gravity
    currPos[0] = prevPos[0] + prevVel[0] * timeSincePrevFrame + playerThrow[0] * timeSincePrevFrame * timeSincePrevFrame / 2;
    currPos[1] = prevPos[1] + prevVel[1] * timeSincePrevFrame + g[1] * timeSincePrevFrame * timeSincePrevFrame / 2
        + playerThrow[1] * timeSincePrevFrame * timeSincePrevFrame / 2;
    currPos[2] = prevPos[2] + prevVel[2] * timeSincePrevFrame + playerThrow[2] * timeSincePrevFrame * timeSincePrevFrame / 2;

    // Calculate current velocity influenced by throw and gravity
    currVel[0] = prevVel[0] + playerThrow[0] * timeSincePrevFrame;
    currVel[1] = prevVel[1] + g[1] * timeSincePrevFrame + playerThrow[1] * timeSincePrevFrame;
    currVel[2] = prevVel[2] + playerThrow[2] * timeSincePrevFrame;

    // Check for collision
    //Appropriate values are inverted, eg- x and z velocities to not change if ball
    // collides with the floor
    if (currPos[1] <= ballSize){ //Floor
        currVel[0] = currVel[0] * dropOff;
        currVel[1] = - currVel[1] * dropOff;
        currVel[2] = currVel[2] * dropOff;

        //Needed to stop ball clipping through the wall occasionally as computed position exceeds courtBounds
        currPos[1] = ballSize;
    }

    if (currPos[0] >= (courtBounds - ballSize) || currPos[0] <= (-courtBounds + ballSize) ){ //x axis walls

        currVel[0] = - currVel[0] * dropOff;
        currVel[2] = currVel[2] * dropOff;

        if(currPos[0] >= (courtBounds - ballSize)){
            currPos[0] = courtBounds - ballSize;
        }

        if(currPos[0] <= (-courtBounds + ballSize)){
            currPos[0] = -courtBounds + ballSize;
        }
        //Stops continuous velocity of throw after first collision
        playerThrow[0] = 0;
    }

    if (currPos[1] >= (courtBounds - ballSize) ){ //ceiling

        currVel[0] = currVel[0] * dropOff;
        currVel[1] = - currVel[1] * dropOff;
        currVel[2] = currVel[2] * dropOff;

        currPos[1] = (courtBounds - ballSize);
        playerThrow[1] = 0;
    }

    //Updated to compensate for backboard
    if (currPos[2] >= (courtBounds - ballSize) || currPos[2] <= (-courtBounds + ballSize + 10) ){ //z axis walls

        currVel[0] = currVel[0] * dropOff;
        currVel[2] = -currVel[2] * dropOff;

        if(currPos[2] >= (courtBounds - ballSize)){
            currPos[2] = courtBounds - ballSize;
        }

        if(currPos[2] <= (-courtBounds + ballSize + 10)){
            currPos[2] = -courtBounds + ballSize + 10;
        }

        playerThrow[2] = 0;
    }
/////////////////////////////////// Collision with hoop ///////////////////////////////////////////////////////////////////
    //Back hoop
    //Updated positions due to excessive clipping past bounds. Works well as a form on 'magnetism' for scoring in the game
    if(currPos[2] <= (-230 + ballSize) && currPos[2] >= (-230 - ballSize)){
        if(currPos[0] <= (25 + ballSize) && currPos[0] >= (-25 - ballSize)){
            if(currPos[1] <= (120 + ballSize) && currPos[1] >= (115 - ballSize)){
                currVel[0] = currVel[0] * hoopDropOff;
                currVel[1] = - currVel[1] * hoopDropOff;
                currVel[2] = currVel[2] * hoopDropOff;

                if(currPos[1] <= (120 + ballSize)){
                    currPos[1] = (120 + ballSize);
                }

                if(currPos[1] >= (115 - ballSize)){
                    currPos[1] = (115 - ballSize);
                }

                if(currPos[2] <= (-230 + ballSize)){
                    currPos[2] = (-230 + ballSize);
                }

                if(currPos[2] >= (-230 - ballSize)){
                    currPos[2] = (-230 - ballSize);
                }

                playerThrow[0] = 0;
                playerThrow[2] = 0;
            }
        }
    }
    //Front hoop
    if(currPos[2] <= (-175 + ballSize) && currPos[2] >= (-175 - ballSize)){

        if(currPos[0] <= (25 + ballSize) && currPos[0] >= (-25 - ballSize)){
            if(currPos[1] <= (120 + ballSize) && currPos[1] >= (115 - ballSize)){
                currVel[0] = - currVel[0] * hoopDropOff;
                currVel[1] = - currVel[1] * hoopDropOff;
                currVel[2] = - currVel[2] * hoopDropOff;

                if(currPos[1] <= (120 + ballSize)){
                    currPos[1] = (120 + ballSize);
                }

                if(currPos[1] >= (115 - ballSize)){
                    currPos[1] = (115 - ballSize);
                }

                playerThrow[0] = 0;
                playerThrow[2] = 0;
            }
        }
    }
    //Left hoop
    if(currPos[2] >= (-230 - ballSize) && currPos[2] <= -175 + ballSize){

        if(currPos[0] >= (-25 - ballSize) && currPos[0] <= (-20 + ballSize)){
            if(currPos[1] <= (120 + ballSize) && currPos[1] >= (115 - ballSize)){
                currVel[0] = - currVel[0] * hoopDropOff;
                currVel[1] = - currVel[1] * hoopDropOff;
                currVel[2] = - currVel[2] * hoopDropOff;

                if(currPos[0] >= (-25 - ballSize)){
                    currPos[0] = (-25 - ballSize);
                }

                if(currPos[0] <= (-20 + ballSize)){
                    currPos[0] = (-20 + ballSize);
                }

                playerThrow[0] = 0;
                playerThrow[2] = 0;
            }
        }
    }
    //Right hoop
    if(currPos[2] >= (-230 - ballSize) && currPos[2] <= -175 + ballSize){

        if(currPos[0] <= (25 + ballSize) && currPos[0] >= (20 - ballSize)){
            if(currPos[1] <= (120 + ballSize) && currPos[1] >= (115 - ballSize)){
                currVel[0] = - currVel[0] * hoopDropOff;
                currVel[1] = - currVel[1] * hoopDropOff;
                currVel[2] = - currVel[2] * hoopDropOff;

                if(currPos[0] <= (25 + ballSize)){
                    currPos[0] = (25 + ballSize);
                }

                if(currPos[0] >= (20 - ballSize)){
                    currPos[0] = (20 - ballSize);
                }

                playerThrow[0] = 0;
                playerThrow[2] = 0;
            }
        }
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Loses energy over time emulating air resistance
    playerThrow[1] = playerThrow[1] * 0.98;

    prevPos[0] = currPos[0];
    prevPos[1] = currPos[1];
    prevPos[2] = currPos[2];

    prevVel[0] = currVel[0];
    prevVel[1] = currVel[1];
    prevVel[2] = currVel[2];

    prevTime = currTime;

    glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy)
{
    //Responds to keyboard input
    switch (key)
    {
        //Adds gravity
        case 'e' :
            g[1] = 981;
            break;
        //Inverts gravity
        case 'q' :
            g[1] = -981;
            break;
        //Returns ball to user location
        case 'r' :
            if((x+camX * 50) < courtBounds - ballSize && (x+camX * 50) > -courtBounds + ballSize){
                if((z+camZ * 50) < courtBounds - ballSize && (z+camZ * 50) > -courtBounds + ballSize){
                    ballHeld = true;

                currPos[0] = x+camX * 50;
                currPos[1] = 50;
                currPos[2] = z+camZ * 50;

                prevPos[0] = x+camX * 50;
                prevPos[1] = 50;
                prevPos[2] = z+camZ * 50;

                prevVel[0] = 0;
                prevVel[1] = 0;
                prevVel[2] = 0;

                currVel[0] = 0;
                currVel[1] = 0;
                currVel[2] = 0;

                g[1] = 0;

                playerThrow[0] = 0;
                playerThrow[1] = 0;
                playerThrow[2] = 0;
                }
            }
            break;
        //Throws ball depending on flow of time
        case ' ' :
            if(ballHeld == true){
                g[1] = -980;
                if(timeControl == 2000.0f){
                    playerThrow[0] = camX * 600;
                    playerThrow[1] = 2750;
                    playerThrow[2] = camZ * 600;
                }
                else{
                    playerThrow[0] = camX * 400;
                    playerThrow[1] = 2000;
                    playerThrow[2] = camZ * 400;
                }
                ballHeld = false;
            }
            break;
        //Changes to bowling ball with colour and
        //increased loss of energy per collision
        case 'c' :
            ballColour[0] = 0.25;
            ballColour[1] = 0.25;
            ballColour[2] = 0.25;

            dropOff = 0.5;
            break;
        //Changes to basketball with colour and
        //increased loss of energy per collision
        case 'v' :
            ballColour[0] = 1.0;
            ballColour[1] = 0.5;
            ballColour[2] = 0.0;

            dropOff = 0.8;
            break;
        //Changes to elastic rubber ball with colour and
        //increased loss of energy per collision
        case 'x' :
            ballColour[0] = 1.5;
            ballColour[1] = 0.5;
            ballColour[2] = 0.5;

            dropOff = 0.95;
            break;
        //Move forward
        case 'w' :
            deltaMove = 0.5f;
            break;
        //Move backward
        case 's' :
            deltaMove = -0.5f;
            break;
        //Slow time
        case 't':
            timeControl = 2000.0f;
            break;
        //Normal Time
        case 'y':
            timeControl = 1000.0f;
            break;
        case 27 : //ESC key
            exit(0);
    }

    glutPostRedisplay();
}

void releaseKey(int key, int x, int y) {
    //Stops movement on key release
    switch (key) {
        case 'w' :
            deltaMove = 0;
            break;
        case 's' :
            deltaMove = 0;
            break;
    }
}

void mouseMove(int x, int y) {
    //Motion if left mouse is clicked
    if (xOrigin >= 0) {
		//Update deltaAngle
		deltaAngle = (x - xOrigin) * 0.005f;
		//Updates camera direction
		camX = sin(angle + deltaAngle);
		camZ = -cos(angle + deltaAngle);
	}
}

void mouseButton(int button, int state, int x, int y) {
	//Motion if left mouse is clicked
	if (button == GLUT_LEFT_BUTTON) {
		//Release of button
		if (state == GLUT_UP){
			angle += deltaAngle;
			xOrigin = -1;
		}
		else{
			xOrigin = x;
		}
	}
}

void updatePath()
{
    //Updates time and position
    startTime = glutGet(GLUT_ELAPSED_TIME) / timeControl;
    prevTime = startTime;

    currPos[0] = startPos[0];
    currPos[1] = startPos[1];
    currPos[2] = startPos[2];

    prevPos[0] = startPos[0];
    prevPos[1] = startPos[1];
    prevPos[2] = startPos[2];

    prevVel[0] = 0;
    prevVel[1] = 0;
    prevVel[2] = 0;

    currVel[0] = 0;
    currVel[1] = 0;
    currVel[2] = 0;
}

int main(int argc, char** argv)
{
    //Initialisation of window and display
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000,1000);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Freethrows");
    glutDisplayFunc(display);

    //Mouse and keyboard functions
    glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processSpecialKeys);
	glutKeyboardUpFunc(releaseKey);

	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
    //Animation
    glutTimerFunc(seconds, animate, 0);
    myinit();
    updatePath();
    //Loop
    glutMainLoop();

    return 0;
}
