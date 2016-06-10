 // 420 odedTarget.cpp: implementation of the CodedTarget class.
//�����־ʶ��  12λ���ͱ���, ��ͼ ��7+4��-6-6-4, 
// ���� 2015-4-22
//ʹ�÷�����
//(1) ��ʼ��:  �����ڴ�ռ䣬ִ��Initial( width, height, target_num_set=350)��
//			width ͼ���,  height ��, width��height�������룻 
//			target_num_set �����Ŀ������ Ĭ��331, ��ȷ�����ֵ�ɼӿ�����ٶȡ�	
//		ע����������ͼ�񣬳�ʼ��ֻ��ִ��һ�Σ�

//(2) �������ã����Զ����˹�����2�ַ�ʽ��				
//		2.1  �Զ���ȡ��ִ��Get_adaptive_parameter(IplImage *img��int mode=0)��img ����ͼ��
//				mode ģʽ	��0��ȫ�Զ�ģʽ��Ĭ��ֵ��
//							��1����ͨģʽ���ٶȿ죻
//							��2�������ղ�����ģʽ��
//				
//		ע�����ڹ��պ�Ŀ���С�仯���������ͼ��Get_adaptive_parameter()ֻ��ִ��һ�Ρ� 
//
//		2.2 �˹����ã����������²�����
//			m_dia : ��־����Բֱ�����ظ����� ������ԲΪ��Чֱ����d=sqrt(a*b)*2, a,b Ϊ��Բ�����᣻	 
//			m_thresh�� ��ֵ������	=0  �̶�ȫ����ֵ(��ͨģʽ���踳ֵ m_th ȫ����ֵ), ��������С��
//									=1  ��̬ȫ����ֵ���Զ����� m_th ȫ����ֵ��
//									=2  ��̬�ֲ���ֵ, �ʺϹ��ղ�������������������
//			
// 
//(3) ��⣬ִ��Decoding(IplImage *img)  img ����ͼ��
//
//(4) ��������ͨ������ m_target �ṹ�壬 �μ�ͷ�ļ��� m_targetNum ʵ��Ŀ������
//				������ʾĿ�ִ꣬�� Draw_target()�� ͨ�� m_imgc ����ʾ���Ŀ��Ĳ�ɫͼ��
//(5) ����˵����
//    a һ�����룬Decoding_oneStep()���ں���ʼ�����Զ�������ȡ��ʹ�÷��㣬�ʺ϶�ʵʱ��Ҫ�󲻸ߵĳ��ϡ�
//	  b  Decoding_unlimitedSize(IplImage *img) Ŀ��ߴ������ƽ��룬�ʺ϶�Ŀ������ߴ����С�ߴ�֮�� > 4 �ĳ��ϡ�

// uneven illumination
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_codedTarget.h"

#include <math.h>

#define   PI 3.1415

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CodedTarget::CodedTarget()
{
	m_init=false;
	m_bw=false;
	m_imgt_flg=false;

	m_refine_corner=true;			//Ĭ�����ýǵ㶨λ
//	m_refine_corner=false;			
	
	m_targetNum=0;
	m_circleNum=0;

	m_dia=30;
	m_th=100;

}

CodedTarget::~CodedTarget()
{
	if (m_init)
	{
		cvReleaseImage( &m_imgc);	
		cvReleaseImage( &m_img_bw);

		delete []m_target;

		if (m_imgt_flg)
			cvReleaseImage( &m_imgt);
			
	}

}

//��ͼ���ֵ����ֵ
int CodedTarget::Otsu(IplImage *img)
{
	int h=img->height;
	int w=img->width;

    float hist[256];

	int i, j;
	for (i=0; i < 256; i++)
		hist[i]=0;

	BYTE *pb=(BYTE*)img->imageData;	

	int wh=w*h;
	int st=5;

	if ( wh < 300000)
		st=3;
	else if ( wh < 1000000)
		st=4;
	else if ( wh < 3000000)
		st=5;

	int n=0;
	for (j=0; j<h; j+=st)
		for ( i=0; i<w; i+=st)
		{
			hist[pb[j*w+i]]+=1;	
			n++;
		}

	float sz=(float)n;

	for (i=0; i < 256; i++)
		hist[i]=hist[i]/sz;

	float avg=0;
	for (i=0; i<256; i++)
		avg+=hist[i]*i;

	float p1=0, mp1=0;
	float var, max=0;
	int th=0;

	for (i=0; i<256; i++)
	{
		p1+=hist[i];
		mp1+=hist[i]*i;

		var=(avg*p1-mp1)*(avg*p1-mp1)/(p1*(1-p1));

		if ( var > max )
		{
			max=var;
			th=i;
		}

	}

//	printf("th=%d, avg=%f, \n", th, avg);

	if ( avg < th-10 )
	{
		th=(th*2+avg*1)/3;
	}

//	printf("th2=%d, avg=%f, \n", th, avg);

	return th;

}

//������Բ
void CodedTarget::CenterCircle(IplImage *img)
{
	if ( m_dia <= 5 )
	{
		m_circleNum=0;
		return;
	}
	
	CvSize sz=cvGetSize(img);
	int h=sz.height;
	int w=sz.width;

/*	
	IplConvKernel *m_e1;
	m_e1=cvCreateStructuringElementEx( 3, 3, 1, 1,CV_SHAPE_RECT);

	cvErode(img,img, m_e1);				//������
	cvDilate(img,img,m_e1);

	cvReleaseStructuringElement(&m_e1);
*/
	
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	
	cvFindContours( img, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE); 	//CV_RETR_EXTERNAL  SIMPLE

	CvBox2D box;

	int		len=m_dia*PI;
	float	area=(m_dia/2)*(m_dia/2)*PI;
		
///////
	m_boxv.clear( );
	
	int k=0;
	for( ; contour != 0; contour = contour->h_next )
    {
		
		int count= contour->total; //��������

		if ( count < 15 )
			continue;
		
		double length = cvArcLength(contour);  
		double tmparea=fabs(cvContourArea(contour, CV_WHOLE_SEQ)); 
		
		if ( count< len*0.3 || count > len*3 )
        {
			continue;
        }

		if ( length< len*0.4 || length > len*2.5 )
        {
			continue;
        }
		
		if ( tmparea < area*0.25 || tmparea > area*4.5 )
        {
 //          cvSeqRemove(contour,0);
			continue;
        }

		float c=length*length/(4*PI*tmparea);
		if ( c > 1.9 )
		{
			continue;
		}

		float lc=length/count;
		if ( lc > 4 )
		{
			continue;
		}

//		printf(" count=%d, length = %f, tmparea=%f,  c= %f,\n ", count,length,  tmparea, c);
//		printf(" a= %f, b= %f, angle =%f, \n", box.size.width, box.size.height, box.angle);
		
//		CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );
//		CvScalar color = CV_RGB( 255, 0, 0 );
		/* replace CV_FILLED with 1 to see the outlines */
		
//		cvDrawContours( m_imgc, contour, color, color, -1, 1, 8 );
									
		box=cvFitEllipse2(contour);

		float	a = box.size.width*0.5;
		float   b = box.size.height*0.5;
	
		int rad=sqrt(a*b)*3;
		int cx=box.center.x;
		int cy=box.center.y;

		if ( cx < rad || (m_w-cx) < rad || cy < rad || (m_h-cy) < rad )
			continue;

		float re=Fit_ellipse_residual(contour, box);
	
		float re_max=0.08;
		int dia=sqrt(a*b)*2;
		
		if ( dia < 15)
			re_max=0.11;
		else if ( dia < 20)
			re_max=0.10;
		else if ( dia < 35)
			re_max=0.09;

		if ( re > re_max)
		{
			continue;
		}

		k++;
		m_boxv.push_back( box );

//		printf( "re = %f, \n", re);
//		printf(" count=%d, length = %f, tmparea=%f,  c= %f,\n ", count,length,  tmparea, c);
//		printf(" a= %f, b= %f, angle =%f, \n", box.size.width, box.size.height, box.angle);
								
	}

//	printf("target number_pre  = %d, \n", k);
//	printf("target number v = %d, \n", m_boxv.size( ));

	int length=k;

	if ( length > 1)
		Remove_near(&m_boxv);

	m_circleNum=m_boxv.size( );

//	printf("circle number = %d, \n", m_circleNum);

//	cvNamedWindow( "Components", 1 );	
//	cvShowImage( "Components", m_imgc );

	cvReleaseMemStorage(&storage);

}

