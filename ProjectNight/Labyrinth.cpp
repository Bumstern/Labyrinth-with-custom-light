#include "Labyrinth.h"

void generateLabyrinth(std::vector<std::vector<CellGenLab> >& maze, short int x, short int y, int& width, int& length) {
	// Пометим ячейку как посещенную
	maze[y][x].isVisited = true;

	// Зададим случайные направления для генерации
	std::vector<char> directions = { UP, DOWN, LEFT, RIGHT };
	std::random_shuffle(directions.begin(), directions.end());

	// Пройдемся по этим направлениям
	// Если та клетка уже была посещена - переходим к следующему направлению
	// Иначе сгенерируем лабиринт для этой клетки
	for (int i = 0; i < directions.size(); ++i) {
		switch (directions[i]) {
		case UP:
			if (y - 1 < 0 || maze[y - 1][x].isVisited) break;

			maze[y][x].gateExist[UP] = true;
			maze[y - 1][x].gateExist[DOWN] = true;
			generateLabyrinth(maze, x, y - 1, width, length);
			break;

		case DOWN:
			if (y + 1 >= width || maze[y + 1][x].isVisited) break;

			maze[y][x].gateExist[DOWN] = true;
			maze[y + 1][x].gateExist[UP] = true;
			generateLabyrinth(maze, x, y + 1, width, length);
			break;

		case LEFT:
			if (x - 1 < 0 || maze[y][x - 1].isVisited) break;

			maze[y][x].gateExist[LEFT] = true;
			maze[y][x - 1].gateExist[RIGHT] = true;
			generateLabyrinth(maze, x - 1, y, width, length);
			break;

		case RIGHT:
			if (x + 1 >= length || maze[y][x + 1].isVisited) break;

			maze[y][x].gateExist[RIGHT] = true;
			maze[y][x + 1].gateExist[LEFT] = true;
			generateLabyrinth(maze, x + 1, y, width, length);
			break;
		}
	}
}

void createMap(std::vector<std::vector<CellGenLab> >& maze, std::vector<std::vector<CellLineLab> >& lineMap, int lengthOfCell, int originX, int originY) {
	int widthOfMaze = maze.size();
	int lengthOfMaze = maze[0].size();

	// Создадим структуру хранящую конец каждого множества
	// Создадим счетчик множеств
	std::map<int, std::pair<int, int>> endCoors;
	int multCounter = 0;

	// ---- Распределим сначала линии по множествам и запишем их концы ---- //

	//// Сначала пробежимся по горизонтальным линиям ////
	for (int i = 0; i < widthOfMaze; ++i) {
		for (int j = 0; j < lengthOfMaze; ++j) { // Будем двигаться слева направо сверху вниз 
												 // И проверять встретили ли узел
			// Чтобы добавить линию в множество, нужно чтобы она существовала
			if (maze[i][j].gateExist[UP] == false) {
				// Присваиваем множество стенке
				lineMap[i][j].edgeID[UP] = multCounter;
				if (i - 1 >= 0) lineMap[i - 1][j].edgeID[DOWN] = multCounter;

				// Если встретили узел
				if (maze[i][j].gateExist[RIGHT] == false ||
					(i - 1 >= 0 && maze[i - 1][j].gateExist[RIGHT] == false) ||
					(j + 1 < lengthOfMaze && maze[i][j + 1].gateExist[UP])) {
					// Записываем в структуру endCoors координату конечной точки для данного множества
					endCoors.insert({ multCounter , {(j + 1) * lengthOfCell + originX, i * lengthOfCell + originY} });

					// Увеличиваем счетчик
					++multCounter;
				}
			}
		}
	}
	//// Также учтем самую нижнуюю границу лабиринта ////
	for (int j = 0; j < lengthOfMaze; ++j) {

		// Чтобы добавить линию в множество, нужно чтобы она существовала
		if (maze[widthOfMaze - 1][j].gateExist[DOWN] == false) {
			// Присваиваем множество стенке
			lineMap[widthOfMaze - 1][j].edgeID[DOWN] = multCounter;

			// Если встретили узел
			if (maze[widthOfMaze - 1][j].gateExist[RIGHT] == false ||
				(j + 1 < lengthOfMaze && maze[widthOfMaze - 1][j + 1].gateExist[DOWN])) {
				// Записываем в структуру endCoors координату конечной точки для данного множества
				endCoors.insert({ multCounter, {(j + 1) * lengthOfCell + originX, widthOfMaze * lengthOfCell + originY} });

				// Увеличиваем счетчик
				++multCounter;
			}
		}
	}


	//// Теперь пробежимся по вертикальным линиям ////
	for (int j = 0; j < lengthOfMaze; ++j) {
		for (int i = 0; i < widthOfMaze; ++i) {	// Будем двигаться сверху вниз слева направо
												// И проверять встретили ли узел

			// Чтобы добавить линию в множество, нужно чтобы она существовала
			if (maze[i][j].gateExist[LEFT] == false) {
				// Присваиваем множество стенке
				lineMap[i][j].edgeID[LEFT] = multCounter;
				if (j - 1 >= 0) lineMap[i][j - 1].edgeID[RIGHT] = multCounter;

				// Если встретили узел
				if (maze[i][j].gateExist[DOWN] == false ||
					(j - 1 >= 0 && maze[i][j - 1].gateExist[DOWN] == false) ||
					(i + 1 < widthOfMaze && maze[i + 1][j].gateExist[LEFT])) {
					// Записываем в структуру endCoors координату конечной точки для данного множества
					endCoors.insert({ multCounter, {j * lengthOfCell + originX, (i + 1) * lengthOfCell + originY} });

					// Увеличиваем счетчик
					++multCounter;
				}
			}
		}
	}
	//// Также учтем самую правую границу лабиринта ////
	for (int i = 0; i < widthOfMaze; ++i) {

		// Чтобы добавить линию в множество, нужно чтобы она существовала
		if (maze[i][lengthOfMaze - 1].gateExist[RIGHT] == false) {
			// Присваиваем множество стенке
			lineMap[i][lengthOfMaze - 1].edgeID[RIGHT] = multCounter;

			// Если встретили узел
			if (maze[i][lengthOfMaze - 1].gateExist[DOWN] == false ||
				(i + 1 < widthOfMaze && maze[i + 1][lengthOfMaze - 1].gateExist[RIGHT])) {
				// Записываем в структуру endCoors координату конечной точки для данного множества
				endCoors.insert({ multCounter, { lengthOfMaze * lengthOfCell + originX, (i + 1) * lengthOfCell + originY} });

				// Увеличиваем счетчик
				++multCounter;
			}
		}
	}


	// ---- Затем будем заполнять карту линиями (т.е для каждой линии писать ее конец) ---- // 
	std::pair<int, int> strtPoint, endPoint;
	for (int i = 0; i < widthOfMaze; ++i) {
		for (int j = 0; j < lengthOfMaze; ++j) {
			for (int dir = 0; dir < 4; ++dir) {
				// Если стенка существует задать конечную и начальную точку
				if (endCoors.find(lineMap[i][j].edgeID[dir]) != endCoors.end())
					endPoint = endCoors[lineMap[i][j].edgeID[dir]];
				// иначе если стенки нет, то пропустить
				else
					continue;

				if (dir == UP || dir == LEFT)
					strtPoint = { j * lengthOfCell + originX, i * lengthOfCell + originY };
				else if (dir == DOWN)
					strtPoint = { j * lengthOfCell + originX, (i + 1) * lengthOfCell + originY };
				else if (dir == RIGHT)
					strtPoint = { (j + 1) * lengthOfCell + originX, i * lengthOfCell + originY };

				lineMap[i][j].walls[dir].setParametrs(strtPoint.first, strtPoint.second, endPoint.first, endPoint.second);
			}
		}
	}
}

