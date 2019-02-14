#include <Windows.h>
#include <iostream>
#include "display.h"
#include "mesh.h"
#include "shader.h"
#include "inputManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

const int nCubesRow = 4;
const int nCubesWall = nCubesRow * nCubesRow;
const int nTotalCubes = nCubesRow * nCubesRow * nCubesRow;

static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 800;

float direction = 1.0;
float PI = 3.14159265358979323846;
float theta = radians(PI / 2);
float factor = 3283;

std::vector<char> moves;
vec3 v[5 * 5 * 5];
mat4 Matrices[nTotalCubes];	
mat4 movesMatrices[nTotalCubes];

int bottom[nCubesWall];	
int top[nCubesWall];	
int front[nCubesWall]; 
int back[nCubesWall];
int left[nCubesWall];  
int right[nCubesWall];

int right2[nCubesWall];
int left2[nCubesWall];
int top2[nCubesWall];
int bottom2[nCubesWall];
int front2[nCubesWall];
int back2[nCubesWall];

void mulAngle(int kind) {
	if (theta <= abs(radians(PI / 2))){ 
		theta = theta * 2;
		if (kind == 0){
			moves.push_back('A');
		}
	}
}

void divAngle(int kind) {
	theta = theta / 2;
	if (kind == 0) {
		moves.push_back('Z');
	}
}

void copyMat() {
	for (int i = 0; i < nTotalCubes; i++) {
		Matrices[i] = movesMatrices[i];
	}
}

void rotateCube(int cubeIdx, float dir, char axis, bool wholeCube) {
	mat4 rotxf = glm::rotate(mat4(1), theta * dir * factor, vec3(1, 0, 0));
	mat4 rotyf = glm::rotate(mat4(1), theta * dir * factor, vec3(0, 1, 0));
	mat4 rotzf = glm::rotate(mat4(1), theta * dir * factor, vec3(0, 0, 1));

	mat4 trans = glm::translate(mat4(1), v[20]);
	mat4 transop = glm::translate(mat4(1), -v[20]);

	mat4 Tz = trans * rotzf * transop;
	mat4 Tx = trans * rotxf * transop;
	mat4 Ty = trans * rotyf * transop;

	switch (axis) {
	case 'x': {
		if (wholeCube) {
			Matrices[cubeIdx] = Tx * Matrices[cubeIdx];
		}
		else {
			movesMatrices[cubeIdx] = Tx * movesMatrices[cubeIdx];
		}
	} break;

	case 'y': {
		if (wholeCube) {
			Matrices[cubeIdx] = Ty * Matrices[cubeIdx];
		}
		else {
			movesMatrices[cubeIdx] = Ty * movesMatrices[cubeIdx];
		}
	} break;

	case 'z': {
		if (wholeCube) {
			Matrices[cubeIdx] = Tz * Matrices[cubeIdx];
		}
		else {
			movesMatrices[cubeIdx] = Tz * movesMatrices[cubeIdx];
		}
	} break;
	};
}

void rotateCubeRight(int isWall) {
	for (int i = 0; i < nTotalCubes; i++) {
		rotateCube(i, 1.0, 'x', true);
	}

	if (!isWall) {
		moves.push_back('R');
	}
}

void rotateCubeLeft(int isWall) {
	for (int i = 0; i < nTotalCubes; i++) {
		rotateCube(i, -1.0, 'x', true);
	}

	if (!isWall) {
		moves.push_back('L');
	}
}

void rotateCubeUp(int isWall) {
	for (int i = 0; i < nTotalCubes; i++) {
		rotateCube(i, -1.0, 'y', true);
	}

	if (!isWall) {
		moves.push_back('U');
	}
}

void rotateCubeDown(int isWall) {
	for (int i = 0; i < nTotalCubes; i++) {
		rotateCube(i, 1.0, 'y', true);
	}

	if (!isWall) {
		moves.push_back('D');
	}
}

