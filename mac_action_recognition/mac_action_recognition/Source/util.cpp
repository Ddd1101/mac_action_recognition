#include "util.h"


Mat elem3 = getStructuringElement(MORPH_RECT, Size(3, 3));
Mat elem5 = getStructuringElement(MORPH_RECT, Size(5, 5));
Mat elem7 = getStructuringElement(MORPH_RECT, Size(7, 7));
//∂‘±»∂»
void ContrastAndBright(Mat &src, Mat &dst, double alpha, double beta) {
    // ÷¥––±‰ªª new_image(i,j) = alpha    * image(i,j) + beta
    for (int y = 0; y < src.rows; y++)
    {
        for (int x = 0; x < src.cols; x++)
        {
            for (int c = 0; c < 3; c++)
            {
                dst.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha * (src.at<Vec3b>(y, x)[c]) + beta);
            }
        }
    }
}


void srcAmend(Mat &src) {
    Mat tmp;
    
    //‘ˆº”∂‘±»∂»
    tmp = Mat::zeros(src.size(), src.type());
    ContrastAndBright(src, tmp, 1.3, 0);
    src = tmp;
}

void bgAmend(Mat &mask) {
    Rect boundRect;
    Wicket result;
    //’‚¡Ω∏ˆ≤Ÿ◊˜æÕ «œ»»•‘Îµ„£¨‘Ÿ∞—ø’∂¥ÃÓ≥‰∆¿¥
    morphologyEx(mask, mask, MORPH_OPEN, elem5);//ø™‘ÀÀ„=∏Ø ¥+≈Ú’Õ
    //dilate(mask, mask, element3);
    morphologyEx(mask, mask, MORPH_CLOSE, elem5);//±’‘ÀÀ„=≈Ú’Õ+∏Ø ¥
    medianBlur(mask, mask, 5);//÷–÷µ¬À≤®
}

//»•µÙ»ÀÃÂÕ‚µƒ∆‰À˚≤ø∑÷
void filterBg(Mat &mask) {
    vector<vector<Point>> contours;
    Mat contours_src = mask.clone();
    findContours(contours_src, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);//≤È’“¬÷¿™
    sort(contours.begin(), contours.end(), ContoursSortFun);
    Mat toMask(mask.size(), CV_8UC3, Scalar(0, 0, 0));//¥¥Ω®“ª∏ˆ»´∫⁄µƒÕº∆¨
    vector<vector<Point>> ::iterator it;
    if (contours.size() > 0 && contours[0].size() > 250) {
        drawContours(toMask, contours, 0, Scalar(255, 255, 255), -1);
        cvtColor(toMask, mask, CV_BGR2GRAY);
    }
    else {
        toMask.copyTo(mask);
    }
}

//»•µÙ»ÀÃÂÕ‚µƒ∆‰À˚≤ø∑÷≤¢µ√µΩÕ‚Ω”æÿ’Û
Wicket filterBg_boundRect(Mat &mask) {
    vector<vector<Point>> contours;
    Rect boundRect;
    Wicket result;
    Mat contours_src = mask.clone();
    findContours(contours_src, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);//≤È’“¬÷¿™
    sort(contours.begin(), contours.end(), ContoursSortFun);//Ω¯––¬÷¿™≈≈–Ú£¨µ⁄“ª∏ˆº¥Œ™ƒø±Í¬÷¿™
    Mat toMask(mask.size(), CV_8UC3, Scalar(0, 0, 0));//¥¥Ω®“ª∏ˆ»´∫⁄µƒÕº∆¨
    vector<vector<Point>> ::iterator it;
    if (contours.size() > 0 && contours[0].size() > 250) {
        drawContours(toMask, contours, 0, Scalar(255, 255, 255), -1);
        cvtColor(toMask, mask, CV_BGR2GRAY);
        boundRect = boundingRect(Mat(contours[0]));
        result.isEx = 1;
        result.x = boundRect.x;
        result.y = boundRect.y;
        result.height = boundRect.height;
        result.width = boundRect.width;
        
        IplImage tmp = IplImage(mask);
        CvPoint center;
        double m00, m10, m01;
        CvMoments moment;
        double top, bottom;
        cvMoments((CvArr*)&tmp, &moment, 1);
        m00 = cvGetSpatialMoment(&moment, 0, 0);
        m10 = cvGetSpatialMoment(&moment, 1, 0);
        m01 = cvGetSpatialMoment(&moment, 0, 1);
        center.x = (int)(m10 / m00);
        center.y = (int)(m01 / m00);
        top = boundRect.y;
        bottom = boundRect.y + boundRect.height;
        result.core = (bottom - center.y) / boundRect.height;
    }
    else {
        toMask.copyTo(mask);
    }
    return result;
}