//�����ʼ��
// width ͼ���,   height ��,  target_num_set �����Ŀ������ Ĭ��350
void CodedTarget::Initial(int width, int height, int target_num_set)
{
	if ( m_init)
	{
		cvReleaseImage( &m_imgc);
		cvReleaseImage( &m_img_bw);

		delete []m_target;
	}
	
	m_init=false;

//////

	if ( target_num_set < 0 || target_num_set > 10000 )
	{
		//AfxMessageBox("target_num_set Ϊ����̫��!");
		return;

	}

	if ( width < 50 || height < 50 )
	{
		//AfxMessageBox("ͼ�� width �� height ����̫С!");
		return;

	}

	if (  width%4 != 0 )
	{
		//AfxMessageBox("ͼ���Ӧ�ܱ�4������");
		return;
	}

//////////

	m_target=new TARGET[target_num_set+20];
	m_targetNum_set=target_num_set;
/////
	
	m_h=height;
	m_w=width;
	
	CvSize sz;

	sz.height=height;
	sz.width=width;

	m_imgc=cvCreateImage( sz, IPL_DEPTH_8U, 3 );
	m_img_bw=cvCreateImage( sz, IPL_DEPTH_8U, 1 );

	m_init=true;
	m_bw=false;

}

//��Բ�任Ϊ��׼Բ, ����ڲ�ֵ
void CodedTarget::Rectify_ellipse(int sn)
{
	vector <CvBox2D>::iterator it;
	it=m_boxv.begin( )+sn-1;

	CvBox2D box=*it;

	float     xc = box.center.x;
    float	  yc = box.center.y;

    float	a = box.size.width*0.5;
    float   b = box.size.height*0.5;

	if (m_imgt_flg)
		cvReleaseImage( &m_imgt);

//	int rad2=sqrt(a*b);
//	printf("rad = %d, \n", rad2);

	int rad=sqrt(a*b)*5.3;
	rad=rad-rad%4;

	m_imgt=cvCreateImage( cvSize(rad, rad), IPL_DEPTH_8U, 1 );
	m_imgt_flg=true;

/////////////
//	float t=sqrt(a*b);

	m_rad=sqrt(a*b);

	float ang = box.angle*PI/180;  ///
	float cosa=cos(ang);
	float sina=sin(ang);

	float r00, r01, r10, r11;

	float r1=sqrt(a/b);
	float r2=sqrt(b/a);

//*
	r00=cosa*r1;
	r01=-sina*r2;
	r10=sina*r1;
	r11=cosa*r2;

//////
	CvSize sz=cvGetSize(m_imgt);

	int h=sz.height;
	int w=sz.width;

	CvSize sz2=cvGetSize(m_img_orig);

	int h2=sz2.height;
	int w2=sz2.width;

	BYTE *pb=(BYTE*)m_imgt->imageData;	
	BYTE *pb2=(BYTE*)m_img_orig->imageData;
	
	int x, y;
	int x2, y2;
	int indx, num=h2*w2;

	int i, j;
	for (j=0; j< h; j++)
		for ( i=0; i< w; i++)
		{
			x=i-w/2;
			y=j-h/2;

			x2=x*r00+y*r01+xc+0.5;
			y2=x*r10+y*r11+yc+0.5;
		
			indx=y2*w2+x2;

			if ( indx > 0 && indx < num  ) ///y2>=0 && y2 < h2 && x2>=0 && x2 < w2
				pb[j*w+i]=pb2[indx];
			else
				pb[j*w+i]=0;

		}

}

//ͼ���ֵ��, th �Ҷ���ֵ
void CodedTarget::Binary(int th)
{
	int th2=th;

	if (m_thresh == 0)
	{
		cvThreshold(m_img_orig, m_img_bw, th, 255, CV_THRESH_BINARY);
		
	}
	else if (m_thresh == 1)
	{
		th2=Otsu(m_img_orig);
		cvThreshold(m_img_orig, m_img_bw, th2, 255, CV_THRESH_BINARY);
	
	}
	else if (m_thresh == 2)
	{
		int w=m_dia*3;
		w=(w-w%2)+1;

		w=(w > 3)? w:3;
//		printf("w=%d,\n", w);
		
		cvAdaptiveThreshold(m_img_orig, m_img_bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY,w, -10);
	
	}

//	printf(" threshold = %d, \n", th2);

	m_th=th2;
	m_bw=true;

 	//cvNamedWindow( "img4", 0 );
 	//cvResizeWindow("img4",m_ww, m_wh);
 	//cvShowImage( "img4", m_img_bw);

}


//��Բ������
float CodedTarget::Fit_ellipse_residual(CvSeq *contour, CvBox2D box)
{

	int count= contour->total; //��������
	
	CvPoint *pts=new CvPoint[count];
		
	cvCvtSeqToArray(contour,pts,CV_WHOLE_SEQ);

	float     xc = box.center.x;
    float	  yc = box.center.y;

    float	a = box.size.width*0.5;
    float   b = box.size.height*0.5;

	float ang = -box.angle*PI/180;
	float cosa=cos(ang);
	float sina=sin(ang);

	float x, y, t;
	int k;

	double re=0;

	for (k=0; k< count; k++)
	{
		x=pts[k].x-xc;
		y=pts[k].y-yc;

		t=fabs( (x*cosa-y*sina)*(x*cosa-y*sina)/(a*a) 
			+ (x*sina+y*cosa)*(x*sina+y*cosa)/(b*b) -1 );

//		t=fabs( (x*cosa+y*sina)*(x*cosa+y*sina)/(a*a) 
//			+ (x*sina-y*cosa)*(x*sina-y*cosa)/(b*b) -1 );

		re+=t;


	}

	delete []pts;
	return re/count;

}


//ȥ��������Բ�ӽ������
void CodedTarget::Remove_near(vector <CvBox2D> *boxv)
{
//	int len=boxv.size( );

	float d, th=m_dia*1.1;	//1.5 1.3
	float x, y;
	float x2, y2;
	bool change;

	vector <CvBox2D>::iterator it, it2;

	for ( it = (*boxv).begin( ) ; it != (*boxv).end( )-1 && it != (*boxv).end( ); )
	{
		it2 = it+1; 
		change=false;

		for (  ; it2 != (*boxv).end( ) ;  )
		{
			x=(*it).center.x;
			y=(*it).center.y;
			
			x2=(*it2).center.x;
			y2=(*it2).center.y;
			
			d=(x-x2)*(x-x2)+(y-y2)*(y-y2);
			d=sqrt(d);

			if ( d < th )
			{
//*
				if ( fabs((*it).size.width* (*it).size.height) > fabs( (*it2).size.width* (*it2).size.height ))
				{
					it2=(*boxv).erase(it2);
//					printf("it2, \n");
					continue;

				}
				else
				{
//					printf("it, \n");
					it=(*boxv).erase(it);
			
					change=true;
					break;
				}
  //*/

			}
			
			it2++;

		}

		if (!change)
			it++;

	}
	
}

