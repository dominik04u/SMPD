#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <QImage>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FSupdateButtonState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateDatabaseInfo()
{
    ui->FScomboBox->clear();
    for(unsigned int i=1; i<=database.getNoFeatures(); ++i)
        ui->FScomboBox->addItem(QString::number(i));

    ui->FStextBrowserDatabaseInfo->setText("noClass: " +  QString::number(database.getNoClass()));
    ui->FStextBrowserDatabaseInfo->append("noObjects: "  +  QString::number(database.getNoObjects()));
    ui->FStextBrowserDatabaseInfo->append("noFeatures: "  +  QString::number(database.getNoFeatures()));

}

void MainWindow::FSupdateButtonState(void)
{
    if(database.getNoObjects()==0)
    {
        FSsetButtonState(false);
    }
    else
        FSsetButtonState(true);

}


void MainWindow::FSsetButtonState(bool state)
{
   ui->FScomboBox->setEnabled(state);
   ui->FSpushButtonCompute->setEnabled(state);
   ui->FSpushButtonSaveFile->setEnabled(state);
   ui->FSradioButtonFisher->setEnabled(state);
   ui->FSradioButtonSFS->setEnabled(state);
}

void MainWindow::on_FSpushButtonOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open TextFile"), "", tr("Texts Files (*.txt)"));

    if ( !database.load(fileName.toStdString()) )
        QMessageBox::warning(this, "Warning", "File corrupted !!!");
    else
        QMessageBox::information(this, fileName, "File loaded !!!");

    FSupdateButtonState();
    updateDatabaseInfo();
}

void MainWindow::on_FSpushButtonCompute_clicked()
{
    int dimension = ui->FScomboBox->currentText().toInt();


    if( ui->FSradioButtonFisher ->isChecked())
    {
    if (dimension == 1 && database.getNoClass() == 2)
        {
            float FLD = 0, tmp;
            int max_ind = -1;

            //std::map<std::string, int> classNames = database.getClassNames();
            for (uint i = 0; i < database.getNoFeatures(); ++i)
            {
                std::map<std::string, float> classAverages;
                std::map<std::string, float> classStds;

                for (auto const &ob : database.getObjects())
                {
                    classAverages[ob.getClassName()] += ob.getFeatures()[i];
                    classStds[ob.getClassName()] += ob.getFeatures()[i] * ob.getFeatures()[i];
                }

                std::for_each(database.getClassCounters().begin(), database.getClassCounters().end(), [&](const std::pair<std::string, int> &it)
                {
                    classAverages[it.first] /= it.second;
                    classStds[it.first] = std::sqrt(classStds[it.first] / it.second - classAverages[it.first] * classAverages[it.first]);
                }
                );

                tmp = std::abs(classAverages[ database.getClassNames()[0] ] - classAverages[database.getClassNames()[1]]) / (classStds[database.getClassNames()[0]] + classStds[database.getClassNames()[1]]);

                if (tmp > FLD)
                {
                    FLD = tmp;
                    max_ind = i;
                }

              }

            ui->FStextBrowserDatabaseInfo->append("max_ind: "  +  QString::number(max_ind) + " " + QString::number(FLD));
          }
     }
}



void MainWindow::on_FSpushButtonSaveFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
    tr("Open TextFile"), "D:\\Users\\Krzysiu\\Documents\\Visual Studio 2015\\Projects\\SMPD\\SMPD\\Debug\\", tr("Texts Files (*.txt)"));

        QMessageBox::information(this, "My File", fileName);
        database.save(fileName.toStdString());
}

void MainWindow::on_PpushButtonSelectFolder_clicked()
{
}

void MainWindow::on_CpushButtonOpenFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open TextFile"), "", tr("Texts Files (*.txt)"));

    if ( !database.load(fileName.toStdString()) )
        QMessageBox::warning(this, "Warning", "File corrupted !!!");
    else
        QMessageBox::information(this, fileName, "File loaded !!!");

    FSupdateButtonState();
    updateDatabaseInfo();
}

void MainWindow::on_CpushButtonSaveFile_clicked()
{

}

void MainWindow::on_CpushButtonTrain_clicked()
{
    int partValue=ui->CplainTextEditTrainingPart->toPlainText().toInt(); //wartość pobrana z formularza, do dzielenia setu
    if(partValue>=100 || partValue<=0){
        ui->CtextBrowser->append("Błąd");
    }
    else{
        ui->CtextBrowser->append("Part: "+QString::number(partValue));
        testSet.clear();
        trainSet.clear();
        shuffledObjects.clear();
    //    //qDebug() << objects.size();
        int trainPart=(database.getObjects().size()*partValue)/100;
    //    //qDebug() << trainPart;

        for(int i=0; i< database.getObjects().size(); i++)
        {
                shuffledObjects.push_back(database.getObjects()[i]);
        }
    //        //std::copy( objects.begin(), objects.end(), shuffledObjects.begin());
        std::random_device rd;
        std::default_random_engine engine(rd());
        std::shuffle(std::begin(shuffledObjects), std::end(shuffledObjects), engine);
    //    std::copy( shuffledObjects.begin(), shuffledObjects.begin()+trainPart, trainSet.begin());
        for(int i=0; i< trainPart; i++)
        {
                trainSet.push_back(shuffledObjects[i]);
        }

    //    qDebug() << "trainset"<<trainSet.size();
    //    std::copy( shuffledObjects.begin()+trainPart, shuffledObjects.end(), testSet.begin());
        for(int i=trainPart; i< shuffledObjects.size(); i++)
        {
                testSet.push_back(shuffledObjects[i]); 
        }

        ui->CtextBrowser->append("TrainSet: "+QString::number(trainSet.size()));
        ui->CtextBrowser->append("TestSet: "+QString::number(testSet.size()));
    //    qDebug() << "trainset"<<testSet.size();
    }

}

