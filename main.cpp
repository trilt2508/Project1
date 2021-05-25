#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <map>
#include <vector>
#include <ctime>
#include "minheap.hpp"
#include <math.h>
#include <deque>
#include <time.h>

using namespace cv;
using namespace std;

#define PI 3.14159265

typedef struct Car {
	double L;
	double v;
	double R;
}Car;

Car* car = (Car*)malloc(sizeof(Car));

Car_Terrain* xuatphat = (Car_Terrain*)malloc(sizeof(Car_Terrain));
Car_Terrain* ketthuc = (Car_Terrain*)malloc(sizeof(Car_Terrain));

bool found = false;

int Map[101][101];
int Map_mo[101][101];

Car_Terrain* ketqua;

PriorityQueue Open_Start;
int test = 1;

map<int, int> check_xyteta_Start;
map<int, Car_Terrain*> map_Car_Terrain_Start;

PriorityQueue Open_Finish;

map<int, int> check_xyteta_Finish;
map<int, Car_Terrain*> map_Car_Terrain_Finish;


int k_c(Car_Terrain* p, Car_Terrain* q);
void readWall() {
	ifstream infile;
	int x;
	infile.open("Map.txt");
	for (int i = 1; i <= 50; i++) {
		for (int j = 1; j <= 100; j++) {
			Map[i][j] = 0;
			Map_mo[i][j] = 0;
		}
	}
	for (int i = 1; i <= 3; i++) {
		for (int j = 56; j <= 58; j ++) {
			Map_mo[i][j] = 1;
		}
	}
	infile >> xuatphat->x >> xuatphat->y >> xuatphat->teta;
	infile >> ketthuc->x >> ketthuc->y >> ketthuc->teta;
	xuatphat->gx = 0;
	xuatphat->hx = k_c(xuatphat, ketthuc);
	xuatphat->fx = xuatphat->gx + xuatphat->hx;
	xuatphat->parent = NULL;
	ketthuc->fx = 0;
	ketthuc->gx = 0;
	ketthuc->hx = 0;
	ketthuc->parent = NULL;
	for (int i = 1; i <= 50; i++) {
		for (int j = 1; j <= 100; j++) {
			infile >> x;
			Map[i][j] = x;
		}
	}
	infile.close();
	return;
}

void draw(Car* car, Car_Terrain* car_Terrain, Car_Terrain* startPoint, Car_Terrain* finishPoint) {
	double teta = (double)car_Terrain->teta - 90;
	if (teta > 180)
		teta -= 360;
	teta = teta * PI / 180;
	int x1 = car->R * cos(atan(0.5) + teta);
	int y1 = car->R * sin(atan(0.5) + teta);
	int x2 = car->R * cos(atan(0.5) - teta);
	int y2 = car->R * sin(-atan(0.5) + teta);
	int x = car_Terrain->x * 10;
	int y = car_Terrain->y * 10;
	Mat image = Mat::zeros(1000, 500, CV_8UC3);
	line(image, cv::Point(x + x1, y + y1), cv::Point(x + x2, y + y2), cv::Scalar(0, 0, 255), 1); // Dau xe
	line(image, cv::Point(x - x1, y - y1), cv::Point(x - x2, y - y2), cv::Scalar(0, 255, 0), 1);
	line(image, cv::Point(x + x1, y + y1), cv::Point(x - x2, y - y2), cv::Scalar(0, 0, 255), 1);
	line(image, cv::Point(x - x1, y - y1), cv::Point(x + x2, y + y2), cv::Scalar(0, 0, 255), 1);
	line(image, cv::Point(x + x1, y + y1), cv::Point(x - x1, y - y1), cv::Scalar(0, 0, 255), 1);
	line(image, cv::Point(x + x2, y + y2), cv::Point(x - x2, y - y2), cv::Scalar(0, 0, 255), 1);
	cv::circle(image, cv::Point(10 * finishPoint->x, 10 * finishPoint->y), 10, cv::Scalar(0, 140, 255), 1);
	cv::circle(image, cv::Point(10 * startPoint->x, 10 * startPoint->y), 10, cv::Scalar(0, 140, 255), 1);
	//Ve tuong

	for (int i = 1; i <= 50; i++) {
		for (int j = 1; j <= 100; j++) {
			if (Map[i][j] == 2)
				cv::rectangle(image, cv::Rect(10 * i - 5, 10 * j - 5, 10, 10), cv::Scalar(0, 255, 255), -1);
		}
	}

	imshow("Display Window", image);
	waitKey(80);
}