void updateWholeCube() {
	for (std::vector<char>::iterator it = moves.begin(); it != moves.end(); ++it){
		char move = *it;
		if (move == 'A') {
			mulAngle(1);
		}
		else if (move == 'Z') {
			divAngle(1);
		}
		else if (move == 'U') {
			rotateCubeUp(1);
		}
		else if (move == 'D') {
			rotateCubeDown(1);
		}
		else if (move == 'R') {
			rotateCubeRight(1);
		}
		else if (move == 'L') {
			rotateCubeLeft(1);
		}
	}
}

void updateWallArraysIdxs(int numCubes){
	int frontIndx = 0, backIndx = 0, bottomIndx = 0, topIndx = 0, leftIndx = 0, rightIndx = 0;
	int bottom2Indx = 0, top2Indx = 0, left2Indx = 0, right2Indx = 0, front2Indx = 0, back2Indx = 0;

	for (int i = 0; i < numCubes; i++) {
		mat4 currCubeMat = movesMatrices[i];
		float x = currCubeMat[3].x;
		float y = currCubeMat[3].y;
		float z = currCubeMat[3].z;

		if (nCubesRow == 2) {
			if (x >= 1.02  && x <= 1.03) {
				top[topIndx] = i;
				topIndx++;
			}
			else if (x <= -1.02  && x >= -1.03) {
				bottom[bottomIndx] = i;
				bottomIndx++;
			}
			if (y >= 1.02 && y <= 1.03) {
				right[rightIndx] = i;
				rightIndx++;
			}
			else if (y <= -1.02 && y >= -1.03) {
				left[leftIndx] = i;
				leftIndx++;
			}
			if (z <= -1.02 && z >= -1.03) {
				front[frontIndx] = i;
				frontIndx++;
			}
			else if (z >= 1.02 && z <= 1.03) {
				back[backIndx] = i;
				backIndx++;
			}
		}

		else if (nCubesRow == 3) {
			if (x >= 1.02 * 2 && x <= 1.03 * 2) {
				top[topIndx] = i;
				topIndx++;
			}
			if (x <= -1.02 * 2 && x >= -1.03 * 2) {
				bottom[bottomIndx] = i;
				bottomIndx++;
			}
			if (y >= 1.02 * 2 && y <= 1.03 * 2) {
				right[rightIndx] = i;
				rightIndx++;
			}
			if (y <= -1.02 * 2 && y >= -1.03 * 2) {
				left[leftIndx] = i;
				leftIndx++;
			}
			if (z <= 0.2 && z >= -0.2) {
				front[frontIndx] = i;
				frontIndx++;
			}
			if (z >= 1.02 * 4 && z <= 1.03 * 4) {
				back[backIndx] = i;
				backIndx++;
			}
		}

		else if (nCubesRow == 4) {
			if (x >= 1.02 * 3 && x <= 1.03 * 3) {
				top[topIndx] = i;
				topIndx++;
			}
			else if (x <= -1.02 * 3 && x >= -1.03 * 3) {
				bottom[bottomIndx] = i;
				bottomIndx++;
			}
			if (y >= 1.02 * 3 && y <= 1.03 * 3) {
				right[rightIndx] = i;
				rightIndx++;
			}
			else if (y <= -1.02 * 3 && y >= -1.03 * 3) {
				left[leftIndx] = i;
				leftIndx++;
			}
			if (z <= -1.02 * 3 && z >= -1.03 * 3) {
				front[frontIndx] = i;
				frontIndx++;
			}
			else if (z >= 1.02 * 3 && z <= 1.03 * 3) {
				back[backIndx] = i;
				backIndx++;
			}

			if (x >= 1.02 * 1 && x <= 1.03 * 1) {
				top2[top2Indx] = i;
				top2Indx++;
			}
			else if (x <= -1.02 * 1 && x >= -1.03 * 1) {
				bottom2[bottom2Indx] = i;
				bottom2Indx++;
			}
			if (y >= 1.02 * 1 && y <= 1.03 * 1) {
				right2[right2Indx] = i;
				right2Indx++;
			}
			if (y <= -1.02 * 1 && y >= -1.03 * 1) {
				left2[left2Indx] = i;
				left2Indx++;
			}
			if (z <= 1.02 * 1 && z >= -1.03 * 1) {
				front2[front2Indx] = i;
				front2Indx++;
			}
			if (z >= 1.02 * 1 && z <= 1.03 * 1) {
				back2[back2Indx] = i;
				back2Indx++;
			}
		}

		else if (nCubesRow == 5) {
			if (x >= 1.02 * 4 && x <= 1.03 * 4) {
				top[topIndx] = i;
				topIndx++;
			}
			else if (x <= -1.02 * 4 && x >= -1.03 * 4) {
				bottom[bottomIndx] = i;
				bottomIndx++;
			}
			if (y >= 1.02 * 4 && y <= 1.03 * 4) {
				right[rightIndx] = i;
				rightIndx++;
			}
			else if (y <= -1.02 * 4 && y >= -1.03 * 4) {
				left[leftIndx] = i;
				leftIndx++;
			}
			if (z <= -1.02 * 2 && z >= -1.03 * 2) {
				front[frontIndx] = i;
				frontIndx++;
			}
			else if (z >= 1.02 * 6 && z <= 1.03 * 6) {
				back[backIndx] = i;
				backIndx++;
			}
			if (x >= 1.02 * 2 && x <= 1.03 * 2) {
				top2[top2Indx] = i;
				top2Indx++;
			}
			else if (x <= -1.02 * 2 && x >= -1.03 * 2) {
				bottom2[bottom2Indx] = i;
				bottom2Indx++;
			}
			if (y >= 1.02 * 2 && y <= 1.03 * 2) {
				right2[right2Indx] = i;
				right2Indx++;
			}
			if (y <= -1.02 * 2 && y >= -1.03 * 2) {
				left2[left2Indx] = i;
				left2Indx++;
			}
			if (z <= 1.02 * 0.1 && z >= -1.03 * 0.1) {
				front2[front2Indx] = i;
				front2Indx++;
			}
			if (z >= 1.02 * 4 && z <= 1.03 * 4) {
				back2[back2Indx] = i;
				back2Indx++;
			}
		}
	}
}

