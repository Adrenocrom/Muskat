#ifndef EVALUATOR_H
#define EVALUATOR_H

#pragma once

struct ResultEntry {
	double 	   angle;
	double 		PSNR;
	cv::Scalar MSSIM;
};

class Evaluator{
public:
	
	Evaluator();
	~Evaluator();

	void runEvaluation();

	void setScene(Scene* scene);

	cv::Scalar getMSSIM( const cv::Mat& i1, const cv::Mat& i2);

	int getEvaluationId();

	double getPSNR(const cv::Mat& I1, const cv::Mat& I2);

	void newMessure(int SceneId, int messureId, const string& name);
	void addResult(int id, cv::Mat* img, double duration);
	bool hasResults();

private:

	Scene*	m_scene;
	uint	m_cnt;		// counts recived results
	
	int		m_scene_id;
	int		m_messure_id;

	string	m_name;
	string	m_filename;

	vector<cv::Mat> m_results;
	vector<double>	m_durations;
	
	double	m_min_duration;
	double	m_max_duration;
	double	m_mean_duration;
};

#endif