int k_c(Car_Terrain* p, Car_Terrain* q) {
	double S;
	S = sqrt((p->x - q->x) * (p->x - q->x) + (p->y - q->y) * (p->y - q->y));
	int teta = abs(p->teta - q->teta);
	if (teta == 45) {
		S += 3;
	}
	else if (teta == 90) {
		S += 6;
	}
	else if (teta == 135) {
		S += 9;
	}if (teta == 180) {
		S += 15;
	}
	return S;
}


void Link(Car_Terrain* start, Car_Terrain* finish) {
	deque<Car_Terrain*> Deque;
	Car_Terrain* p = start->parent;
	while (p != NULL) {
		Deque.push_front(p);
		p = p->parent;
	}
	p = finish;
	while (p != NULL) {
		p->teta = (p->teta + 180) % 360;
		Deque.push_back(p);
		p = p->parent;
	}
	p = Deque.front();
	Deque.pop_front();
	Car_Terrain* q = p;
	while (Deque.size() != 0) {
		q->parent = Deque.front();
		Deque.pop_front();
		q = q->parent;
	}
	
	ketqua = p;
	q->parent = NULL;
}

Car_Terrain* create_Car_Terrain(int x, int y, int teta, int gx, Car_Terrain* p) {
	Car_Terrain* q = (Car_Terrain*)malloc(sizeof(Car_Terrain));
	q->x = x;
	q->y = y;
	q->teta = teta;
	q->parent = p;
	q->gx = gx;
	q->hx = k_c(q, ketthuc);
	q->fx = q->gx + q->hx;
	return q;
}

//Start
void addOpen_Start(Car_Terrain* p) {
	Open_Start.push(p);
	check_xyteta_Start[p->x * 1000000 + p->y * 1000 + p->teta] = 1;
	map_Car_Terrain_Start[p->x * 1000000 + p->y * 1000 + p->teta] = p;
	return;
}

Car_Terrain* popOpen_Start() {
	Car_Terrain* p = Open_Start.top();
	Open_Start.pop();
	check_xyteta_Start[p->x * 1000000 + p->y * 1000 + p->teta] = -1;
	return p;
}


void sinhthemQ_Start(Car_Terrain* p) {
	switch (p->teta)
	{
	case 0: {
		if (p->y == 100) break;
		//TH1
		int q_x, q_y, q_teta;
		q_x = p->x;
		q_y = p->y + 1;
		q_teta = 0;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(q, qqq);
					found = true;
					return;
				}
				addOpen_Start(q);
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->x != 1) {
			q_x = p->x - 1;
			q_y = p->y + 1;
			q_teta = 45;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x != 50) {
			q_x = p->x + 1;
			q_y = p->y + 1;
			q_teta = 315;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 45: {
		if (p->y == 100 || p->x == 1) break;
		//TH1
		int q_x, q_y, q_teta;
		q_x = p->x - 1;
		q_y = p->y + 1;
		q_teta = 45;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(q, qqq);
					found = true;
					return;
				}
				addOpen_Start(q);
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->x > 2) {
			q_x = p->x - 2;
			q_y = p->y + 1;
			q_teta = 90;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->y < 99) {
			q_x = p->x - 1;
			q_y = p->y + 2;
			q_teta = 0;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 90: {
		//TH1
		if (p->x == 1)
			break;
		int q_x, q_y, q_teta;
		q_x = p->x - 1;
		q_y = p->y;
		q_teta = 90;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(q, qqq);
					found = true;
					return;
				}
				addOpen_Start(q);
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y != 1) {
			q_x = p->x - 1;
			q_y = p->y - 1;
			q_teta = 135;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->y != 100) {
			q_x = p->x - 1;
			q_y = p->y + 1;
			q_teta = 45;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 135: {
		//TH1
		if (p->y == 1 || p->x == 1) break;
		int q_x, q_y, q_teta;
		q_x = p->x - 1;
		q_y = p->y - 1;
		q_teta = 135;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(q, qqq);
					found = true;
					return;
				}
				addOpen_Start(q);
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y > 2) {
			q_x = p->x - 1;
			q_y = p->y - 2;
			q_teta = 180;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x > 2) {
			q_x = p->x - 2;
			q_y = p->y - 1;
			q_teta = 90;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 180: {
		//TH1
		if (p->y == 1) break;
		int q_x, q_y, q_teta;
		q_x = p->x;
		q_y = p->y - 1;
		q_teta = 180;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(q, qqq);
					found = true;
					return;
				}
				addOpen_Start(q);
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->x != 1) {
			q_x = p->x - 1;
			q_y = p->y - 1;
			q_teta = 135;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x != 50) {
			q_x = p->x + 1;
			q_y = p->y - 1;
			q_teta = 225;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 225: {
		if (p->x == 50 || p->y == 1) break;
		//TH1
		int q_x, q_y, q_teta;
		q_x = p->x + 1;
		q_y = p->y - 1;
		q_teta = 225;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(q, qqq);
					found = true;
					return;
				}
				addOpen_Start(q);
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y > 2) {
			q_x = p->x + 1;
			q_y = p->y - 2;
			q_teta = 180;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x < 49) {
			q_x = p->x + 2;
			q_y = p->y - 1;
			q_teta = 270;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 270: {
		//TH1
		if (p->x == 50) break;
		int q_x, q_y, q_teta;
		q_x = p->x + 1;
		q_y = p->y;
		q_teta = 270;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(q, qqq);
					found = true;
					return;
				}
				addOpen_Start(q);
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y != 1) {
			q_x = p->x + 1;
			q_y = p->y - 1;
			q_teta = 225;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->y != 100) {
			q_x = p->x + 1;
			q_y = p->y + 1;
			q_teta = 315;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 315: {
		if (p->x == 50 || p->y == 100) break;
		//TH1
		int q_x, q_y, q_teta;
		q_x = p->x + 1;
		q_y = p->y + 1;
		q_teta = 315;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(q, qqq);
					found = true;
					return;
				}
				addOpen_Start(q);
			}
			if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y < 99) {
			q_x = p->x + 1;
			q_y = p->y + 2;
			q_teta = 0;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x < 49) {
			q_x = p->x + 2;
			q_y = p->y + 1;
			q_teta = 270;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(q, qqq);
						found = true;
						return;
					}
					addOpen_Start(q);
				}
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Start[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	default:
		break;
	}
}


