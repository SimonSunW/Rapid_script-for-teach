//�����Ƿ񱣴�ͼƬ
#define saveImages 1
//�����Ƿ��¼��Ƶ
#define recordVideo 1

// ����OpenCV API
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

//����PYLON API.
#include <pylon/PylonIncludes.h>

#include<iostream>

#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>    
#endif

//�����ռ�.
using namespace Pylon;
using namespace cv;
using namespace std;
//����ץȡ��ͼ����
static const uint32_t c_countOfImagesToGrab = 20;



void main()
{
	 Mat imgThreshold;
	 Mat canny_output;
	 RNG rng(1234);
	 String string1;
	 double g_dConAreaRed=0;
	 double g_dConAreaBlue=0;
	 double g_dConAreaBlack=0;
	 vector<vector<Point> > contours;
     vector<Vec4i> hierarchy;
    //Pylon�Զ���ʼ������ֹ
    Pylon::PylonAutoInitTerm autoInitTerm;
    try
    {
        //�����������������ʶ��������
        CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());
        // ��ӡ���������
        std::cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
        //��ȡ����ڵ�ӳ���Ի���������
        GenApi::INodeMap& nodemap = camera.GetNodeMap();
        //�����
        camera.Open();
        //��ȡ��������Ⱥ͸߶�
        GenApi::CIntegerPtr width = nodemap.GetNode("Width");
        GenApi::CIntegerPtr height = nodemap.GetNode("Height");

        //���������󻺳���,Ĭ��Ϊ10
        camera.MaxNumBuffer = 5;
        // �½�pylon ImageFormatConverter����.
        CImageFormatConverter formatConverter;
        //ȷ��������ظ�ʽ
        formatConverter.OutputPixelFormat = PixelType_BGR8packed;
        // ����һ��Pylonlmage��������������OpenCV images
        CPylonImage pylonImage;

        //����һ�����α�����������ץȡ��ͼ���Լ������ļ�������
        int grabbedlmages = 0;

        // �½�һ��OpenCV video creator����.
        VideoWriter cvVideoCreator;
        //�½�һ��OpenCV image����.

        Mat openCvImage;
        // ��Ƶ�ļ���

        std::string videoFileName = "openCvVideo.avi";
        // ������Ƶ֡��С
        cv::Size frameSize = Size((int)width->GetValue(), (int)height->GetValue());

        //������Ƶ�������ͺ�֡�ʣ�������ѡ��
        // ֡�ʱ���С�ڵ����������֡��
        cvVideoCreator.open(videoFileName, CV_FOURCC('D', 'I', 'V','X'), 3, frameSize, true);
        //cvVideoCreator.open(videoFileName, CV_F0URCC('M','P',,4','2��), 20, frameSize, true);
        //cvVideoCreator.open(videoFileName, CV_FOURCC('M', '3', 'P', 'G'), 20, frameSize, true);


        // ��ʼץȡc_countOfImagesToGrab images.
        //���Ĭ����������ץȡģʽ
        camera.StartGrabbing(c_countOfImagesToGrab, GrabStrategy_LatestImageOnly);

        //ץȡ�������ָ��
        CGrabResultPtr ptrGrabResult;

        // ��c_countOfImagesToGrab images��ȡ�ָ��ɹ�ʱ��Camera.StopGrabbing() 
        //��RetrieveResult()�����Զ�����ֹͣץȡ
    
        while (camera.IsGrabbing())

        {
            // �ȴ����պͻָ�ͼ�񣬳�ʱʱ������Ϊ5000 ms.
            camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

            //���ͼ��ץȡ�ɹ�
            if (ptrGrabResult->GrabSucceeded())
            {
                // ��ȡͼ������
                cout <<"SizeX: "<<ptrGrabResult->GetWidth()<<endl;
                cout <<"SizeY: "<<ptrGrabResult->GetHeight()<<endl;

                //��ץȡ�Ļ�������ת����pylon image.
                formatConverter.Convert(pylonImage, ptrGrabResult);

                // �� pylon imageת��OpenCV image.
                openCvImage = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *) pylonImage.GetBuffer());



 //��ȡ��ɫ��8λͼ��
	 inRange(openCvImage,Scalar(5,5,60),Scalar(80,70,110),imgThreshold); 
	 //������ (ȥ��һЩ���)  
     Mat element = getStructuringElement(MORPH_RECT, Size(7, 7));  
     morphologyEx(imgThreshold, imgThreshold, MORPH_OPEN, element);  
     //�ղ��� (����һЩ��ͨ��)  
     morphologyEx(imgThreshold, imgThreshold, MORPH_CLOSE, element);
     blur( imgThreshold, imgThreshold, Size(3,3) );
     // ��Canny���Ӽ���Ե
     Canny( imgThreshold, canny_output,3, 9, 3 );
     // Ѱ������
     findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
     // �������
     Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
	 for( int i = 0; i< contours.size(); i++ )
       {
         Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
         drawContours( drawing, contours, i,color, 3,8, hierarchy, 0, Point() );
       }
      //�������������  
     for (int i = 0; i < (int)contours.size(); i++)  
       {  
          g_dConAreaRed = contourArea(contours[i],false);  
          cout << "����" << i << "����ɫ���������Ϊ����" << g_dConAreaRed << endl;  
       }
	 if(g_dConAreaRed>10000)
	 { 
		 string1="Red";
		 cout << string1 << endl;  
	 }


	 //��ȡ��ɫ��8λͼ��
	 inRange(openCvImage,Scalar(90,0,0),Scalar(150,70,30),imgThreshold);
	 //������ (ȥ��һЩ���)  
     morphologyEx(imgThreshold, imgThreshold, MORPH_OPEN, element);  
     //�ղ��� (����һЩ��ͨ��)  
     morphologyEx(imgThreshold, imgThreshold, MORPH_CLOSE, element);
     blur( imgThreshold, imgThreshold, Size(3,3) );
     // ��Canny���Ӽ���Ե
     Canny( imgThreshold, canny_output,3, 9, 3 );
     // Ѱ������
     findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
     // �������
	 for( int i = 0; i< contours.size(); i++ )
       {
         Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
         drawContours( drawing, contours, i,color, 3,8, hierarchy, 0, Point() );
       }
      //�������������  
     for (int i = 0; i < (int)contours.size(); i++)  
       {  
          g_dConAreaBlue = contourArea(contours[i],false);  
          cout << "����" << i << "����ɫ���������Ϊ����" << g_dConAreaBlue << endl;  
       }
	 if(g_dConAreaBlue>10000)
	 {
		 string1="Blue";
	     cout << string1 << endl; 
	 }
	 //��ȡ��ɫ��8λͼ��
	 inRange(openCvImage,Scalar(10,10,0),Scalar(70,70,80),imgThreshold);
	 //������ (ȥ��һЩ���)  
	 morphologyEx(imgThreshold, imgThreshold, MORPH_OPEN, element);  
     //�ղ��� (����һЩ��ͨ��)  
     morphologyEx(imgThreshold, imgThreshold, MORPH_CLOSE, element);
     blur( imgThreshold, imgThreshold, Size(3,3) );
     // ��Canny���Ӽ���Ե
     Canny( imgThreshold, canny_output,3, 9, 3 );
     // Ѱ������
     findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
     // �������
	 for( int i = 0; i< contours.size(); i++ )
       {
         Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
         drawContours( drawing, contours, i,color, 3,8, hierarchy, 0, Point() );
       }
      //�������������  
     for (int i = 0; i < (int)contours.size(); i++)  
       {  
          g_dConAreaBlack = contourArea(contours[i],false);  
          cout << "����" << i << "����ɫ���������Ϊ����" << g_dConAreaBlack << endl;  
       }
	 if(g_dConAreaBlack>10000)
     {
		 string1="Black";	
		 cout << string1 << endl; 
	 }
      // �ڴ�������ʾ���
     namedWindow( "�����ͼ��", CV_WINDOW_AUTOSIZE );
     imshow( "�����ͼ��", drawing );
	 char key = (char) waitKey(3);  
     if(key == 27)  
	 break;  


                //�����Ҫ����ͼƬ
                if (saveImages)
                {
                   std::ostringstream s;
                    // �����������ļ����洢ͼƬ
                   s << "image_" << grabbedlmages << ".jpg";
                   std::string imageName(s.str());
                    //����OpenCV image.
                   imwrite(imageName, openCvImage);
                   grabbedlmages++;
                }

                //�����Ҫ��¼��Ƶ
                if (recordVideo)
                {
            
                    cvVideoCreator.write(openCvImage);
                }

                //�½�OpenCV display window.
                namedWindow("OpenCV Display Window", CV_WINDOW_NORMAL); // other options: CV_AUTOSIZE, CV_FREERATIO
                //��ʾ��ʱӰ��.
                imshow("OpenCV Display Window", openCvImage);

                // Define a timeout for customer's input in
                // '0' means indefinite, i.e. the next image will be displayed after closing the window.
                // '1' means live stream
                waitKey(30);

            }

        }            

    }
    catch (GenICam::GenericException &e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl
            << e.GetDescription() << endl;
    }
    return;
}