 
 
#pragma once
 

#include "opencv\\cv.h"
#include "opencv\\highgui.h"
#include <vector>
using namespace std;

class CodedTarget  
{
public:
	bool Check_centerCircle(IplImage *img, int rad);
	void CenterCircle_unlimitedSize(IplImage *img);
	void Decoding_unlimitedSize(IplImage *img);
	void Decoding_oneStep(IplImage *img, int mode=0, int target_num_set=350);
	void Rectify_ellipse4(int sn);
	int Target_gray(IplImage *img, int rad);
	void Get_adaptive_parameter(IplImage *img, int mode=0);
	void Refine_corner(IplImage *img, CvBox2D box, CvPoint2D32f *pt_out);
	void Draw_circle();
	void Draw_target();
	void Rectifying(int sn);
	void Rectify_ellipse3(int sn);
	void Decoding(IplImage *img);

	void CodeCenter(float *xy, CvPoint *pts, int count, IplImage *img, bool *flg);
	void Rectify_ellipse2(int sn);
	void DecodeAll();
	int Decode(IplImage * img, int rad, int th=0);
	void Binary(int th);
	void Rectify_ellipse(int sn);
	void Initial(int width, int height, int target_num_set=350);
	void CenterCircle( IplImage * img);
	int Otsu(IplImage * img);

	CodedTarget();
	virtual ~CodedTarget();
		void Draw_mark();    //huangjin
//////////
	struct TARGET
	{
		float x;	//标志点坐标x
		float y;	//y  （x, y）坐标系为图像坐标系，原点为图像左上角。
		int code;	//码值
		int sn;		//检测序号
		int re;		//保留

	}  *m_target;		//输出目标，m_target[i] 为第i 个目标， i < m_targetNum


	IplImage * m_img_orig;		//输入图像
	IplImage * m_img_bw;		//二值化图像
	IplImage * m_imgc;		//输出彩色图像
	IplImage * m_imgt;		//临时图像

	int m_h;	//输入图像高
	int m_w;	//宽

	bool m_init;	//初始化标志
	bool m_bw;		//二值化图像标志
	bool m_refine_corner;	//是否利用对角标志精细定位

	int m_circleNum;	//中心圆数
	int m_targetNum;	//实测目标数
	int m_targetNum_set;  //设定目标数

	int m_dia;		//中心圆设定直径
	int m_rad;		//中心圆实测半径

	int m_th;		//二值化阈值
	int m_thresh;	//二值化方法，0 固定全局阈值，1 动态全局阈值，  2  动态局部阈值

///////
//////mono//////private:
	void Get_dia_gray_local(IplImage *img, int *dia, int *gray, int *num);
	void Get_dia_gray(IplImage *img, int *dia, int *gray, int *num);

	void BubbleSort(int *a, int n);
	int	 Angle(CvPoint pt);
	void Remove_near(vector <CvBox2D> *boxv);
	float Fit_ellipse_residual( CvSeq* contour, CvBox2D box);
	int LeastCode(int c, int len);

	vector <CvBox2D> m_boxv;
	bool m_imgt_flg;
//	int m_target_th;

};

 