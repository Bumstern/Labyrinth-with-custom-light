#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <ctime>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include "Light2D.h"
#include "Labyrinth.h"

// ---- ��������� ----
const double PI = 4 * atan(1);


// ���������� ����� 
void drawMap(sf::RenderWindow *window, std::vector<Line> lines) {
	sf::VertexArray lineRayDraw(sf::Lines, 2);
	for (int i = 0; i < lines.size(); ++i) {
		lineRayDraw[0].position = sf::Vector2f(lines[i].getStartCoor().x, lines[i].getStartCoor().y); lineRayDraw[0].color = sf::Color::Blue;
		lineRayDraw[1].position = sf::Vector2f(lines[i].getEndCoor().x, lines[i].getEndCoor().y); lineRayDraw[1].color = sf::Color::Blue;
		window->draw(lineRayDraw);
	}

}

// ��������� �� ����������� ������ �� �������
void intersectionTestWithWalls(std::vector<std::vector<CellLineLab> >& lineMap, sf::Vector2f& player, sf::Vector2f& velocity, float decel, int cellX, int cellY, float radius, float deltaTime) {
	int i = cellY >= lineMap.size() ? lineMap.size() - 1 : cellY; if (i < 0) i = 0;
	int j = cellX >= lineMap[0].size() ? lineMap[0].size() - 1 : cellX; if (j < 0) j = 0;

	for (int k = 0; k < 4; ++k) {
		if (lineMap[i][j].edgeID[k] != -1) {
			switch (k) {
			case UP:
				if (abs(lineMap[i][j].walls[k].getStartCoor().y - player.y) <= radius) {
					player.y += radius - abs(lineMap[i][j].walls[k].getStartCoor().y - player.y);
					velocity.y *= decel * deltaTime;
				}
				break;
			case DOWN:
				if (abs(lineMap[i][j].walls[k].getStartCoor().y - player.y) <= radius) {
					player.y -= radius - abs(lineMap[i][j].walls[k].getStartCoor().y - player.y);
					velocity.y *= decel * deltaTime;
				}
				break;
			case LEFT:
				if (abs(lineMap[i][j].walls[k].getStartCoor().x - player.x) <= radius) {
					player.x += radius - abs(lineMap[i][j].walls[k].getStartCoor().x - player.x);
					velocity.x *= decel * deltaTime;
				}
				break;
			case RIGHT:
				if (abs(lineMap[i][j].walls[k].getStartCoor().x - player.x) <= radius) {
					player.x -= radius - abs(lineMap[i][j].walls[k].getStartCoor().x - player.x);
					velocity.x *= decel * deltaTime;
				}
				break;
			}
		}
	}
}

// ������������ ��������� ������ � ������� ����������
void keyboardControl(sf::Vector2f& player, double radius, sf::Vector2f& velocity, const float accel, const float decel, const float maxSpeed, float deltaTime) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		velocity.x -= accel * deltaTime;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		velocity.x += accel * deltaTime;
	}
	else {
		velocity.x *= decel * deltaTime;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		velocity.y -= accel * deltaTime;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		velocity.y += accel * deltaTime;
	}
	else {
		velocity.y *= decel * deltaTime;
	}

	if (velocity.x < -maxSpeed) velocity.x = -maxSpeed;
	if (velocity.x > maxSpeed)  velocity.x = maxSpeed;
	if (velocity.y < -maxSpeed) velocity.y = -maxSpeed;
	if (velocity.y > maxSpeed)  velocity.y = maxSpeed;

	player += velocity;
}

// ��������������� �������
void NotSusFun(sf::Vector3i& rgb, float& rgbTimer, float deltaTime) {
	if (rgbTimer >= 0.2) {
		rgb = { rand() % 256, rand() % 256, rand() % 256 };
		rgbTimer = 0;
	}
	rgbTimer += deltaTime;
}

// ������� ����������� ������ ������
bool winnerTest(sf::Vector2f player, MyPoint originOfMaze) {
	if (player.y < originOfMaze.y)
		return true;
	else
		return false;
}


