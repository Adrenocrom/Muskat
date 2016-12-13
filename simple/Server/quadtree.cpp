/***********************************************************
 *
 *
 *						QUADTREE HEADER
 *					 =====================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Defines the quadtree class. More informations
 *		are in quadtree.h
 *
 ***********************************************************/

#include "muskat.h"

// creates Quadtree
QuadTree::QuadTree(uint w, uint h, uint max_depth) {
	m_T_leaf 	 = 0;
	m_T_internal = 0;

	// stores node indices for every depth
	m_w = w;
	m_h = h;
	m_ws = w-1;
	m_hs = h-1;
	
	m_max_depth = max_depth;

	// create root node
	QtreeNode root;
	root.rect.min_x = 0;
	root.rect.min_y = 0;
	root.rect.max_x = m_ws;
	root.rect.max_y = m_hs;
	nodes.push_back(root);
	m_num_nodes = 1;

	hnodes.resize(max_depth);

	// id: id of the root node
	// next depth is 1
	subdivide(0, 1);
}

// generate seeds with the quadtree
// gx, gy are gradient images
// background is used to discard background seeds
// traverse the tree from bottom to the top
list<cv::Point2f> QuadTree::generateSeeds(cv::Mat& depth, cv::Mat& gx, cv::Mat& gy, bool background) {
	list<cv::Point2f> seeds; // stores seeds
	
	uint r_size = m_max_depth-1;
	uint size = hnodes[r_size].size();

	ushort g_x, g_y;
	// calc c_x, c_y in the leaf nodes
	
	// check if background seeds should be used
	// the code is inserted twice to fast as possible
	if(background) { 
		for(uint n = 0; n < size; ++n) {
			QtreeNode& leaf = nodes[hnodes[r_size][n]];
			QtreeRect& rect = leaf.rect;

			// begin with the leaf nodes
			// init with the first values
			leaf.H_x = gx.at<ushort>(rect.min_y, rect.min_x);
			leaf.H_y = gy.at<ushort>(rect.min_y, rect.min_x);
			leaf.L_x = leaf.H_x;
			leaf.L_y = leaf.H_y;
			for(int y = rect.min_y; y <= rect.max_y; ++y) {
				for(int x = rect.min_x; x <= rect.max_x; ++x) {
					// access pixel values (slowest part)
					g_x = gx.at<ushort>(y, x);	
					g_y = gy.at<ushort>(y, x);
				
					leaf.H_x = MU_MAX( leaf.H_x, g_x );
					leaf.H_y = MU_MAX( leaf.H_y, g_y );
					leaf.L_x = MU_MIN( leaf.L_x, g_x );
					leaf.L_y = MU_MIN( leaf.L_y, g_y );
				}
			}

			leaf.c_x = leaf.H_x - leaf.L_x;
			leaf.c_y = leaf.H_y - leaf.L_y;

			// if the differences are above the threshold the region of the node
			// is not planar and the corner points will be added as seed
			if(leaf.c_x >= m_T_leaf || leaf.c_y >= m_T_leaf) {
				// region is non planar
	
				// discard the seed if the depth is equal to the maximum
				if(depth.at<ushort>(rect.min_y, rect.min_x) != USHRT_MAX) {
					cv::Point2f p1(rect.min_x, rect.min_y);
					seeds.push_back(p1);
				}

				if(depth.at<ushort>(rect.min_y, rect.max_x) != USHRT_MAX) {
					cv::Point2f p2(rect.max_x, rect.min_y);
					seeds.push_back(p2);
				}

				if(depth.at<ushort>(rect.max_y, rect.min_x) != USHRT_MAX) {
					cv::Point2f p3(rect.min_x, rect.max_y);
					seeds.push_back(p3);
				}

				if(depth.at<ushort>(rect.max_y, rect.max_x) != USHRT_MAX) {
					cv::Point2f p4(rect.max_x, rect.max_y);
					seeds.push_back(p4);		
				}
			}
		}

		// same like the leaf nodes but uses Hxy and Lxy to compute Cxy
		// calc c_x, c_y in the inner nodes
		for(int d = (int)r_size - 1; d >= 0; --d) {
			size = hnodes[d].size();
			for(uint n = 0; n < size; ++n) {
				QtreeNode& node = nodes[hnodes[d][n]];
	
				// accessing the children for Hxy and Lxy
				QtreeNode& nw = nodes[node.nw];
				QtreeNode& ne = nodes[node.ne];
				QtreeNode& sw = nodes[node.sw];
				QtreeNode& se = nodes[node.se];
	
				// find the new highest and lowest values
				node.H_x = MU_MAX4(nw.H_x, ne.H_x, sw.H_x, se.H_x);
				node.H_y = MU_MAX4(nw.H_y, ne.H_y, sw.H_y, se.H_y);
				node.L_x = MU_MIN4(nw.L_x, ne.L_x, sw.L_x, se.L_x);
				node.L_y = MU_MIN4(nw.L_y, ne.L_y, sw.L_y, se.L_y);
				
				// calc the difference
				node.c_x = node.H_x - node.L_x;
				node.c_y = node.H_y - node.L_y;
				if(node.c_x >= m_T_internal || node.c_y >= m_T_internal) {
					QtreeRect& rect = node.rect;

					if(depth.at<ushort>(rect.min_y, rect.min_x) != USHRT_MAX) {
						cv::Point2f p1(rect.min_x, rect.min_y);
						seeds.push_back(p1);
					}

					if(depth.at<ushort>(rect.min_y, rect.max_x) != USHRT_MAX) {
						cv::Point2f p2(rect.max_x, rect.min_y);
						seeds.push_back(p2);
					}

					if(depth.at<ushort>(rect.max_y, rect.min_x) != USHRT_MAX) {
						cv::Point2f p3(rect.min_x, rect.max_y);
						seeds.push_back(p3);
					}

					if(depth.at<ushort>(rect.max_y, rect.max_x) != USHRT_MAX) {
						cv::Point2f p4(rect.max_x, rect.max_y);
						seeds.push_back(p4);		
					}
				} 
			}
		}
	} else { // same procedure like above but without checking for background seeds
		for(uint n = 0; n < size; ++n) {
			QtreeNode& leaf = nodes[hnodes[r_size][n]];
			QtreeRect& rect = leaf.rect;

			// init with the first values
			leaf.H_x = gx.at<ushort>(rect.min_y, rect.min_x);
			leaf.H_y = gy.at<ushort>(rect.min_y, rect.min_x);
			leaf.L_x = leaf.H_x;
			leaf.L_y = leaf.H_y;
			for(int y = rect.min_y; y <= rect.max_y; ++y) {
				for(int x = rect.min_x; x <= rect.max_x; ++x) {
					g_x = gx.at<ushort>(y, x);
					g_y = gy.at<ushort>(y, x);
				
					leaf.H_x = MU_MAX( leaf.H_x, g_x );
					leaf.H_y = MU_MAX( leaf.H_y, g_y );
					leaf.L_x = MU_MIN( leaf.L_x, g_x );
					leaf.L_y = MU_MIN( leaf.L_y, g_y );
				}
			}

			leaf.c_x = leaf.H_x - leaf.L_x;
			leaf.c_y = leaf.H_y - leaf.L_y;
			if(leaf.c_x >= m_T_leaf || leaf.c_y >= m_T_leaf) {
				// region is non planar
	
				cv::Point2f p1(rect.min_x, rect.min_y);
				seeds.push_back(p1);

				cv::Point2f p2(rect.max_x, rect.min_y);
				seeds.push_back(p2);

				cv::Point2f p3(rect.min_x, rect.max_y);
				seeds.push_back(p3);

				cv::Point2f p4(rect.max_x, rect.max_y);
				seeds.push_back(p4);		
			}
		}

		// calc c_x, c_y in the inner nodes
		for(int d = (int)r_size - 1; d >= 0; --d) {
			size = hnodes[d].size();
			for(uint n = 0; n < size; ++n) {
				QtreeNode& node = nodes[hnodes[d][n]];
	
				QtreeNode& nw = nodes[node.nw];
				QtreeNode& ne = nodes[node.ne];
				QtreeNode& sw = nodes[node.sw];
				QtreeNode& se = nodes[node.se];
	
				node.H_x = MU_MAX4(nw.H_x, ne.H_x, sw.H_x, se.H_x);
				node.H_y = MU_MAX4(nw.H_y, ne.H_y, sw.H_y, se.H_y);
				node.L_x = MU_MIN4(nw.L_x, ne.L_x, sw.L_x, se.L_x);
				node.L_y = MU_MIN4(nw.L_y, ne.L_y, sw.L_y, se.L_y);
				
				node.c_x = node.H_x - node.L_x;
				node.c_y = node.H_y - node.L_y;
				if(node.c_x >= m_T_internal || node.c_y >= m_T_internal) {
					QtreeRect& rect = node.rect;

					cv::Point2f p1(rect.min_x, rect.min_y);
					seeds.push_back(p1);

					cv::Point2f p2(rect.max_x, rect.min_y);
					seeds.push_back(p2);

					cv::Point2f p3(rect.min_x, rect.max_y);
					seeds.push_back(p3);

					cv::Point2f p4(rect.max_x, rect.max_y);
					seeds.push_back(p4);		
				} 
			}
		}
	}

	// all points are inserted, now sort them in O(N logN)
	seeds.sort([](cv::Point2f& a, cv::Point2f& b) {
		if(a.y < b.y) {
			return true;
		}
		else if(a.y == b.y) {
			if(a.x < b.x) {
				return true;
			}	
		}
		return false;
	});

	// and check in O(N) for duplicates
	seeds.unique();
	return seeds;
}