void rotateWall(int wall[], char arrow) {
	updateWallArraysIdxs(nTotalCubes);
	for (int i = 0; i < nCubesWall; i++) {
		rotateCube(wall[i], direction, arrow, false);
	}

	theta = radians(PI / 2);
	copyMat();
	updateWholeCube();
}

void initMat2() {
	v[0] = vec3(1.025, -1.025, -1.025);				
	Matrices[0] = glm::translate(mat4(1), v[0]); 	
	movesMatrices[0] = glm::translate(mat4(1), v[0]);
	
	v[1] = vec3(1.025, 1.025, -1.025); 				
	Matrices[1] = glm::translate(mat4(1), v[1]); 		
	movesMatrices[1] = glm::translate(mat4(1), v[1]);
	
	v[2] = vec3(-1.025, -1.025, -1.025);
	Matrices[2] = glm::translate(mat4(1), v[2]);	
	movesMatrices[2] = glm::translate(mat4(1), v[2]);   
	
	v[3] = vec3(-1.025, 1.025, -1.025);
	Matrices[3] = glm::translate(mat4(1), v[3]);		
	movesMatrices[3] = glm::translate(mat4(1), v[3]);   

	v[4] = vec3(1.025, -1.025, 1.025);
	Matrices[4] = glm::translate(mat4(1), v[4]);	  
	movesMatrices[4] = glm::translate(mat4(1), v[4]); 
	
	v[5] = vec3(1.025, 1.025, 1.025);
	Matrices[5] = glm::translate(mat4(1), v[5]);	 
	movesMatrices[5] = glm::translate(mat4(1), v[5]); 
	
	v[6] = vec3(-1.025, -1.025, 1.025);
	Matrices[6] = glm::translate(mat4(1), v[6]);  
	movesMatrices[6] = glm::translate(mat4(1), v[6]);

	v[7] = vec3(-1.025, 1.025, 1.025);
	Matrices[7] = glm::translate(mat4(1), v[7]);
	movesMatrices[7] = glm::translate(mat4(1), v[7]); 

	v[20] = vec3(0, 0, 0);
}