///������־����
int CodedTarget::Decode(IplImage *img, int rad, int th)
{
	CvSize sz=cvGetSize(img);

	int h=sz.height;
	int w=sz.width;

//	int area30=PI*rad*rad*( 2.5*2.5- (7/4.0)*(7/4.0) )/12.0;
//	int area30=PI*rad*rad*( (21/9.0)*(21/9.0)- (15/9.0)*(15/9.0) )/12.0;
	int area30=PI*rad*rad*( (23/11.0)*(23/11.0)- (17/11.0)*(17/11.0) )/12.0;

//	if (m_thresh == 2)
	{
	 
//		int th=Target_gray(m_imgt, m_rad);
		cvThreshold(img, img, th, 255, CV_THRESH_BINARY);

	}
//	else
//		cvThreshold(img, img, m_th*1.05, 255, CV_THRESH_BINARY);

//	cvThreshold(img, img, m_th*1.05, 255, CV_THRESH_BINARY);

	if (!Check_centerCircle(img,rad))
			return -1;

	IplImage *imgbw=cvCloneImage(img);

	bool *flg=new bool[h*w];
	
	int i;
	for ( i=0; i< h*w; i++)
		flg[i]=true;

//	IplConvKernel *m_e1;
//	m_e1=cvCreateStructuringElementEx( 3, 3, 1, 1,CV_SHAPE_RECT);

//	cvErode(img,img, m_e1);				//������
//	cvDilate(img,img,m_e1);

//	cvReleaseStructuringElement(&m_e1);

/*
	cvNamedWindow( "img5", 0);
	cvResizeWindow("img5",w, h);

	cvShowImage( "img5", img);
//*/

//	IplImage	*imgx=cvCreateImage( sz, IPL_DEPTH_8U, 1 );

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;

	int num=cvFindContours( img, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE ); 	//CV_RETR_EXTERNAL 

	float *areas=new float[num];
	float *lens=new float[num];
	int *arean=new int[num];
	int *ang=new int[num];

	float area_rt=1.0;
	float len_rt=1.0;

	int minCount=PI*rad/5;
	minCount=(minCount > 6 ) ? minCount : 6;

////////////////////////////////////////////////////

	bool cntflg=false;
	int k=0;
	for( ; contour != 0; contour = contour->h_next )	//contour = contour->h_next
    {
				
		int count= contour->total; //��������
		 
		if ( count < minCount )
			continue;

		CvPoint *pts=new CvPoint[count];
		cvCvtSeqToArray(contour,pts,CV_WHOLE_SEQ);

		int i;
		int x=0, y=0;

		for (i=0; i< count; i++)
		{
			x+=pts[i].x;
			y+=pts[i].y;

		}

		x=x/count-w/2;
		y=y/count-h/2;

		float dist=sqrt(x*x + y*y+0.0f);

		double tmparea=fabs(cvContourArea(contour, CV_WHOLE_SEQ)); 

		double length = cvArcLength(contour, CV_WHOLE_SEQ);  

//////////

		float a_tmp=tmparea/(PI*rad*rad);
		float l_tmp=length/(2*PI*rad);	
//		l_tmp=1;
		if ( dist < rad*0.3 && ( a_tmp > 0.7  &&  a_tmp < 1.4 ) 
				&& ( l_tmp > 0.5 && l_tmp < 2 ) )
		{
			area_rt = a_tmp;
			len_rt = length/(2*PI*rad);
	
//			printf(" area_rt = %f, len_rt = %f,  \n", area_rt, len_rt );
//			printf(" xc= %d, yc= %d, \n", x, y);

			CvBox2D box;
			box=cvFitEllipse2(contour);

			float re=Fit_ellipse_residual(contour, box);
			
//			printf("re=%f, count=%d, length=%f,  \n", re, count, length);
//			printf("re=%f, dist=%f, a_tmp=%f, l_tmp=%f, rad=%d, \n", re, dist, a_tmp, l_tmp, rad);

			if ( re > 0.11)
			{
				delete []pts;

//				printf("re=%f, dist=%f, a_tmp=%f, rad=%d, \n", re, dist, a_tmp, rad);

				delete []areas;
				delete []lens;
				delete []arean;
			
				delete []ang;
				delete []flg;

				cvReleaseImage( &imgbw);
				cvReleaseMemStorage(&storage);

				return -1;
			}

			cntflg=true;
			delete []pts;
			continue;

		}

/////////////

		if ( dist > rad*(23/11.0)*1.1 || dist < rad*0.5 )
        {
			delete []pts;
			continue;
        }
		
		float ra=tmparea/area30;
		if ( ra < 0.5 || ra > 7 )
        {
			delete []pts;
			continue;
        }

//		float rl=(length-6*2)/(len30*2);	 
		float rl=((length*11/rad-6*2)/2)/(PI*2*20/12);
//		printf("rl=%f, rl2=%f,\n", rl, rl2);

		if ( rl < 0.5 || rl > 8 )
        {
			delete []pts;
			continue;
        }

		float r12=ra/rl;

//		printf("r12=%f, \n", r12);
		if ( r12 < 0.45 || r12 > 2.5 )
		{
			delete []pts;
			continue;
        }

		float r12d=fabs(ra-rl);
		if ( r12d > 1.4 )    
		{
			delete []pts;
			continue;
        }

//		printf("r12=%f, r1d2=%f, \n", r12, r12d);

///////////////

		int t, tx,ty;
		int min=10000, max=0;
		float sum=0;
		for (i=0; i< count; i++)
		{
	
			tx=pts[i].x-w/2;
			ty=pts[i].y-h/2;
			
			t=sqrt(tx*tx + ty*ty +0.0f);
			sum+=t;

			if ( t > max)
				max=t;

			if (t< min )
				min=t;

		}

		float md=sum/count;

		if ( md < rad*(16/11.0) || md > rad*(24.0/11.0) )
		{
			delete []pts;
			continue;
		}

		if (min < rad*(11.5/11.0))
		{
			delete []pts;
			continue;
		}

		if (max > rad*(27.5/11.0) )
		{
			delete []pts;
			continue;
		}

//////////////

		float xy[2];
		CodeCenter( xy, pts, count, imgbw, flg);
//		printf(" xf= %f, yf = %f, count=%d, \n", xy[0], xy[1], count);
//		printf("area=%f, len=%f, \n", tmparea, length);

///////
		areas[k]=tmparea;
		lens[k]=length;

		CvPoint cent;		
//		cent.x=x;
//		cent.y=y;

		if ( xy[0] >0)
			cent.x=xy[0]+0.5;
		else
			cent.x=xy[0]-0.5;

		if ( xy[1] >0)
			cent.y=xy[1]+0.5;
		else
			cent.y=xy[1]-0.5;


		ang[k]=Angle(cent);
//		printf(" x= %d, y=%d, ang n=%d, \n", x, y, ang[k]);

		k++;

//		printf("  code belt pixel number = %d, \n", count);

		delete []pts;

	}

	cvReleaseImage( &imgbw);

//	printf(" decode n=%d, \n", k);
/////////////////////////////////////////////////////////

	if ( k==0 || k > 11 || !cntflg )
	{
		delete []areas;
		delete []lens;
		delete []arean;
			
		delete []ang;
		delete []flg;
		cvReleaseMemStorage(&storage);

		return -1;
	}

	float *arf=new float[k];

//	area_rt=1;
//	len_rt=1;

	for (i=0; i< k; i++)
	{
		float ar=(areas[i])/(area30*area_rt);
		arf[i]=ar;

//		float lr=((lens[i]*8/rad-6*2)/2)/(len_rt*PI*2*17/12);
//		float lr=((lens[i]*9/rad-6*2)/2)/(len_rt*PI*2*18/12);
		float lr=((lens[i]*11/rad-6*2)/2)/(len_rt*PI*2*20/12);

		int ari=(int )(ar*100)%100;
		int lri=(int )(lr*100)%100;

//		printf(" ar= %f, lr= %f, \n", ar, lr);

		if ( ari < 35 || ari > 65 )
        {
			arean[i]=ar+0.45;
 
        }
		else if ( lri < 40 || lri > 60 )
		{
			arean[i]=lr+0.45;
		}
		else
			arean[i]=(ar+lr)/2+0.45;

		if ( arean[i] > 11)
		{
			delete []areas;
			delete []lens;
			delete []arean;
				
			delete []ang;
			delete []flg;
			cvReleaseMemStorage(&storage);
			
			return -1;
		}

//		arean[i]=ar+0.45;
//		arean[i]=lr+0.45;

	}	

////////////////////////

	int  j, ind=0;
	int code[12];

	for (i=0; i< 12; i++)
		code[i]=0;

	int *ang2=new int[k];
	for (i=0; i< k; i++)
		ang2[i]=ang[i];

	BubbleSort(ang2, k);
//	printf(" ang =%d, \n", ang2[0]);

	int n, ang_pre, a_pre;
//	float a_pref;
	for (i=0; i< k; i++)
	{
		for (j=0; j< k; j++)
			if ( ang[j] == ang2[i] )
				break;
		
		if (i > 0 )
		{
			int t=ang[j]- ang_pre;
			t=t/30.0-(arean[j]+a_pre)/2.0+0.5;

//			t=t/30.0-(arf[j]+a_pref)/2.0+0.45;

			for (n=0; n< t; n++)
			{
				code[ind]=0;
				ind++;
			}
		}
		
		for (n=0; n< arean[j]; n++)
		{
			code[ind]=1;
			ind++;
		}

		ang_pre=ang[j];
		a_pre=arean[j];
//		a_pref=arf[j];

	}

	int codef=0;
	for (i=0; i< 12; i++)
		codef+=code[i]*pow(2+0.0f, 11-i);

//	printf(" code_pre =%d, \n", codef);

	codef=LeastCode(codef, 12);

//	printf(" code  = %d, \n", codef);

	delete []areas;
	delete []lens;
	delete []arean;
	delete []arf;

	delete []ang;
	delete []ang2;

	delete []flg;
	
	cvReleaseMemStorage(&storage);


//	cvReleaseImage( &imgbw);

	return codef;

}


