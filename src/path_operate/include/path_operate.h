#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include <tinyxml2.h>
#include "ros/ros.h"
using namespace std;

struct Path_data{
	double x;
	double y;
	double yaw;
	double curvature;
	double left_width;
	double right_width;
};

struct ParkingPoint{
	size_t index;
	float parkingDuration; // 停车时长，单位s，0表示不停车，-1永久停车，此定义不得轻易改动
};

struct TrafficLightPoint{
	size_t index;
	float parkingDuration; // 停车时长，单位s，0表示不停车，-1永久停车，此定义不得轻易改动
};

struct TurnLightRange{
	int type;
	size_t start_index;
	size_t end_index;
};

struct SpeedRange{
	float speed;
	size_t start_index;
	size_t end_index;
};

class Path{
private:
	//txt数据
	int path_id_;
	int num_;
	std::vector<Path_data> path_;
	string txt_id_;
	bool passable_;
	
	//xml数据
	std::vector<ParkingPoint> parking_points_;
	std::vector<TrafficLightPoint> traffic_light_points_;
	std::vector<TurnLightRange> turn_light_ranges_;
	std::vector<SpeedRange> speed_ranges_;
	
public:
	Path();
	Path(int id, string& txt_id, string& xml_id);
	~Path();
	//txt数据
	std::vector<Path_data> getPath();
	int getId();
	Path_data getPath_data(int a);
	int getNum();
	void addPath(std::vector<Path_data> path_new);
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

Path::Path(int id, string& txt_id, string& xml_id)
{
	// 读取txt路径数据
	path_id_ = id;
	txt_id_ = txt_id;
	ifstream txt;
	passable_ = true;
	string line;
	Path_data p;
	num_ = 0;
	txt.open(txt_id.c_str());
	if(!txt)
		cout<<"open:\n"<<txt_id<<"\nfailed!"<<endl;
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
	
	//读取xml路径数据
	using namespace tinyxml2;
	XMLDocument Doc;
	XMLError res = Doc.LoadFile(xml_id.c_str());	//
	if(XML_ERROR_FILE_NOT_FOUND == res)
		cout<<"open\n"<<xml_id<<"\nfailed!"<<endl;
	else if(XML_SUCCESS != res)
		cout<<"file\n"<<xml_id<<"\nparse error"<<endl;
	tinyxml2::XMLElement *pRoot = Doc.RootElement();
	if(pRoot == NULL)
		cout<<"file\n"<<xml_id<<"\nhas no root node"<<endl;
	//读取停车点数据
	tinyxml2::XMLElement *pParkingPoints = pRoot->FirstChildElement("ParkingPoints");
	ParkingPoint parking_point;
	if(pParkingPoints)
	{
		tinyxml2::XMLElement *pParkingPoint = pParkingPoints->FirstChildElement("ParkingPoint");
		while(pParkingPoint)
		{
			uint32_t id = pParkingPoint->Unsigned64Attribute("id");
			uint32_t index = pParkingPoint->Unsigned64Attribute("index");
			float duration = pParkingPoint->FloatAttribute("duration");
			parking_point.index = index;
			parking_point.parkingDuration = duration;
			parking_points_.push_back(parking_point);
			// 转到下一子节点
			pParkingPoint = pParkingPoint->NextSiblingElement("ParkingPoint");
		}
	}
	else
		cout<<"No Parking Points in path info file!"<<endl;
	
	//读取红绿灯停车点数据
	tinyxml2::XMLElement *pTrafficLightPoints = pRoot->FirstChildElement("TrafficLightPoints");
	TrafficLightPoint traffic_light_point;
	if(pTrafficLightPoints)
	{
		tinyxml2::XMLElement *pTrafficLightPoint = pTrafficLightPoints->FirstChildElement("TrafficLightPoint");
		while(pTrafficLightPoint)
		{
			uint32_t id = pTrafficLightPoint->Unsigned64Attribute("id");
			uint32_t index = pTrafficLightPoint->Unsigned64Attribute("index");
			traffic_light_point.index = index;
			traffic_light_point.parkingDuration = 0.0;
			traffic_light_points_.push_back(traffic_light_point);
			// 转到下一子节点
			pTrafficLightPoint = pTrafficLightPoint->NextSiblingElement("TrafficLightPoint");  
		}	
	}
	else
		cout<<"No Traffic Light Points in path info file!"<<endl;
	//读取转向灯信息
	tinyxml2::XMLElement *pTurnRanges = pRoot->FirstChildElement("TurnRanges");
	TurnLightRange turn_light_range;
	if(pTurnRanges)
	{
		tinyxml2::XMLElement *pTurnRange = pTurnRanges->FirstChildElement("TurnRange");
		while(pTurnRange)
		{
			int type = pTurnRange->IntAttribute("type");
			size_t start = pTurnRange->Unsigned64Attribute("start");
			size_t end = pTurnRange->Unsigned64Attribute("end");
			turn_light_range.type = type;
			turn_light_range.start_index = start;
			turn_light_range.end_index = end;
			turn_light_ranges_.push_back(turn_light_range);
			// 转到下一子节点
			pTurnRange = pTurnRange->NextSiblingElement("TurnRange"); 
		}
	}
	else
		cout<<"No Turn Light Points in path info file!"<<endl;
	//读取速度区间信息
	tinyxml2::XMLElement *pSpeedRanges = pRoot->FirstChildElement("SpeedRanges");
	SpeedRange speed_range;
	if(pSpeedRanges)
	{
		tinyxml2::XMLElement *pSpeedRange = pSpeedRanges->FirstChildElement("SpeedRange");
		while(pSpeedRange)
		{
			float speed = pSpeedRange->FloatAttribute("speed");
			size_t start = pSpeedRange->Unsigned64Attribute("start");
			size_t end = pSpeedRange->Unsigned64Attribute("end");
			speed_range.speed = speed;
			speed_range.start_index = start;
			speed_range.end_index = end;
			speed_ranges_.push_back(speed_range);
			// 转到下一子节点
			pSpeedRange = pSpeedRange->NextSiblingElement("SpeedRange"); 
		}
	}
	else
		cout<<"No Speed Ranges in path info file!"<<endl;
		
	//cout<<turn_light_ranges_[0].type<<'\t'<<turn_light_ranges_[0].start_index<<'\t'<<turn_light_ranges_[0].end_index<<endl;
	
}

std::vector<Path_data> Path::getPath()
{
	return path_;
}

int Path::getId()
{
	return path_id_;
}

Path_data Path::getPath_data(int a)
{
	return path_[a];
}

int Path::getNum()
{
	return num_;
}

void Path::addPath(std::vector<Path_data> path_new)
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
