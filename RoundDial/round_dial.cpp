#include "round_dial.h"
#include <QPainter>
#include <QTimer>
#include <QTime>
#include <QTextItem>

namespace ff0x
{

namespace
{

enum : int
{
    DEF_DIAMETER = 250
};

}//namespace

RoundDial::RoundDial(QWidget *parent) :
    QWidget(parent),
    mMin(-100),
    mMax(100),
    mValue(0),
    mStep( 1 ),
    mMinAngle(-150),
    mMaxAngle(+150),
    mUseSafetyRanges( false )
{
}

RoundDial::~RoundDial()
{

}

void RoundDial::paintEvent( QPaintEvent* /*e */)
{
    int side = qMin(width(), height());
    int radius = side / 2;
    const QPoint center( 0, 0 );
    const QPoint nubmer_point( 0, -(radius - radius*0.11) );
    const QPoint arrow[4] = {
        QPoint( radius*0.02, radius*0.1),
        center,
        QPoint( -radius*0.02, radius*0.1),
        QPoint(0, -radius*0.9)
    };

    const QPoint lile_risk[2] = {
        QPoint( 0, -(radius - radius*0.01) ),
        QPoint( 0, -(radius - radius*0.05) )
    };
    const QPoint big_risk[2] = {
        QPoint( 0, -(radius - radius*0.01) ),
        QPoint( 0, -(radius - radius*0.09) )
    };

    QFont text_font = font();
    text_font.setPointSize( 6 * side/DEF_DIAMETER );

    static QColor arrow_body_color( 0x00, 0x00, 0x00 );//( 0x00, 0xff, 0x00 );
    static QColor arrow_border_color( 0xff, 0x66, 0x66 );

    QPoint window_center(  width() / 2, height() / 2 );

    QPainter painter( this );
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate ( window_center );

    painter.setFont( text_font );
    painter.setPen( Qt::SolidLine );

    //рисуем окружность
    painter.save();
    painter.drawEllipse( center, radius, radius );
    painter.restore();


    qreal step = ( mMaxAngle - mMinAngle )/ (mMax - mMin) * mStep;

    //рисуем зоны
    if ( mUseSafetyRanges )
    {
        auto draw_zones = [&]( RangeList const& range, QColor color )
        {
            foreach (auto zone, range)
            {
                painter.save();
                painter.rotate( -90 );

                auto start_angle = mMinAngle + ( qMax( zone.x(), mMin )  - mMin ) * step / mStep;
                auto stop_angle = mMinAngle + ( qMax( zone.y(), mMin )  - mMin ) * step / mStep;


                auto res = QRect( -radius, -radius, side, side );

                painter.setPen( Qt::NoPen );
                painter.setBrush( color );
                painter.drawPie( res, start_angle*16, (stop_angle - start_angle)*16 );

                painter.restore();
            }
        };

        draw_zones( mGreenRanges, Qt::green );
        draw_zones( mYellowRanges, Qt::yellow );
        draw_zones( mRedRanges, Qt::red );
    }

    //рисуем риски
    for ( qreal angle = mMinAngle; round( angle * 100 )/100 <= mMaxAngle; angle += step )
    {
        painter.save();
        painter.rotate( angle );

        qint32 ofset = ceil( round( ( ( angle - mMinAngle ) / step ) * 100 ) / 100 );

        if ( ofset % 5 != 0 )
            painter.drawLine( lile_risk[0], lile_risk[1] );
        else
            painter.drawLine( big_risk[0], big_risk[1] );

        if ( !( ofset % 10 ) )
        {
            painter.translate( nubmer_point );
            painter.rotate( 90 );
            QString text = QString::number( mMin + ofset * mStep );
            painter.drawText( 0, 0, text );
            painter.rotate( -90 );
        }
        //рисуем значение
        painter.restore();
    }


    painter.setPen( arrow_border_color );
    painter.setBrush( arrow_body_color );
    //рисуем стрелку
    painter.save();

    qreal pos = mValue - mMin;
    painter.rotate( mMinAngle + pos * step / mStep  );
    painter.drawConvexPolygon( arrow, 4 );
    painter.restore();



    /*QColor minuteColor(0, 127, 127, 191);

    int side = qMin(width(), height());
    QTime time = QTime::currentTime();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);

    painter.setPen(Qt::NoPen);
    painter.setBrush(hourColor);

    painter.save();
    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    painter.drawConvexPolygon(hourHand, 3);
    painter.restore();

    painter.setPen(hourColor);

    for (int i = 0; i < 12; ++i)
    {
        painter.drawLine(88, 0, 96, 0);
        painter.rotate(30.0);
    }


    painter.setPen(Qt::NoPen);
    painter.setBrush(minuteColor);

    painter.save();
    painter.rotate(6.0 * (time.minute() + time.second() / 60.0));
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();

    for (int j = 0; j < 60; ++j) {
        if ((j % 5) != 0)
            painter.drawLine(92, 0, 96, 0);
        painter.rotate(6.0);
    }*/
}


void RoundDial::SetRange( qreal min, qreal max )
{
    mMin = min;
    mMax = max;

    update_value( mValue );
}
void RoundDial::SetAngles( qreal min, qreal max )
{
    mMinAngle = min;
    mMaxAngle = max;

    update_value( mValue );
}

void RoundDial::SetStep( qreal step )
{
    mStep = step;

    update();
}
qreal RoundDial::Step ()
{
    return mStep;
}
qreal RoundDial::Value()
{
    return mValue;
}
void RoundDial::EnableSaffetyRanges( bool use )
{
    mUseSafetyRanges = use;
}
RoundDial::RangeList RoundDial::GreenRanges()
{
    return mGreenRanges;
}
RoundDial::RangeList RoundDial::YellowRanges()
{
    return mYellowRanges;
}
RoundDial::RangeList RoundDial::RedRanges()
{
    return mRedRanges;
}

void RoundDial::SetGreenRanges( RangeList const& ranges )
{
    mGreenRanges = ranges;
}
void RoundDial::SetYellowRanges( RangeList const& ranges )
{
    mYellowRanges = ranges;
}
void RoundDial::SetRedRanges( RangeList const& ranges )
{
    mRedRanges = ranges;
}

void RoundDial::update_value( qreal val )
{
    if ( val > mMin )
    {
        if ( val < mMax)
        {
            mValue = val;
        }
        else
            mValue = mMax;
    }
    else
        mValue = mMin;
    update();
}
}//namespace ff0x
