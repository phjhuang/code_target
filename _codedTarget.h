 
 
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
		float x;	//��־������x
		float y;	//y  ��x, y������ϵΪͼ������ϵ��ԭ��Ϊͼ�����Ͻǡ�
		int code;	//��ֵ
		int sn;		//������
		int re;		//����

	}  *m_target;		//���Ŀ�꣬m_target[i] Ϊ��i ��Ŀ�꣬ i < m_targetNum


	IplImage * m_img_orig;		//����ͼ��
	IplImage * m_img_bw;		//��ֵ��ͼ��
	IplImage * m_imgc;		//�����ɫͼ��
	IplImage * m_imgt;		//��ʱͼ��

	int m_h;	//����ͼ���
	int m_w;	//��

	bool m_init;	//��ʼ����־
	bool m_bw;		//��ֵ��ͼ���־
	bool m_refine_corner;	//�Ƿ����öԽǱ�־��ϸ��λ

	int m_circleNum;	//����Բ��
	int m_targetNum;	//ʵ��Ŀ����
	int m_targetNum_set;  //�趨Ŀ����

	int m_dia;		//����Բ�趨ֱ��
	int m_rad;		//����Բʵ��뾶

	int m_th;		//��ֵ����ֵ
	int m_thresh;	//��ֵ��������0 �̶�ȫ����ֵ��1 ��̬ȫ����ֵ��  2  ��̬�ֲ���ֵ

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

 