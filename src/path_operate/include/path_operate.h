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
	double path_length_;
	
	//xml数据
	string xml_id_;
	std::vector<ParkingPoint> parking_points_;
	std::vector<TrafficLightPoint> traffic_light_points_;
	std::vector<TurnLightRange> turn_light_ranges_;
	std::vector<SpeedRange> speed_ranges_;
	
	
public:
	Path();
	Path(int id, string& txt_id, string& xml_id);
	~Path();
	
	//txt数据
	std::vector<Path_data> getPath() { return path_; }
	int getId() { return path_id_; }
	Path_data getPath_data(int a) { return path_[a]; }
	int getNum() { return num_; }
	double getLength() { return path_length_; }
	void setTxtId(string& txt_id);
	void setXmlId(string& xml_id);
	void addPath(std::vector<Path_data> path_new);
	void addPathInfo(Path path_new);
	void closePath();
	void openPath();
	void clearPath();
	void writePath();
	
	//xml数据
	std::vector<ParkingPoint> getParkingPoints() { return parking_points_; }
	std::vector<TrafficLightPoint> getTraffidLightPoints() { return traffic_light_points_; }
	std::vector<TurnLightRange> getTurnLightRanges() { return turn_light_ranges_; }
	std::vector<SpeedRange> getSpeedRanges() { return speed_ranges_; }
	void writeXml();
};

