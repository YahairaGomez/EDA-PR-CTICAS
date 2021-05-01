#include "pch.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <queue>
#include <string>
#include <sstream>

using namespace std;

int maxNumPoints = 0;


class Point {
public:
	float x;
	float y;
	Point() {}
	Point(float x, float y) {
		this->x = x;
		this->y = y;
	}
};

class QuadTree {
public:
	Point topLeftPoint;
	Point botRightPoint;
	int height = 0;
	vector<Point> data;
	int numberOfPoints = 0;

	vector<QuadTree *> quadrants;


	QuadTree(Point _topLeftPoint, Point _botRightPoint,int height = 0) {
		topLeftPoint = _topLeftPoint;
		botRightPoint = _botRightPoint;
		this->height = height;
		for (int i = 0; i < 4; i++) {
			quadrants.push_back(nullptr);
		}
	}

	void exportStructureToTxt() {
		QuadTree* s = this;
		int actualHeigth = s->height;
		queue<QuadTree*> path;
		string fileName = "data.txt";
		path.push(s);
		ofstream myfile;
		myfile.open(fileName.c_str());
		while (!path.empty())
		{
			s = path.front();
			if (s->height == 12 )
			{
				int color = s->numberOfPoints % 100;
				string colorJ = "";
				if (color >= 0 && color <= 30) colorJ = "\'red\'";
				else if (color > 30 && color <= 60) colorJ = "\'orange\'";
				else if (color > 60 && color <= 100) colorJ = "\'yellow\'";


				myfile << s->topLeftPoint.x << " " << s->topLeftPoint.y << "$" << s->botRightPoint.x << " " << s->botRightPoint.y << "/" << colorJ << endl;
			}

			

			for (int i = 0; i < 4; i++)
			{
				if (s->quadrants[i] != nullptr)
				{
					path.push(s->quadrants[i]);
				}
			}

			path.pop();
			
			
		}

		myfile.close();

	}

	bool isInCube(Point topLeftFrontPoint,Point botRightBackPoint,Point p) {
		float x0 = min(topLeftFrontPoint.x,botRightBackPoint.x);
		float x1 = max(topLeftFrontPoint.x, botRightBackPoint.x);
		float y0 = min(topLeftFrontPoint.y, botRightBackPoint.y);
		float y1 = max(topLeftFrontPoint.y, botRightBackPoint.y);

		return (p.x>=x0 && p.x<=x1 && p.y >=y0 && p.y<=y1);
	}

	bool findPoint(vector<Point> data,Point p) {
		for (int i = 0; i < data.size(); i++) {
			if (data[i].x == p.x && data[i].y == p.y) {
				return true;
			}
		}
		return false;
	}

	void search(Point p) {
		QuadTree *s = this;
		if (s->isInCube(s->topLeftPoint, s->botRightPoint, p)) {
			while (s->quadrants[0] != nullptr&&s->quadrants[1] != nullptr&&s->quadrants[2] != nullptr&&s->quadrants[3] != nullptr) {
				for (int i = 0; i < s->quadrants.size(); i++) {
					if (s->quadrants[i]->isInCube(s->quadrants[i]->topLeftPoint, s->quadrants[i]->botRightPoint, p)) {
						s = s->quadrants[i];
						break;
					}
				}
			}
			cout << "El punto esta en el cuadrante con los puntos :p1(" << s->topLeftPoint.x << "," << s->botRightPoint.y << ")" << "y p2(" << s->topLeftPoint.x << "," << s->botRightPoint.y << ")" << endl;
		}
		else {
			cout << "El Punto no esta dentro de los limites iniciales" << endl;
		}
	}

