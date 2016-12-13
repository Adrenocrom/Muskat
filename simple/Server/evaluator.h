/***********************************************************
 *
 *
 *						EVALUATOR HEADER
 *					 ======================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Declaration of the evaluator class.
 *		Implements the PSNR and the SSIM.
 *		Stores the result values and images.
 *
 *
 ***********************************************************/

#ifndef EVALUATOR_H
#define EVALUATOR_H

#pragma once

// for every result: angle, psnr
// and for the SSIM r,g,b and mean are stored
struct ResultEntry {
	double 	   angle;
	double 		PSNR;
	cv::Scalar MSSIM;
};

class Evaluator{
public:
	
	Evaluator(Config* config, Compressor* compressor);
	~Evaluator();

	// after all results are recived the psnr and ssim
	// will be stored, by running this function
	void runEvaluation();

	// resize the vectors which stores the information
	// not all scenes have the same size
	void setScene(Scene* scene);

	// calculate the ssim values from ground truth image and the extrapolated one
	cv::Scalar getMSSIM( const cv::Mat& i1, const cv::Mat& i2);

	// id of the current handled evaluation, used to run multiple evaluations
	int getEvaluationId();

	// calculate psnr from ground truth and extrapolated image
	double getPSNR(const cv::Mat& I1, const cv::Mat& I2);

	// resset counter for reciving result images, can be used 
	// to recive them async.
	void newMessure(int SceneId, int messureId, const string& name, const string& short_name, int num_vertices = -1, int num_indices = -1, int num_triangles = -1);
	
	// increase result counter and stores image
	void addResult(int id, cv::Mat* img, double duration);

	// if all result images are recived, this function return true
	bool hasResults();

private:


	Scene*			m_scene;
	
	Config* 		m_config;
	Compressor* 	m_compressor;

	uint			m_cnt;				// counts recived results
	
	int				m_scene_id;			
	int				m_messure_id;

	string			m_name;
	string			m_short_name;		// short names are needed for tex, to 
										// distinguish all datasets
	string			m_filename;			// needed to create directory

	// mesh informations
	int				m_num_vertices;
	int				m_num_indices;
	int				m_num_triangles;

	// stores quality results
	vector<cv::Mat> m_results;
	
	// stores extrapolation time
	vector<double>	m_durations;
	
	double			m_min_duration;
	double			m_max_duration;
	double			m_mean_duration;
};

#endif
