#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui -> btStart, SIGNAL(clicked()), this, SLOT(btnStart()));
    connect(ui -> btStop, SIGNAL(clicked()), this, SLOT(btnStop()));
    connect(ui -> btPause, SIGNAL(clicked()), this, SLOT(btnPause()));
    connect(ui -> btContinue, SIGNAL(clicked()), this, SLOT(btnContinue()));
    connect(ui -> btApllySets, SIGNAL(clicked()), this, SLOT(apllySets()));
    connect(ui -> sbInterval, SIGNAL(editingFinished()), this, SLOT(applySpeed()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerEvent()));

    timer.setInterval(200);
    timer.stop();

    ui -> lStatus -> setText("<font color=\"red\">Stopped</font>");
    apllySets();
}

MainWindow::~MainWindow()
{
    delete ui;

    if(generation != NULL)
        delete generation;
}


void MainWindow::applySpeed()
{
    timer.setInterval(ui -> sbInterval -> value());
}

void MainWindow::timerEvent()
{
    ui -> lcdGeneration -> display(ui -> lcdGeneration -> value() + 1);
    generation -> genNext();

    Genetic::MinSearchIndividual* bestres = generation -> getIndividualBest();
    ui -> lcdBestScore -> display(bestres -> getScore());
    int r = ui -> tableWidget -> rowCount();
    int lc = ui -> tableWidget -> columnCount()-1;
    ui -> tableWidget -> setRowCount(r + 1);
    ui -> tableWidget-> setCurrentCell(r, 1);
    QString str;
    for(int c = 0; c < lc; ++c)
    {
        str.setNum((double)(bestres -> getDna()[c]));
        ui -> tableWidget -> setItem(r, c, new QTableWidgetItem(str));
    }
    double q = bestres -> getScore();
    if(ui -> rbMax -> isChecked())
        q *= -1;
    str.setNum(q);
    ui -> tableWidget -> setItem(r, lc, new QTableWidgetItem(str));
}

void MainWindow::btnStart()
{
    string fnc = ui -> tbFunction -> text().toStdString();
    if(ui -> rbMax -> isChecked())
        fnc = "0-(" + fnc + ")";
    if(!minSearchParser.parse(fnc))
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setText("Irregular expression!");
        mb.exec();
        return;
    }

   // minSearchParser.printNotation();

    if(generation != NULL)
        delete generation;

    generation = new Genetic::Generation< Genetic::MinSearchIndividual >
            (ui -> sbSize -> value(), &geneticSettings);
    generation -> init(time(NULL), max(2, minSearchParser.getVariablesCount()));

    ui -> lcdGeneration -> display(0);

    ui -> btPause -> setEnabled(true);
    ui -> btContinue -> setEnabled(false);
    ui -> btStop -> setEnabled(true);
    ui -> btStart -> setEnabled(false);

    ui -> gbSize -> setEnabled(false);
    ui -> gbFunction -> setEnabled(false);
    ui -> gbTargetValue -> setEnabled(false);

    timer.start();
    ui ->lStatus -> setText("<font color=\"green\">Running</font>");
    ui -> tableWidget -> clear();
    ui -> tableWidget -> setColumnCount(minSearchParser.getVariablesCount() + 1);
    ui -> tableWidget -> setRowCount(0);
    int lc = ui -> tableWidget -> columnCount()-1;
    QString str;
    for(int c = 0; c < lc; ++c)
    {
        str.setNum(c);
        str = "{" + str + "}";
        ui -> tableWidget -> setHorizontalHeaderItem(c, new QTableWidgetItem(str));
    }
    str = "Result";
    ui -> tableWidget -> setHorizontalHeaderItem(lc, new QTableWidgetItem(str));
}

void MainWindow::btnStop()
{
    ui -> btPause -> setEnabled(false);
    ui -> btContinue -> setEnabled(false);
    ui -> btStop -> setEnabled(false);
    ui -> btStart -> setEnabled(true);

    ui -> gbSize -> setEnabled(true);
    ui -> gbFunction -> setEnabled(true);
    ui -> gbTargetValue -> setEnabled(true);

    timer.stop();
    ui -> lStatus -> setText("<font color=\"red\">Stopped</font>");
}

void MainWindow::btnPause()
{
    ui -> btPause -> setEnabled(false);
    ui -> btContinue -> setEnabled(true);
    ui -> btStop -> setEnabled(true);
    ui -> btStart -> setEnabled(false);

    timer.stop();
    ui -> lStatus -> setText("<font color=\"blue\">Paused</font>");
}


void MainWindow::btnContinue()
{
    ui -> btPause -> setEnabled(true);
    ui -> btContinue -> setEnabled(false);
    ui -> btStop -> setEnabled(true);
    ui -> btStart -> setEnabled(false);

    timer.start();
    ui -> lStatus -> setText("<font color=\"green\">Running</font>");
}

void MainWindow::apllySets()
{
    bool errFound = false;

    Genetic::RecombinationType rt;
    Genetic::ParentsSelectionType pt;
    Genetic::NewGenerationSelectionType st;

    if(ui -> cbRecombination -> currentIndex() == -1)
        errFound = true;
    else
        rt = (Genetic::RecombinationType)(0x100 + (ui -> cbRecombination -> currentIndex()));

    if(ui -> cbParentsSelection -> currentIndex() == -1)
        errFound = true;
    else
        pt = (Genetic::ParentsSelectionType)(0x300 + (ui -> cbParentsSelection -> currentIndex()));

    if(ui -> cbGenerationSelection -> currentIndex() == -1)
        errFound = true;
    else
        st = (Genetic::NewGenerationSelectionType)(0x200 + (ui -> cbGenerationSelection -> currentIndex()));

    if(!errFound)
    {
        geneticSettings.setGenerationSelectionParameter(ui -> sbSelectionParameter -> value());
        geneticSettings.setMutationAttempts(ui -> sbMaxMutations -> value());
        geneticSettings.setMutationParameter(ui -> sbMutationParameter -> value());
        geneticSettings.setMutationProbability(ui -> sbMutationProbability -> value() / 100.0);
        geneticSettings.setNewGenerationSelectionType(st);
        geneticSettings.setParentsSelectionType(pt);
        geneticSettings.setRecombinationParameter(ui -> sbRecombParam -> value());
        geneticSettings.setRecombinationType(rt);
    }
    else
    {
        QMessageBox mb;
        mb.setIcon(QMessageBox::Critical);
        mb.setText("Genetic settings are incorrect and can`t be applied!");
        mb.exec();
    }
}