//Finish
void addOpen_Finish(Car_Terrain* p) {
	Open_Finish.push(p);
	check_xyteta_Finish[p->x * 1000000 + p->y * 1000 + p->teta] = 1;
	map_Car_Terrain_Finish[p->x * 1000000 + p->y * 1000 + p->teta] = p;
	return;
}

Car_Terrain* popOpen_Finish() {
	Car_Terrain* p = Open_Finish.top();
	Open_Finish.pop();
	check_xyteta_Finish[p->x * 1000000 + p->y * 1000 + p->teta] = -1;
	return p;
}

void sinhthemQ_Finish(Car_Terrain* p) {
	switch (p->teta)
	{
	case 0: {
		if (p->y == 100) break;
		//TH1
		int q_x, q_y, q_teta;
		q_x = p->x;
		q_y = p->y + 1;
		q_teta = 0;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->x != 1) {
			q_x = p->x - 1;
			q_y = p->y + 1;
			q_teta = 45;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x != 50) {
			q_x = p->x + 1;
			q_y = p->y + 1;
			q_teta = 315;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH1
		if (p->y == 1) break;
		q_x = p->x;
		q_y = p->y - 1;
		q_teta = 0;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->x != 1) {
			q_x = p->x - 1;
			q_y = p->y - 1;
			q_teta = 315;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x != 50) {
			q_x = p->x + 1;
			q_y = p->y - 1;
			q_teta = 45;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 45: {
		if (p->y == 100 || p->x == 1) break;
		//TH1
		int q_x, q_y, q_teta;
		q_x = p->x - 1;
		q_y = p->y + 1;
		q_teta = 45;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->x > 2) {
			q_x = p->x - 2;
			q_y = p->y + 1;
			q_teta = 90;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->y < 99) {
			q_x = p->x - 1;
			q_y = p->y + 2;
			q_teta = 0;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		if (p->x == 50 || p->y == 1) break;
		//TH1
		q_x = p->x + 1;
		q_y = p->y - 1;
		q_teta = 45;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y > 2) {
			q_x = p->x + 1;
			q_y = p->y - 2;
			q_teta = 0;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x < 49) {
			q_x = p->x + 2;
			q_y = p->y - 1;
			q_teta = 90;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 90: {
		//TH1
		if (p->x == 1)
			break;
		int q_x, q_y, q_teta;
		q_x = p->x - 1;
		q_y = p->y;
		q_teta = 90;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y != 1) {
			q_x = p->x - 1;
			q_y = p->y - 1;
			q_teta = 135;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->y != 100) {
			q_x = p->x - 1;
			q_y = p->y + 1;
			q_teta = 45;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH1
		if (p->x == 50) break;
		q_x = p->x + 1;
		q_y = p->y;
		q_teta = 90;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y != 1) {
			q_x = p->x + 1;
			q_y = p->y - 1;
			q_teta = 45;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->y != 100) {
			q_x = p->x + 1;
			q_y = p->y + 1;
			q_teta = 135;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 135: {
		//TH1
		if (p->y == 1 || p->x == 1) break;
		int q_x, q_y, q_teta;
		q_x = p->x - 1;
		q_y = p->y - 1;
		q_teta = 135;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y > 2) {
			q_x = p->x - 1;
			q_y = p->y - 2;
			q_teta = 180;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x > 2) {
			q_x = p->x - 2;
			q_y = p->y - 1;
			q_teta = 90;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		if (p->x == 50 || p->y == 100) break;
		//TH1
		q_x = p->x + 1;
		q_y = p->y + 1;
		q_teta = 135;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y < 99) {
			q_x = p->x + 1;
			q_y = p->y + 2;
			q_teta = 180;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x < 49) {
			q_x = p->x + 2;
			q_y = p->y + 1;
			q_teta = 90;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 180: {
		//TH1
		if (p->y == 1) break;
		int q_x, q_y, q_teta;
		q_x = p->x;
		q_y = p->y - 1;
		q_teta = 180;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->x != 1) {
			q_x = p->x - 1;
			q_y = p->y - 1;
			q_teta = 135;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x != 50) {
			q_x = p->x + 1;
			q_y = p->y - 1;
			q_teta = 225;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		if (p->y == 100) break;
		//TH1
		q_x = p->x;
		q_y = p->y + 1;
		q_teta = 180;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->x != 1) {
			q_x = p->x - 1;
			q_y = p->y + 1;
			q_teta = 225;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x != 50) {
			q_x = p->x + 1;
			q_y = p->y + 1;
			q_teta = 135;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 225: {
		if (p->x == 50 || p->y == 1) break;
		//TH1
		int q_x, q_y, q_teta;
		q_x = p->x + 1;
		q_y = p->y - 1;
		q_teta = 225;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y > 2) {
			q_x = p->x + 1;
			q_y = p->y - 2;
			q_teta = 180;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x < 49) {
			q_x = p->x + 2;
			q_y = p->y - 1;
			q_teta = 270;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		if (p->y == 100 || p->x == 1) break;
		//TH1
		q_x = p->x - 1;
		q_y = p->y + 1;
		q_teta = 225;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->x > 2) {
			q_x = p->x - 2;
			q_y = p->y + 1;
			q_teta = 270;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->y < 99) {
			q_x = p->x - 1;
			q_y = p->y + 2;
			q_teta = 180;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 270: {
		//TH1
		if (p->x == 50) break;
		int q_x, q_y, q_teta;
		q_x = p->x + 1;
		q_y = p->y;
		q_teta = 270;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y != 1) {
			q_x = p->x + 1;
			q_y = p->y - 1;
			q_teta = 225;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->y != 100) {
			q_x = p->x + 1;
			q_y = p->y + 1;
			q_teta = 315;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH1
		if (p->x == 1)
			break;
		q_x = p->x - 1;
		q_y = p->y;
		q_teta = 270;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					q->gx = p->gx + 1;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y != 1) {
			q_x = p->x - 1;
			q_y = p->y - 1;
			q_teta = 315;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->y != 100) {
			q_x = p->x - 1;
			q_y = p->y + 1;
			q_teta = 225;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						q->gx = p->gx + 1.6;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 1.6) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	case 315: {
		if (p->x == 50 || p->y == 100) break;
		//TH1
		int q_x, q_y, q_teta;
		q_x = p->x + 1;
		q_y = p->y + 1;
		q_teta = 315;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y < 99) {
			q_x = p->x + 1;
			q_y = p->y + 2;
			q_teta = 0;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x < 49) {
			q_x = p->x + 2;
			q_y = p->y + 1;
			q_teta = 270;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH1
		if (p->y == 1 || p->x == 1) break;
		q_x = p->x - 1;
		q_y = p->y - 1;
		q_teta = 315;
		if (Map[q_x][q_y] == 0) {
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					q->gx = p->gx + 1.6;
					q->fx = q->gx + q->hx;
					q->parent = p;
				}
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
				Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 1.6, p);
				if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
					Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
					Link(qqq, q);
					found = true;
					return;
				}
				addOpen_Finish(q);
			}
			if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
				Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
				if (q->gx > p->gx + 1.6) {
					check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
				}
			}
		}
		//TH2
		if (p->y > 2) {
			q_x = p->x - 1;
			q_y = p->y - 2;
			q_teta = 0;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		//TH3
		if (p->x > 2) {
			q_x = p->x - 2;
			q_y = p->y - 1;
			q_teta = 270;
			if (Map[q_x][q_y] == 0) {
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						q->gx = p->gx + 2.5;
						q->fx = q->gx + q->hx;
						q->parent = p;
					}
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == 0) {
					Car_Terrain* q = create_Car_Terrain(q_x, q_y, q_teta, p->gx + 2.5, p);
					if (check_xyteta_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)] == 1) {
						Car_Terrain* qqq = map_Car_Terrain_Start[q_x * 1000000 + q_y * 1000 + ((q_teta + 180) % 360)];
						Link(qqq, q);
						found = true;
						return;
					}
					addOpen_Finish(q);
				}
				if (check_xyteta_Finish[q_x * 1000000 + q_y * 1000 + q_teta] == -1) {
					Car_Terrain* q = map_Car_Terrain_Finish[q_x * 1000000 + q_y * 1000 + q_teta];
					if (q->gx > p->gx + 2.5) {
						check_xyteta_Finish[q->x * 1000000 + q->y * 1000 + q->teta] = 1;
					}
				}
			}
		}
		break;
	}
	default:
		break;
	}
}



