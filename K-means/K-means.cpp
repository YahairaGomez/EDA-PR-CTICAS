// Implementation of the KMeans Algorithm
#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

template<typename T>
void pop_front(std::vector<T>& v)
{
	if (v.size() > 0) {
		v.erase(v.begin());
	}
}
struct Point
{

	int id_point, id_cluster;
	vector<long double> values;
	
	Point(int id_point, vector<double>& coordinates)
	{
		this->id_point = id_point;

		this->values.push_back(coordinates[0]);
		this->values.push_back(coordinates[1]);

		id_cluster = -1;
	}
	long double getValue(int index) { return values[index]; }

	void addValue(double value) { values.push_back(value); }

	int getIDPoint(){ return id_point;}
	
	void setCluster(int id_cluster){ this->id_cluster = id_cluster;}
	
	int getCluster(){ return id_cluster;}
	
};


struct  Cluster
{

	int id_cluster;
	vector<long double> central_values;
	vector<Point> points;

	Cluster(int id_cluster, Point point)
	{
		this->id_cluster = id_cluster;

		central_values.push_back(point.getValue(0));
		central_values.push_back(point.getValue(1));
		
		points.push_back(point);
	}
	Point getPoint(int index) { return points[index]; }

	void addPoint(Point point) { points.push_back(point); }

	long getTotalPoints() { return points.size(); }

	//When a point is in a new cluster, I have to remove it from the previous one
	bool removePoint(int id_point)
	{
		int total_points = points.size();

		for(int i = 0; i < total_points; i++)
		{
			if(points[i].getIDPoint() == id_point)
			{
				points.erase(points.begin() + i);
				return true;
			}
		}
		return false;
	}
	int getIDCluster() { return id_cluster; }

	long double getCentralValue(int index){ return central_values[index];}

	void setCentralValue(int index, double value){	central_values[index] = value;}	
	
};

class KMeans
{
private:
	int K; // number of clusters
	long  total_points;
	int max_iterations;
	vector<Cluster> clusters;

	// return ID of nearest center 
	int getIDNearestCenter(Point point)
	{
		long double sum = 0.0, min_dist;
		int id_cluster_center = 0;

		sum += pow(clusters[0].getCentralValue(0) - point.getValue(0), 2.0) + pow(clusters[0].getCentralValue(1) - point.getValue(1), 2.0);
		
		min_dist = sqrt(sum);

		for(int i = 1; i < K; i++)
		{
			long double dist;
			sum = 0.0;

			sum += pow(clusters[i].getCentralValue(0) - point.getValue(0), 2.0) + pow(clusters[i].getCentralValue(1) - point.getValue(1), 2.0);
			
			dist = sqrt(sum);

			if(dist < min_dist)
			{
				min_dist = dist;
				id_cluster_center = i;
			}
		}

		return id_cluster_center;
	}

public:
	KMeans(int K, long total_points, int max_iterations)
	{
		this->K = K;
		this->total_points = total_points;
		
		this->max_iterations = max_iterations;
	}
	

	void go(vector<Point> & points)
	{
		ofstream myfile;
		myfile.open("output.csv");
		myfile << "x,y,c" << endl;

		if (!myfile)
		{
			cout << "Writing to file failed" << endl;
		}

		if(K > total_points)
			return;
		vector<int> descart_index;

		// I choose K distinct values for the centers of the clusters randomly (those that can't be)
		for(int i = 0; i < K; i++)
		{
			while(true)
			{
				int index_point = rand() % total_points;

				if(find(descart_index.begin(), descart_index.end(), index_point) == descart_index.end())
				{
					descart_index.push_back(index_point);
					points[index_point].setCluster(i);
					Cluster cluster(i, points[index_point]);
					clusters.push_back(cluster);
					break;
				}
			}
		}

		int iter = 1;

		while(true)
		{
			bool done = true;

			// Associates each point to the nearest center
			for(int i = 0; i < total_points; i++)
			{
				int id_old_cluster = points[i].getCluster();
				int id_nearest_center = getIDNearestCenter(points[i]);

				if(id_old_cluster != id_nearest_center)
				{
					if(id_old_cluster != -1)
						clusters[id_old_cluster].removePoint(points[i].getIDPoint());

					points[i].setCluster(id_nearest_center);
					clusters[id_nearest_center].addPoint(points[i]);
					done = false;
				}
			}

			// recalculating center of each cluster
			for(int i = 0; i < K; i++)
			{
				for(int j = 0; j < 2; j++)
				{
					int total_points_cluster = clusters[i].getTotalPoints();
					double sum = 0.0;

					if(total_points_cluster > 0)
					{
						for(int p = 0; p < total_points_cluster; p++)
							sum += clusters[i].getPoint(p).getValue(j);
						clusters[i].setCentralValue(j, sum / total_points_cluster);
					}
				}
			}
		
			//Break iteration, next!!!
			if(done == true || iter >= max_iterations) break;
			iter++;
		}
		
		// Writing to our file "OUTPUT.CSV"
		for(int i = 0; i < K; i++)
		{
			int total_points_cluster =  clusters[i].getTotalPoints();

			for(int j = 0; j < total_points_cluster; j++)
			{
				for (int p = 0; p < 2 ; p+=2) {
					//WRITE!!!! 
					myfile << clusters[i].getPoint(j).getValue(p) << "," << clusters[i].getPoint(j).getValue(p + 1) << "," << clusters[i].getIDCluster() + 1 << endl;
					
				}	
			}
			
		}
		myfile.close();
	}
};

int main(int argc, char *argv[])
{	
	vector<Point> points;
	vector<double> values;
	ifstream file("data_set1.csv");
	string linea;
	char delimiter = ',';
	double x_value, y_value;

	//Read the first row, so we can discard it (name of columns long, lat)
	getline(file, linea);

	int i = 0;
	long cont = 0;	//total number of points
	string x, y;
	
	while (getline(file, linea))
	{
		stringstream stream(linea); // Convert the string to a stream

		// Extract all values from that row (x,y)
		getline(stream, x, delimiter);
		x_value = stof(x);
		getline(stream, y, delimiter);
		y_value = stof(y);

		//Adding this coordinates   
		values.push_back(x_value);
		values.push_back(y_value);
		cont += 2;

		//Adding this point and its index (i, x_value, y_value)
		Point p(i, values);
		points.push_back(p);
		i++;

		//Remove so it won't reinsert the same value twice
		pop_front(values);
		pop_front(values);
	}
	
	//Calling the function and praying for it to work (crossing my fingers)
	KMeans kmeans(5, cont/2, 100);
	kmeans.go(points);

	return 0;
}

