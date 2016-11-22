#ifndef QUADTREE_H
#define QUADTREE_H

#pragma once

struct QtreeRect {
	int min_x, min_y;
	int max_x, max_y;
};

struct QtreeNode {
	int nw;	// north west (-1 means node is a leaf)
	int ne;	// north east
	int sw;	// south west
	int se;	// south east
	
	ushort	H_x, H_y;
	ushort	L_x, L_y;
	ushort	c_x, c_y;

	QtreeRect rect;

	void createLeaf();
};

class QuadTree {
public:
	QuadTree(uint w, uint h, uint max_depth);

	std::list<cv::Point2f> generateSeeds(cv::Mat& gx, cv::Mat& gy);

	void setTleaf( ushort T_leaf );
	void setTinternal( ushort T_internal );

private:
	uint m_w;	// width of the image
	uint m_h;	// height of the image
	uint m_ws;	// width - 1
	uint m_hs;	// height -1 

	uint m_max_depth;

	uint m_num_nodes;	// like nodes.size();

	// thresholds
	ushort m_T_leaf;
	ushort m_T_internal;
	
	// stores the nodes
	std::vector<QtreeNode> 	 nodes;

	// stores node ids for every level
	std::vector< std::vector<uint> >  hnodes;

	// subdivide the current node and stores the childs in nodes
	// id: id of node which will be divided
	void subdivide(int id, uint current_depth);

	//void calcCxy(std::list<cv::Point2f>& seeds);

	// get nodes by reference
	std::vector<QtreeNode>*	 getNodes() {return &nodes;}
};

#endif
