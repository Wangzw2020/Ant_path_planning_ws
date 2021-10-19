#include "map_operate.h"

#define NUM_OF_POINTS 100

string files_position = "/home/wzw/workspace/Ant_path_planning_ws/src/path_operate/src/files.txt";

class GetPath;

int main()
{
	//将文件读入对应的地图文件
	ifstream map_txt;
	map_txt.open(files_position.c_str());
	if(!map_txt)
		cout<<"open map file failed!"<<endl;
	string line, all_path_position, final_path_position, graph_position;
	int n = 1;
	while(map_txt.good())
	{
		getline(map_txt,line);
		if (line.length()==0)
			break;
		std::stringstream ss(line);
		if (n==1)
			ss >> all_path_position;
		else if (n==2)
			ss >> final_path_position;
		else if (n==3)
			ss >> graph_position;
		n++;
	}
	
	Map map;
	map.load_all_path(all_path_position);	//读txt、xml
	
	map.load_graph(graph_position);			//读路网信息
	
	map.printAllNodes();
	map.printAllPath();
	
	map.setStartPoint(1);		//设定起点终点
	map.setEndPoint(3);
	
//	map.setPassable(3,0);
//	map.setPassable(4,0);
//	map.setPassable(5,0);
//	map.setPassable(6,0);
	
	//路径计算 输出
	
	if(map.compute_path())		
	{
		map.setB_num(NUM_OF_POINTS);
		map.load_final_path(final_path_position);
		map.splice_final_path();
		map.recordPath();
	}
	else
		cout<<"No such path!!!"<<endl;
	

	
	return 0;
}

class GetPath{
private:
	
public:

};











