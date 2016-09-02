#include "muskat.h"

using namespace cv;

Evaluator::Evaluator() {
	m_cnt 	= 0;
	m_scene = nullptr;
}

Evaluator::~Evaluator() {

}

void Evaluator::setScene(Scene* scene) {
	m_scene = scene;
	results.resize(m_scene->m_infos.size());
}

void Evaluator::runEvaluation(string path) {
	uint r_size = results.size();
	list<ResultEntry> entries;
	list<ResultEntry> meanEntries;
	ResultEntry entry;

	cout<<"start evaluation"<<endl;

	for(uint i = 0; i < r_size; ++i) {
		entry.PSNR  = getPSNR(m_scene->m_fbs[i].rgb, results[i]);
		entry.MSSIM = getMSSIM(m_scene->m_fbs[i].rgb, results[i]);
		entry.angle = m_scene->m_infos[i].offangle;
		entries.push_back(entry);

		QString filename = "res/frame_"+ QString::number(i) +".png";
		imwrite(filename.toStdString(), results[i]);
	}

	entries.sort([this](ResultEntry& l, ResultEntry& r) {
		if(l.angle > r.angle) 
			return true; 
		return false; 
	});

	entry = entries.front();
	double cnt = 1;
	for(auto it = entries.begin(); it != entries.end(); ++it) {
		cout<<entry.angle<<" "<<it->angle<<endl;
		if(it->angle < entry.angle || next(it) == entries.end()) {
			entry.angle;
			entry.PSNR	   /= cnt;
			entry.MSSIM[0] /= cnt;
			entry.MSSIM[1] /= cnt;
			entry.MSSIM[2] /= cnt;
			entry.MSSIM[3] /= cnt;
			meanEntries.push_back(entry);
			
			cnt   = 1;
			entry = *it;
		} else {
			entry.PSNR	+= it->PSNR;
			entry.MSSIM[0] += it->MSSIM[0];
			entry.MSSIM[1] += it->MSSIM[1];
			entry.MSSIM[2] += it->MSSIM[2];
			entry.MSSIM[3] += it->MSSIM[3];

			cnt++;
		}
	}

	ofstream file;
  	file.open("res/results.tex");

	file<< "\\begin{filecontents}{div_data.csv}\n";
	file<< "a,p,r,g,b,m\n";

	for(auto it = entries.begin(); it != entries.end(); ++it) {
		file<<it->angle<<","
			<<it->PSNR<<","
			<<it->MSSIM[2]<<","
			<<it->MSSIM[1]<<","
			<<it->MSSIM[0]<<","
			<<it->MSSIM[3]<<"\n";
	}
	
	file<< "\\end{filecontents}\n\n";

	file<< "\\begin{filecontents}{div_data_mean.csv}\n";
	file<< "a,p,r,g,b,m\n";

	for(auto it = meanEntries.begin(); it != meanEntries.end(); ++it) {
		file<<it->angle<<","
			<<it->PSNR<<","
			<<it->MSSIM[2]<<","
			<<it->MSSIM[1]<<","
			<<it->MSSIM[0]<<","
			<<it->MSSIM[3]<<"\n";
	}
	
	file<< "\\end{filecontents}\n\n";


	file.close();
	cout<<"end evaluation"<<endl;
}

void Evaluator::newMessure() {
	m_cnt = 0;
}

void Evaluator::addResult() {
	m_cnt++;
}	

bool Evaluator::hasResults() {
	if(m_cnt == m_scene->m_infos.size())
		return true;
	return false;
}

double Evaluator::getPSNR(const Mat& I1, const Mat& I2)
{
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2

    Scalar s = sum(s1);         // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double  mse =sse /(double)(I1.channels() * I1.total());
        double psnr = 10.0*log10((255*255)/mse);
        return psnr;
    }
}

Scalar Evaluator::getMSSIM( const Mat& i1, const Mat& i2)
{
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d     = CV_32F;

    Mat I1, I2;
    i1.convertTo(I1, d);           // cannot calculate on one byte large values
    i2.convertTo(I2, d);

    Mat I2_2   = I2.mul(I2);        // I2^2
    Mat I1_2   = I1.mul(I1);        // I1^2
    Mat I1_I2  = I1.mul(I2);        // I1 * I2

    /*************************** END INITS **********************************/

    Mat mu1, mu2;   // PRELIMINARY COMPUTING
    GaussianBlur(I1, mu1, Size(11, 11), 1.5);
    GaussianBlur(I2, mu2, Size(11, 11), 1.5);

    Mat mu1_2   =   mu1.mul(mu1);
    Mat mu2_2   =   mu2.mul(mu2);
    Mat mu1_mu2 =   mu1.mul(mu2);

    Mat sigma1_2, sigma2_2, sigma12;

    GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;

    GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;

    GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    ///////////////////////////////// FORMULA ////////////////////////////////
    Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

    Mat ssim_map;
    divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

    Scalar mssim = mean( ssim_map ); // mssim = average of ssim map

	mssim[3] = (mssim[0] + mssim[1] + mssim[2]) / 3.0; // save mead in alpha

    return mssim;
}
