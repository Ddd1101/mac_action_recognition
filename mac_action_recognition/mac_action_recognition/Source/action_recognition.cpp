#include "action_recognition.h"

action_recognition::action_recognition(QWidget *parent)
: QMainWindow(parent)
{
    ui.setupUi(this);
    rgbPath.clear();
    grayPath.clear();
    stdRgbPath = "";
    stdGrayPath = "";
    //svm1
    train_data1 = ml::TrainData::loadFromCSV("C:\\dataset\\svm1\\stand_pos.csv", 1);
    m1 = train_data1->getTrainSamples();
    normalize(m1, m1, CV_32FC1);
    labels1.assign(m1.rows, +1);
    train_data1 = ml::TrainData::loadFromCSV("C:\\dataset\\svm1\\stand_neg.csv", 1);
    m2 = train_data1->getTrainSamples();
    normalize(m2, m2, CV_32FC1);
    labels1.insert(labels1.end(), m2.rows, -1);
    m = mergeRow(m1, m2);
    
    svm1 = SVM::create();
    svm1->setCoef0(0.0);
    svm1->setDegree(3);
    svm1->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 1e-3));
    svm1->setGamma(0);
    svm1->setKernel(SVM::LINEAR); //≤…”√œﬂ–‘∫À∫Ø£¨∆‰À˚µƒsigmoid ∫ÕRBF ø…◊‘––…Ë÷√£¨∆‰÷µ”…0-5°£
    svm1->setNu(0.5);
    svm1->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
    svm1->setC(0.01); // From paper, soft classifier
    svm1->setType(SVM::EPS_SVR); // C_SVC; // EPSILON_SVR; // may be also NU_SVR; // do regression task
    svm1->train(m, ROW_SAMPLE, Mat(labels1));
    
    //svm2
    train_data2 = ml::TrainData::loadFromCSV("C:\\dataset\\svm2\\train\\svm2_pos.csv", 1);
    m1 = train_data2->getTrainSamples();
    normalize(m1, m1, CV_32FC1);
    labels2.assign(m1.rows, +1);
    train_data2 = ml::TrainData::loadFromCSV("C:\\dataset\\svm2\\train\\svm2_neg.csv", 1);
    m2 = train_data2->getTrainSamples();
    normalize(m2, m2, CV_32FC1);
    labels2.insert(labels2.end(), m2.rows, -1);
    m = mergeRow(m1, m2);
    
    svm2 = SVM::create();
    svm2->setCoef0(0.0);
    svm2->setDegree(3);
    svm2->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 1e-3));
    svm2->setGamma(0);
    svm2->setKernel(SVM::LINEAR); //≤…”√œﬂ–‘∫À∫Ø£¨∆‰À˚µƒsigmoid ∫ÕRBF ø…◊‘––…Ë÷√£¨∆‰÷µ”…0-5°£
    svm2->setNu(0.5);
    svm2->setP(0.1); // for EPSILON_SVR, epsilon in loss function?
    svm2->setC(0.01); // From paper, soft classifier
    svm2->setType(SVM::EPS_SVR); // C_SVC; // EPSILON_SVR; // may be also NU_SVR; // do regression task
    svm2->train(m, ROW_SAMPLE, Mat(labels2));
    
    imgCount = 0;
    
    z_m = new ZernikeMoment();
    
    while (svm2W.empty() != 1) {
        svm2W.pop();
    }
    
    //ui.srcPathText->setText("C:\\dataset\\dataset\\pos\\1");
    
    //÷°≤•∑≈
    time_clock = new QTimer();
    time_clock->setInterval(10);
    time_clock->stop();
    connect(time_clock, SIGNAL(timeout()), this, SLOT(frameProcess()));
    
    QPixmap myPix("C:\\default.jpg");
    ui.srcImg->setPixmap(myPix);
    ui.srcImg->show();
    
    //µÿ÷∑ƒ¨»œ÷µ
    pathNum = "1";
    rgbPath = "C:\\dataset\\test\\rgb\\pos\\";
    grayPath = "C:\\dataset\\test\\gray\\pos\\";
    
    //ui.svm1Text->setReadOnly(true);
    //ui.svm2Text->setReadOnly(true);
    
    //Ãÿ’˜œ‘ æ
    if (ui.isTrait->isChecked()) {
        lightTrait = Mat(480, 640, CV_8UC1, Scalar(0));
        whTraitImg = Mat(360, 480, CV_8UC1, Scalar(0));
    }
}

