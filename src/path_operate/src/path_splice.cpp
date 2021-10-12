#include<iostream>
#include<fstream>
#include<vector>
#include"ros/ros.h"
using namespace std;

char *txt_id1 = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/txt_test/path1.txt";
char *txt_id2 = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/txt_test/path3.txt";

char *txt_new_id = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/txt_test/path_final.txt";

struct path_data{
	double x;
	double y;
	double yaw;
	double curvature;
	double left_width;
	double right_width;
};

void compute_points();

void load_path(char *txt_id,std::vector<path_data> &path);	//给出txt位置后存入path中
void write_path(char *txt_id,std::vector<path_data> path);	//给出最终路径的存储位置

void path_planning();

int main()
{
	//将所有路径文件读入
	
	std::vector<path_data> path1;
	load_path(txt_id1,path1);
	
	std::vector<path_data> path2;
	load_path(txt_id2,path2);
	
	
		
	std::vector<path_data> path_final;	
	path_final.insert(path_final.end(),path1.begin(),path1.end());
	path_final.insert(path_final.end(),path2.begin(),path2.end());
	
	write_path(txt_new_id,path_final);
	
	return 0;	
}

void load_path(char *txt_id,std::vector<path_data> &path)
{
	fstream txt;
	string line;
	path_data p;
	
	txt.open(txt_id);
	if(!txt)
		cout<<"open"<<txt_id<<"failed!"<<endl;
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
}

void write_path(char *txt_id,std::vector<path_data> path)
{
	ofstream txt(txt_new_id);
	if(!txt)
		cout<<"new open failed!"<<endl;
	for (int i = 0;i<path.size();i++)
		txt << path[i].x << '\t' << path[i].y << '\t' << path[i].yaw << '\t' << path[i].curvature << '\t' << path[i].left_width << '\t' << path[i].right_width << '\n';
	txt.close();
}