//º∆À„÷ÿ–ƒ
Wicket core(Mat mask) {
    Wicket result;
    Rect boundRect;
    //»•µÙ»ÀÃÂÕ‚µƒ∆‰À˚≤ø∑÷
    vector<vector<Point>> contours;
    Mat contours_src = mask.clone();
    findContours(contours_src, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);//≤È’“¬÷¿™
    sort(contours.begin(), contours.end(), ContoursSortFun);
    Mat toMask(480, 640, CV_8UC3, Scalar(0, 0, 0));//¥¥Ω®“ª∏ˆ»´∫⁄µƒÕº∆¨
    vector<vector<Point>> ::iterator it;
    
    //÷ÿ–ƒ
    double top, bottom;
    double s;
    if (contours.size() > 0 && contours[0].size() > 250) {
        result.isEx = 1;
        drawContours(toMask, contours, 0, Scalar(255, 255, 255), -1);
        cvtColor(toMask, mask, CV_BGR2GRAY);
        top = bottom = 0;
        IplImage tmp = IplImage(mask);
        CvPoint center;
        double m00, m10, m01;
        CvMoments moment;
        cvMoments((CvArr*)&tmp, &moment, 1);
        m00 = cvGetSpatialMoment(&moment, 0, 0);
        m10 = cvGetSpatialMoment(&moment, 1, 0);
        m01 = cvGetSpatialMoment(&moment, 0, 1);
        center.x = (int)(m10 / m00);
        center.y = (int)(m01 / m00);
        boundRect = boundingRect(Mat(contours[0]));
        top = boundRect.y;
        bottom = boundRect.y + boundRect.height;
        result.x = boundRect.x;
        result.y = boundRect.y;
        result.height = boundRect.height;
        result.width = boundRect.width;
        //cout << center.x << " -- " << center.y << " -- " << top << " -- " << bottom << " -- " << (bottom - center.y) / boundRect.height ;
        result.core = (bottom - center.y) / boundRect.height;
        //rectangle(mask, Rect(boundRect.x, boundRect.y, boundRect.width, boundRect.height), Scalar(255, 255, 255), 1, 8);
        return result;
    }
    toMask.copyTo(mask);
    return result;
}

bool ContoursSortFun(vector<cv::Point> contour1, vector<cv::Point> contour2) {
    return (contour1.size() > contour2.size());
}

double centerPoint(Mat mask, vector<vector<Point>> contours) {
    double result;
    Rect boundRect;
    double top, bottom;
    top = bottom = 0;
    IplImage tmp = IplImage(mask);
    CvPoint center;
    double m00, m10, m01;
    CvMoments moment;
    cvMoments((CvArr*)&tmp, &moment, 1);
    m00 = cvGetSpatialMoment(&moment, 0, 0);
    m10 = cvGetSpatialMoment(&moment, 1, 0);
    m01 = cvGetSpatialMoment(&moment, 0, 1);
    center.x = (int)(m10 / m00);
    center.y = (int)(m01 / m00);
    boundRect = boundingRect(Mat(contours[0]));
    top = boundRect.y;
    bottom = boundRect.y + boundRect.height;
    result = (bottom - center.y) / boundRect.height;
    return result;
}

//«∞æ∞∑÷∏Ó
Mat getApartFrame(Mat &src, Mat &mask) {
    //===== ∂‘«∞æ∞—⁄¬Î◊ˆ¥¶¿Ì£¨π˝¬À‘Îµ„
    vector<vector<Point>> contours;
    
    //≤È’“¬÷¿™
    findContours(mask, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
    //Ω´≤È’“µΩµƒ¬÷¿™ªÊ÷∆µΩ—⁄¬Î
    Mat dst;
    dst.create(mask.size(), mask.type());
    Mat mask2(dst.size(), CV_8U, Scalar(0));
    drawContours(mask2, contours, -1, Scalar(255), CV_FILLED);
    
    //∂‘«∞æ∞—⁄¬Îµƒ‘Ÿ¥Œ—⁄¬Î¥¶¿Ì
    Mat fgmask1;
    mask.copyTo(fgmask1, mask2);
    //===== «∞æ∞—⁄¬Î¥¶¿ÌÕÍ±œ
    
    //ªÒ»°«∞æ∞ÕºœÒ
    dst = Scalar::all(0);
    src.copyTo(dst, fgmask1);
    return dst;
}

//––»À∏˙◊Ÿ
void humanRecognition(Mat img) {
    HOGDescriptor hog;
    hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());//opencv÷–ƒ¨»œµƒ—µ¡∑ ˝æ›
    
    vector< Rect > detections;
    vector< double > foundWeights;
    
    hog.detectMultiScale(img, detections, foundWeights);//…Ë÷√ºÏ≤‚µƒœ‡πÿ≤Œ ˝º∞∑µªÿ ˝æ›
    //hog.detectMultiScale(img, detections, 0, Size(8, 8), Size(32, 32), 1.05, 2);
    for (size_t j = 0; j < detections.size(); j++)
    {
        if (foundWeights[j] < 1) continue; //«Â≥˛»®÷µΩœ–°µƒºÏ≤‚¥∞ø⁄
        Scalar color = Scalar(0, foundWeights[j] * foundWeights[j] * 200, 0);
        rectangle(img, detections[j], color, img.cols / 400 + 1);
    }
    
    imshow("human", img);
}

