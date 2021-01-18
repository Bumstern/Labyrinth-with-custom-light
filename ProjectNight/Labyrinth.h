#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#include "Light2D.h"

#define UP		0	// ������� ������
#define DOWN	1	// ������ ������
#define LEFT	2	// ����� ������
#define RIGHT	3	// ������ ������

#define NOT_EXIST -1	// �������� ���� ������ �� ����������


// ������ ��� ������������� ���������
struct CellGenLab {
	bool isVisited;		// �������� �� ������
	bool gateExist[4];	// ����� ������� ����������
};

// ������ ���������� ���������
struct CellLineLab {
	int edgeID[4];	// � ����� ���������� ��������� ������
	Line walls[4];	// ���������� ������ � ����� ����
};


// �������� ������������ ��������
// ��������: 7569 �����
void generateLabyrinth(std::vector<std::vector<CellGenLab> >& maze, short int x, short int y, int& width, int& length);

// ������� ����� ��������� ��������� �����
void createMap(std::vector<std::vector<CellGenLab> >& maze, std::vector<std::vector<CellLineLab> >& lineMap, int lengthOfCell, int originX, int originY);

// ������� ����������� ��� ����������� �������� ���������
void createLabyrinth(std::vector<std::vector<CellLineLab> >& lineMap, int width, int length, int lengthOfCell, int originX, int originY);

// ������� ����� ������� ��������� ������
void areaOfVisibility(std::vector<std::vector<CellLineLab> >& lineMap, std::vector<Line>& areaMap, int areaRadius, int centreX, int centreY);