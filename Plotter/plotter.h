#pragma once

#include <QWidget>
#include "../GraphBuilder/graph_builder.h"
#include <memory>

namespace Ui {
class Plotter;
}

class Plotter : public QWidget
{
    Q_OBJECT

public:
    explicit Plotter(std::unique_ptr< ff0x::BasicGraphBuilder > builder,
                      std::unique_ptr<QPointF> mXDefaults  = nullptr,
                      std::unique_ptr<QPointF> mYDefaults  = nullptr,
                      std::unique_ptr<double> DX    = nullptr,
                      std::unique_ptr<double> DY    = nullptr,
                      QWidget *parent = 0);
    ~Plotter();

private slots:
    void on_bt_x_scale_up_clicked();
    void on_bt_x_scale_down_clicked();
    void on_bt_y_scale_up_clicked();
    void on_bt_y_scale_down_clicked();
    void on_bt_0_scale_clicked();

private:
    Ui::Plotter *ui;
    std::unique_ptr< ff0x::BasicGraphBuilder > mBuilder;

    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    //базовые границы графика
    std::unique_ptr<QPointF> mXDefaults;
    std::unique_ptr<QPointF> mYDefaults;
    std::unique_ptr<double> mDX;
    std::unique_ptr<double> mDY;

    int64_t mXMove = 0; //в пикселях перемещение по оси X
    int64_t mYMove = 0; //в пикселях перемещение по оси Y
    int64_t mXScale = 0; //количество увеличений по оси X +в большую сторону - в меньшую
    int64_t mYScale = 0; //количество увеличений по оси Y +в большую сторону - в меньшую

    struct SeletionState
    {
        QPoint mStartPoint;
        QPoint mEndPoint;
    };
    std::unique_ptr<SeletionState> mSelection;
    std::unique_ptr<QPoint> mMove;

public slots:
    void OnDataUpdated( ff0x::BasicGraphBuilder::GraphDataLine data );
    void Repaint();
};

