#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

// ���������� �����
struct MyPoint {
	double x;
	double y;
};

class Line {
private:
	double x0, y0;			// ��������� ����������
	double x, y;			// �������� ����������
	double xProj, yProj;	// �������� ����� �� ������������ ��� 
	double angle;			// ���� ������� �����
public:
	Line(double x0, double y0, double x, double y);

	void setParametrs(double x0, double y0, double x, double y);

	Line& operator= (const Line& other);

	MyPoint getStartCoor();	// ��������� ��������� ������
	MyPoint getEndCoor();	// ��������� ��������� �����
	MyPoint getProjCoor();	// ��������� ��������
	double getAngle();		// ��������� ���� �������
};


double find_distance_between_ray_vector_and_line(Line ray, Line l);

void createLight(sf::RenderWindow* window, std::vector<Line> map, double x0Ray, double y0Ray, sf::Vector3i rgb);