void action_recognition::start() {
    startTime = static_cast<double>(getTickCount());
    
    time_clock->start();
    imgCount = 0;
    if (ui.posRadio->isChecked()) {
        rgbPath = "C:\\dataset\\test\\rgb\\pos\\";
        grayPath = "C:\\dataset\\test\\gray\\pos\\";
        /*t1p = "C:\\dataset\\test\\t1\\pos\\";
         t2p = "C:\\dataset\\test\\t2\\pos\\";
         t3p = "C:\\dataset\\test\\t3\\pos\\";
         t4p = "C:\\dataset\\test\\t4\\pos\\";*/
    }
    else {
        rgbPath = "C:\\dataset\\test\\rgb\\neg\\";
        grayPath = "C:\\dataset\\test\\gray\\neg\\";
        /*t1p = "C:\\dataset\\test\\t1\\neg\\";
         t2p = "C:\\dataset\\test\\t2\\neg\\";
         t3p = "C:\\dataset\\test\\t3\\neg\\";
         t4p = "C:\\dataset\\test\\t4\\neg\\";*/
    }
    stdRgbPath = rgbPath += (ui.pathNum->text().toStdString());
    stdGrayPath = grayPath += (ui.pathNum->text().toStdString());
    /*t1p += (ui.pathNum->text().toStdString());
     t2p += (ui.pathNum->text().toStdString());
     t3p += (ui.pathNum->text().toStdString());
     t4p += (ui.pathNum->text().toStdString());
     t1p += "\\";
     t2p += "\\";
     t3p += "\\";
     t4p += "\\";*/
    
    //cout << stdRgbPath << endl;
    
    if (ui.isTrait->isChecked()) {
        lightTrait = Mat(480, 640, CV_8UC1, Scalar(0));
        
        whTraitImg = Mat(360, 480, CV_8UC1, Scalar(0));
        while (whTrait.empty() != 1) {
            whTrait.pop();
        }
        coreTraitImg = Mat(360, 480, CV_8UC1, Scalar(0));
    }
    
    
    while (coreTrait.empty() != 1) {
        coreTrait.pop();
    }
    
    svm2Show = QString::fromLocal8Bit("        ’˝≥£");
    ui.status->setText(svm2Show);
    while (svm2W.empty() != 1) {
        svm2W.pop();
    }
}

void action_recognition::stop() {
    time_clock->stop();
    while(!svm2W.empty()) {
        svm2W.pop();
    }
    svm2Show = QString::fromLocal8Bit("        ’˝≥£");
    ui.status->setText(svm2Show);
}