// set leaf threshold
void QuadTree::setTleaf( ushort T_leaf ) {
	m_T_leaf = T_leaf;
}

// set inner threshold
void QuadTree::setTinternal( ushort T_internal ) {
	m_T_internal = T_internal;
}

// subdive a node, if the currend depth is the maximum one create leaf nodes
void QuadTree::subdivide(int id, uint current_depth) {
	QtreeNode node = nodes[id];
	hnodes[current_depth-1].push_back( (uint)id );

	if(current_depth == m_max_depth) {
		node.createLeaf();
	} else {
		QtreeRect r = node.rect;
		int p_m_x = r.min_x + (int)((float)(r.max_x - r.min_x) / 2.0f);
		int p_m_y = r.min_y + (int)((float)(r.max_y - r.min_y) / 2.0f);

		QtreeNode nw;
		nw.rect.min_x = r.min_x;
		nw.rect.min_y = r.min_y;
		nw.rect.max_x = p_m_x;
		nw.rect.max_y = p_m_y;
		node.nw = m_num_nodes;
		m_num_nodes++;

		QtreeNode ne;
		ne.rect.min_x = p_m_x;
		ne.rect.min_y = r.min_y;
		ne.rect.max_x = r.max_x;
		ne.rect.max_y = p_m_y;
		node.ne = m_num_nodes;
		m_num_nodes++;

		QtreeNode sw;
		sw.rect.min_x = r.min_x;
		sw.rect.min_y = p_m_y;
		sw.rect.max_x = p_m_x;
		sw.rect.max_y = r.max_y;
		node.sw = m_num_nodes;
		m_num_nodes++;

		QtreeNode se;
		se.rect.min_x = p_m_x;
		se.rect.min_y = p_m_y;
		se.rect.max_x = r.max_x;
		se.rect.max_y = r.max_y;
		node.se = m_num_nodes;
		m_num_nodes++;

		nodes.push_back(nw);
		nodes.push_back(ne);
		nodes.push_back(sw);
		nodes.push_back(se);
		
		nodes[id] = node;

		subdivide(node.nw, current_depth+1);
		subdivide(node.ne, current_depth+1);
		subdivide(node.sw, current_depth+1);
		subdivide(node.se, current_depth+1);

	}
}