//����Ƕ�
int CodedTarget::Angle(CvPoint pt)
{
	float x=pt.x+0.01;
	float y=pt.y+0.01;

	float a=atan( fabs(y/x));

	int a2=a*180/PI;

	if ( x < 0 && y > 0)
		return 180-a2;

	else if ( x < 0 && y < 0)
		return 180+a2;

	else if ( x > 0 && y < 0)
		return 360-a2;
	else
		return   a2;

}


//ð�ݷ�����
void CodedTarget::BubbleSort(int *a, int n)
{
	int i, j, exchange;
	int tmp;

	for ( i=0; i < n-1; i++)
	{
		exchange=0;

		for ( j=n-1; j> i; j--)
			if ( a[j] < a[j-1] )
			{
				tmp=a[j];
				a[j]=a[j-1];
				a[j-1]=tmp;

				exchange=1;

			}

		if (exchange==0)
			return;
	}


}


//ѭ����������С��ֵ
int CodedTarget::LeastCode(int c, int len)
{
	int i, t2=pow(2+0.0f,len)-1;

	int min=c,t;
	for (i=1; i< len; i++)
	{
		t=(c >> i) | (c << (len-i));
		t=t&t2;

		if (t < min )
			min=t;

	}

	//h
	if (min <12)
	{
		return 0;
	}
	if (min>600 && min !=1783 )
	{
		return 0;
	}

	if ( min>=15 && min<=265)
	{
		
		min = min/2+3+ 4*((min-2)/12-1)  ;
	}
	else
		min = min / 2 + 1000;


	return min;

}


//���б�־�����Ͷ�λ
void CodedTarget::DecodeAll()
{
	int k;

	CvBox2D box;
	
	vector <CvBox2D>::iterator it;
	it = m_boxv.begin( );

	int i=0;
	
	for (k=1; k<= m_circleNum; k++)
	{
		
//////////////
//		Rectify_ellipse3(k);
		Rectifying(k);

		int t=0;
//		if (m_thresh == 2)
			t=Target_gray(m_imgt, m_rad);

		int cd=Decode(m_imgt, m_rad, t);

		box=*(it+k-1);

		if ( cd >1 && cd < 2047)
		{
						
			if ( i< m_targetNum_set-1)
			{
//				int i=k-1;
								
				m_target[i].code=cd;
				m_target[i].sn=i+1;

				m_target[i].re=k-1;


				if ( m_refine_corner)
				{
					CvPoint2D32f ptf;
					Refine_corner(m_img_orig, box, &ptf);

					m_target[i].x=ptf.x;
					m_target[i].y=ptf.y;
				}
				else
				{
					m_target[i].x=box.center.x;
					m_target[i].y=box.center.y;
				}

				i++;
 							
			}


		}
/////
		
	}

	m_targetNum=i;

}

 
////��Բ�任Ϊ��׼Բ2, ˫���Բ�ֵ
void CodedTarget::Rectify_ellipse2(int sn)
{

	vector <CvBox2D>::iterator it;
	it=m_boxv.begin( )+sn-1;

	CvBox2D box=*it;

	float     xc = box.center.x;
    float	  yc = box.center.y;

    float	a = box.size.width*0.5;
    float   b = box.size.height*0.5;

	if (m_imgt_flg)
		cvReleaseImage( &m_imgt);

//	int rad2=sqrt(a*b);
//	printf("rad = %d, \n", rad2);

	int rad=sqrt(a*b)*5.3;

	rad=rad-rad%4;
	m_imgt=cvCreateImage( cvSize(rad, rad), IPL_DEPTH_8U, 1 );

	m_imgt_flg=true;

/////////////

	m_rad=sqrt(a*b);

	float ang = box.angle*PI/180;  
	float cosa=cos(ang);
	float sina=sin(ang);

	float r00, r01, r10, r11;

	float r1=sqrt(a/b);
	float r2=sqrt(b/a);

//*
	r00=cosa*r1;
	r01=-sina*r2;
	r10=sina*r1;
	r11=cosa*r2;

//////
	CvSize sz=cvGetSize(m_imgt);

	int h=sz.height;
	int w=sz.width;

	CvSize sz2=cvGetSize(m_img_orig);

	int h2=sz2.height;
	int w2=sz2.width;

	BYTE *pb=(BYTE*)m_imgt->imageData;	
	BYTE *pb2=(BYTE*)m_img_orig->imageData;
	
	int x, y;
	int x2, y2;
	float xf, yf;
	float xp, yp;
	int f00, f01, f10, f11;

	float xt,xt2, yt;
	int i, j;
	for (j=0; j< h; j++)
		for ( i=0; i< w; i++)
		{
			x=i-w/2;
			y=j-h/2;

			xf=x*r00+y*r01+xc;
			yf=x*r10+y*r11+yc;

			x2=xf;
			y2=yf;

			if ( y2 < 0 || y2 > h2-2 || x2 < 0 || x2 > w2-2 )
				pb[j*w+i]=0;
			else
			{
				xp=xf-x2;
				yp=yf-y2;

				f00=pb2[y2*w2+x2];
				f01=pb2[y2*w2+x2+1];
				f10=pb2[(y2+1)*w2+x2];
				f11=pb2[(y2+1)*w2+x2+1];

				xt=f00*(1-xp)+f01*xp;
				xt2=f10*(1-xp)+f11*xp;

				yt=xt*(1-yp)+xt2*yp;

				pb[j*w+i]=yt+0.5;

			}

		}


}


//���������
void CodedTarget::CodeCenter(float *xy, CvPoint *pts, int count, IplImage *img, bool *flg)
{
	CvSize sz=cvGetSize(img);
	
	int h=sz.height;
	int w=sz.width;
		
	CvPoint ptc;
	int x,y;
	int n, num=0;
	int sumx=0, sumy=0;

	BYTE *pb=(BYTE*)img->imageData;	

	for (int i=0; i< count; i++)
	{
		ptc=pts[i];
		
		int j, k;
		for (j=-2; j<=2; j++)
			for (k=-2; k<=2; k++)
			{
				x=ptc.x+k;
				y=ptc.y+j;
								
				if ( x >=0 && x<w && y >=0 && y<h )
				{
					n=y*w+x;
					
					if ( pb[n] >0 && flg[n] )
					{
						sumx+=x;
						sumy+=y;

						flg[n]=false;
						num++;

					}

				}

			}
						
	}


	if (num > 0)
	{
		xy[0]=(float )sumx/num-w/2;
		xy[1]=(float )sumy/num-h/2;

	}


}