void createLabyrinth(std::vector<std::vector<CellLineLab> >& lineMap, int width, int length, int lengthOfCell, int originX, int originY) {
	
	// Создадим структуру хранящую лабиринт
	std::vector<std::vector<CellGenLab>> maze(width, std::vector<CellGenLab>(length, { false, {false, false, false, false} }));
	// Сгенерируем лабиринт
	generateLabyrinth(maze, rand() % length, rand() % width, width, length);
	maze[0][length / 2].gateExist[UP] = true;

	// Иницилизируем структуру хранящую карту лабиринта	
	for (int i = 0; i < lineMap.size(); ++i) {
		for (int j = 0; j < lineMap[0].size(); ++j) {
			for (int k = 0; k < 4; ++k) {
				lineMap[i][j].edgeID[k] = NOT_EXIST;
				lineMap[i][j].walls[k] = { NOT_EXIST, NOT_EXIST, NOT_EXIST, NOT_EXIST };
			}
		}
	}
	

	// Создаем карту лабиринта
	createMap(maze, lineMap, lengthOfCell, originX, originY);

}

void areaOfVisibility(std::vector<std::vector<CellLineLab> >& lineMap, std::vector<Line>& areaMap, int areaRadius, int centreX, int centreY) {
	// Координаты прохода по строке
	int strtI = centreY - areaRadius < 0 ? 0 : centreY - areaRadius,
		endI  = centreY + areaRadius >= lineMap.size() ? lineMap.size() - 1 : centreY + areaRadius;

	// Координаты прохода по столбцам
	int strtJ = centreX - areaRadius < 0 ? 0 : centreX - areaRadius,
		endJ  = centreX + areaRadius >= lineMap[0].size() ? lineMap[0].size() - 1 : centreX + areaRadius;
		
	// Создадим структуру хранящую отрисованные множества
	std::set<int> drawedBefore;
	areaMap.clear();

	// Пройдемся по области двумерного массива
	for (int i = strtI; i <= endI; ++i) {
		for (int j = strtJ; j <= endJ; ++j) {
			for (int dir = 0; dir < 4; ++dir) {
				// Если стенка существует и не была отрисована до этого
				if (lineMap[i][j].edgeID[dir] != NOT_EXIST &&
					drawedBefore.find(lineMap[i][j].edgeID[dir]) == drawedBefore.end()) {

					// Добавим стенку в область видимости
					areaMap.push_back(lineMap[i][j].walls[dir]);
					// Пометим ее как посещенную
					drawedBefore.insert(lineMap[i][j].edgeID[dir]);
				}
			}
		}
	}
}

