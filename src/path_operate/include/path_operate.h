
#include<iostream>
#include<fstream>
#include<vector>
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
	int path_id_;
	int num_;
	std::vector<path_data> path_;
	char *txt_id_;
	bool passable_;
	
public:
	Path();
	Path(int id, char *txt_id);
	std::vector<path_data> getPath();
	int getId();
	path_data getPath_data(int a);
	int getNum();
	void addPath(std::vector<path_data> path_new);
	void closePath();
	void openPath();
	void clearPath();
	void writePath();
	~Path();
};

Path::Path()
{
	cout<<"new path created!"<<endl;
}

Path::Path(int id, char *txt_id)
{
	path_id_ = id;
	txt_id_ = txt_id;
	fstream txt;
	passable_ = true;
	string line;
	path_data p;
	num_ = 0;
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
		path_.push_back(p);
		num_++;
	}
	txt.close();
	cout<<"path"<<id<<" loaded!"<<endl;	
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
}

void Path::writePath()
{	
	ofstream txt(txt_id_);
	if(!txt)
		cout<<"write open failed!"<<endl;
	for (int i = 0;i<path_.size();i++)
		txt << path_[i].x << '\t' << path_[i].y << '\t' << path_[i].yaw << '\t' << path_[i].curvature << '\t' << path_[i].left_width << '\t' << path_[i].right_width << '\n';
	txt.close();
}

Path::~Path()
{

}
