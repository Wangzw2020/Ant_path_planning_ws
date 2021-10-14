#include "path_operate.h"

struct path_data;
class Path;

#define NUM_OF_POINTS 100
Path compute_points(Path a,Path b); // 计算路径a->b中间的路径点

struct way_point{
	int id;
	
};

class Map{
private:
	std::vector<Path> all_path_;
	int final_path_id_[100];	//记录规划后的路径id 初始值-1
	std::vector<Path> final_path_;
	
public:
	Map();
	void load_all_path(string& all_path_position);
	void load_final_path(string& final_path_position);
	std::vector<Path> getAllPath() { return all_path_; }
	void setFinalPathId(int *id);
	int* getFinalPathId() { return final_path_id_; }
	void splice_final_path();
	
	void recordPath();

	
};

Map::Map()
{
	cout<<"New Map Created!"<<endl;
	for (int i=0; i<100; i++)
		final_path_id_[i] = -1;
}

void Map::load_all_path(string& all_path_position)
{
	std::vector<string> txt_id;
	std::vector<string> xml_id;
	ifstream all_path_txt(all_path_position.c_str());
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
		//cout<<"txt_id="<<txt_id_k<<endl;
		//cout<<"xml_id="<<xml_id_k<<endl;
	}
	
	for (int i=0; i<txt_id.size(); i++)
	{
		Path path(i+1,txt_id[i],xml_id[i]);
		all_path_.push_back(path);
	}
	cout<<"all path loaded!"<<endl;	
}

void Map::load_final_path(string& final_path_position)
{
	std::vector<string> txt_id;
	std::vector<string> xml_id;
	ifstream final_path_txt(final_path_position.c_str());
	if (!final_path_txt)
		cout<<"final path file open failed"<<endl;
	string line, txt_id_k, xml_id_k;
	while (final_path_txt.good())
	{
		getline(final_path_txt,line);
		if (line.length()==0)
			break;
		std::stringstream ss(line);
		ss>>txt_id_k>>xml_id_k;
		txt_id.push_back(txt_id_k);
		xml_id.push_back(xml_id_k);
		//cout<<"txt_id="<<txt_id_k<<endl;
		//cout<<"xml_id="<<xml_id_k<<endl;
	}
	Path path;
	for (int i=0; i<txt_id.size(); i++)
	{
		path.setTxtId(txt_id[i]);
		path.setXmlId(xml_id[i]);
		final_path_.push_back(path);
	}
	cout<<"final path files loaded!"<<endl;	
}

void Map::splice_final_path()
{
	Path bezier;
	for (int j = 0; j<sizeof(final_path_id_); j++)
	{
		if (final_path_id_[j]==-1)
			break;
		for (int i = 0; i<all_path_.size(); i++)
		{
			if (all_path_[i].getId() == final_path_id_[j])
			{
				if (final_path_[0].getNum()!=0)
				{
					bezier = compute_points(final_path_[0],all_path_[all_path_[i].getId()-1]);
					final_path_[0].addPath(bezier.getPath());		
				}				
				final_path_[0].addPath(all_path_[i].getPath());
				final_path_[0].addPathInfo(all_path_[i]);
			}
		}
	}
}

Path compute_points(Path a,Path b)
{
	//读取前一条路的最后两个点以及后一条路的前两个点 并计算两直线交点
	Path_data p = a.getPath_data(a.getNum()-1);
	double x1 = p.x;
	double y1 = p.y;
	double yaw1 = p.yaw;
	//cout<< std::fixed << std::setprecision(3) << x1-670900 <<'\t'<< y1-3529400 <<endl;
	p = a.getPath_data(a.getNum()-10);
	double x2 = p.x;
	double y2 = p.y;
	double yaw2 = p.yaw;
	//cout<< std::fixed << std::setprecision(3) << x2-670900 <<'\t'<< y2-3529400 <<endl;
	p = b.getPath_data(0);
	double x3 = p.x;
	double y3 = p.y;
	double yaw3 = p.yaw;
	//cout<< std::fixed << std::setprecision(3) << x3-670900 <<'\t'<< y3-3529400 <<endl;
	p = b.getPath_data(10);
	double x4 = p.x;
	double y4 = p.y;
	double yaw4 = p.yaw;
	//cout<< std::fixed << std::setprecision(3) << x4-670900 <<'\t'<< y4-3529400 <<endl;
	//cout<<yaw1<<'\t'<<yaw3<<endl;
	double x = (y3-y1+tan(yaw1)*x1-tan(yaw3)*x3)/(tan(yaw1)-tan(yaw3));
	double y = ((tan(yaw1)*tan(yaw3))*(x3-x1)+tan(yaw3)*y1-tan(yaw1)*y3)/(tan(yaw3)-tan(yaw1));
	
	//double x = (y3*x4*x2-y4*x3*x2-y3*x4*x1+y4*x3*x1-y1*x2*x4+y2*x1*x4+y1*x2*x3-y2*x1*x3)/(x4*y2-x4*y1-x3*y2+x3*y1-x2*y4+x2*y3+x1*y4-x1*y3);
	//double y = (-y3*x4*y2+y4*x3*y2+y3*x4*y1-y4*x3*y1+y1*x2*y4-y1*x2*y3-y2*x1*y4+y2*x1*y3)/(y4*x2-y4*x1-y3*x2+x1*y3-y2*x4+y2*x3+y1*x4-y1*x3);
	
	//cout<< std::fixed << std::setprecision(3) << x-670900 <<'\t'<< y-3529400 <<endl;
	
	Path curve;
	std::vector<Path_data> b_curve;
	Path_data b_point;
	if ((x4*y2-x4*y1-x3*y2+x3*y1-x2*y4+x2*y3+x1*y4-x1*y3)==0)
	{
		for (double i=0; i<=(NUM_OF_POINTS-1); i++)
		{
			double t = i/NUM_OF_POINTS;
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
		for (double i=0; i<=(NUM_OF_POINTS-1); i++)
		{
			double t = i/NUM_OF_POINTS;
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
	//计算航向角
	
	b_curve[0].yaw = atan((b_curve[0].y-b_curve[1].y)/(b_curve[0].x-b_curve[1].x))-3.14;
	if (b_curve[0].yaw<=-3.14)
		b_curve[0].yaw += 3.14;
	for (int k=1; k<=b_curve.size(); k++)
	{
		b_curve[k].yaw = atan((b_curve[k-1].y-b_curve[k].y)/(b_curve[k-1].x-b_curve[k].x))-3.14;
		if (b_curve[k].yaw<=-3.14)
			b_curve[k].yaw += 3.14;
	}
	
	curve.addPath(b_curve);
	
	return curve;
}

void Map::recordPath()
{
	for (int i=0; i<final_path_.size(); i++)
	{
		final_path_[i].writePath();
		final_path_[i].writeXml();
	}
		
}

void Map::setFinalPathId(int *id)
{
	for (int i=0; i<sizeof(id); i++)
		final_path_id_[i] = id[i];
}