void initMat() {
	v[0] = vec3(1, 1, 1);
	movesMatrices[0] = glm::mat4(1);			
	v[1] = vec3(2.05, 0, 0);
	movesMatrices[1] = glm::translate(mat4(1), v[1]);
	v[2] = vec3(2.05, 2.05, 0);
	movesMatrices[2] = glm::translate(mat4(1), v[2]); 
	v[5] = vec3(2.05, -2.05, 0);
	movesMatrices[5] = glm::translate(mat4(1), v[5]); 
	v[10] = vec3(-2.05, -2.05, 0);
	movesMatrices[10] = glm::translate(mat4(1), v[10]); 
	v[11] = vec3(-2.05, 2.05, 0);
	movesMatrices[11] = glm::translate(mat4(1), v[11]);
	v[22] = vec3(0, -2.05, 0);
	movesMatrices[22] = glm::translate(mat4(1), v[22]);
	v[19] = vec3(0, 2.05, 0);
	movesMatrices[19] = glm::translate(mat4(1), v[19]); 

	v[6] = vec3(2.05, -2.05, 2.05);
	movesMatrices[6] = glm::translate(mat4(1), v[6]);  
	v[12] = vec3(-2.05, 0, 2.05);
	movesMatrices[12] = glm::translate(mat4(1), v[12]);  
	v[13] = vec3(-2.05, 2.05, 2.05);
	movesMatrices[13] = glm::translate(mat4(1), v[13]); 
	v[14] = vec3(-2.05, 0, 0);
	movesMatrices[14] = glm::translate(mat4(1), v[14]); 
	v[15] = vec3(-2.05, -2.05, 2.05);
	movesMatrices[15] = glm::translate(mat4(1), v[15]); 
	v[3] = vec3(2.05, 0, 2.05);
	movesMatrices[3] = glm::translate(mat4(1), v[3]); 
	v[4] = vec3(2.05, 2.05, 2.05);
	movesMatrices[4] = glm::translate(mat4(1), v[4]);  
	v[21] = vec3(0, 2.05, 2.05);
	movesMatrices[21] = glm::translate(mat4(1), v[21]); 
	v[23] = vec3(0, -2.05, 2.05);
	movesMatrices[23] = glm::translate(mat4(1), v[23]); 

	v[16] = vec3(-2.05, 0, 4.1);
	movesMatrices[16] = glm::translate(mat4(1), v[16]);  
	v[17] = vec3(-2.05, 2.05, 4.1);
	movesMatrices[17] = glm::translate(mat4(1), v[17]); 
	v[18] = vec3(-2.05, -2.05, 4.1);
	movesMatrices[18] = glm::translate(mat4(1), v[18]); 
	v[24] = vec3(0, 0, 4.1);
	movesMatrices[24] = glm::translate(mat4(1), v[24]);  
	v[25] = vec3(0, 2.05, 4.1);
	movesMatrices[25] = glm::translate(mat4(1), v[25]);  
	v[26] = vec3(0, -2.05, 4.1);
	movesMatrices[26] = glm::translate(mat4(1), v[26]); 
	v[7] = vec3(2.05, 0, 4.1);
	movesMatrices[7] = glm::translate(mat4(1), v[7]);   
	v[8] = vec3(2.05, 2.05, 4.1);
	movesMatrices[8] = glm::translate(mat4(1), v[8]);  
	v[9] = vec3(2.05, -2.05, 4.1);
	movesMatrices[9] = glm::translate(mat4(1), v[9]);   

	v[20] = vec3(0, 0, 2.05);
	movesMatrices[20] = glm::translate(mat4(1), v[20]); 

	copyMat();
}

void initMat4() {
	float K = 1.025 * 2;
	for (int iX = 0; iX < nCubesRow; iX++) {
		for (int iY = 0; iY < nCubesRow; iY++) {
			for (int iZ = 0; iZ < nCubesRow; iZ++) {
				v[iZ + iY * nCubesRow + iX * nCubesWall] = vec3(float(K * iX - 1.5 * K), float(K * iY - 1.5 * K), float(iZ * K - 1.5 * K));
				Matrices[iZ + iY * nCubesRow + iX * nCubesWall] = glm::translate(mat4(1), v[iZ + iY * nCubesRow + iX * nCubesWall]);
				movesMatrices[iZ + iY * nCubesRow + iX * nCubesWall] = glm::translate(mat4(1), v[iZ + iY * nCubesRow + iX * nCubesWall]);
			}
		}
	}

	v[20] = vec3(0, 0, 0);
}

