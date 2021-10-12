#include<iostream>
#include<fstream>
#include<vector>
#include"ros/ros.h"
using namespace std;

#define start 0					//which line to start changing
#define end 4					//which line to end changing
#define row 3					//which parameter in path_data is changed.  1:x 2:y 3:yaw 4:curvature 5:left_width 6:right_width
#define target_value 213.222	//the value after change the data

char *txt_id = "/home/wzw/path/test1.txt";			//which file you want to change?
char *new_txt_id = "/home/wzw/path/test1_new.txt";	//which file you want to output to?

struct path_data{
	double x;
	double y;
	double yaw;
	double curvature;
	double left_width;
	double right_width;
};

void changeData(std::vector<path_data>& path,int startline,int endline,int list,double value);

int main()
{
	fstream txt;
	txt.open(txt_id);
	if(!txt)
		cout<<"open failed!"<<endl;
		
	std::vector<path_data> path;
	path_data p;
	string line;
	
	while(txt.good())
	{
		getline(txt,line);
		if (line.length()==0)
			break;
		
		std::stringstream ss(line);
		ss >> p.x >> p.y >> p.yaw >> p.curvature >> p.left_width >> p.right_width;
		path.push_back(p);
	}
	txt.close();
	
	ofstream txt_new(new_txt_id);
	if(!txt_new)
		cout<<"open failed!"<<endl;
		
	changeData(path,start,end,row,target_value);
	for (int i = 0;i<path.size();i++)
		txt_new << path[i].x << '\t' << path[i].y << '\t' << path[i].yaw << '\t' << path[i].curvature << '\t' << path[i].left_width << '\t' << path[i].right_width << '\n';
	
	txt_new.close();
	return 0;
}

void changeData(std::vector<path_data>& path,int startline,int endline,int list,double value)
{
	int i = startline;
	if (list == 1)
		for (;i<=endline;i++)
			path[i].x = value;
	else if (list == 2)
		for (;i<=endline;i++)
			path[i].y = value;
	else if (list == 3)
		for (;i<=endline;i++)
			path[i].yaw = value;
	else if (list == 4)
		for (;i<=endline;i++)
			path[i].curvature = value;
	else if (list == 5)
		for (;i<=endline;i++)
			path[i].left_width = value;
	else if (list == 6)
		for (;i<=endline;i++)
			path[i].right_width = value;
}
