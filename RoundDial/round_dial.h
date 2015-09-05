#pragma once
#include <QWidget>

namespace ff0x
{

class RoundDial : public QWidget
{
    Q_OBJECT

public:
    typedef QVector< QPointF > RangeList;
    explicit RoundDial(QWidget *parent = 0);
    ~RoundDial();

    void SetRange( qreal min, qreal max );
    void SetAngles( qreal min, qreal max );

    void SetStep( qreal step );

    qreal Step ();
    qreal Value();

    void EnableSaffetyRanges( bool use = true );

    RangeList GreenRanges();
    RangeList YellowRanges();
    RangeList RedRanges();

    void SetGreenRanges( RangeList const& ranges );
    void SetYellowRanges( RangeList const& ranges );
    void SetRedRanges( RangeList const& ranges );

public slots:
    void update_value( qreal val );

protected:
    void paintEvent(QPaintEvent *);

    qreal mMin; //минимальное значение
    qreal mMax; //максимальное значение
    qreal mValue; //текущее значение
    qreal mStep; // цена деления

    qreal mMinAngle; //минимальный угол на табло
    qreal mMaxAngle; //максимальный угол на табло

    bool mUseSafetyRanges;

    RangeList mGreenRanges;
    RangeList mYellowRanges;
    RangeList mRedRanges;
};


}//namespace name

