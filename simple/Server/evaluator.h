#ifndef EVALUATOR_H
#define EVALUATOR_H

#pragma once

struct Result {
	double 	   angle;
	double 		PSNR;
	cv::Scalar MSSIM;

	
}

class Evaluator{
public:
	vector<cv::Mat> results;
	
	Evaluator();
	~Evaluator();

	void runEvaluation(std::string path);

	void setScene(Scene* scene);

	cv::Scalar getMSSIM( const cv::Mat& i1, const cv::Mat& i2);

	double getPSNR(const cv::Mat& I1, const cv::Mat& I2);

	void newMessure();
	void addResult();
	bool hasResults();

private:

	Scene*	m_scene;
	uint	m_cnt;		// counts recived results 
};

#endif