////����������
// img ����ͼ�� 
void CodedTarget::Decoding(IplImage *img)
{
	m_targetNum=0;

	CvSize sz=cvGetSize(img);

	if ( sz.width != m_w || sz.height != m_h )
	{
		//AfxMessageBox("�����ͼ��� width �� height �� Initial()�����ò���!");
		return;

	}

	m_img_orig=img;

	Binary(m_th);

	CenterCircle(m_img_bw);

	DecodeAll();

//	Draw_circle();
//	Draw_target();

}

 
////��Բ�任Ϊ��׼Բ3, ˫���Բ�ֵ, �ֱ�����չ
void CodedTarget::Rectify_ellipse3(int sn)
{
	vector <CvBox2D>::iterator it;
	it=m_boxv.begin( )+sn-1;

	CvBox2D box=*it;

	float     xc = box.center.x;
    float	  yc = box.center.y;

    float	a = box.size.width*0.5;
    float   b = box.size.height*0.5;

	if (m_imgt_flg)
		cvReleaseImage( &m_imgt);

	int rad=sqrt(a*b)*5.3*2;
	rad=rad-rad%4;

	m_imgt=cvCreateImage( cvSize(rad, rad), IPL_DEPTH_8U, 1 );

	m_imgt_flg=true;

/////////////

	m_rad=sqrt(a*b)*2;

	float ang = box.angle*PI/180+0;  ///???
	float cosa=cos(ang);
	float sina=sin(ang);

	float r00, r01, r10, r11;

	float r1=sqrt(a/b);
	float r2=sqrt(b/a);

//*
	r00=cosa*r1;
	r01=-sina*r2;
	r10=sina*r1;
	r11=cosa*r2;

//////
	CvSize sz=cvGetSize(m_imgt);

	int h=sz.height;
	int w=sz.width;

	CvSize sz2=cvGetSize(m_img_orig);

	int h2=sz2.height;
	int w2=sz2.width;

	BYTE *pb=(BYTE*)m_imgt->imageData;	
	BYTE *pb2=(BYTE*)m_img_orig->imageData;
	
	float x, y;
	int x2, y2;
	float xf, yf;
	float xp, yp;
	int f00, f01, f10, f11;
//	int xc2=xc-w/2, yc2=yc-h/2;

	float xt,xt2, yt;
	int i, j;
	for (j=0; j< h; j++)
		for ( i=0; i< w; i++)
		{
			x=i-w/2;
			y=j-h/2;

			x=x/2;
			y=y/2;

			xf=x*r00+y*r01+xc;
			yf=x*r10+y*r11+yc;

			x2=xf;
			y2=yf;

			if ( y2 < 0 || y2 > h2-2 || x2 < 0 || x2 > w2-2 )
				pb[j*w+i]=0;
			else
			{
				xp=xf-x2;
				yp=yf-y2;

				f00=pb2[y2*w2+x2];
				f01=pb2[y2*w2+x2+1];
				f10=pb2[(y2+1)*w2+x2];
				f11=pb2[(y2+1)*w2+x2+1];

				xt=f00*(1-xp)+f01*xp;
				xt2=f10*(1-xp)+f11*xp;

				yt=xt*(1-yp)+xt2*yp;

				pb[j*w+i]=yt+0.5;

			}

		}


}

//ѡ����Բ�任����
void CodedTarget::Rectifying(int sn)
{

	vector <CvBox2D>::iterator it;
	it=m_boxv.begin( )+sn-1;

	CvBox2D box=*it;

    float	a = box.size.width*0.5;
    float   b = box.size.height*0.5;

	int dia=sqrt(a*b)*2;
//	printf("dia=%d, \n", dia);

	if (dia < 25)
		Rectify_ellipse3(sn);	//�ֱ�����չ
	else if (dia < 40)
		Rectify_ellipse2(sn);	//˫���Բ�ֵ
	else if (dia < 70)
		Rectify_ellipse(sn);	//����ڲ�ֵ
	else
		Rectify_ellipse4(sn);	//�ֱ���ѹ��


}

//��Ŀ��
void CodedTarget::Draw_target()
{
	if (!m_init)
		return;
	
	BYTE *pb=(BYTE*)m_img_orig->imageData;	
	BYTE *pbc=(BYTE*)m_imgc->imageData;	

	int i, j;
	int t;
	for( j=0;j<m_h;j++)
	{
		for(i=0;i<m_w;i=i+1)
		{
			t=pb[j*m_w+i];

			pbc[i*3+j*m_w*3]=t;
  			pbc[i*3+1+j*m_w*3]=t;
			pbc[i*3+2+j*m_w*3]=t;

		}
	}

//	cvNamedWindow( "Components", 1 );	
//	cvShowImage( "Components", m_imgc );

///////
	
	vector <CvBox2D>::iterator it;
	it = m_boxv.begin( );

	CvBox2D box;
	CvPoint center;
	CvSize size;

	CvFont font;

	if ( m_dia < 41 )
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.7, 0.7, 0, 2);
	else
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 2);


	CString cs;
	int indx;

	for (int k=0; k< m_targetNum; k++)
	{
		indx=m_target[k].re;
		
		box=*(it+indx);

		center.x = cvRound(box.center.x);
		center.y = cvRound(box.center.y);
		size.width = cvRound(box.size.width*0.5);
		size.height = cvRound(box.size.height*0.5);
		box.angle = -box.angle;
		
		//����Բ
		cvEllipse(m_imgc,center,size,box.angle,0,360,CV_RGB(255,0,255),1,CV_AA,0);

/////
		cs.Format("%d ", m_target[k].sn);
		
		cvPutText(m_imgc, cs, center, &font, CV_RGB(0, 255, 0));


		cs.Format("/%d", m_target[k].code );
		center.x = center.x+m_rad;
		
		cvPutText(m_imgc, cs, center, &font, CV_RGB(255, 0, 0 ));

/////////������ʮ��

		center.x=m_target[k].x+0.5;
		center.y=m_target[k].y+0.5;

//		cvCircle(m_imgc, center,2, CV_RGB(255, 0, 0),2,8, 0);

		CvPoint  cp1, cp2, cp3, cp4;

		int s=7;

		cp1.x=center.x-s;
		cp1.y=center.y;

		cp2.x=center.x+s;
		cp2.y=center.y;

		cp3.x=center.x;
		cp3.y=center.y-s;

		cp4.x=center.x;
		cp4.y=center.y+s;

		int c=255;

//		cvCircle(img, cpc, s, CV_RGB(c, 0 , 0));  
		
		cvLine( m_imgc, cp1, cp2, CV_RGB(c,0,0), 1,8-8 , 0 );	
		cvLine( m_imgc, cp3, cp4, CV_RGB(c,0,0), 1,8-8 , 0 );
	
	}

}

//������Բ
void CodedTarget::Draw_circle()
{
	if (!m_init)
		return;
	
	BYTE *pb=(BYTE*)m_img_orig->imageData;	
	BYTE *pbc=(BYTE*)m_imgc->imageData;	

	int i, j;
	int t;
	for( j=0;j<m_h;j++)
	{
		for(i=0;i<m_w;i=i+1)
		{
			t=pb[j*m_w+i];

			pbc[i*3+j*m_w*3]=t;
  			pbc[i*3+1+j*m_w*3]=t;
			pbc[i*3+2+j*m_w*3]=t;

		}
	}

///////
	
	vector <CvBox2D>::iterator it;
	it = m_boxv.begin( );

	CvBox2D box;
	CvPoint center;
	CvSize size;

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 2);

	CString cs;

	for (int k=0; k< m_circleNum; k++)
	{

		box=*(it+k);

		center.x = cvRound(box.center.x);
		center.y = cvRound(box.center.y);
		size.width = cvRound(box.size.width*0.5);
		size.height = cvRound(box.size.height*0.5);
		box.angle = -box.angle;
		
		//����Բ
		cvEllipse(m_imgc,center,size,box.angle,0,360,CV_RGB(255,0,255),1,CV_AA,0);

/////
		cvCircle(m_imgc, center,2, CV_RGB(255, 0, 0),2,8, 0);

///////
				
		cs.Format("%d ", k+1 );
		
		cvPutText(m_imgc, cs, center, &font, CV_RGB(0, 0, 255));

	}

}