void initMat5() {
	float K = 1.025 * 2;
	for (int iX = 0; iX < nCubesRow; iX++) {
		for (int iY = 0; iY < nCubesRow; iY++) {
			for (int iZ = 0; iZ < nCubesRow; iZ++) {
				v[iZ + iY * nCubesRow + iX * nCubesWall] = vec3(float(K * iX - K * 2), float(K * iY - K * 2), float(iZ * K - K));
				Matrices[iZ + iY * nCubesRow + iX * nCubesWall] = glm::translate(mat4(1), v[iZ + iY * nCubesRow + iX * nCubesWall]);
				movesMatrices[iZ + iY * nCubesRow + iX * nCubesWall] = glm::translate(mat4(1), v[iZ + iY * nCubesRow + iX * nCubesWall]);
			}
		}
	}

	v[20] = vec3(0, 0, 2.05);
}

void key_callBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				if (action == GLFW_PRESS){
					glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			}
			case GLFW_KEY_SPACE: {
				if (action == GLFW_PRESS)
					direction = -direction;
				break;
			}
			case GLFW_KEY_Z: {
				if (action == GLFW_PRESS)
					divAngle(0);
				break;
			}
			case GLFW_KEY_A: {
				if (action == GLFW_PRESS)
					mulAngle(0);
				break;
			}

			case GLFW_KEY_R: {
				rotateWall(right, 'y');
				break;
			}
			case GLFW_KEY_L: {
				rotateWall(left, 'y');
				break;
			}
			case GLFW_KEY_U: {
				rotateWall(top, 'x');
				break;
			}
			case GLFW_KEY_D: {
				rotateWall(bottom, 'x');
				break;
			}
			case GLFW_KEY_B: {
				rotateWall(back, 'z');
				break;
			}
			case GLFW_KEY_F: {
				rotateWall(front, 'z');
				break;
			}
			case GLFW_KEY_E: {
				if (nCubesRow > 3)
					rotateWall(right2, 'y');
				break;
			}
			case GLFW_KEY_K: {
				if (nCubesRow > 3)
					rotateWall(left2, 'y');
				break;
			}
			case GLFW_KEY_J: {
				if (nCubesRow > 3)
					rotateWall(top2, 'x');
				break;
			}
			case GLFW_KEY_C: {
				if (nCubesRow > 3)
					rotateWall(bottom2, 'x');
				break;
			}
			case GLFW_KEY_N: {
				if (nCubesRow > 3)
					rotateWall(back2, 'z');
				break;
			}
			case GLFW_KEY_G: {
				if (nCubesRow > 3)
					rotateWall(front2, 'z');
				break;
			}

			case GLFW_KEY_RIGHT: {
				rotateCubeRight(0);
				break;
			}
			case GLFW_KEY_LEFT: {
				rotateCubeLeft(0);
				break;
			}
			case GLFW_KEY_UP: {
				rotateCubeUp(0);
				break;
			}
			case GLFW_KEY_DOWN: {
				rotateCubeDown(0);
				break;
			}
			default:
				break;
			}
		}
}

