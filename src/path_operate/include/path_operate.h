#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include <tinyxml2.h>
#include "ros/ros.h"
using namespace std;

struct path_data{
	double x;
	double y;
	double yaw;
	double curvature;
	double left_width;
	double right_width;
};

class Path{
private:
	//txt数据
	int path_id_;
	int num_;
	std::vector<path_data> path_;
	string txt_id_;
	bool passable_;
	
	//xml数据
	
	
	
public:
	Path();
	Path(int id, string& txt_id);
	~Path();
	//txt数据
	std::vector<path_data> getPath();
	int getId();
	path_data getPath_data(int a);
	int getNum();
	void addPath(std::vector<path_data> path_new);
	void closePath();
	void openPath();
	void clearPath();
	void writePath();
	
	//xml数据
	
};

Path::Path()
{
	cout<<"new path created!"<<endl;
	num_ = 0;
}

Path::Path(int id, string& txt_id)
{
	path_id_ = id;
	txt_id_ = txt_id;
	ifstream txt;
	passable_ = true;
	string line;
	path_data p;
	num_ = 0;
	txt.open(txt_id.c_str());
	if(!txt)
		cout<<"open"<<id<<" failed!"<<endl;
	while(txt.good())
	{
		getline(txt,line);
		if (line.length()==0)
			break;
		
		std::stringstream ss(line);
		ss >> p.x >> p.y >> p.yaw >> p.curvature >> p.left_width >> p.right_width;
		path_.push_back(p);
		num_++;
	}
	txt.close();
}

std::vector<path_data> Path::getPath()
{
	return path_;
}

int Path::getId()
{
	return path_id_;
}

path_data Path::getPath_data(int a)
{
	return path_[a];
}

int Path::getNum()
{
	return num_;
}

void Path::addPath(std::vector<path_data> path_new)
{
	num_ += path_new.size();
	path_.insert(path_.end(),path_new.begin(),path_new.end());
}

void Path::openPath()
{
	passable_=true;
}

void Path::closePath()
{
	passable_=false;
}

void Path::clearPath()
{
	cout<<"path"<<path_id_<<" cleared!"<<endl;	
	path_.clear();
	num_ = 0;
}

void Path::writePath()
{	
	ofstream txt(txt_id_.c_str());
	if(!txt)
		cout<<"write open failed!"<<endl;
	for (int i = 0;i<path_.size();i++)
		txt << path_[i].x << '\t' << path_[i].y << '\t' << path_[i].yaw << '\t' << path_[i].curvature << '\t' << path_[i].left_width << '\t' << path_[i].right_width << '\n';
	txt.close();
}

Path::~Path()
{

}