void action_recognition::frameProcess() {
    ////cv::cvtColor(image, image, CV_RGB2RGBA);//ÕºœÒ‘⁄QTœ‘ æ«∞£¨±ÿ–Î◊™ªØ≥…QImage∏Ò Ω£¨Ω´RGBA∏Ò Ω◊™ªØ≥…RGB
    //stdRgbPath = rgbPath.toStdString();
    //cout << stdRgbPath << endl;
    glob(stdRgbPath, rgbFiles);
    glob(stdGrayPath, grayFiles);
    //if (imgCount < files.size()) {
    //    image = cv::imread(files[imgCount]);//∂¡»°ÕºœÒ
    //    cv::cvtColor(image, image, CV_RGB2RGBA);//ÕºœÒ‘⁄QTœ‘ æ«∞£¨±ÿ–Î◊™ªØ≥…QImage∏Ò Ω£¨Ω´RGBA∏Ò Ω◊™ªØ≥…RGB
    //    showFrame(image);
    //}
    int isStand = 0;
    float svm2Res = 0;
    if (imgCount < rgbFiles.size() && imgCount < grayFiles.size()) {
        image = cv::imread(rgbFiles[imgCount]);
        if (ui.isTrait->isChecked()) {
            whTraitImg = Mat(360, 480, CV_8UC1, Scalar(0));
            coreTraitImg = Mat(360, 480, CV_8UC1, Scalar(0));
        }
        
        //srcGray= cv::imread(grayFiles[imgCount]);
        Mat tmp;
        cvtColor(image, tmp, COLOR_RGB2GRAY);
        
        //image = cv::imread(grayFiles[imgCount]);
        //cvtColor(image, tmp, COLOR_RGB2GRAY);
        if (imgCount == 0) {
            Vibe_Bgs.init(tmp);
            Vibe_Bgs.processFirstFrame(tmp);
            //cout << "training gmm compelete" << endl;
        }
        else {
            //threshold(src, src, 0,255, THRESH_BINARY);//◊Ó¥Û¿‡º‰≤Ó
            Vibe_Bgs.testAndUpdate(tmp);
            mask = Vibe_Bgs.getMask();
            if (ui.isTrait->isChecked()) {
                lightTrait = getLightTrait(rgbFiles[imgCount - 1], rgbFiles[imgCount]);//œ° Ëπ‚¡˜Ãÿ’˜
            }
            morphologyEx(mask, mask, MORPH_OPEN, elem7);//ø™‘ÀÀ„=∏Ø ¥+≈Ú’Õ
            morphologyEx(mask, mask, MORPH_CLOSE, elem9);//±’‘ÀÀ„=≈Ú’Õ+∏Ø ¥
            medianBlur(mask, mask, 3);//÷–÷µ¬À≤®
            
            
            
            bound = filterBg_boundRect(mask);
            
            isStand = 0;
            //svm1-’æ◊ÀºÏ≤‚
            if (bound.isEx == 1) {
                if (coreTrait.size() >= 40) {
                    coreTrait.pop();
                    coreTrait.push(bound.core);
                }
                else {
                    coreTrait.push(bound.core);
                }
                if (whTrait.size() >= 40) {
                    whTrait.pop();
                    whTrait.push(float(bound.width) / float(bound.height));
                }
                else {
                    whTrait.push(float(bound.width) / float(bound.height));
                }
                float labels[4] = { float(bound.width) / float(bound.height), bound.width * bound.height, bound.width + bound.height, bound.height };
                Mat test(1, 4, CV_32F, labels);
                float response = svm1->predict(test);
                //cout << "svm1  :";
                if (response < 0.23) {
                    //cout << 0 << "    ";
                    isStand = 0;
                }
                else {
                    //cout << 1 << "    ";
                    isStand = 1;
                }
                rectangle(image, Rect(bound.x, bound.y, bound.width, bound.height), Scalar(255, 255, 255), 1, 8);
            }
            //svm2-À§µπºÏ≤‚
            int index = 0;
            if (isStand == 0 && bound.isEx == 1) {
                z_m->readImg(mask);
                
                z_modes[index++] = z_m->getZernike(0, 0);
                
                z_modes[index++] = z_m->getZernike(1, 1);
                
                z_modes[index++] = z_m->getZernike(2, 0);
                
                z_modes[index++] = z_m->getZernike(2, 2);
                
                z_modes[index++] = z_m->getZernike(3, 1);
                
                z_modes[index++] = z_m->getZernike(3, 3);
                
                z_modes[index++] = z_m->getZernike(4, 0);
                
                z_modes[index++] = z_m->getZernike(4, 2);
                
                z_modes[index++] = z_m->getZernike(4, 4);
                
                z_modes[index] = float(bound.width) / float(bound.height);
                
                Mat test(1, 10, CV_32F, z_modes);
                svm2Res = svm2->predict(test);
                svm2Res *= (-1);
                
                if (svm2W.size() < 15) {
                    if (svm2Res > 15) {
                        svm2W.push(1);
                    }
                    else {
                        svm2W.push(0);
                    }
                }
                else {
                    svm2W.pop();
                    if (svm2Res > 15) {
                        svm2W.push(1);
                    }
                    else {
                        svm2W.push(0);
                    }
                }
                if (svm2W.size() >= 15) {
                    int sum = 0;
                    for (int i = 0; i < svm2W.size(); i++) {
                        int tmp = 0;
                        tmp = svm2W.front();
                        svm2W.pop();
                        svm2W.push(tmp);
                        sum += tmp;
                    }
                    if (sum > 13) {
                        svm2Show = QString::fromLocal8Bit("        æØ∏Ê");
                        ui.status->setText(svm2Show);
                    }
                }
                cout << "svm2  : " << svm2Res << endl;
            }
            //cout << endl;
        }
        if (mask.empty() != 1) {
            cv::resize(mask, mask, Size(mask.cols * 0.75, mask.rows * 0.75), 0, 0, INTER_LINEAR);
            if (mask.channels() == 3) {
                cv::cvtColor(mask, mask, CV_RGB2RGBA);
                showBgFrame();
            }
            else if (mask.channels() == 1) {
                cv::cvtColor(mask, mask, CV_GRAY2BGRA);
                showBgFrame();
            }
        }
        if (whTrait.size() > 0) {
            for (int i = 0; i < whTrait.size(); i++) {
                cv::Point pointInterest;//Ãÿ’˜µ„£¨”√“‘ª≠‘⁄ÕºœÒ÷–
                pointInterest.x = i * 12;//Ãÿ’˜µ„‘⁄ÕºœÒ÷–∫·◊¯±Í
                float tmp = whTrait.front();
                whTrait.pop();
                whTrait.push(tmp);
                pointInterest.y = 240 - tmp * 80;//Ãÿ’˜µ„‘⁄ÕºœÒ÷–◊›◊¯±Í
                if (ui.isTrait->isChecked()) {
                    cv::circle(whTraitImg, pointInterest, 4, cv::Scalar(255, 255, 255));//‘⁄ÕºœÒ÷–ª≠≥ˆÃÿ’˜µ„£¨2 «‘≤µƒ∞Îæ∂
                }
                
            }
        }
        if (coreTrait.size() > 0) {
            for (int i = 0; i < coreTrait.size(); i++) {
                cv::Point pointInterest;//Ãÿ’˜µ„£¨”√“‘ª≠‘⁄ÕºœÒ÷–
                pointInterest.x = i * 12;//Ãÿ’˜µ„‘⁄ÕºœÒ÷–∫·◊¯±Í
                float tmp = coreTrait.front();
                coreTrait.pop();
                coreTrait.push(tmp);
                pointInterest.y = 240 - tmp * 80;//Ãÿ’˜µ„‘⁄ÕºœÒ÷–◊›◊¯±Í
                if (ui.isTrait->isChecked()) {
                    cv::circle(coreTraitImg, pointInterest, 4, cv::Scalar(255, 255, 255));//‘⁄ÕºœÒ÷–ª≠≥ˆÃÿ’˜µ„£¨2 «‘≤µƒ∞Îæ∂
                }
                
            }
        }
        
        if (ui.isTrait->isChecked()) {
            if (whTraitImg.channels() == 1) {
                cv::cvtColor(whTraitImg, whTraitImg, CV_GRAY2BGRA);
            }
            else {
                cv::cvtColor(whTraitImg, whTraitImg, CV_RGB2RGBA);
            }
            
            if (coreTraitImg.channels() == 1) {
                cv::cvtColor(coreTraitImg, coreTraitImg, CV_GRAY2BGRA);
            }
            else {
                cv::cvtColor(coreTraitImg, coreTraitImg, CV_RGB2RGBA);
            }
            
            cv::resize(lightTrait, lightTrait, Size(lightTrait.cols * 0.75, lightTrait.rows * 0.75), 0, 0, INTER_LINEAR);
            cv::cvtColor(lightTrait, lightTrait, CV_GRAY2BGRA);
            
        }
        
        cv::resize(image, image, Size(image.cols * 1.5, image.rows * 1.5), 0, 0, INTER_LINEAR);
        cv::cvtColor(image, image, CV_RGB2RGBA);
        showSrcFrame();
        showWHTFrame();
        showCTFrame();
        if (ui.isTrait->isChecked() && !coreTraitImg.empty()) {
            string corePath = "C:\\dataset\\test\\t3\\tmp\\";
            corePath += to_string(imgCount);
            corePath += ".jpg";
            imwrite(corePath, coreTraitImg);
        }
        showLTFrame();
    }
    //auto workCursor1 = ui.svm1Text->textCursor();
    /*workCursor1.movePosition(QTextCursor::Start);
     workCursor1.insertText(QString::number(isStand, 10));
     workCursor1.insertBlock();*/
    
    //“∆∂Øπˆ∂ØÃıµΩµ◊≤ø
    //QScrollBar *scrollbar = ui.svm1Text->verticalScrollBar();
    
    
    //auto workCursor2 = ui.svm2Text->textCursor();
    /*workCursor2.movePosition(QTextCursor::Start);
     workCursor2.insertText(QString("%1").arg(svm2Res*(-1)));
     workCursor2.insertBlock();*/
    
    if (imgCount == (rgbFiles.size() - 1) || imgCount == (grayFiles.size() - 1)) {
        time_clock->stop();
    }
    curTime = ((double)getTickCount() - startTime) / getTickFrequency();
    double frameRate = imgCount / curTime;
    QString frameRateShow;
    frameRateShow.setNum(frameRate, 'g', 3);
    ui.frameRate->setText(frameRateShow);
    
    imgCount++;
}

