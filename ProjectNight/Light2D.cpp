#include "Light2d.h"

Line::Line(double x0, double y0, double x, double y) {
	this->x0 = x0, this->y0 = y0, this->x = x, this->y = y;
	xProj = x - x0, yProj = y - y0;
	angle = atan2(yProj, xProj);
}

void Line::setParametrs(double x0, double y0, double x, double y) {
	this->x0 = x0, this->y0 = y0;
	this->x = x, this->y = y;
	xProj = x - x0, yProj = y - y0;
	angle = atan2(yProj, xProj);
}

Line& Line::operator= (const Line& other) {
	this->x0 = other.x0;
	this->y0 = other.y0;
	this->x = other.x;
	this->y = other.y;
	this->xProj = other.xProj;
	this->yProj = other.yProj;
	this->angle = other.angle;
	return *this;
}

MyPoint Line::getStartCoor() {
	MyPoint res{ x0, y0 };
	return res;
}

MyPoint Line::getEndCoor() {
	MyPoint res{ x, y };
	return res;
}

MyPoint Line::getProjCoor() {
	MyPoint res{ xProj, yProj };
	return res;
}

double Line::getAngle() {
	return angle;
}


double find_distance_between_ray_vector_and_line(Line ray, Line l) {
	double dxR = ray.getProjCoor().x, dyR = ray.getProjCoor().y;
	double x0R = ray.getStartCoor().x, y0R = ray.getStartCoor().y;
	double dxL = l.getProjCoor().x, dyL = l.getProjCoor().y;
	double x0L = l.getStartCoor().x, y0L = l.getStartCoor().y;

	// Расстояние от начала линии до точки пересечения
	double z = (dxR * (y0L - y0R) - dyR * (x0L - x0R)) / (dxL * dyR - dxR * dyL);
	if (0 <= z && z <= 1) {
		// Расстояние от начала луча до точки его пересечения
		double t = (dxL * (y0R - y0L) - dyL * (x0R - x0L)) / (dxR * dyL - dxL * dyR);
		if (t > 0) {
			return t;
		}
		else {
			return -1;
		}
	}
	else {
		return -1;
	}
}



void createLight(sf::RenderWindow* window, std::vector<Line> map, double x0Ray, double y0Ray, sf::Vector3i rgb) {
	// Проверка, существует ли карта для построения
	if (map.size() == 0) return;

	// Создадим структуру хранящую луч
	Line ray(x0Ray, y0Ray, x0Ray, y0Ray); 
	// Создадим структуру хранящую лучи для
	// дальнейшей отрисовки освещения 
	std::vector<Line> lightArr;

	// Будем пробегаться по карте линий и искать длину наименьшего пересечения
	for (int lineIter = 0; lineIter < map.size(); ++lineIter) {
		// Создадим точку хранящую координату конца линии,
		// для которой в данный момент ищутся пересечения
		MyPoint linePoint;

		// Найдем точки пересечений для концов линии
		for (int k = 0; k < 2; ++k) {

			if (k == 0) linePoint = map[lineIter].getStartCoor(); 
			if (k == 1) linePoint = map[lineIter].getEndCoor();	  
			 
			// Смещаем луч на угол
			for (int angleBias = 0; angleBias < 3; ++angleBias) {
				ray.setParametrs(x0Ray, y0Ray, linePoint.x, linePoint.y);
				double baseAngle = ray.getAngle();	// Угол не отклоненного луча
				double resAngle;					// Угол на который будет отклонен луч

				if (angleBias == 0) resAngle = baseAngle - 0.001f;
				if (angleBias == 1) resAngle = baseAngle;
				if (angleBias == 2) resAngle = baseAngle + 0.001f;

				// Создадим вектор-луч направленный под углом resAngle
				ray.setParametrs(x0Ray, y0Ray, x0Ray + cos(resAngle), y0Ray + sin(resAngle));
				
				// Найдем пересечения
				double distanceToIntersectionPoint = 2147483647; // Расстояние от начала луча до точки пересечения
				double tempDistance = -1;
				for (int i = 0; i < map.size(); ++i) {
					tempDistance = find_distance_between_ray_vector_and_line(ray, map[i]);
					// Из всех пересечений выбираем наименьшее
					if (tempDistance != -1 && tempDistance < distanceToIntersectionPoint) {
						distanceToIntersectionPoint = tempDistance;
					}
				}

				// Присваиваем лучу точку пересечения, как конечную
				ray.setParametrs(x0Ray, y0Ray, x0Ray + distanceToIntersectionPoint * ray.getProjCoor().x, y0Ray + distanceToIntersectionPoint * ray.getProjCoor().y);
				
				// Передаем полученный луч в массив для дальнейшего построения освещения
				lightArr.push_back(ray);
			}
		}
	}

	// Отсортируем массив освещения по углу
	sort(lightArr.begin(), lightArr.end(), [](Line l1, Line l2) { return l1.getAngle() < l2.getAngle(); });

	sf::VertexArray drawLight(sf::TriangleFan, map.size() * 6 + 2);

	drawLight[0].position = sf::Vector2f(x0Ray, y0Ray); // Начальная точка построения веера треугольников - координаты игрока
	drawLight[0].color = sf::Color(rgb.x, rgb.y, rgb.z);
	for (int shadowIter = 0; shadowIter < lightArr.size(); ++shadowIter) {
		drawLight[1 + shadowIter].position = sf::Vector2f(lightArr[shadowIter].getEndCoor().x, lightArr[shadowIter].getEndCoor().y);
		drawLight[1 + shadowIter].color = sf::Color(rgb.x, rgb.y, rgb.z);
	}
	drawLight[lightArr.size() + 1].position = sf::Vector2f(lightArr[0].getEndCoor().x, lightArr[0].getEndCoor().y);
	drawLight[lightArr.size() + 1].color = sf::Color(rgb.x, rgb.y, rgb.z);

	window->draw(drawLight);
}
