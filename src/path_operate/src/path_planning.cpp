#include "map_operate.h"

#define NUM_OF_POINTS 100

string all_path_position = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/all_path.txt";
string final_path_position = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/final_path.txt";
string graph_position = "/home/wzw/workspace/txt_operate_ws/src/path_operate/src/graph_position.txt";


int main()
{
	//将路径文件读入
	
	Map map;
	map.load_all_path(all_path_position);
	
	map.load_graph(graph_position);
	map.printAllNodes();
	map.printAllPath();
	
	//
	map.setStartPoint(1);
	map.setEndPoint(3);
	//记录规划后的路径id
	int id[100] = {0};
	
	id[0]=1;
	id[1]=2;
	id[2]=3;
	
	map.setFinalPathId(id);
	
	map.setB_num(NUM_OF_POINTS);
	
	//int *a = map.getFinalPathId();

	map.load_final_path(final_path_position);
	
	map.splice_final_path();
	
	map.recordPath();
	
	return 0;
}