int main(int argc, char** argv){
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");

	Vertex vertices[] = 
	{
		Vertex(glm::vec3(-1, -1, -1), glm::vec2(1, 0), glm::vec3(0, 0, -1), glm::vec3(0, 0, 1)), 
		Vertex(glm::vec3(-1, 1, -1), glm::vec2(0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 0, 1)),
		Vertex(glm::vec3(1, 1, -1), glm::vec2(0, 1), glm::vec3(0, 0, -1), glm::vec3(0, 0, 1)),
		Vertex(glm::vec3(1, -1, -1), glm::vec2(1, 1), glm::vec3(0, 0, -1), glm::vec3(0, 0, 1)),

		Vertex(glm::vec3(-1, -1, 1), glm::vec2(1, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 1)), 
		Vertex(glm::vec3(-1, 1, 1), glm::vec2(0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 1)),
		Vertex(glm::vec3(1, 1, 1), glm::vec2(0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 1, 1)),
		Vertex(glm::vec3(1, -1, 1), glm::vec2(1, 1), glm::vec3(0, 0, 1), glm::vec3(0, 1, 1)),

		Vertex(glm::vec3(-1, -1, -1), glm::vec2(0, 1), glm::vec3(0, -1, 0), glm::vec3(1, 1, 0)), 
		Vertex(glm::vec3(-1, -1, 1), glm::vec2(1, 1), glm::vec3(0, -1, 0), glm::vec3(1, 1, 0)),
		Vertex(glm::vec3(1, -1, 1), glm::vec2(1, 0), glm::vec3(0, -1, 0), glm::vec3(1, 1, 0)),
		Vertex(glm::vec3(1, -1, -1), glm::vec2(0, 0), glm::vec3(0, -1, 0), glm::vec3(1, 1, 0)),

		Vertex(glm::vec3(-1, 1, -1), glm::vec2(0, 1), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),  
		Vertex(glm::vec3(-1, 1, 1), glm::vec2(1, 1), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
		Vertex(glm::vec3(1, 1, 1), glm::vec2(1, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
		Vertex(glm::vec3(1, 1, -1), glm::vec2(0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),

		Vertex(glm::vec3(-1, -1, -1), glm::vec2(1, 1), glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0)), 
		Vertex(glm::vec3(-1, -1, 1), glm::vec2(1, 0), glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0)),
		Vertex(glm::vec3(-1, 1, 1), glm::vec2(0, 0), glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0)),
		Vertex(glm::vec3(-1, 1, -1), glm::vec2(0, 1), glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0)),

		Vertex(glm::vec3(1, -1, -1), glm::vec2(1, 1), glm::vec3(1, 0, 0), glm::vec3(1, 0, 1)),  
		Vertex(glm::vec3(1, -1, 1), glm::vec2(1, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 1)),
		Vertex(glm::vec3(1, 1, 1), glm::vec2(0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 1)),
		Vertex(glm::vec3(1, 1, -1), glm::vec2(0, 1), glm::vec3(1, 0, 0), glm::vec3(1, 0, 1))
	};

	unsigned int indices[] = 
	{ 
		0, 1, 2,
		0, 2, 3,

		6, 5, 4,
		7, 6, 4,

		10, 9, 8,
		11, 10, 8,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		22, 21, 20,
		23, 22, 20
	};

	Mesh mesh(vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0]));
	
	Shader shader("./res/shaders/basicShader");

	switch (nCubesRow){
	case 2:
		initMat2(); break;
	case 3:
		initMat(); break;
	case 4:
		initMat4(); break;
	case 5:
		initMat5(); break;
	default:
		break;
	}

	int lview = -7;

	if (nCubesRow > 3) {
		lview = -10;
	}

	vec3 pos = vec3(0, 0, lview);
	vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);
	vec3 up = glm::vec3(1.0f, 0.0f, 0.0f);
	vec3 side = glm::vec3(0.0f, 1.0f, 0.0f);
	mat4 P = glm::perspective(100.0f, (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, 0.1f, 100.0f);

	mat4 M = glm::mat4(1);
	mat4 M1 = glm::translate(mat4(1), vec3(3, 0, 0));
	mat4 M2 = glm::translate(mat4(1), vec3(-3, 0, 0));
	P = P * glm::lookAt(pos, pos + forward, up);
	mat4 MVP = P * M;

	updateWallArraysIdxs(nTotalCubes);

	glfwSetKeyCallback(display.m_window, key_callBack);
	while (!glfwWindowShouldClose(display.m_window)){
		Sleep(1);
		display.Clear(1.0f, 1.0f, 1.0f, 1.0f);
		for (int i = 0; i < nTotalCubes; i++) {
			MVP = P * Matrices[i];
			shader.Bind();
			shader.Update(MVP, Matrices[i]);
			mesh.Draw();
		}

		display.SwapBuffers();
		glfwPollEvents();
	}

	return 0;
}
