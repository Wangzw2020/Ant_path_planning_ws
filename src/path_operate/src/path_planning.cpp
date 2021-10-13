#include "map_operate.h"

string all_path_id = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/all_path.txt";

string txt_new_id = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/txt_test/path_final.txt";
string xml_new_id = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/txt_test/path_final.xml";

std::vector<Path> load_path();

Path compute_points(Path a,Path b);

int main()
{
	//将路径文件读入
	std::vector<Path> all_path;
	all_path = load_path();
	
	//记录规划后的路径id
	int id[100] = {0};
	id[0]=1;
	id[1]=5;
	id[2]=9;
	id[3]=13;
	/*
	//最终存储位置
	Path path_final(0,txt_new_id,xml_new_id);
	path_final.clearPath();
	Path bezier;
	for (int j = 0; j<sizeof(id); j++)
	{
		if (id[j]==0)
			break;
		for (int i = 0; i<all_path.size(); i++)
			if (all_path[i].getId() == id[j])
			{
				if (path_final.getNum()!=0)
				{
					bezier = compute_points(path_final,all_path[all_path[i].getId()-1]);
					path_final.addPath(bezier.getPath());		
				}
				path_final.addPath(all_path[i].getPath());
			}
	}	
					
	//cout<<path_final.getNum()<<endl;
	path_final.writePath();
	*/
	return 0;
}

std::vector<Path> load_path()
{
	std::vector<Path> all_path;
	std::vector<string> txt_id;
	std::vector<string> xml_id;
	ifstream all_path_txt(all_path_id.c_str());
	if (!all_path_txt)
		cout<<"all path file open failed"<<endl;
	string line, txt_id_k, xml_id_k;
	while (all_path_txt.good())
	{
		getline(all_path_txt,line);
		if (line.length()==0)
			break;
		std::stringstream ss(line);
		ss>>txt_id_k>>xml_id_k;
		txt_id.push_back(txt_id_k);
		xml_id.push_back(xml_id_k);
		cout<<"txt_id="<<txt_id_k<<endl;
		cout<<"xml_id="<<xml_id_k<<endl;
	}
	
	for (int i=0; i<txt_id.size(); i++)
	{
		Path path(i+1,txt_id[i],xml_id[i]);
		all_path.push_back(path);
	}
	
	cout<<"all path loaded!"<<endl;

	return all_path;
}

Path compute_points(Path a,Path b)
{
	//读取前一条路的最后两个点以及后一条路的前两个点 并计算两直线交点
	Path_data p = a.getPath_data(a.getNum()-1);
	double x1 = p.x;
	double y1 = p.y;
	//cout<< x1 <<'\t'<< y1 <<endl;
	p = a.getPath_data(a.getNum()-2);
	double x2 = p.x;
	double y2 = p.y;
	//cout<< x2 <<'\t'<< y2 <<endl;
	p = b.getPath_data(0);
	double x3 = p.x;
	double y3 = p.y;
	//cout<< x3 <<'\t'<< y3 <<endl;
	p = b.getPath_data(1);
	double x4 = p.x;
	double y4 = p.y;
	//cout<< x4 <<'\t'<< y4 <<endl;
	double x = (y3*x4*x2-y4*x3*x2-y3*x4*x1+y4*x3*x1-y1*x2*x4+y2*x1*x4+y1*x2*x3-y2*x1*x3)/(x4*y2-x4*y1-x3*y2+x3*y1-x2*y4+x2*y3+x1*y4-x1*y3);
	double y = (-y3*x4*y2+y4*x3*y2+y3*x4*y1-y4*x3*y1+y1*x2*y4-y1*x2*y3-y2*x1*y4+y2*x1*y3)/(y4*x2-y4*x1-y3*x2+x1*y3-y2*x4+y2*x3+y1*x4-y1*x3);
	//cout<< x <<'\t'<< y <<endl;
	
	Path curve;
	std::vector<Path_data> b_curve;
	Path_data b_point;
	if ((x4*y2-x4*y1-x3*y2+x3*y1-x2*y4+x2*y3+x1*y4-x1*y3)==0)
	{
		for (double i=0; i<=9; i++)
		{
			double t = i/10;
			b_point.x = (1-t)*x1 + t*x3;
			b_point.y = (1-t)*y1 + t*y3;
			b_point.yaw = 0;
			b_point.curvature = 0;
			b_point.left_width = 0;
			b_point.right_width = 0;
			//cout<<b_point.x<<'\t'<<b_point.y<<endl;
			b_curve.push_back(b_point);
		}
	}
	else
	{
		for (double i=0; i<=9; i++)
		{
			double t = i/10;
			b_point.x = (1-t)*(1-t)*x1 + 2*t*(1-t)*x + t*t*x3;
			b_point.y = (1-t)*(1-t)*y1 + 2*t*(1-t)*y + t*t*y3;
			b_point.yaw = 0;
			b_point.curvature = 0;
			b_point.left_width = 0;
			b_point.right_width = 0;
			//cout<<b_point.x<<'\t'<<b_point.y<<endl;
			b_curve.push_back(b_point);
		}
	}
	b_curve[0].yaw = atan((b_curve[0].y-b_curve[1].y)/(b_curve[0].x-b_curve[1].x));
	for (int k=1; k<=b_curve.size(); k++)
	{
		b_curve[k].yaw = atan((b_curve[k-1].y-b_curve[k].y)/(b_curve[k-1].x-b_curve[k].x));
	}
	
	curve.addPath(b_curve);
	
	return curve;
}