Path::Path()
{
	//cout<<"New Path Created!"<<endl;
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
	for (int i=1; i<num_/100; ++i)
	{
		path_length_ += sqrt((path_[100*i].x - path_[100*i-100].x) 
						* (path_[100*i].x - path_[100*i-100].x)
						+ (path_[100*i].y - path_[100*i-100].y) 
						* (path_[100*i].y - path_[100*i-100].y));
	}
	path_length_ = sqrt((path_[0].x - path_[num_-1].x) * (path_[0].x - path_[num_-1].x)
					+ (path_[0].y - path_[num_-1].y) * (path_[0].y - path_[num_-1].y));
	//cout<<"path_length_=" << path_length_<<endl;
	txt.close();
	
	//读取xml路径数据
	using namespace tinyxml2;
	xml_id_ = xml_id;
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
			if (duration == -1)
			{
				pParkingPoint = pParkingPoint->NextSiblingElement("ParkingPoint");
				continue;
			}
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

void Path::setTxtId(string& txt_id)
{
	txt_id_ = txt_id;
}

void Path::setXmlId(string& xml_id)
{
	xml_id_ = xml_id;
}

void Path::addPath(std::vector<Path_data> path_new)
{
	num_ += path_new.size();
	path_.insert(path_.end(),path_new.begin(),path_new.end());		
}

void Path::addPathInfo(Path path_new)
{
	int index, start, end;
	//cout<<"path "<< path_new.getId() << " Info adding"<<endl;
	std::vector<ParkingPoint> parking_points = path_new.getParkingPoints();
	std::vector<TrafficLightPoint> traffic_light_points = path_new.getTraffidLightPoints();
	std::vector<TurnLightRange> turn_light_ranges = path_new.getTurnLightRanges();
	std::vector<SpeedRange> speed_ranges = path_new.getSpeedRanges();
	for(size_t i=0; i<parking_points.size(); i++)
	{
		if (parking_points[i].parkingDuration == -1)
			continue;
		index = path_new.getNum() - parking_points[i].index;
		parking_points[i].index = num_ - index;
		parking_points_.push_back(parking_points[i]);
	}
	for(size_t i=0; i<turn_light_ranges.size(); i++)
	{
		if (turn_light_ranges[i].type == 0)
			continue;
		start = path_new.getNum() - turn_light_ranges[i].start_index;
		turn_light_ranges[i].start_index = num_ - start;
		end = path_new.getNum() - turn_light_ranges[i].end_index;
		turn_light_ranges[i].end_index = num_ - end;
		turn_light_ranges_.push_back(turn_light_ranges[i]);
	}
	for(size_t i=0; i<speed_ranges.size(); i++)
	{
		start = path_new.getNum() - speed_ranges[i].start_index;
		speed_ranges[i].start_index = num_ - start;
		end = path_new.getNum() - speed_ranges[i].end_index;
		speed_ranges[i].end_index = num_ - end;
		speed_ranges_.push_back(speed_ranges[i]);
	}
	for(size_t i=0; i<traffic_light_points.size(); i++)
	{
		index = path_new.getNum() - traffic_light_points[i].index;
		traffic_light_points[i].index = num_ - index;
		traffic_light_points_.push_back(traffic_light_points[i]);
	}
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
		txt << std::fixed << std::setprecision(3) << path_[i].x << '\t' 
			<< path_[i].y << '\t' << path_[i].yaw << '\t' << path_[i].curvature 
			<< '\t' << path_[i].left_width << '\t' << path_[i].right_width
			<< "\r\n";
	txt.close();
	cout<<"txt recorded!"<<endl;
}

void Path::writeXml()
{
	
	tinyxml2::XMLDocument doc;
	//1.添加声明
    tinyxml2::XMLDeclaration* declaration = doc.NewDeclaration();
    doc.InsertFirstChild(declaration); //在最前插入声明

    //2.创建根节点
    tinyxml2::XMLElement* pathInfoNode = doc.NewElement("PathInfo");
    doc.InsertEndChild(pathInfoNode);  //在最后插入根节点
	{ //ParkingPoints
    tinyxml2::XMLElement* parkingPointsNode = doc.NewElement("ParkingPoints");
    pathInfoNode->InsertEndChild(parkingPointsNode);

    // 创建Description子节点,并插入父节点
    tinyxml2::XMLElement* discriptionNode = doc.NewElement("Description");
    parkingPointsNode->InsertEndChild(discriptionNode);

    tinyxml2::XMLElement* idElement = doc.NewElement("id");
    discriptionNode->InsertEndChild(idElement);
    idElement->InsertEndChild(doc.NewText("the sequence of the parking point"));

    tinyxml2::XMLElement* indexElement = doc.NewElement("index");
    discriptionNode->InsertEndChild(indexElement);
    indexElement->InsertEndChild(doc.NewText("the parking point position in global path"));

    tinyxml2::XMLElement* durationElement = doc.NewElement("duration");
    discriptionNode->InsertEndChild(durationElement);
    durationElement->InsertEndChild(doc.NewText("parking time(s), -1 for destination"));

    tinyxml2::XMLElement* addEle = doc.NewElement("add");
    discriptionNode->InsertEndChild(addEle);
    addEle->InsertEndChild(doc.NewText("To add a parking point manually, please follow the format below"));

    size_t park_point_id = 0;
    for(size_t i=0; i<parking_points_.size(); ++i)
    {
        const ParkingPoint park_point = parking_points_[i];
        tinyxml2::XMLElement* pointElement = doc.NewElement("ParkingPoint");
        parkingPointsNode->InsertEndChild(pointElement); //在最后插入节点
        pointElement->SetAttribute("id", park_point_id++);
        pointElement->SetAttribute("index", park_point.index);
        pointElement->SetAttribute("duration", park_point.parkingDuration);
    }
	
    //创建ParkingPoint节点
    tinyxml2::XMLElement* pointElement = doc.NewElement("ParkingPoint");
    parkingPointsNode->InsertEndChild(pointElement); //在最后插入节点

    //为节点增加属性,终点停车
    pointElement->SetAttribute("id", park_point_id);
    pointElement->SetAttribute("index", path_.size()-1);
    pointElement->SetAttribute("duration", -1);

    }
    //cout<<"ParkingPoint recorded"<<endl;
	{//TurnRanges
    tinyxml2::XMLElement* turnRangesNode = doc.NewElement("TurnRanges");
    pathInfoNode->InsertEndChild(turnRangesNode);

    //创建Description子节点,并插入父节点
    tinyxml2::XMLElement* discriptionNode = doc.NewElement("Description");
    turnRangesNode->InsertEndChild(discriptionNode);

    tinyxml2::XMLElement* typeElement = doc.NewElement("type");
    discriptionNode->InsertEndChild(typeElement);
    typeElement->InsertEndChild(doc.NewText("-1: left turn, 0: none, 1: right turn"));

    tinyxml2::XMLElement* startElement = doc.NewElement("start");
    discriptionNode->InsertEndChild(startElement);
    startElement->InsertEndChild(doc.NewText("the start index of turn"));

    tinyxml2::XMLElement* endElement = doc.NewElement("end");
    discriptionNode->InsertEndChild(endElement);
    endElement->InsertEndChild(doc.NewText("the end index of turn"));

    tinyxml2::XMLElement* addEle = doc.NewElement("add");
    discriptionNode->InsertEndChild(addEle);
    addEle->InsertEndChild(doc.NewText("To add a turn range manually, please follow the format below"));

    //创建TurnRange节点
    tinyxml2::XMLElement* turnRangeNode = doc.NewElement("TurnRange");
    turnRangesNode->InsertEndChild(turnRangeNode);

    //添加属性,示例
    turnRangeNode->SetAttribute("type", 0);
    turnRangeNode->SetAttribute("start", 0);
    turnRangeNode->SetAttribute("end", 15);
    for(size_t i=0; i<turn_light_ranges_.size(); ++i)
    {
    	const TurnLightRange turn_range = turn_light_ranges_[i];
        tinyxml2::XMLElement* turnRangeNode = doc.NewElement("TurnRange");
        turnRangesNode->InsertEndChild(turnRangeNode);

        turnRangeNode->SetAttribute("type", turn_range.type);
        turnRangeNode->SetAttribute("start", turn_range.start_index);
        turnRangeNode->SetAttribute("end", turn_range.end_index);
    }
    } //end TurnRanges
	//cout<<"TurnRanges recorded"<<endl;
	{//SpeedRanges
        tinyxml2::XMLElement* speedRangesNode = doc.NewElement("SpeedRanges");
        pathInfoNode->InsertEndChild(speedRangesNode);

        //创建Description子节点,并插入父节点
        tinyxml2::XMLElement* discriptionNode = doc.NewElement("Description");
        speedRangesNode->InsertEndChild(discriptionNode);

        tinyxml2::XMLElement* typeElement = doc.NewElement("speed");
        discriptionNode->InsertEndChild(typeElement);
        typeElement->InsertEndChild(doc.NewText("the max speed in the range of road"));

        tinyxml2::XMLElement* startElement = doc.NewElement("start");
        discriptionNode->InsertEndChild(startElement);
        startElement->InsertEndChild(doc.NewText("the start index of the given max speed"));

        tinyxml2::XMLElement* endElement = doc.NewElement("end");
        discriptionNode->InsertEndChild(endElement);
        endElement->InsertEndChild(doc.NewText("the end index of the given max speed"));

        tinyxml2::XMLElement* addEle = doc.NewElement("add");
        discriptionNode->InsertEndChild(addEle);
        addEle->InsertEndChild(doc.NewText("To add a max speed range manually, please follow the format below"));
        //添加属性,示例
        for(size_t i=0; i<speed_ranges_.size(); ++i)
        {
        	const SpeedRange speed_range = speed_ranges_[i];
            //创建speedRange节点
            tinyxml2::XMLElement* speedRangeNode = doc.NewElement("SpeedRange");
            speedRangesNode->InsertEndChild(speedRangeNode);

            speedRangeNode->SetAttribute("speed", speed_range.speed);
            speedRangeNode->SetAttribute("start", speed_range.start_index);
            speedRangeNode->SetAttribute("end", speed_range.end_index);
        }
    }//end SpeedRanges
	//cout<<"SpeedRanges recorded"<<endl;
	{//TrafficLightPoints
        tinyxml2::XMLElement* pointsNode = doc.NewElement("TrafficLightPoints");
        pathInfoNode->InsertEndChild(pointsNode);

        //创建Description子节点,并插入父节点
        tinyxml2::XMLElement* discriptionNode = doc.NewElement("Description");
        pointsNode->InsertEndChild(discriptionNode);

        tinyxml2::XMLElement* typeElement = doc.NewElement("index");
        discriptionNode->InsertEndChild(typeElement);
        typeElement->InsertEndChild(doc.NewText("the traffic light position in global path"));

        tinyxml2::XMLElement* startElement = doc.NewElement("id");
        discriptionNode->InsertEndChild(startElement);
        startElement->InsertEndChild(doc.NewText("the sequence of the traffic light point"));

        tinyxml2::XMLElement* addEle = doc.NewElement("add");
        discriptionNode->InsertEndChild(addEle);
        addEle->InsertEndChild(doc.NewText("To add a max speed range manually, please follow the format below"));

        //添加属性
        for(int id=0; id<traffic_light_points_.size(); ++id)
        {
            //创建point节点
            const TrafficLightPoint& point = traffic_light_points_[id];
            tinyxml2::XMLElement* pointNode = doc.NewElement("TrafficLightPoint");
            pointsNode->InsertEndChild(pointNode);

            pointNode->SetAttribute("id", id);
            pointNode->SetAttribute("index", point.index);
        }
    }//end TrafficLightPoints
	//cout<<"TrafficLightPoints recorded"<<endl;
	doc.SaveFile(xml_id_.c_str());
	cout<<"xml recorded!"<<endl;
}


Path::~Path()
{

}
