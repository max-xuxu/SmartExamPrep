#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SubjectInfo.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include "DBFactory.h"

#define FACE_MODEL_NAME "face.xml"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    srand(time(0));
    addLabels();
    cc.load("haarcascade_frontalface_alt2.xml");//加载模型
    vc.open(0);//打开摄像头
    connect(&timer, &QTimer::timeout, this, &MainWindow::onTimeout);
    timer.start(50);//开启定时器
    initReconigzer();//初始化人脸识别器
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTimeout()
{
    vc>>frame;//读取一帧视频
    flip(frame, frame, 1);//翻转图像
    findFace();//找脸
    showImage();//显示
}

void MainWindow::addLabels()
{
    int num = SubjectInfo::getInstance()->getNum();//获得学生数
    const int lineNum = 8;//每一行的QLabel数量
    for(int i = 0;i < num;i++)
    {
        QLabel* label = new QLabel(this);//创建QLabel对象
        //控件的geometry属性在布局中是没用的，需要设置MaxSize和MinSize来调整大小
        label->setMaximumSize(QSize(50, 50));
        label->setMinimumSize(QSize(50, 50));
        label->setScaledContents(true);//缩放显示的图片
        QPixmap pixmap(":/avatar.png");//资源文件中的图片路径都是以:开头的
        label->setPixmap(pixmap);//显示图片
        int y = i/lineNum;
        int x = i%lineNum;
        ui->gridLayout->addWidget(label, y, x);
    }
}

void MainWindow::findFace()
{
    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    cc.detectMultiScale(gray, faces, 1.1, 5, 0, Size(150, 150), Size(200, 200));
    
    for(auto i : faces)
    {
        rectangle(frame, i, Scalar(0, 0, 255), 2);
    }
}

void MainWindow::showImage()
{
    Mat rgb;
    cvtColor(frame, rgb, COLOR_BGR2RGB);
    
    QImage img(rgb.data, rgb.cols, rgb.rows, rgb.cols*rgb.channels(), QImage::Format_RGB888);
    ui->label_frame->setPixmap(QPixmap::fromImage(img));
    ui->label_frame->resize(rgb.cols, rgb.rows);
}

void MainWindow::initReconigzer()
{
    QFile file(FACE_MODEL_NAME);
    if(file.exists())
    {
        recognizer = FaceRecognizer::load<LBPHFaceRecognizer>("face.xml");
    }
    else
    {
        recognizer = LBPHFaceRecognizer::create();
    }
    recognizer->setThreshold(80);//设置阈值
}

void MainWindow::studyFace(int id)
{
    //人脸学习
    Mat face = frame(faces[0]);//假设只有一个人
    cvtColor(face, face, COLOR_BGR2GRAY);
    cv::resize(face, face, Size(128, 128));
    vector<Mat> studyFaces;
    studyFaces.push_back(face);
    vector<int> labels;
    labels.push_back(id);
    
    recognizer->update(studyFaces, labels);
    recognizer->save(FACE_MODEL_NAME);
}

int MainWindow::checkFace()
{
    if(recognizer->empty())
    {
        return -1;//人脸识别器为空不查找
    }
    Mat face = frame(faces[0]);
    cvtColor(face, face, COLOR_BGR2GRAY);
    cv::resize(face, face, Size(128, 128));
    
    int label;
    double cfds;
    recognizer->predict(face, label, cfds);
    return label;
}

void MainWindow::randSeet(int id)
{
    QLabel* label;
    do
    {
        int pos = rand()%SubjectInfo::getInstance()->getNum();//随机一个位置
        //获得对应位置的控件
        QWidget* w = ui->gridLayout->itemAt(pos)->widget();
        //返回值是QWidget类型，它是所有控件的基类类型，我们需要把它转化为QLabel类型，用动态转换
        label = dynamic_cast<QLabel*>(w);
    }while(repeatSeet(label));//循环判断随机的座位是否已经有人
    Mat face = frame(faces[0]);
    Mat rgb;
    cvtColor(face, rgb, COLOR_BGR2RGB);
    QImage img(rgb.data, rgb.cols, rgb.rows, rgb.cols*rgb.channels(), QImage::Format_RGB888); 
    label->setPixmap(QPixmap::fromImage(img));
    stus[id] = label;//保存学生和座位的信息
}

bool MainWindow::repeatSeet(QLabel *label)
{
    for(auto i : stus)
    {
        if(i.second == label)
        {
            return true;
        }
    }
    return false;
}


void MainWindow::on_pushButton_in_clicked()
{
    //获得考生考号
    QString strid = ui->lineEdit_stuid->text();
    bool ok;
    int stuid = strid.toInt(&ok);//将字符串转换为整数
    if(!ok)
    {
        QMessageBox::warning(this, "警告", "请正确输入考号");
        return;
    }
    if(stus.count(stuid) > 0)//考号不能重复
    {
        QMessageBox::warning(this, "警告", "该考生已经进入考场");
        return;
    }
    //判断是否有人脸
    if(faces.size() == 0)
    {
        QMessageBox::warning(this, "警告", "请正视摄像头，摘掉伪装！");
        return;
    }
   
    //学习人脸
    studyFace(stuid);
    //随机一个座位
    randSeet(stuid);
    
    //插入数据库
    ExamHistoryEntity ee;
    ee.subjectId = SubjectInfo::getInstance()->getSubjectId();
    ee.subjectName = SubjectInfo::getInstance()->getSubjectName();
    ee.stuid = stuid;
    qDebug()<<"insert : "<<stuid;
    qDebug()<<DBFactory::getInstance()->getExamHistoryDao()->addHistory(ee);
}

void MainWindow::on_pushButton_out_clicked()
{
    //判断是否有人脸
    if(faces.size() == 0)
    {
        QMessageBox::warning(this, "警告", "请正视摄像头，摘掉伪装！");
        return;
    }
    int stuid = checkFace();
    if(stuid == -1)
    {
        QMessageBox::warning(this, "警告", "离开考场失败!");
        return;
    }
    
    if(stus.count(stuid) == 0)
    {
        QMessageBox::warning(this, "警告", "考场中没有你！");
        return;
    }
    
    QLabel* label = stus[stuid];//获得学生对应的QLabel对象
    //清除学生图片
    label->setPixmap(QPixmap(":/avatar.png"));
    
    stus.erase(stuid);//从数据中删除学生
    
    //数据操作
    ExamHistoryEntity ee;
    ee.subjectId = SubjectInfo::getInstance()->getSubjectId();
    ee.subjectName = SubjectInfo::getInstance()->getSubjectName();
    ee.stuid = stuid;
    DBFactory::getInstance()->getExamHistoryDao()->updateHistory(ee);
}