//���öԽǱ�־��ϸ��λ
void CodedTarget::Refine_corner(IplImage *img, CvBox2D box, CvPoint2D32f *pt_out)
{
	CvPoint2D32f ptf, ptf2;
	ptf.x=box.center.x;
	ptf.y=box.center.y;
	
	////
	int a=(int)(box.size.width*0.5);
	int b=(int)(box.size.height*0.5);

/*
	float af=box.size.width;
	float bf=box.size.height;

	float d1, d2;
	if (af < bf)
	{
		d1=af;
		d2=bf;
	}
	else
	{
		d1=bf;
		d2=af;

	}
	
	float ang=acos(d1/d2);
	ang=ang*180/PI;

//*/	
	int min=(a < b )? a:b;
	min=min*0.6;
	
//	printf(" x=%f, y=%f, ab_min=%d, ang=%f, \n", ptf.x, ptf.y, min, ang);
	
	bool flg=false;
	
	if ( min >= 3 )
	{
		flg=true;
		
		int w1=2, w2=-1;
		
		if ( min <= 4 )
			w1=2;
		else if (min <= 6)
		{
			w1=3;
//			w2=1;
		}
		else if (min <= 8 )
		{
			w1=4;
			w2=1;
		}
		else if (min <= 10)
		{
			w1=5;
			w2=1;
		}
		else if (min <= 12)
		{
			w1=6;
			w2=2;
		}
		else if (min <= 14)
		{
			w1=7;
			w2=2;
			
		}
		else if (min <= 17)
		{
			w1=8;
			w2=2;
			
		}
		else if (min <= 25)
		{
			w1=9;
			w2=2;
			
		}
		else
		{
			w1=11;
			w2=2;

		}
		//				w1=5;  
		//				w2=1;
		
		ptf2=ptf;
		
		//ptf2.x+=1;
		//ptf2.y+=1;
		
		cvFindCornerSubPix(img, &ptf2, 1, cvSize(w1,w1),	cvSize(w2, w2),
			cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 20, 0.1) );
		
//		printf(" x2=%f, y2=%f, \n", ptf2.x, ptf2.y);

////////
		
		float dx=ptf2.x-ptf.x;
		float dy=ptf2.y-ptf.y;
		
		float dist=sqrt( dx*dx + dy*dy+0.0f); 
		
		int dia=sqrt(a*b+0.0f)*2;
		float rd=dist/dia;
//		printf("dia2=%d, \n", dia);
		
		if (  dist > 1.0  )
		{
			if ( dia < 100 )
			{
				if ( rd > 0.015)
					flg=false;
			}
			else if ( dia < 200 )
			{
				if ( dist > 1.5  )
					flg=false;
				
			}
			else
			{
				if ( dist > 2.0  )
					flg=false;
				
			}
			
//			printf(" dist=%f, rd=%f, \n", dist, rd);
			
		}		
				
	}
	
	if (flg)
	{
		*pt_out=ptf2;

	}
	else
	{
		*pt_out=ptf;
		
	}
	
	
}

//ȫ����ֵ��ȡ����
// ���룺img ͼ�� �����dia ����Բֱ��, gray �Ҷ���ֵ, num Ŀ����
void CodedTarget::Get_dia_gray(IplImage *img, int *dia, int *gray, int *num)
{
	*num=0;
	
	CvSize sz=cvGetSize(img);
	int h=sz.height;
	int w=sz.width;

	int hw=(h< w)? h:w;
	
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	
	cvFindContours( img, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE); 	
	//CV_RETR_EXTERNAL  CV_CHAIN_APPROX_SIMPLE

	CvBox2D box;

	float	lenmin=7*PI;
	float	lenmax=(hw/3.5)*PI;

	float	areamin=3.5*3.5*PI;
	float	areamax=(hw/7.0)*(hw/7.0)*3*PI;
		
///////
	m_boxv.clear( );
	
	int k=0;
	for( ; contour != 0; contour = contour->h_next )
    {
		
		int count= contour->total; //��������

		if ( count < 15 )
			continue;
		
		double length = cvArcLength(contour);  
		double tmparea=fabs(cvContourArea(contour, CV_WHOLE_SEQ)); 
		
		if ( length< lenmin || count > lenmax )
        {
			continue;
        }

		if ( tmparea < areamin || tmparea > areamax)
        {
			continue;
        }

		float c=length*length/(4*PI*tmparea);
		if ( c > 1.9)
		{
			continue;
		}

		float lc=length/count;
		if ( lc > 3.5)
			continue;

//		printf(" count=%d, length = %f, tmparea=%f,  c= %f,\n ", count,length,  tmparea, c);
//		printf(" a= %f, b= %f, angle =%f, \n", box.size.width, box.size.height, box.angle);
								
		box=cvFitEllipse2(contour);
//		box=cvFitEllipse2(PointArray);

		float	a = box.size.width*0.5;
		float   b = box.size.height*0.5;
	
		int rad=sqrt(a*b)*3;
		int cx=box.center.x;
		int cy=box.center.y;

		if ( cx < rad || (m_w-cx) < rad || cy < rad || (m_h-cy) < rad )
			continue;

		float re=Fit_ellipse_residual(contour, box);
	
		float re_max=0.08;
		int dia=sqrt(a*b)*2;
		
		if ( dia < 15)
			re_max=0.11;
		else if ( dia < 20)
			re_max=0.10;
		else if ( dia < 35)
			re_max=0.09;

		if ( re > re_max)
		{
			continue;
		}

		k++;
		m_boxv.push_back( box );

//		printf( "re = %f, c= %f, lc=%f, \n", re, c, lc);
//		printf(" count=%d, length = %f, tmparea=%f,  \n ", count,length,  tmparea);
//		printf(" a= %f, b= %f, angle =%f, \n", box.size.width, box.size.height, box.angle);
								
	}

	cvReleaseMemStorage(&storage);

//	printf("target number_pre  = %d, \n", k);

	int length=k;

	m_circleNum=m_boxv.size( );

	if (m_circleNum <=0 )
		return ;

////////
	
	vector <CvBox2D>::iterator it;
	it = m_boxv.begin( );

	int i=0;
//	int min=10000, max=0;
	int gmin=1000, gmax=0;

	float a, b;
	int diat, sum=0;
	int t, sumt=0;
	
	for (k=1; k<= m_circleNum; k++)
	{
		
//////////////

		Rectifying(k);

		t=Target_gray(m_imgt, m_rad);

		int cd=Decode(m_imgt, m_rad, t);

		box=*(it+k-1);

		if ( cd >1 && cd < 2047)
		{
			a = box.size.width*0.5;
			b = box.size.height*0.5;

			diat=sqrt(a*b)*2;
//			printf("dia=%d, cd=%d, th=%d, \n", diat, cd, t);

			sum+=diat;

//			if (diat > max)
//				max=diat;

//			if (diat < min )
//				min=diat;

			if (t > gmax)
				gmax=t;

			if (t < gmin )
				gmin=t;

			sumt+=t;

			i++;
										
		}
/////
		
	}

	if (i < 1)
		return ;

	*dia=sum/i;

//	m_th=sumt/i;

	gray[0]=sumt/i;
	gray[1]=gmin;
	gray[2]=gmax;

	*num=i;

//	printf("n=%d, dia mean=%d, min=%d, max=%d, \n", i, *dia, min, max);

	m_targetNum=i;

	if ( *dia < 5 || *dia > hw/3 )
		*num=0;


}

