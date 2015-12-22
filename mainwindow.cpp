#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GraphBuilder/graph_builder.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    test = new ff0x::RoundDial ( this );
    test->setObjectName( QStringLiteral("RoundDial") );
    {
        test->SetAngle( 210 );
        test->SetRange( -100, 100 );
        test->SetStep( 1 );
        ff0x::RoundDial::RangeList green_zone;
        green_zone.push_back( QPointF(-60, 60) );
        ff0x::RoundDial::RangeList yelow_zone;
        yelow_zone.push_back( QPointF(-90, -60) );
        yelow_zone.push_back( QPointF( 60, 90) );
        ff0x::RoundDial::RangeList red_zone;
        red_zone.push_back( QPointF(-100, -90) );
        red_zone.push_back( QPointF( 90, 100) );

        test->SetGreenRanges( green_zone );
        test->SetYellowRanges( yelow_zone );
        test->SetRedRanges( red_zone );

        test->EnableSaffetyRanges();
    }
    ui->widget->layout()->addWidget( test );


    test2 = new ff0x::RoundDial ( this );
    test2->setObjectName( QStringLiteral("RoundDial2") );
    {
        test2->SetAngle( 210 );
        test2->SetRange( -1000, 1000 );
        test2->SetStep( 50 );
        ff0x::RoundDial::RangeList green_zone;
        green_zone.push_back( QPointF(-600, 600) );
        ff0x::RoundDial::RangeList yelow_zone;
        yelow_zone.push_back( QPointF(-900, -600) );
        yelow_zone.push_back( QPointF( 600, 900) );
        ff0x::RoundDial::RangeList red_zone;
        red_zone.push_back( QPointF(-1000, -900) );
        red_zone.push_back( QPointF( 900, 1000) );

        test2->SetGreenRanges( green_zone );
        test2->SetYellowRanges( yelow_zone );
        test2->SetRedRanges( red_zone );

        test2->EnableSaffetyRanges();
    }

    ui->widget->layout()->addWidget( test2 );

    ff0x::GraphBuilder build( 840, 480 );
    ff0x::GraphBuilder::GraphDataType data;
    data.push_back( QPoint( -90, 90 ) );
    data.push_back( QPoint( -80, 90 ) );
    data.push_back( QPoint( -80, 80 ) );
    data.push_back( QPoint( -70, 80 ) );
    data.push_back( QPoint( -70, 70 ) );
    data.push_back( QPoint( -60, 70 ) );
    data.push_back( QPoint( -60, 60 ) );
    data.push_back( QPoint( -50, 60 ) );
    data.push_back( QPoint( -50, 50 ) );
    data.push_back( QPoint( -40, 50 ) );
    data.push_back( QPoint( -40, 40 ) );
    data.push_back( QPoint( -30, 40 ) );
    data.push_back( QPoint( -30, 30 ) );
    data.push_back( QPoint( -20, 30 ) );
    data.push_back( QPoint( -20, 20 ) );
    data.push_back( QPoint( -10, 20 ) );
    data.push_back( QPoint( -10, 10 ) );
    data.push_back( QPoint( 0, 0 ) );
    auto pixmap = build.Draw( data, 200, 100, 20, 20, "Бар.", "Шт." ,true );
    pixmap.save( "img.png", "PNG" );
}

MainWindow::~MainWindow()
{
    test->deleteLater();
    test2->deleteLater();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    test->update_value( test->Value() - 1 );
}

void MainWindow::on_pushButton_2_clicked()
{
    test->update_value( test->Value() + 1 );
}