void begin(Car_Terrain* start, Car_Terrain* finish) {
	finish->teta = (finish->teta + 180) % 360;
	addOpen_Start(start);
	addOpen_Finish(finish);
	Car_Terrain* p_Start;
	Car_Terrain* p_Finish;
	/*clock_t _start, _end;
	double cpu_time_used;

	_start = clock();*/
	while (Open_Start.size() != 0 && Open_Finish.size() != 0) {
		p_Start = popOpen_Start();
		p_Finish = popOpen_Finish();
		if (found) {
			break;
		}
		sinhthemQ_Start(p_Start);
		if (!found)
			sinhthemQ_Finish(p_Finish);
	}/*
	_end = clock();
	cpu_time_used = ((double)(_end - _start)) / CLOCKS_PER_SEC;
	cout << "Thoi gian thuc hien tim kiem la: " << cpu_time_used << " s";*/
}

void go() {
	Car_Terrain* p = ketqua;
	int x = 1;
	if (p == NULL) {
		cout << "ko tim dc";
		return;
	}
	while (x == 1) {
		p = ketqua;
		while (p != NULL)
		{
			draw(car, p, xuatphat, ketthuc);
			if (p->parent != NULL && test)
				if (Map_mo[p->parent->x][p->parent->y]) {
					for (int i = 1; i <= 3; i++) {
						for (int j = 56; j <= 58; j += 2) {
							Map[i][j] = 1;
							Map_mo[i][j] = 0;
						}
					}
					for (int j = 1; j <= 3; j++) {
						Map[j][57] = 2;
						Map_mo[j][57] = 0;
					}
					xuatphat = p;
					if (xuatphat->teta >= 180)
						xuatphat->teta -= 180;
					else
						xuatphat->teta += 180;
					check_xyteta_Start.clear();
					check_xyteta_Finish.clear();
					Open_Start.Clear();
					Open_Finish.Clear();
					map_Car_Terrain_Start.clear();	
					map_Car_Terrain_Finish.clear();
					found = false;
					ketqua = NULL;
					test--;
					xuatphat->y++;
					xuatphat->parent = NULL;
					ketthuc->parent = NULL;
					begin(xuatphat, ketthuc);
					go();
					return;
				}
			p = p->parent;

		}
		p = ketthuc;
		cout << "Nhap 1 de tiep tuc: ";
		cin >> x;
	}
}


int main() {
	car->L = 30;
	car->R = car->L * sqrt(5) / 4;
	car->v = 10;
	readWall();
	begin(xuatphat, ketthuc);
	go();
}