//��ȡ����Ӧ�����������Ҷ���ֵ������Բ��׼ֱ���Ͷ�ֵ��ģʽ
//img ����ͼ��, mode ģʽ	��0��ȫ�Զ�ģʽ��Ĭ��ֵ��
//							��1����ͨģʽ���ٶȿ죻
//							��2�������ղ�����ģʽ��
void CodedTarget::Get_adaptive_parameter(IplImage *img, int mode)
{
	if (!m_init)
	{
		//AfxMessageBox(" ����ִ��Initial(), ��ʼ��!");
		return;

	}
	
	CvSize sz=cvGetSize(img);

	if ( sz.width != m_w || sz.height != m_h )
	{
		//AfxMessageBox("�����ͼ��� width �� height �� Initial()�����ò���!");
		return;

	}
	
	if (mode !=0 && mode !=1 && mode !=2 )
		mode=0;

	m_dia=0;
	m_th=0;
//////////////////////////1
	
	m_img_orig=img;

	m_thresh=1;
	Binary(0);

	int dia, gray[3], num;
	Get_dia_gray(m_img_bw, &dia, gray, &num);   //ȫ����ֵ��ȡ�Ҷ���ֵ������Բ��׼ֱ��
	
//	printf("num1=%d, dia=%d, gray 0 =%d, 1=%d, 2=%d,  \n", num, dia, gray[0], gray[1], gray[2]);
	
	if (num < 1)
	{
//		return;	

//*
		m_thresh=2;
		m_dia=20;

		Binary(0);

		Get_dia_gray(m_img_bw, &dia, gray, &num);   //ȫ����ֵ��ȡ�Ҷ���ֵ������Բ��׼ֱ��

		if (num < 1)
			return;	
//	*/
		
	}

	m_dia=dia;
	m_th=gray[0];

	if ( (num == m_targetNum_set || mode ==1) && ( mode !=2 ) )
	{
		m_thresh=0;

		return;
	}

//////////////////2

//	m_dia=dia;

	m_thresh=2;
	Binary(0);

	int dia2=30, gray2[3], num2;
	Get_dia_gray_local(m_img_bw, &dia2, gray2, &num2);  //�ֲ�����ֵ��ȡ�Ҷ���ֵ������Բ��׼ֱ��

//	printf("num2=%d, dia=%d, gray 0 =%d, 1=%d, 2=%d,  \n", num2, dia2, gray2[0], gray2[1], gray2[2]);

	m_dia=dia2;
	m_th=gray2[0];

	if (  num2 == num )
	{
		m_thresh=0;

		return;

	}

//////////////////3

	m_thresh=0;
	Binary(m_th);	
	
	CenterCircle(m_img_bw);
/////////
	num=0;

	m_circleNum=m_boxv.size( );

	vector <CvBox2D>::iterator it;
	it = m_boxv.begin( );

	int i=0;
	int gmin=1000, gmax=0;

	float a, b;
	int diat, sum=0;
	int t, sumt=0;

	CvBox2D box;

	for (int k=1; k<= m_circleNum; k++)
	{
		
//////////////

		Rectifying(k);

		t=Target_gray(m_imgt, m_rad);

		int cd=Decode(m_imgt, m_rad, t);

		box=*(it+k-1);

		if ( cd >1 && cd < 2047)
		{
			a = box.size.width*0.5;
			b = box.size.height*0.5;

			diat=sqrt(a*b)*2;
//			printf("dia=%d, cd=%d, th=%d, \n", diat, cd, t);

			sum+=diat;

			if (t > gmax)
				gmax=t;

			if (t < gmin )
				gmin=t;

			sumt+=t;

			i++;
										
		}
		
	}

	if (i >= 1 )
	{
		dia=sum/i;

		gray[0]=sumt/i;
		gray[1]=gmin;
		gray[2]=gmax;

		num=i;
	}

////////
//	Get_dia_gray(m_img_bw, &dia, gray, &num);   //ȫ����ֵ��ȡ�Ҷ���ֵ������Բ��׼ֱ��

//	printf("num3=%d, dia=%d, gray 0 =%d, 1=%d, 2=%d,  \n", num, dia, gray[0], gray[1], gray[2]);

	if (  num2 > num && abs(gray2[2]-gray2[1]) > 10 ) 
	{
		m_thresh=2;

		return;

	}

	if (  num2 == num && abs(gray2[2]-gray2[1]) > 60 ) 
	{
		m_thresh=2;
		return;

	}

	if (mode == 0)
	{
		m_thresh=0;

	}
	else if (mode == 2)
	{
		m_thresh=2;

	}

//	printf("thresh =%d, dia=%d, gray=%d, \n", m_thresh, m_dia, m_th);

}


//�󵥸�Ŀ��Ҷ���ֵ
int CodedTarget::Target_gray(IplImage *img, int rad)
{
	CvSize sz=cvGetSize(img);
	
	int h=sz.height;
	int w=sz.width;
	
	BYTE *pb=(BYTE*)img->imageData;	

////
	int j,i;
	int h2=h/2, w2=w/2;
	int n2=1, sum2=0;
	int n3=1, sum3=0;

	for (j=0; j<h; j+=3)
		for ( i=0; i<w; i+=3)
		{
			sum2+=pb[j*w+i];
			n2++;
		}

	int mean=sum2/n2;

	n2=1;
	sum2=0;

	int t;
	
	for (j=0; j<h; j+=2)
		for ( i=0; i<w; i+=2)
		{								
			t=pb[j*w+i];

			if ( ( abs(i-w2)+abs(j-h2) ) < rad )
			{
				if ( t > mean )
				{
					sum2+=t;
					n2++;
				}
			}
			else
			{
				if ( t < mean)
				{
					sum3+=t;
					n3++;
				}
				
			}
			
		}
		
	int m2=sum2/n2;
	int m3=sum3/n3;

//	int th2=(m2+m3)/2;
	int th2=m2*0.55+m3*0.45;

//	printf("m2=%d, m3=%d, th2=%d,  \n", m2, m3, th2);

	return th2;

}

//�ֲ���ֵ��ȡ����
// ���룺img ͼ��  // ����� dia ����Բֱ���� gray �Ҷ���ֵ, num Ŀ������
void CodedTarget::Get_dia_gray_local(IplImage *img, int *dia, int *gray, int *num)
{
	*num=0;
	
	CenterCircle(img);

	if (m_circleNum < 1 )
		return ;

////////

	CvSize sz=cvGetSize(img);
	int h=sz.height;
	int w=sz.width;

	int hw=(h< w)? h:w;

/////
	
	vector <CvBox2D>::iterator it;
	it = m_boxv.begin( );

	CvBox2D box;

	int i=0;
	int gmin=1000, gmax=0;

	float a, b;
	int diat, sum=0;
	int t, sumt=0;
	
	for (int k=1; k<= m_circleNum; k++)
	{
		
//////////////

		Rectifying(k);

		t=Target_gray(m_imgt, m_rad);

		int cd=Decode(m_imgt, m_rad,t);

		box=*(it+k-1);

		if ( cd >1 && cd < 2047)
		{
			a = box.size.width*0.5;
			b = box.size.height*0.5;

			diat=sqrt(a*b)*2;
//			printf("dia=%d, cd=%d, th=%d, \n", diat, cd, t);

			sum+=diat;


			if (t > gmax)
				gmax=t;

			if (t < gmin )
				gmin=t;

			sumt+=t;

			i++;
										
		}
/////
		
	}

	if (i < 1)
		return ;

	*dia=sum/i;

	gray[0]=sumt/i;
	gray[1]=gmin;
	gray[2]=gmax;

	*num=i;

//	printf("n=%d, dia mean=%d, min=%d, max=%d, \n", i, *dia, min, max);

	m_targetNum=i;

	if ( *dia < 5 || *dia > hw/3 )
		*num=0;
	 

}


////��Բ�任Ϊ��׼Բ4, ����ڲ�ֵ, �ֱ���ѹ��
void CodedTarget::Rectify_ellipse4(int sn)
{
	vector <CvBox2D>::iterator it;
	it=m_boxv.begin( )+sn-1;

	CvBox2D box=*it;

	float     xc = box.center.x;
    float	  yc = box.center.y;

    float	a = box.size.width*0.5;
    float   b = box.size.height*0.5;

	if (m_imgt_flg)
		cvReleaseImage( &m_imgt);

	int rad=sqrt(a*b)*5.3;  //5.7

	rad=rad-rad%4+4;
	int rad2=160;				//�̶�ֱ��

	m_imgt=cvCreateImage( cvSize(rad2, rad2), IPL_DEPTH_8U, 1 );

	m_imgt_flg=true;

/////////////

	float rt=(float )rad/rad2;
//	printf("rt=%f,\n", rt);

	m_rad=sqrt(a*b)/rt+0.5;
//	printf("rt=%f,m_rad=%d, \n", rt, m_rad);

	float ang = box.angle*PI/180+0;  ///???
	float cosa=cos(ang);
	float sina=sin(ang);

	float r00, r01, r10, r11;

	float r1=sqrt(a/b);
	float r2=sqrt(b/a);

//*
	r00=cosa*r1;
	r01=-sina*r2;
	r10=sina*r1;
	r11=cosa*r2;

//////
	CvSize sz=cvGetSize(m_imgt);

	int h=sz.height;
	int w=sz.width;

	CvSize sz2=cvGetSize(m_img_orig);

	int h2=sz2.height;
	int w2=sz2.width;

	BYTE *pb=(BYTE*)m_imgt->imageData;	
	BYTE *pb2=(BYTE*)m_img_orig->imageData;
	
	float x, y;
	int x2, y2;

	int i, j;
	for (j=0; j< h; j++)
		for ( i=0; i< w; i++)
		{
			x=i-w/2;
			y=j-h/2;

			x=x*rt;
			y=y*rt;

			x2=x*r00+y*r01+xc+0.5;
			y2=x*r10+y*r11+yc+0.5;
		
//*
			if ( y2>=0 && y2 < h2 && x2>=0 && x2 < w2 )
				pb[j*w+i]=pb2[y2*w2+x2];
			else
				pb[j*w+i]=0;

		}


}

