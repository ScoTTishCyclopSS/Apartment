#include "plane.h"
const int planeNAttribsPerVertex = 8;
const int planeNVertices = 4;
const int planeNTriangles = 2;

const float planeVertices[] = {
  -4.37584f,0.25255f,1.40429f,0.0f,1.0f,-0.0f,0.0f,0.0f,
  -3.91188f,0.25255f,1.40429f,0.0f,1.0f,-0.0f,1.0f,0.0f,
  -3.91188f,0.25255f,0.551929f,0.0f,1.0f,-0.0f,1.0f,1.0f,
  -4.37584f,0.25255f,0.551929f,0.0f,1.0f,-0.0f,0.0f,1.0f,
}; // end planeVertices

const float plane2Vertices[] = {
  -4.37584f,-0.25255f,1.40429f,0.0f,1.0f,-0.0f,0.0f,0.0f,
  -3.91188f,-0.25255f,1.40429f,0.0f,1.0f,-0.0f,1.0f,0.0f,
  -3.91188f,-0.25255f,0.551929f,0.0f,1.0f,-0.0f,1.0f,1.0f,
  -4.37584f,-0.25255f,0.551929f,0.0f,1.0f,-0.0f,0.0f,1.0f,
}; // end planeVertices

const unsigned planeTriangles[] = {
  0, 1, 2,
  0, 2, 3,
}; // end planeTriangles

