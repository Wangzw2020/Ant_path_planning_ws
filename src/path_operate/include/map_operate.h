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
	double dis_to_start_;
	double dis_to_end_;   
	string parent_node_;
	
public:
	Node();
	
	void setNodeName(string node_name);
	void setDistance(string node_name, int id, double dis);
	void setDistoStart(double dis_to_start);
	void setDistoEnd(double dis_to_end);
	void setParentNode(string name);
	
	string getNodeName() { return node_name_; }
	double getDistoStart() { return dis_to_start_; }
	double getDistoEnd() { return dis_to_end_; }
	string getParentName() { return parent_node_; }
	int getId(string node);			//得到通向节点node的路径id
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
	//路径信息处理
	void load_all_path(string& all_path_position);
	std::vector<Path> getAllPath() { return all_path_; }
	std::vector<int> getFinalPathId() { return final_path_id_; }
	void setPassable(int id, bool pass);
	
	//节点信息处理
	void load_graph(string& graph_position);
	void addPath(string node_name1, string node_name2, int id);
	void printAllNodes();
	void printAllPath();
	
	//路径规划
	void setStartPoint(int id);
	void setEndPoint(int id);
	void setB_num(int b);
	bool compute_path();
	
	//路径拼接保存
	void load_final_path(string& final_path_position);	//读取最终存储位置
	void splice_final_path();
	void recordPath();

	
};

Node::Node()
{
	dis_to_start_ = DBL_MAX;
	dis_to_end_ = DBL_MAX;
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

void Node::setDistoStart(double dis_to_start)
{
	dis_to_start_ = dis_to_start;
}
void Node::setDistoEnd(double dis_to_end)
{
	dis_to_end_ = dis_to_end;
}

void Node::setParentNode(string name)
{
	parent_node_ = name;
	//cout<<"parent_node setted!"<< node->getNodeName()<<endl;
}

int Node::getId(string node)
{
	for(int i=0; i<distances_.size(); i++)
		if (distances_[i].node_go == node)
			return distances_[i].path_id;
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

void Map::setPassable(int id, bool pass)
{
	cout<<"set path "<< id <<" = "<< pass<<endl;
	if (pass == 1)
		all_path_[id].openPath();
	
	if (pass == 0)
		all_path_[id].closePath();
		
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
	//读取前一条路的最后两个点以及后一条路的前两个点 并计算两直线交点 现在用的是最后一个点及其航向角
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
	if (yaw1 - yaw3 <= 0.1 && yaw1 - yaw3 >=-0.1)
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

void Map::setStartPoint(int id)
{
	start_point_ = id;
	std::vector<Edge> edges;
	for(int i=0; i<all_nodes_.size(); ++i)
	{
		edges = all_nodes_[i].getEdge();
		for(int j=0; j<edges.size(); ++j)
			if (edges[j].path_id == start_point_)
				for(int m=0; m<all_nodes_.size(); ++m)
					if (edges[j].node_go == all_nodes_[m].getNodeName())
						start_node_ = all_nodes_[m];
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
				end_node_ =  all_nodes_[i];
		}
	}
	cout<< "end node: " << end_node_.getNodeName() <<endl;
}

bool Map::compute_path()
{
	cout<<"start path planning!"<<endl;
	std::vector<Node> nodes, final_nodes;
	start_node_.setDistoStart(0.0);
	nodes.push_back(start_node_);
	Node current_node, node, final_node;
	std::vector<Edge> current_edges;
	double min_dis;
	
	while(nodes.size()!=0)
	{
		min_dis = nodes[0].getDistoEnd();
		current_node = nodes[0];
		int index=0;
		for(int k=0; k<nodes.size(); ++k)
		{
			if (nodes[k].getDistoEnd() <= min_dis)
			{
				min_dis = nodes[k].getDistoEnd();
				current_node = nodes[k];
				index = k;
				//cout<<"k="<<k<<endl;
			}
		}
		std::vector<Node>::iterator it = nodes.begin()+index;
		nodes.erase(it);
		if (current_node.getNodeName() == end_node_.getNodeName())
		{	
			final_node = current_node;
			while(final_node.getNodeName() != start_node_.getNodeName())
			{
				final_nodes.push_back(final_node);
				for(int i=0; i<all_nodes_.size(); ++i)
				{
					if (final_node.getParentName() == all_nodes_[i].getNodeName())
						final_node = all_nodes_[i];
				}
			}
			final_nodes.push_back(final_node);
			reverse(final_nodes.begin(),final_nodes.end());
			break;
		}
		current_edges = current_node.getEdge();
		for(int i=0; i<current_edges.size(); ++i)
		{
			for(int j=0; j<all_nodes_.size(); ++j)
				if (all_nodes_[j].getNodeName() == current_edges[i].node_go && all_path_[current_edges[i].path_id].isPassable() == 1 )
					node = all_nodes_[j];
			if (node.getDistoStart() > current_node.getDistoStart() + current_edges[i].distance)
			{
				node.setDistoStart(current_node.getDistoStart() + current_edges[i].distance);
				node.setDistoEnd(node.getDistoStart()-5);
				node.setParentNode(current_node.getNodeName());
				for(int i=0; i<all_nodes_.size(); ++i)
					if (node.getNodeName() == all_nodes_[i].getNodeName())
					{
						all_nodes_[i].setParentNode(current_node.getNodeName());
						all_nodes_[i].setDistoStart(current_node.getDistoStart() + current_edges[i].distance);
						all_nodes_[i].setDistoEnd(node.getDistoStart()-5);
					}
//				cout<<"Node: "<<node.getNodeName()<<"\t parent:"<<current_node.getNodeName()<<endl;
				if(hasNode(node,nodes) == false)
					nodes.push_back(node);
			}
		}
	}
	if (final_nodes.size()==0)
	{
		cout<<"no path!"<<endl;
		return false;
	}
	final_path_id_.push_back(start_point_);
	for(int i=0; i<final_nodes.size()-1 ; ++i)
	{
		final_path_id_.push_back(final_nodes[i].getId(final_nodes[i+1].getNodeName()));
	}
	final_path_id_.push_back(end_point_);
	
	cout<<"path planned:   ";
	for (int i=0; i<final_path_id_.size() ; ++i)
	{
		if (i == final_path_id_.size()-1)
		{
			cout<<final_path_id_[i]<<endl;
		}
		else
			cout<<final_path_id_[i]<<"->";
	}
	return true;
}

void Map::setB_num(int b)
{
	b_num_ = b;
}

void Map::addPath(string node_name1, string node_name2, int id)
{
	for(int i=0; i<all_nodes_.size(); ++i)
		if (all_nodes_[i].getNodeName() == node_name1)
		{
			all_nodes_[i].setDistance(node_name2, id, all_path_[id-1].getLength());
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
				<< edges[j].distance << ", path: " << edges[j].path_id <<endl;
	}
}

bool hasNode(Node node, std::vector<Node> nodes)
{
	for (int i=0; i<nodes.size(); ++i)
		if (node.getNodeName() == nodes[i].getNodeName())
			return true;
			
	return false;
}