//һ������, ����ʼ���Ͳ�����ȡ��
//img ����ͼ��,  mode ���ģʽ, �μ�Get_adaptive_parameter(), arget_num_set �����Ŀ����,
void CodedTarget::Decoding_oneStep(IplImage *img,  int mode, int target_num_set)
{
	CvSize sz=cvGetSize(img);

	int h=sz.height;
	int w=sz.width;

	if ( sz.width != m_w || sz.height != m_h )
	{
		//AfxMessageBox("�����ͼ��� width �� height �� Initial()�����ò���!");
		return;

	}

	Initial(w, h, target_num_set);

	Get_adaptive_parameter(img, mode);

	Decoding(img);					//����

//	Draw_circle();
//	Draw_target();

}

////����������2, Ŀ��ߴ磨����Բֱ����������
void CodedTarget::Decoding_unlimitedSize(IplImage *img)
{
	m_targetNum=0;

	CvSize sz=cvGetSize(img);

	if ( sz.width != m_w || sz.height != m_h )
	{
		//AfxMessageBox("�����ͼ��� width �� height �� Initial()�����ò���!");
		return;

	}

	m_img_orig=img;

	Binary(m_th);

	CenterCircle_unlimitedSize(m_img_bw);

	DecodeAll();

//	Draw_circle();
//	Draw_target();

}

////������Բ2, Ŀ��ߴ磨����Բֱ����������
void CodedTarget::CenterCircle_unlimitedSize(IplImage *img)
{
	CvSize sz=cvGetSize(img);
	int h=sz.height;
	int w=sz.width;

	int hw=(h< w)? h:w;
	
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* contour = 0;
	
	cvFindContours( img, storage, &contour, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE); 	
	//CV_RETR_EXTERNAL  CV_CHAIN_APPROX_SIMPLE

	CvBox2D box;

	float	lenmin=8*PI;
	float	lenmax=(hw/3.5)*PI;

	float	areamin=4*4*PI;
	float	areamax=(hw/7.0)*(hw/7.0)*3*PI;
		
///////
	m_boxv.clear( );
	
	int k=0;
	for( ; contour != 0; contour = contour->h_next )
    {
		
		int count= contour->total; //��������

		if ( count < 15 )
			continue;
		
		double length = cvArcLength(contour);  
		double tmparea=fabs(cvContourArea(contour, CV_WHOLE_SEQ)); 
		
		if ( length< lenmin || count > lenmax )
        {
			continue;
        }

		if ( tmparea < areamin || tmparea > areamax)
        {
			continue;
        }

		float c=length*length/(4*PI*tmparea);
		if ( c > 1.9)
		{
			continue;
		}


		float lc=length/count;
		if ( lc > 3.5)
			continue;

//		printf(" count=%d, length = %f, tmparea=%f,  c= %f,\n ", count,length,  tmparea, c);
//		printf(" a= %f, b= %f, angle =%f, \n", box.size.width, box.size.height, box.angle);
								
		box=cvFitEllipse2(contour);

		float	a = box.size.width*0.5;
		float   b = box.size.height*0.5;
	
		int rad=sqrt(a*b)*3;
		int cx=box.center.x;
		int cy=box.center.y;

		if ( cx < rad || (m_w-cx) < rad || cy < rad || (m_h-cy) < rad )
			continue;

		float re=Fit_ellipse_residual(contour, box);

		float re_max=0.08;
		int dia=sqrt(a*b)*2;
		
		if ( dia < 15)
			re_max=0.11;
		else if ( dia < 20)
			re_max=0.10;
		else if ( dia < 35)
			re_max=0.09;

		if ( re > re_max)
		{
			continue;
		}

		k++;
		m_boxv.push_back( box );

//		printf( "re = %f, c= %f, lc=%f, \n", re, c, lc);
//		printf(" count=%d, length = %f, tmparea=%f,  \n ", count,length,  tmparea);
//		printf(" a= %f, b= %f, angle =%f, \n", box.size.width, box.size.height, box.angle);
								
	}

	cvReleaseMemStorage(&storage);

//	printf("target number_pre  = %d, \n", k);

	m_circleNum=m_boxv.size( );

}


//��Ŀ��
void CodedTarget::Draw_mark()
{
	if (!m_init)
		return;	 

	cvCvtColor(m_img_orig,m_imgc,CV_GRAY2RGB );	///////

	vector <CvBox2D>::iterator it;
	it = m_boxv.begin( );

	CvBox2D box;
	CvPoint center;
	CvSize size;
	CvFont font;
//	int pn=m_w*m_h;


	int w=  m_imgc->width ;

	double scale;
	int thick;

	if (w<2208)
	{
		scale=1.1+ m_dia/48.0+ w/4000.0  ;
		scale = scale*0.7 - 0.071;
		thick=2.5+ m_dia/96.0+ w/4000.0   ;
		thick = thick*0.8;
	}
	else
	{
		scale=1.7 + m_dia/128.0+ w/6000.0;
		thick=3.5+ m_dia/128.0+ w/4000.0;
	}


	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, scale, scale, 0, thick);

// 	if ( m_dia < 48 )
// 		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX,3, 3, 0, 4);
// 	else if(m_dia > 128)
// 		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, m_dia/128.0+2.1, m_dia/96.0+2.5, 0, m_dia/128+2);
// 	else
// 		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, m_dia/96.0+2.8, m_dia/96.0+2.3, 0, 2.3+m_dia/64.0);


//	DEBUG_INFO1("m_dia:  %d", m_dia);

	CString cs;
	int indx;

	for (int k=0; k< m_targetNum; k++)
	{
		indx=m_target[k].re;

		box=*(it+indx);

		center.x = cvRound(box.center.x)-9;
		center.y = cvRound(box.center.y)-5;
		size.width = cvRound(box.size.width*0.7);
		size.height = cvRound(box.size.height*0.7);
		//box.angle = -box.angle; /////h

		//����Բ
		//cvEllipse(m_imgc,center,size,box.angle,0,360,CV_RGB(20,238,5),scale-0.3  ,CV_AA,0);

		/////
		//cs.Format("%d ", m_target[k].sn);

		//cvPutText(m_imgc, cs, center, &font, CV_RGB(0, 255, 0));


		cs.Format("%d", m_target[k].code );
		//center.x = center.x+m_rad;

		cvPutText(m_imgc, cs, center, &font, CV_RGB(252, 12,172 ));

		/////////������ʮ��

		center.x=m_target[k].x+0.5;
		center.y=m_target[k].y+0.5;

		//		cvCircle(m_imgc, center,2, CV_RGB(255, 0, 0),2,8, 0);

		CvPoint  cp1, cp2, cp3, cp4;

		int s=size.width*0.4;

		cp1.x=center.x-s-1;
		cp1.y=center.y;

		cp2.x = center.x + s + 1;
		cp2.y=center.y;

		cp3.x=center.x;
		cp3.y = center.y - s - 1;

		cp4.x=center.x;
		cp4.y=center.y+s+1;


		//		cvCircle(img, cpc, s, CV_RGB(c, 0 , 0));  
		int line= m_dia/64.0f+0.6f ;


		cvLine( m_imgc, cp1, cp2, CV_RGB(2,252,12), 1.1+m_dia/64.0,8 , 0 );	
		cvLine( m_imgc, cp3, cp4, CV_RGB(2,252,12), 1.1+m_dia/64.0,8 , 0 );

	}

	//m_imgc->origin=1;
	cvFlip(m_imgc); 

	//cvNamedWindow("aaa",0);cvShowImage("aaa",m_imgc);

	//cvSaveImage("qqqqq.bmp",m_imgc);

}


////����Բ��Ч���ж�
bool CodedTarget::Check_centerCircle(IplImage *img, int rad)
{
	CvSize sz=cvGetSize(img);
	
	int h=sz.height;
	int w=sz.width;
	
	int cx=w/2;
	int cy=h/2;
	
	int i, j;
	
	int rad2=rad*5.8/11;  //h
	
	BYTE *pb=(BYTE*)img->imageData;	
	
	int sum0=0, sum1=0;
	
	for (j=cy-rad2; j<= cy+rad2; j++)
		for (i=cx-rad2; i<= cx+rad2; i++)
		{
			if ( pb[j*w+i] > 0 )
				sum1++;
			else
				sum0++;
			
		}
		
		
		float rt=(float)sum0/(sum1+sum0);
		
		//	printf("rt=%f, \n",rt);
		
		if ( rt < 0.7 && rt > 0.2)
			return true;
		else
			return false;
		


}