void QtreeNode::createLeaf() {
	nw = -1;
	ne = -1;
	sw = -1;
	se = -1;
}

// same procedure like generateSeeds but additonal draws the point to the seed image
void QuadTree::drawGenerateSeeds(cv::Mat& seedimg, cv::Mat& depth, cv::Mat& gx, cv::Mat& gy, bool background) {
	uint r_size = m_max_depth-1;
	uint size = hnodes[r_size].size();

	ushort g_x, g_y;
	// calc c_x, c_y in the leaf nodes
	
	if(background) {
		for(uint n = 0; n < size; ++n) {
			QtreeNode& leaf = nodes[hnodes[r_size][n]];
			QtreeRect& rect = leaf.rect;

			// init with the first values
			leaf.H_x = gx.at<ushort>(rect.min_y, rect.min_x);
			leaf.H_y = gy.at<ushort>(rect.min_y, rect.min_x);
			leaf.L_x = leaf.H_x;
			leaf.L_y = leaf.H_y;
			for(int y = rect.min_y; y <= rect.max_y; ++y) {
				for(int x = rect.min_x; x <= rect.max_x; ++x) {
					g_x = gx.at<ushort>(y, x);
					g_y = gy.at<ushort>(y, x);
				
					leaf.H_x = MU_MAX( leaf.H_x, g_x );
					leaf.H_y = MU_MAX( leaf.H_y, g_y );
					leaf.L_x = MU_MIN( leaf.L_x, g_x );
					leaf.L_y = MU_MIN( leaf.L_y, g_y );
				}
			}

			leaf.c_x = leaf.H_x - leaf.L_x;
			leaf.c_y = leaf.H_y - leaf.L_y;
			if(leaf.c_x >= m_T_leaf || leaf.c_y >= m_T_leaf) {
				// region is non planar
	
				if(depth.at<ushort>(rect.min_y, rect.min_x) != USHRT_MAX) {
					cv::Point2f p1(rect.min_x, rect.min_y);
					circle(seedimg, p1, 2, cv::Scalar(255, 0, 0, 255), CV_FILLED);
				}

				if(depth.at<ushort>(rect.min_y, rect.max_x) != USHRT_MAX) {
					cv::Point2f p2(rect.max_x, rect.min_y);
					circle(seedimg, p2, 2, cv::Scalar(255, 0, 0, 255), CV_FILLED);
				}

				if(depth.at<ushort>(rect.max_y, rect.min_x) != USHRT_MAX) {
					cv::Point2f p3(rect.min_x, rect.max_y);
					circle(seedimg, p3, 2, cv::Scalar(255, 0, 0, 255), CV_FILLED);
				}

				if(depth.at<ushort>(rect.max_y, rect.max_x) != USHRT_MAX) {
					cv::Point2f p4(rect.max_x, rect.max_y);
					circle(seedimg, p4, 2, cv::Scalar(255, 0, 0, 255), CV_FILLED);
				}
			}
		}

		// calc c_x, c_y in the inner nodes
		for(int d = (int)r_size - 1; d >= 0; --d) {
			size = hnodes[d].size();
			for(uint n = 0; n < size; ++n) {
				QtreeNode& node = nodes[hnodes[d][n]];
	
				QtreeNode& nw = nodes[node.nw];
				QtreeNode& ne = nodes[node.ne];
				QtreeNode& sw = nodes[node.sw];
				QtreeNode& se = nodes[node.se];
	
				node.H_x = MU_MAX4(nw.H_x, ne.H_x, sw.H_x, se.H_x);
				node.H_y = MU_MAX4(nw.H_y, ne.H_y, sw.H_y, se.H_y);
				node.L_x = MU_MIN4(nw.L_x, ne.L_x, sw.L_x, se.L_x);
				node.L_y = MU_MIN4(nw.L_y, ne.L_y, sw.L_y, se.L_y);
				
				node.c_x = node.H_x - node.L_x;
				node.c_y = node.H_y - node.L_y;
				if(node.c_x >= m_T_internal || node.c_y >= m_T_internal) {
					QtreeRect& rect = node.rect;

					if(depth.at<ushort>(rect.min_y, rect.min_x) != USHRT_MAX) {
						cv::Point2f p1(rect.min_x, rect.min_y);
						circle(seedimg, p1, 2, cv::Scalar(0, 255, 0, 255), CV_FILLED);
					}

					if(depth.at<ushort>(rect.min_y, rect.max_x) != USHRT_MAX) {
						cv::Point2f p2(rect.max_x, rect.min_y);
						circle(seedimg, p2, 2, cv::Scalar(0, 255, 0, 255), CV_FILLED);
					}

					if(depth.at<ushort>(rect.max_y, rect.min_x) != USHRT_MAX) {
						cv::Point2f p3(rect.min_x, rect.max_y);
						circle(seedimg, p3, 2, cv::Scalar(0, 255, 0, 255), CV_FILLED);
					}

					if(depth.at<ushort>(rect.max_y, rect.max_x) != USHRT_MAX) {
						cv::Point2f p4(rect.max_x, rect.max_y);
						circle(seedimg, p4, 2, cv::Scalar(0, 255, 0, 255), CV_FILLED);
					}
				} 
			}
		}
	} else {
		for(uint n = 0; n < size; ++n) {
			QtreeNode& leaf = nodes[hnodes[r_size][n]];
			QtreeRect& rect = leaf.rect;

			// init with the first values
			leaf.H_x = gx.at<ushort>(rect.min_y, rect.min_x);
			leaf.H_y = gy.at<ushort>(rect.min_y, rect.min_x);
			leaf.L_x = leaf.H_x;
			leaf.L_y = leaf.H_y;
			for(int y = rect.min_y; y <= rect.max_y; ++y) {
				for(int x = rect.min_x; x <= rect.max_x; ++x) {
					g_x = gx.at<ushort>(y, x);
					g_y = gy.at<ushort>(y, x);
				
					leaf.H_x = MU_MAX( leaf.H_x, g_x );
					leaf.H_y = MU_MAX( leaf.H_y, g_y );
					leaf.L_x = MU_MIN( leaf.L_x, g_x );
					leaf.L_y = MU_MIN( leaf.L_y, g_y );
				}
			}

			leaf.c_x = leaf.H_x - leaf.L_x;
			leaf.c_y = leaf.H_y - leaf.L_y;
			if(leaf.c_x >= m_T_leaf || leaf.c_y >= m_T_leaf) {
				// region is non planar
	
				cv::Point2f p1(rect.min_x, rect.min_y);
				circle(seedimg, p1, 2, cv::Scalar(255, 0, 0, 255), CV_FILLED);

				cv::Point2f p2(rect.max_x, rect.min_y);
				circle(seedimg, p2, 2, cv::Scalar(255, 0, 0, 255), CV_FILLED);

				cv::Point2f p3(rect.min_x, rect.max_y);
				circle(seedimg, p3, 2, cv::Scalar(255, 0, 0, 255), CV_FILLED);

				cv::Point2f p4(rect.max_x, rect.max_y);
				circle(seedimg, p4, 2, cv::Scalar(255, 0, 0, 255), CV_FILLED);
			}
		}

		// calc c_x, c_y in the inner nodes
		for(int d = (int)r_size - 1; d >= 0; --d) {
			size = hnodes[d].size();
			for(uint n = 0; n < size; ++n) {
				QtreeNode& node = nodes[hnodes[d][n]];
	
				QtreeNode& nw = nodes[node.nw];
				QtreeNode& ne = nodes[node.ne];
				QtreeNode& sw = nodes[node.sw];
				QtreeNode& se = nodes[node.se];
	
				node.H_x = MU_MAX4(nw.H_x, ne.H_x, sw.H_x, se.H_x);
				node.H_y = MU_MAX4(nw.H_y, ne.H_y, sw.H_y, se.H_y);
				node.L_x = MU_MIN4(nw.L_x, ne.L_x, sw.L_x, se.L_x);
				node.L_y = MU_MIN4(nw.L_y, ne.L_y, sw.L_y, se.L_y);
				
				node.c_x = node.H_x - node.L_x;
				node.c_y = node.H_y - node.L_y;
				if(node.c_x >= m_T_internal || node.c_y >= m_T_internal) {
					QtreeRect& rect = node.rect;

					cv::Point2f p1(rect.min_x, rect.min_y);
					circle(seedimg, p1, 2, cv::Scalar(0, 255, 0, 255), CV_FILLED);

					cv::Point2f p2(rect.max_x, rect.min_y);
					circle(seedimg, p2, 2, cv::Scalar(0, 255, 0, 255), CV_FILLED);

					cv::Point2f p3(rect.min_x, rect.max_y);
					circle(seedimg, p3, 2, cv::Scalar(0, 255, 0, 255), CV_FILLED);

					cv::Point2f p4(rect.max_x, rect.max_y);
					circle(seedimg, p4, 2, cv::Scalar(0, 255, 0, 255), CV_FILLED);
				} 
			}
		}
	}

	cv::imwrite("quadtree_seeds.png", seedimg);
}
