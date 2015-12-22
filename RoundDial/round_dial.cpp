#include "round_dial.h"
#include <QPainter>
#include <QTimer>
#include <QTime>
#include <QTextItem>
#include <math.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

namespace ff0x
{

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
    auto radius = height() / ( sin( (mMaxAngle +10 - 90.0)/180*M_PI ) + 1 );
    radius = qMin( width()/2.0, radius );

    auto hord_height = sin( (mMaxAngle +10 - 90.0)/180*M_PI ) * radius;
    auto w = 2 * radius;
    //auto h = radius + hord_height;
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
        QPoint( 0, -(radius - radius*0.04) )
    };
    const QPoint mid_risk[2] = {
        QPoint( 0, -(radius - radius*0.01) ),
        QPoint( 0, -(radius - radius*0.07) )
    };
    const QPoint big_risk[2] = {
        QPoint( 0, -(radius - radius*0.01) ),
        QPoint( 0, -(radius - radius*0.09) )
    };

    static QColor arrow_body_color( 0x00, 0x00, 0x00 );//( 0x00, 0xff, 0x00 );
    static QColor arrow_border_color( 0xff, 0x66, 0x66 );

    //QPoint window_center( radius,  radius );
    QPoint window_center(  qMax( static_cast<double>( width()/2 ), radius),  qMax( static_cast<double>( height()/2 ), radius) );
    auto border = QRectF( -radius, -radius, w, w );

    QPainter painter( this );
    painter.setRenderHint(QPainter::Antialiasing);

    //painter.drawRect(rect());
    painter.translate ( window_center );
    //painter.drawRect(border);

    painter.setFont( font() );
    painter.setPen( Qt::SolidLine );
    QFontMetrics metrix( font() );

    //рисуем окружность
    {
        painter.save();
        auto angle = qMax( qAbs(mMinAngle), qAbs(mMaxAngle) ) + 10;

        if ( angle > 180 )
            angle -= 10;

        painter.drawChord( border, (90-angle) * 16,  2 * angle *16 );
        painter.restore();
    }


    qreal step = ( mMaxAngle - mMinAngle )/ (mMax - mMin) * mStep;

    //рисуем зоны
    if ( mUseSafetyRanges )
    {
        auto draw_zones = [&]( RangeList const& range, QColor color )
        {
            foreach (auto zone, range)
            {
                painter.save();
                auto start_angle = mMaxAngle - ( qMax( zone.x(), mMin )  - mMin ) * step / mStep;
                auto stop_angle = mMaxAngle - ( qMax( zone.y(), mMin )  - mMin ) * step / mStep;

                painter.setPen( Qt::NoPen );
                painter.setBrush( color );
                painter.drawPie( border, (start_angle+90)*16, (stop_angle - start_angle)*16 );

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

        if ( !( ofset % 10 ) )
            painter.drawLine( big_risk[0], big_risk[1] );
        else if ( !( ofset % 5 ) )
            painter.drawLine( mid_risk[0], mid_risk[1] );
        else
            painter.drawLine( lile_risk[0], lile_risk[1] );

        if ( !( ofset % 10 ) )
        {
            painter.translate( nubmer_point );
            QString text = QString::number( mMin + ofset * mStep );

//            painter.rotate( 90 );

//            int x = 0;
//            int y = 0;

            int x = -metrix.width( text )/2;
            int y = metrix.height()/2;

            painter.drawText( x, y, text );
        }
        //рисуем значение
        painter.restore();
    }


    //рисуем стрелку
    {
        painter.save();
        painter.setPen( arrow_border_color );
        painter.setBrush( arrow_body_color );

        qreal pos = mValue - mMin;
        painter.rotate( mMinAngle + pos * step / mStep  );
        painter.drawConvexPolygon( arrow, 4 );
        painter.restore();
    }


    //пишем цену деления и еденици измерения
    {
        QString text = QString::number( mValue, 'g', 6 ) + " " + mUnits;
        auto width = metrix.width( text );
        auto max_height = hord_height;

        painter.drawText( -width/2, max_height - 3, text );
    }

    //пишем текущее значение
    {
        QString text = "Ц.д. " + QString::number( mStep, 'g', 6 ) + " " + mUnits;
        auto width = metrix.width( text );
        auto max_height = hord_height;

        painter.drawText( -width/2, max_height - 3 - metrix.height() - 3, text );
    }

}


QString RoundDial::Units()
{
    return mUnits;
}
void RoundDial::SetUnits( QString const& u )
{
    mUnits = u;
}

void RoundDial::SetRange( qreal min, qreal max )
{
    mMin = min;
    mMax = max;

    update_value( mValue );
}
void RoundDial::SetAngle( qreal angle )
{
    angle = qMax ( angle, 200.0 );
    mMinAngle = -angle / 2;
    mMaxAngle = angle / 2;

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
