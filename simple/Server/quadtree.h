/***********************************************************
 *
 *
 *						QUADTREE HEADER
 *					 =====================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Declares the quadtree data structure.
 *		uses opencv image container.
 *
 ***********************************************************/

#ifndef QUADTREE_H
#define QUADTREE_H

#pragma once

// defines a rect by min and max points in image coordinats
struct QtreeRect {
	int min_x, min_y;
	int max_x, max_y;
};

// defines quadtree node
struct QtreeNode {
	// indices of child nodes
	int nw;	// north west (-1 means node is a leaf)
	int ne;	// north east
	int sw;	// south west
	int se;	// south east
	
	ushort	H_x, H_y; // stores the highest value of the node region
	ushort	L_x, L_y; // stores the lowest value of the node region
	ushort	c_x, c_y; // difference of highest and lowest values

	QtreeRect rect;

	// sets the indices for the child nodes to -1
	void createLeaf();
};

class QuadTree {
public:
	QuadTree(uint w, uint h, uint max_depth);

	// the following methods generate the seed points at the same way
	std::list<cv::Point2f> generateSeeds(cv::Mat& depth, cv::Mat& gx, cv::Mat& gy, bool background); // used to actuall generate seeds
	void	 			   drawGenerateSeeds(cv::Mat& seedimg, cv::Mat& depth, cv::Mat& gx, cv::Mat& gy, bool background); // used to draw the seeds

	void setTleaf( ushort T_leaf );
	void setTinternal( ushort T_internal );

private:
	uint m_w;	// width of the image
	uint m_h;	// height of the image
	uint m_ws;	// width - 1
	uint m_hs;	// height -1 

	// max depth of the quadtree
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

	// get nodes by reference
	std::vector<QtreeNode>*	 getNodes() {return &nodes;}
};

#endif