Mat mergeRow(cv::Mat A, cv::Mat B) {
    int totalRows = A.rows + B.rows;
    cv::Mat mergedDescriptors(totalRows, A.cols, A.type());
    cv::Mat submat = mergedDescriptors.rowRange(0, A.rows);
    A.copyTo(submat);
    submat = mergedDescriptors.rowRange(A.rows, totalRows);
    B.copyTo(submat);
    return mergedDescriptors;
}

Mat RegionGrow(Mat MatIn, int iGrowPoint, int iGrowJudge) {//iGrowPointŒ™÷÷◊”µ„µƒ≈–∂œÃıº˛£¨iGrowJudgeŒ™…˙≥§Ãıº˛
    Mat MatGrowOld(MatIn.size(), CV_8UC1, Scalar(0));
    Mat MatGrowCur(MatIn.size(), CV_8UC1, Scalar(0));
    Mat MatGrowTemp(MatIn.size(), CV_8UC1, Scalar(0));
    //≥ı ºªØ‘≠ º÷÷◊”µ„
    for (int i = 0; i < MatIn.rows; i++)
    {
        for (int j = 0; j < MatIn.cols; j++)
        {
            int it = MatIn.at<uchar>(i, j);
            
            if (it <= iGrowPoint)//—°»°÷÷◊”µ„£¨◊‘º∫∏¸∏ƒ
            {
                MatGrowCur.at<uchar>(i, j) = 255;
            }
        }
    }
    
    int DIR[8][12] = { { -1,-1 },{ -1,0 },{ -1,1 },{ 0,-1 },{ 0,1 },{ 1,-1 },{ 1,0 },{ 1,1 } };
    Mat MatTemp = MatGrowOld - MatGrowCur;
    int iJudge = countNonZero(MatTemp);
    if (iJudge != 0)//MatGrowOld!=MatGrowCur ≈–∂œ±æ¥Œ∫Õ…œ¥Œµƒ÷÷◊”µ„ «∑Ò“ª—˘£¨»Áπ˚“ª—˘‘Ú÷’÷π—≠ª∑
    {
        MatGrowTemp = MatGrowCur;
        for (int i = 0; i < MatIn.rows; i++)
        {
            for (int j = 0; j < MatIn.cols; j++)
            {
                if (MatGrowCur.at<uchar>(i, j) == 255 && MatGrowOld.at<uchar>(i, j) != 255)
                {
                    for (int iNum = 0; iNum < 9; iNum++)
                    {
                        int iCurPosX = i + DIR[iNum][0];
                        int iCurPosY = j + DIR[iNum][1];
                        if (iCurPosX > 0 && iCurPosX < (MatIn.rows - 1) && iCurPosY>0 && iCurPosY < (MatIn.cols - 1))
                        {
                            if (abs(MatIn.at<uchar>(i, j) - MatIn.at<uchar>(iCurPosX, iCurPosY)) < iGrowJudge)//…˙≥§Ãıº˛£¨◊‘º∫µ˜’˚
                            {
                                MatGrowCur.at<uchar>(iCurPosX, iCurPosY) = 255;
                                
                            }
                        }
                    }
                }
            }
        }
        MatGrowOld = MatGrowTemp;
    }
    return MatGrowCur;
}

Mat getLightTrait(string p1, string p2) {//œ° Ëπ‚¡˜Ãÿ’˜
    Mat src0 = imread(p1);
    Mat src1 = imread(p2);
    Mat gray0;
    Mat gray1;
    int corner_count = 5000;
    vector<Point2f> corners0;
    vector<Point2f> corners1;
    Mat trait(480, 640, CV_8UC1, Scalar(0));
    
    
    cvtColor(src0, gray0, CV_BGR2GRAY);
    goodFeaturesToTrack(gray0, corners0, corner_count, 0.01, 5, Mat(), 3, false, 0.04);
    cornerSubPix(gray0, corners0, Size(10, 10), Size(-1, -1), TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
    cvtColor(src1, gray1, CV_BGR2GRAY);
    goodFeaturesToTrack(gray1, corners1, corner_count, 0.01, 5, Mat(), 3, false, 0.04);
    cornerSubPix(gray1, corners1, Size(10, 10), Size(-1, -1), TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
    
    vector<Point2f>::iterator it;
    for (it = corners0.begin(); it != corners0.end(); it++)
    {
        circle(trait, *it, 3, Scalar(255, 0, 255), 1, 8, 0);
    }
    for (it = corners1.begin(); it != corners1.end(); it++)
    {
        circle(trait, *it, 3, Scalar(255, 0, 255), 1, 8, 0);
    }
    return trait;
}