	void insertPoint(Point p) {
		QuadTree *s = this;
		if (s->isInCube(s->topLeftPoint,s->botRightPoint,p)) {
			s->numberOfPoints++;
			while (s->quadrants[0] != nullptr&&s->quadrants[1] != nullptr&&s->quadrants[2] != nullptr&&s->quadrants[3] != nullptr) {
				for (int i = 0; i < s->quadrants.size(); i++) {
					if (s->quadrants[i]->isInCube(s->quadrants[i]->topLeftPoint, s->quadrants[i]->botRightPoint, p)) {
						s = s->quadrants[i];
						s->numberOfPoints++;
						break;
					}
				}
			} 
			
			s->data.push_back(p);
			

			while (s->data.size() > maxNumPoints && s->height<20) {
				Point midPointSquare((s->topLeftPoint.x + s->botRightPoint.x) / 2, (s->topLeftPoint.y + s->botRightPoint.y) / 2);
				float gapX = abs(s->topLeftPoint.x - s->botRightPoint.x)/2;
				float gapY = abs(s->topLeftPoint.y - s->botRightPoint.y)/2;
				int newHeight = s->height + 1;
				s->quadrants[0] = new QuadTree(s->topLeftPoint, midPointSquare,newHeight);
				s->quadrants[1] = new QuadTree(Point(s->quadrants[0]->topLeftPoint.x, s->quadrants[0]->topLeftPoint.y - gapY), Point(s->quadrants[0]->botRightPoint.x, s->quadrants[0]->botRightPoint.y - gapY), newHeight);
				s->quadrants[2] = new QuadTree(Point(s->quadrants[1]->topLeftPoint.x+gapX, s->quadrants[1]->topLeftPoint.y+gapY), Point(s->quadrants[1]->botRightPoint.x+gapX, s->quadrants[1]->botRightPoint.y+gapY), newHeight);
				s->quadrants[3] = new QuadTree(midPointSquare, s->botRightPoint,newHeight);
				
				for (int i = 0; i<s->data.size()-1; i++) {
					for (int j = 0; j < s->quadrants.size(); j++) {
						if (isInCube(s->quadrants[j]->topLeftPoint, s->quadrants[j]->botRightPoint, s->data[i])) {
							s->quadrants[j]->data.push_back(s->data[i]);
							s->quadrants[j]->numberOfPoints++;
							break;
						}
					}
				}

				s->data.clear();

				for (int j = 0; j < s->quadrants.size(); j++) {
					if (s->quadrants[j]->data.size() > maxNumPoints) {
						s = s->quadrants[j];
						break;
					}
				}
			}
		}
		else {
			//cout << "El punto esta fuera de los limites" << endl;
		}
	}

	
};

vector<vector<string>> getTsvData(string fileName) {
	vector<vector<string> > items;
	ifstream ifs(fileName.c_str());
	if (ifs.fail()) {
		cerr << "error" << endl;
		return items;
	}
	string line;
	while (getline(ifs, line)) {
		stringstream ss(line);
		vector<string> item;
		string tmp;
		while (getline(ss, tmp, '\t')) {
			item.push_back(tmp);
		}
		items.push_back(item);
	}

	return items;
}


int main()
{
	Point p1(-181, 181);
	Point p2(181, -181);

	QuadTree tree (p1, p2);

	maxNumPoints = 200;
	vector<vector<string> > items = getTsvData("data_points.tsv");
	srand(static_cast <unsigned int> (time(0)));
	unsigned t0, t1;

	t0 = clock();
	for (int i = 0; i < items.size(); i++) {
		//float x = rand() % 99 - 99;
		//float y = rand() % 99 - 99;
		/*cout << x << endl;
		cout << y << endl;
		cout << z << endl;*/

		Point p(stof(items[i][0]), stof(items[i][1]));
		//cout << p.x << " " << p.y << endl;
 		tree.insertPoint(p);
		if (i==250000||i==100000)
		{
			cout << "Punto Numero: " << i << endl;
		}
		
		/*if(i>=999997)
			tree.search(p);*/
	}
	tree.exportStructureToTxt();
	t1 = clock();
	double time = (double(t1 - t0) / CLOCKS_PER_SEC);

	cout << "Execution Time: " << time << endl;
}