void MainWindow::on_CpushButtonExecute_clicked()
{
    std::string classifier=ui->CcomboBoxClassifiers->currentText().toStdString();
    //qDebug() << database.getNoFeatures();
    if(classifier=="NN"){
        computeNN(1);
    }
    else if(classifier=="NM"){
         computeNM();
    }
    else if(classifier=="kNN"){
        int k=ui->CcomboBoxK->currentText().toInt();
        computeNN(k);
    }
    else if(classifier=="kNM"){

    }
}

//void MainWindow::computeNN(){
//    int correct=0,incorrect=0;
//    for (auto &test : testSet)
//        {
//            float minDist=10000;
//            std::string cClass="";
//            for (auto &train : trainSet)
//                {
//                    float distance=0;
//                    for(uint i=0;i<database.getNoFeatures();i++){
//                        distance+=(test.getFeatures()[i]-train.getFeatures()[i])*(test.getFeatures()[i]-train.getFeatures()[i]);
//                    }
//                    distance=sqrt(distance);
//                    if(distance<minDist){
//                        minDist=distance;
//                        cClass=train.getClassName();
//                    }
//                }
//            if(cClass==test.getClassName()){
//                correct++;
//            }
//            else{
//                incorrect++;
//            }
//        }
//     ui->CtextBrowser->append("Correct: "+QString::number(correct));
//     ui->CtextBrowser->append("Incorrect: "+QString::number(incorrect));
//     ui->CtextBrowser->append("Classifier efficiency: "+QString::number(float(correct) / float(testSet.size())*100));
//}

void MainWindow::computeNN(int k){
    int correct=0,incorrect=0;
     for (auto &test : testSet){
         float distance = 0;
         std::vector<std::pair<std::string,float>> distances;
         std::string cClass="";
         for (auto &train : trainSet)
             {
                 distance=0;
                 for(uint i=0;i<database.getNoFeatures();i++){
                     distance+=(test.getFeatures()[i]-train.getFeatures()[i])*(test.getFeatures()[i]-train.getFeatures()[i]);
                 }
                 distances.push_back(make_pair(train.getClassName(), sqrt(distance)));
             }
         sort(distances.begin(),distances.end(),[](std::pair<std::string, float> v1, std::pair<std::string, float> v2) { return v1.second < v2.second; });

         std::map<std::string,int> minDist;
         int i=0;

         for(auto const &it:distances){
             minDist[it.first]+=1;
             i++;
             if(i==k)
                 break;
         }
             int coutner=0;
             for(auto const &it:minDist){
                 if(coutner<it.second){
                     coutner=it.second;
                     cClass=it.first;
                 }
             }
             if(cClass==test.getClassName()){
                 correct++;
             }
             else{
                 incorrect++;
             }

     }

     ui->CtextBrowser->append("Correct: "+QString::number(correct));
     ui->CtextBrowser->append("Incorrect: "+QString::number(incorrect));
     ui->CtextBrowser->append("Classifier efficiency: "+QString::number(float(correct) / float(testSet.size())*100));
}

void MainWindow::computeNM(){
    int correct=0,incorrect=0;
    int counter=0;
    std::map<std::string, float[64]> avgs;
    for(uint i=0;i<database.getNoFeatures();i++){
       std::map<std::string, int> classCount;
        for (const auto &train : trainSet){
            avgs[train.getClassName()][i]+=train.getFeatures()[i];
            classCount[train.getClassName()]++;
            counter++;
        }
        for(const auto &it:avgs){
            avgs[it.first][i]/=classCount[it.first];
            counter++;
        }
    }
    for (auto &test : testSet){
        std::map<std::string,float> distance;
        float minDist=1000;
        std::string cClass;
        for(uint i=0;i<database.getNoFeatures();i++){
            for(const auto &it:avgs){
                distance[it.first]=(avgs[it.first][i]-test.getFeatures()[i])*(avgs[it.first][i]-test.getFeatures()[i]);
                counter++;
            }
        }
        for(const auto &it:distance){
            counter++;
            distance[it.first]=sqrt(distance[it.first]);
            if(minDist>distance[it.first]){
                minDist=distance[it.first];
                cClass=it.first;
            }
        }
        if(cClass==test.getClassName()){
            correct++;
        }
        else{
            incorrect++;
        }
    }
    ui->CtextBrowser->append("Correct: "+QString::number(correct));
    ui->CtextBrowser->append("Incorrect: "+QString::number(incorrect));
    ui->CtextBrowser->append("Classifier efficiency: "+QString::number(float(correct) / float(testSet.size())*100));
}

void MainWindow::on_CpushButtonClear_clicked()
{
     ui->CtextBrowser->clear();
}
