#include "ros/ros.h"
#include "path_operate.h"
#define __NAME__ "path_operate"

char *txt_id1 = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/txt_test/path1.txt";
char *txt_id2 = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/txt_test/path2.txt";
char *txt_id3 = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/txt_test/path3.txt";
char *txt_id4 = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/txt_test/path4.txt";

char *txt_new_id = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/txt_test/path_final.txt";

Path compute_points(Path a,Path b);
Path compute_points(Path a,Path b)
{
	//读取前一条路的最后两个点以及后一条路的前两个点 并计算两直线交点
	path_data p = a.getPath_data(a.getNum()-1);
	double x1 = p.x;
	double y1 = p.y;
	cout<< x1 <<'\t'<< y1 <<endl;
	p = a.getPath_data(a.getNum()-2);
	double x2 = p.x;
	double y2 = p.y;
	cout<< x2 <<'\t'<< y2 <<endl;
	p = b.getPath_data(0);
	double x3 = p.x;
	double y3 = p.y;
	cout<< x3 <<'\t'<< y3 <<endl;
	p = b.getPath_data(1);
	double x4 = p.x;
	double y4 = p.y;
	cout<< x4 <<'\t'<< y4 <<endl;
	double x = (y3*x4*x2-y4*x3*x2-y3*x4*x1+y4*x3*x1-y1*x2*x4+y2*x1*x4+y1*x2*x3-y2*x1*x3)/(x4*y2-x4*y1-x3*y2+x3*y1-x2*y4+x2*y3+x1*y4-x1*y3);
	double y = (-y3*x4*y2+y4*x3*y2+y3*x4*y1-y4*x3*y1+y1*x2*y4-y1*x2*y3-y2*x1*y4+y2*x1*y3)/(y4*x2-y4*x1-y3*x2+x1*y3-y2*x4+y2*x3+y1*x4-y1*x3);
	cout<< x <<'\t'<< y <<endl;
	
	Path curve;
	
	return curve;
}

int main()
{
	//将路径文件读入
	std::vector<Path> all_path;
	Path path1(1,txt_id1);
	all_path.push_back(path1);
	Path path2(2,txt_id2);
	all_path.push_back(path2);
	Path path3(3,txt_id3);
	all_path.push_back(path3);
	Path path4(4,txt_id4);
	all_path.push_back(path4);
	cout<<endl;
	//记录规划后的路径id
	int id[2];
	id[0]=1;
	id[1]=2;
	
	Path curve = compute_points(all_path[id[0]-1],all_path[id[1]-1]);
	//最终存储位置
	Path path_final(0,txt_new_id);
	path_final.clearPath();
	
	for (int j = 0; j<sizeof(id); j++)
		for (int i = 0; i<all_path.size(); i++)
			if (all_path[i].getId() == id[j])
				path_final.addPath(all_path[i].getPath());
	

	path_final.writePath();

	return 0;
}

