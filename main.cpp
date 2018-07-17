#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/face.hpp>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;

string loadModelPath = "/home/ryu/Documents/Project/Qt/FaceRecogAndroid/ModelLBPAndroid.xml";
string videoUrl = "rtsp://admin:123456a@@192.168.1.102";
vector<cv::Mat> listDetected;
cv::VideoCapture cap;
cv::Ptr<cv::face::LBPHFaceRecognizer> model;
void DetectPeople()
{
    cv::CascadeClassifier faceCascades;
    faceCascades.load("/home/ryu/Documents/Project/Qt/FaceRecognizeEmbeded/haarcascade_frontalface_alt2.xml");
    vector<cv::Rect> rects;

    cap.open(videoUrl);

    cv::Mat frame;
    cv::Mat crop;
    while(cap.isOpened())
    {
        cap>>frame;
        cv::resize(frame,frame,cv::Size(1920,1080));
        cv::cvtColor(frame,frame,CV_BGR2GRAY);
        faceCascades.detectMultiScale(frame,rects,1.1,2,0|CV_HAAR_SCALE_IMAGE,cv::Size(150,150));
        for(int i=0; i<rects.size();i++)
        {
            crop = frame(rects.at(i));
            cv::imshow("face",crop);
            listDetected.push_back(crop);
            cv::waitKey(1);
        }
    }
}

void RecogFace()
{
    cv::Mat face;
    model = cv::face::LBPHFaceRecognizer::create();
    model->read(loadModelPath);
    int result=-10;
    cv::Mat image;
    while(true)
    {
        if(listDetected.size()>0)
        {
             image = listDetected.at(0);
             result = model->predict(image);
             milliseconds ms = duration_cast< milliseconds >(
                 system_clock::now().time_since_epoch()
             );
             string name = "/home/ryu/Documents/Project/Qt/FaceRecognizeEmbeded/face/"+std::to_string(result)+"/"+std::to_string(ms.count())+".png";
             cout<<name<<endl;
             cv::imwrite(name,image);
             listDetected.erase(listDetected.begin());
        }
    }

}

int main()
{
    cout << "Hello World!" << endl;
    std::thread detect(DetectPeople);
    std::thread recog(RecogFace);
    detect.join();
    recog.join();
    return 0;
}
