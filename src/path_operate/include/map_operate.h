#include "path_operate.h"

struct path_data;
class Path;
class Node;
Path compute_points(Path a,Path b,int b_num); // 计算路径a->b中间的路径点
bool hasNode(Node node, std::vector<Node> nodes);

struct Edge{
	string node_go;
	int path_id;
	double distance;
};

struct way_point{
	int id;
	
};

class Node{
private:
	string node_name_;
	std::vector<Edge> distances_;
	
public:
	Node();
	string getNodeName() { return node_name_; }
	void setNodeName(string node_name);
	void setDistance(string node_name, int id, double dis);
	//void setParentNode(string name);
	std::vector<Edge> getEdge() { return distances_;}
};

class Map{
private:
	std::vector<Path> all_path_;		//所有路的信息
	std::vector<int> final_path_id_;	//最终得到路的id
	std::vector<Path> final_path_;		//最终路径
	std::vector<Node> all_nodes_;		//所有节点信息
	
	int b_num_;
	int start_point_, end_point_;
	Node start_node_, end_node_;
	
public:
	Map();
	void load_all_path(string& all_path_position);	//读txt,xml
	void load_final_path(string& final_path_position);
	std::vector<Path> getAllPath() { return all_path_; }
	void setFinalPathId(int *id);
	std::vector<int> getFinalPathId() { return final_path_id_; }
	
	//节点信息处理
	void load_graph(string& graph_position);
	void addPath(string node_name1, string node_name2, int id);
	void printAllNodes();
	void printAllPath();
	
	//路径计算
	void setStartPoint(int id);
	void setEndPoint(int id);
	
	void compute_path();
	void setB_num(int b);
	void splice_final_path();
	
	void recordPath();

	
};

Node::Node()
{
	
}

void Node::setNodeName(string node_name)
{
	node_name_ = node_name;
}

void Node::setDistance(string node_name, int id, double dis)
{
	Edge edge;
	edge.node_go = node_name;
	edge.path_id = id;
	edge.distance = dis;
	distances_.push_back(edge);
}

Map::Map()
{
	cout<<"New Map Created!"<<endl;
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
	cout<<"all path files loaded!"<<endl;	
}

void Map::load_graph(string& graph_position)
{
	Node node;
	ifstream graph_txt(graph_position.c_str());
	if (!graph_txt)
		cout<<"graph file open failed"<<endl;
	string line, node_name1, node_name2;
	std::vector<string> node_name;
	int id;
	int n=0;
	
	while (graph_txt.good())
	{
		getline(graph_txt,line);
		if (line.length()==0)
			break;
		if (n==0)
		{	
			std::stringstream ss(line);
			string tmp;
			while (ss>>tmp)
			{
				node.setNodeName(tmp);
				all_nodes_.push_back(node);
			}
			n++;
			continue;
		}
		std::stringstream ss(line);
		ss>>node_name1>>node_name2>>id;
		addPath(node_name1,node_name2,id);
		//cout<<"txt_id="<<txt_id_k<<endl;
		//cout<<"xml_id="<<xml_id_k<<endl;
	}
	
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
					bezier = compute_points(final_path_[0],all_path_[all_path_[i].getId()-1],b_num_);
					final_path_[0].addPath(bezier.getPath());		
				}				
				final_path_[0].addPath(all_path_[i].getPath());
				final_path_[0].addPathInfo(all_path_[i]);
			}
		}
	}
}

Path compute_points(Path a,Path b,int b_num)
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
		for (double i=0; i<=(b_num-1); i++)
		{
			double t = i/b_num;
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
		for (double i=0; i<=(b_num-1); i++)
		{
			double t = i/b_num;
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
		if(final_path_[i].getNum()==0)
			continue;
		final_path_[i].writePath();
		final_path_[i].writeXml();
	}
		
}

void Map::setFinalPathId(int *id)
{
	for (int i=0; i<sizeof(id); i++)
		final_path_id_.push_back(id[i]);
}

void Map::setStartPoint(int id)
{
	start_point_ = id;
	std::vector<Edge> edges;
	for(int i=0; i<all_nodes_.size(); ++i)
	{
		edges = all_nodes_[i].getEdge();
		for(int j=0; j<edges.size(); ++j)
		{
			if (edges[j].path_id == start_point_)
				start_node_.setNodeName(edges[j].node_go);
		}
	}
	cout<< "start node: " << start_node_.getNodeName() <<endl;
}

void Map::setEndPoint(int id)
{
	end_point_ = id;
	std::vector<Edge> edges;
	for(int i=0; i<all_nodes_.size(); ++i)
	{
		edges = all_nodes_[i].getEdge();
		for(int j=0; j<edges.size(); ++j)
		{
			if (edges[j].path_id == end_point_)
				end_node_.setNodeName(all_nodes_[i].getNodeName());
		}
	}
	cout<< "end node: " << end_node_.getNodeName() <<endl;
}
/*
void Map::compute_path()
{
	std::vector<Node> nodes;
	nodes.push_back(start_node_);
	Node current_node, node;
	std::vector<Edge> current_edges;
	int flag = 0;
	while(nodes.size()!=0)
	{
		
		double min_dis = DBL_MAX;
		
		current_node = nodes[flag];
		for(int i=0; i<nodes.size(); ++i)
		{
			if ()
			{
				current_node = nodes[i];
			}
		}
		
		
		current_edges = current_node.getEdge();
		
		if (current_node.getNodeName() == end_node_.getNodeName())
		{
			cout<<"path planned!"<<endl;
			break;
		}
		
		for(int i=0; i<current_edges.size(); ++i)
		{
			if (current_edges[i].distance < min_dis)
			{
				node = current_edges[i];
				min_dis = current_edges[i].distance;
				if(hasNode(node,nodes) == false)
					nodes.push_back(node);
					flag++;
			}
		}
	}	
}
*/
void Map::setB_num(int b)
{
	b_num_ = b;
}

void Map::addPath(string node_name1, string node_name2, int id)
{
	for(int i=0; i<all_nodes_.size(); ++i)
		if (all_nodes_[i].getNodeName() == node_name1)
		{
			all_nodes_[i].setDistance(node_name2, all_path_[i].getId(), all_path_[i].getLength());
		}
}

void Map::printAllNodes()
{
	for(int i=0; i<all_nodes_.size(); i++)
		cout<<"node name=" << all_nodes_[i].getNodeName() <<'\t';
	cout<<endl;
}

void Map::printAllPath()
{	
	std::vector<Edge> edges;
	for(int i=0; i<all_nodes_.size(); i++)
	{
		edges = all_nodes_[i].getEdge();
		for(int j=0; j<edges.size(); j++)
			cout<< "from "<< all_nodes_[i].getNodeName() <<" to "
				<< edges[j].node_go << " is: " 
				<< edges[j].distance <<endl;
	}
}

bool hasNode(Node node, std::vector<Node> nodes)
{
	for (int i=0; i<nodes.size(); ++i)
		if (node.getNodeName() == nodes[i].getNodeName())
			return true;
			
	return false;
}
