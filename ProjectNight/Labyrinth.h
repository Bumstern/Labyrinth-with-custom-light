#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#include "Light2D.h"

#define UP		0	// Верхняя стенка
#define DOWN	1	// Нижняя стенка
#define LEFT	2	// Левая стенка
#define RIGHT	3	// Правая стенка

#define NOT_EXIST -1	// Ставится если стенка не существует


// Ячейка для СТРОИТЕЛЬСТВА лабиринта
struct CellGenLab {
	bool isVisited;		// Посещена ли ячейка
	bool gateExist[4];	// Какие проходы существуют
};

// Ячейка СОЗДАННОГО лабиринта
struct CellLineLab {
	int edgeID[4];	// К каким множествам относятся стенки
	Line walls[4];	// Координаты начала и конца стен
};


// Алгоритм Рекрусивного возврата
// Максимум: 7569 ячеек
void generateLabyrinth(std::vector<std::vector<CellGenLab> >& maze, short int x, short int y, int& width, int& length);

// Создает карту лабиринта используя линии
void createMap(std::vector<std::vector<CellGenLab> >& maze, std::vector<std::vector<CellLineLab> >& lineMap, int lengthOfCell, int originX, int originY);

// Функция необходимая для правильного создания лабиринта
void createLabyrinth(std::vector<std::vector<CellLineLab> >& lineMap, int width, int length, int lengthOfCell, int originX, int originY);

// Создает карту области видимости игрока
void areaOfVisibility(std::vector<std::vector<CellLineLab> >& lineMap, std::vector<Line>& areaMap, int areaRadius, int centreX, int centreY);