#pragma once
//opencv
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/video.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml.hpp>
#pragma comment(lib,"shlwapi.lib")
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
//±æœÓƒø
#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QCombobox.h>
#include <QLabel.h>
#include <QTimer.h>
#include "ui_action_recognition.h"
#include "Vibe.h"
#include "util.h"
#include "ZernikeMoment.h"

using namespace std;
using namespace cv;
using namespace cv::ml;

namespace Ui {
    class action_recognition;
}

class action_recognition : public QMainWindow
{
    Q_OBJECT
    
public:
    action_recognition(QWidget *parent = Q_NULLPTR);
    cv::Mat image;
    cv::Mat srcImage;
    cv::Mat srcGray;
    cv::Mat lightTrait;
    cv::Mat whTraitImg;
    cv::Mat coreTraitImg;
    
private:
    Ui::action_recognitionClass ui;
    Mat elem3 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat elem5 = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat elem7 = getStructuringElement(MORPH_RECT, Size(7, 7));
    Mat elem9 = getStructuringElement(MORPH_RECT, Size(9, 9));
    
    string rgbPath;//‘¥ ˝æ›¬∑æ∂
    string grayPath;//ª“∂»Õº¬∑æ∂
    string stdRgbPath;
    string stdGrayPath;
    QString pathNum;
    
    VideoCapture capture;
    vector< String > rgbFiles;
    vector< String > grayFiles;
    ViBe_BGS Vibe_Bgs;
    //svm1—µ¡∑ ˝æ›
    Ptr<ml::TrainData> train_data1;
    Mat m1;
    vector< int > labels1;
    Mat m2;
    Mat m;
    Ptr< SVM > svm1;
    //svm2—µ¡∑ ˝æ›
    Ptr<ml::TrainData> train_data2;
    vector< int > labels2;
    Ptr< SVM > svm2;
    
    int imgCount;
    //Mat src;
    Mat mask;
    Wicket bound;
    
    //svmºÏ≤‚
    float z_modes[10];
    ZernikeMoment *z_m;
    QString svm2Show;
    
    //÷°≤•∑≈
    QTimer *time_clock;
    
    //øÌ∏ﬂÃÿ’˜
    queue<float> whTrait;
    
    //÷ÿ–ƒÃÿ’˜
    queue<float> coreTrait;
    
    queue<bool> svm2W;
    
    //≤‚ ‘÷°¬
    double startTime;
    double curTime;
    
    private slots:              //…˘√˜–≈∫≈∫Ø ˝
    void frameProcess();
    void getRgbPath();
    void showSrcFrame();
    void showBgFrame();
    void showLTFrame();
    void showWHTFrame();
    void showCTFrame();
    void start();
    void stop();
};
