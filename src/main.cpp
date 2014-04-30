#include "../include/Angel.h"
#include <assert.h>
#define FORWARD_STEP 0.4
#define SCALE_ANGLE 0.05
#define G_M 0.67384f
#include <stdlib.h>
#include <vector>
#include <float.h>
#include <stack>
#include "systemcpu.h"
using namespace std;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;
float3 haha;
int index = 0;
GLuint vao,buffer,model_view_loc,color_loc,projection_loc,normal_loc;

point4 points[10];
//----------------------------------------------------------------------------
//VAO VBO GLSL init-----------------------------------------------------------
//----------------------------------------------------------------------------
GLuint vPosition;
systemcpu nbody;

void init( void )
{
    nbody.initialize(4000);

    nbody.treebuilding();
    //nbody.computeForce();
    nbody.traverseTree();
        
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(float3)*nbody._n, nbody._bodies, GL_STATIC_DRAW );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "./glsl/vshader83.glsl", "./glsl/fshader83.glsl" );
    glUseProgram( program );

    vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0 ,BUFFER_OFFSET(0));

    //----------------------------------------------------------------------------
    model_view_loc = glGetUniformLocation( program, "modelview");
    color_loc = glGetUniformLocation(program, "vColor");
    projection_loc = glGetUniformLocation(program, "projection");
    normal_loc = glGetUniformLocation(program,"vNormal");
    //----------------------------------------------------------------------------
    glUniformMatrix4fv(model_view_loc,1,GL_TRUE, Scale(1.0f/250.0f,1.0f/250.0f,1.0f)*Translate(-25,-25,0));

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_POINT_SMOOTH);
    glEnable( GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    //glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE,GL_TRUE);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void idle(void)
{
    //cout << nbody._maxlevel<<endl;
    glutPostRedisplay();
}
double lastTime = 0;
int nbFrames = 0;
void timer()
{
    double currentTime = glutGet(GLUT_ELAPSED_TIME);
    nbFrames++;
    if ( currentTime - lastTime >= 1 )
    { // If last prinf() was more than 1 sec ago
        // printf and reset timer
        printf("%f %fms/frame %d\n", (currentTime),(lastTime), nbFrames);
        nbFrames = 0;
        lastTime = currentTime;
    }


}
int flag=0;
void display(void)
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    int nbFrames=0;
    double lastTime = glutGet(GLUT_ELAPSED_TIME);
    nbody.bounding();
    nbody.treebuilding();
    //nbody.computeForce();
    nbody.traverseTree();
    nbody.updateBodies();
    
    glBufferData( GL_ARRAY_BUFFER, sizeof(float3)*nbody._n, nbody._bodies, GL_STATIC_DRAW );
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0 ,BUFFER_OFFSET(0));
    
    double currentTime = glutGet(GLUT_ELAPSED_TIME);
    printf("%fms/frame %f\n", (currentTime - lastTime), flag++*nbody._tstep);
    glDrawArrays(GL_POINTS, 0,nbody._n);
    glutSwapBuffers();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowSize( 500, 500 );
    glutInitWindowPosition( 512, 112);
    glutCreateWindow( "part1" );
    glewInit();
    lastTime = glutGet(GLUT_ELAPSED_TIME);

    init();
    glutDisplayFunc( display );
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}
