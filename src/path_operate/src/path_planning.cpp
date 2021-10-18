#include "map_operate.h"

#define NUM_OF_POINTS 100

string all_path_position = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/all_path.txt";
string final_path_position = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/final_path.txt";
string graph_position = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/graph_position.txt";


int main()
{
	//将路径文件读入
	
	Map map;
	map.load_all_path(all_path_position);	//读txt、xml
	
	map.load_graph(graph_position);			//读路网信息
	
	map.printAllNodes();
	map.printAllPath();
	
	map.setStartPoint(9);		//设定起点终点
	map.setEndPoint(10);
	
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