int main() {	
	srand(time(NULL)); // ������ ��� ��� ������� �������

	// ������� ����
	sf::RenderWindow window(sf::VideoMode(650, 650), "Project Night");
	window.setPosition(sf::Vector2i(350, 0));
	window.setFramerateLimit(60);	// ����������� �� ������� ������

	// ������� ��������
	int widthOfMaze = 12, lengthOfMaze = 12; // ������ ���������
	if (widthOfMaze * lengthOfMaze <= 0)	
		return 1;
	MyPoint originOfMaze = { 25, 25 };		 // �� ����� ���������� ����� �������������� ��������
	int lengthOfCell = 50;					 // ������ ������ ��������� � ��������
	bool drawMapFlag = false;				 // ���� ������������ ����� �� �������������� ��������
	std::vector<std::vector<CellLineLab>> lineMap(widthOfMaze, std::vector<CellLineLab>(lengthOfMaze,											{ {NOT_EXIST, NOT_EXIST, NOT_EXIST, NOT_EXIST}, {{ NOT_EXIST, NOT_EXIST, NOT_EXIST, NOT_EXIST }, { NOT_EXIST, NOT_EXIST, NOT_EXIST, NOT_EXIST },{ NOT_EXIST, NOT_EXIST, NOT_EXIST, NOT_EXIST },{ NOT_EXIST, NOT_EXIST, NOT_EXIST, NOT_EXIST }} })); // ������� ������� ���������������� �� ���� (�� ��� � �� �����)
	createLabyrinth(lineMap, widthOfMaze, lengthOfMaze, lengthOfCell, originOfMaze.x, originOfMaze.y);

	// ������� ����� ������� ���������
	std::vector<Line> areaMap;

	// ������� �������� ������� ���������
	sf::Image imgLight;
	imgLight.loadFromFile("img/light2D.png");	
	sf::Texture textLight;
	textLight.loadFromImage(imgLight);
	sf::Sprite sprtLight;
	sprtLight.setTexture(textLight);
	bool sprtLightFlag = true;	// ���� ������������ ����� �� �������������� �������� ������� �����

	// ���������� ������
	sf::SoundBuffer music;
	if (!music.loadFromFile("music.wav"))
		return -1;
	sf::Sound arabicDJ;
	arabicDJ.setBuffer(music);

	// ��������� � �������������� ����������
	const double imgSize = imgLight.getSize().x;	// ������ ���������� �������� 
	sf::Vector3i rgb = {100, 125, 200};				// ���� ���������
	float rgbTimer = 0;
	bool NotSusFlag = false;								
	int cellX = lengthOfMaze / 2,					// ���������� ������ � ��������� �������
		cellY = widthOfMaze - 1;					
	int predCellX = -1, predCellY = -1;
	sf::Vector2f player =							// ���������� ������
	{
	 cellX * lengthOfCell + (float)originOfMaze.x + lengthOfCell / 2.0f, 
	 cellY * lengthOfCell + (float)originOfMaze.y + lengthOfCell / 2.0f
	};
	bool noclipMode = false;						// ����� ����������� ��������� ������ �����
	int radius = 5;									// ������ �����
	sf::CircleShape circle(radius);					// ������� ����-�����
	circle.setOutlineThickness(1);					// ������ ������� ����� �����
	circle.setFillColor(sf::Color::Black);			// ���� ������
	sf::Vector2f velocity = {0, 0};					// �������� ������		
	float accel = 5.0;								// ��������� ������
	float decel = 0.1;								// ���������� ������
	float maxSpeed = 5.0;							// ������������ �������� ������
	sf::Clock deltaClock;							// ������ ���� ������� ��� ������� ��������
	float deltaTime;

	// ������� ��� ������������ ���������� � �������������� � ����������
	setlocale(LC_ALL, "ru");
	std::cout <<
		"������ �������������� � ����������:\n" <<
		"1) WASD - ���������� ������� \n" <<
		"2) M - �������� ��������������� ����� \n" <<
		"3) L - ���/���� ������ ��������� \n" <<
		"4) N - ���/���� ����������� ������ ����� \n";


	while (window.isOpen()) {
		deltaTime = deltaClock.restart().asSeconds();

		// ��������� �������
		sf::Event event;
		if (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}	
			if (event.type == sf::Event::KeyPressed) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
					if (NotSusFlag == false) {
						arabicDJ.play();
						accel *= 10.0;
					}
					else {
						arabicDJ.pause();
						accel /= 10.0;
					}
					NotSusFlag = !NotSusFlag;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
					drawMapFlag = !drawMapFlag;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
					sprtLightFlag = !sprtLightFlag;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
					noclipMode = !noclipMode;
				}
			}
		}

		//// ������� ���� ////
		window.clear(sf::Color::Black);

		// �������� ���������� ������ � ���������� ���������� �������
		cellX = (player.x - originOfMaze.x) / lengthOfCell;
		cellY = (player.y - originOfMaze.y) / lengthOfCell;

		// �������� ����� ���������� ������ �� ������������
		keyboardControl(player, radius, velocity, accel, decel, maxSpeed, deltaTime);		

		// ������ ������ � ����� �������
		circle.setPosition(player.x - radius, player.y - radius);

		// ��������� ������ �� �����������
		if (noclipMode == false)
			intersectionTestWithWalls(lineMap, player, velocity, decel, cellX, cellY, radius, deltaTime);
		
		// ������� ����� ������� ��������� 
		if ((predCellX == cellX && predCellY == cellY) == false) // ���� ��� �� ���������� ����� ��� ���� ������
			areaOfVisibility(lineMap, areaMap, 3, cellX, cellY); // ����� ���������� ��� ���������������					
		predCellX = cellX;
		predCellY = cellY;

		// ���� ������ ����� �������� ����� ����� ��������� �������� 
		if (drawMapFlag) drawMap(&window, areaMap);

		// ������ ���������
		if (NotSusFlag) NotSusFun(rgb, rgbTimer, deltaTime);
		createLight(&window, areaMap, player.x, player.y, rgb);

		// ������ ����� ������� ����� �� ���������� ������
		sprtLight.setPosition(player.x - imgSize / 2, player.y - imgSize / 2);		

		window.draw(circle);
		if (sprtLightFlag) window.draw(sprtLight);
		window.display();

		// �������� �� ����� �� ���������
		if (winnerTest(player, originOfMaze)) {
			// ���� �����, �� ������� ����� � ���������� ������ � ��������� �������
			createLabyrinth(lineMap, widthOfMaze, lengthOfMaze, lengthOfCell, originOfMaze.x, originOfMaze.y);
			player ={ (lengthOfMaze / 2) * lengthOfCell + (float)originOfMaze.x + lengthOfCell / 2.0f,
					  (widthOfMaze - 1) * lengthOfCell + (float)originOfMaze.y + lengthOfCell / 2.0f };
		}
	}
}