void action_recognition::getRgbPath() {
    /*rgbPath = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Save path"), QDir::currentPath()));
     if (!rgbPath.isEmpty()) {
     ui.rgbPath->setText(rgbPath);
     }*/
}

void action_recognition::showSrcFrame() {
    QImage img = QImage((const unsigned char*)(image.data),
                        image.cols, image.rows, QImage::Format_RGB32);
    ui.srcImg->setPixmap(QPixmap::fromImage(img));
    ui.srcImg->show();
}

void action_recognition::showBgFrame() {
    if (ui.isTrait->isChecked()) {
        QImage img = QImage((const unsigned char*)(mask.data),
                            mask.cols, mask.rows, QImage::Format_RGB32);
        ui.bgImg->setPixmap(QPixmap::fromImage(img));
        ui.bgImg->show();
    }
}

void action_recognition::showLTFrame() {
    if (ui.isTrait->isChecked()) {
        QImage img = QImage((const unsigned char*)(lightTrait.data),
                            lightTrait.cols, lightTrait.rows, QImage::Format_RGB32);
        ui.lightImg->setPixmap(QPixmap::fromImage(img));
        ui.lightImg->show();
    }
}

void action_recognition::showWHTFrame() {
    if (ui.isTrait->isChecked()) {
        QImage img = QImage((const unsigned char*)(whTraitImg.data),
                            whTraitImg.cols, whTraitImg.rows, QImage::Format_RGB32);
        ui.whImg->setPixmap(QPixmap::fromImage(img));
        ui.whImg->show();
    }
}

void action_recognition::showCTFrame() {
    if (ui.isTrait->isChecked()) {
        QImage img = QImage((const unsigned char*)(coreTraitImg.data),
                            coreTraitImg.cols, coreTraitImg.rows, QImage::Format_RGB32);
        ui.coreImg->setPixmap(QPixmap::fromImage(img));
        ui.coreImg->show();
    }
}

