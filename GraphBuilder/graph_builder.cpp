#include "graph_builder.h"
#include <QPainter>

namespace ff0x
{

GraphBuilder::GraphBuilder(int width, int height, Mode mode, QFont font ):
    mMode( mode ),
    mWidth( width ),
    mHeight( height ),
    mFont( font )
{}

QPixmap GraphBuilder::Draw(const GraphDataLine &data,
                             qreal x_interval,
                             qreal y_interval,
                             qreal x_step,
                             qreal y_step,
                             QString x_label,
                             QString y_label,
                             bool draw_greed ) const
{
    QPixmap picture( mWidth, mHeight );
    QPainter painter( &picture );
    painter.setFont( mFont );
    QFontMetrics metrix( painter.font() );
//    painter.setRenderHint(QPainter::Antialiasing);

    int x_pos = mWidth/2 - 1;
    int y_pos = mHeight/2 - 1;

    if ( mMode == TopHalf || mMode == PlusPlus)
        y_pos = mHeight - ( metrix.height() * 2 );
    if ( mMode == BottomHalf )
        y_pos = ( metrix.height() * 2 );
    if ( mMode == PlusPlus )
        x_pos = ( metrix.width("12345") * 2 );

    int legend_y_ofset = 5;
    int width = mWidth - 2;
    int height = mHeight - 2;
    int x_dist = width - x_pos;
    int legend_start_point = -(height - ( height - y_pos ) - legend_y_ofset  ) ;
    int y_dist = -legend_start_point - metrix.height() * data.size() ;

    // нарисовать рамку
    QRect border( 0, 0, mWidth - 1, mHeight - 1 );
    painter.fillRect( border, Qt::white );
    painter.drawRect( border );

    // переместить рисовальщик в центр изображения
    QPoint window_center ( x_pos, y_pos );
    painter.setPen( Qt::black );
    painter.setPen( Qt::SolidLine );
    painter.translate ( window_center );


    // нарисовать оси x и y
    painter.drawLine( -x_dist, 0, width, 0 );
    painter.drawLine( 0, -y_dist, 0, height );

    qreal x_skale = x_dist / x_interval;
    qreal y_skale = y_dist / y_interval;

    //нарисовать риски/сетку
    {
        painter.save();
        painter.setPen( Qt::lightGray );
        painter.setPen( draw_greed ? Qt::DotLine : Qt::SolidLine );

        qreal dash_lehgth = std::min( y_dist, x_dist )/100;

        // x
        {
            qreal st_y = 0;
            qreal sp_y = 0;
            if ( !draw_greed )
            {
                sp_y = dash_lehgth;
                st_y = -dash_lehgth;
            }
            else
            {
                st_y = -y_dist;
                sp_y = y_dist;
                if ( mMode == PlusPlus )
                {
                    sp_y = 0;
                }
            }
            // положительный диапазон
            {
                qreal pos = 0 + x_step;
                QPointF start( pos * x_skale, st_y ), stop( start.x(), sp_y );
                while ( start.x() < x_dist )
                {
                    painter.drawLine( start, stop );

                    QString t = QString::number(pos);
                    painter.drawText( stop.x() - metrix.width( t )/2 , dash_lehgth + metrix.height(), t );

                    pos += x_step;
                    start.setX( pos * x_skale );
                    stop.setX( pos * x_skale );
                }
            }
            // отрицательный диапазон
            if ( mMode != PlusPlus )
            {
                qreal pos = 0 - x_step;
                QPointF start( pos * x_skale, st_y ), stop( start.x(), sp_y );
                while ( start.x() > -x_dist )
                {
                    painter.drawLine( start, stop );

                    QString t = QString::number(pos);
                    painter.drawText( stop.x() - metrix.width( t )/2 , dash_lehgth + metrix.height(), t );

                    pos -= x_step;
                    start.setX( pos * x_skale );
                    stop.setX( pos * x_skale );
                }
            }
        }

        // y
        {
            qreal st_x = 0;
            qreal sp_x = 0;
            if ( !draw_greed )
            {
                sp_x = dash_lehgth;
                st_x = -dash_lehgth;
            }
            else
            {
                st_x = -x_dist;
                if ( mMode == TopHalf || mMode == PlusPlus )
                {
                    st_x = 0;
                }
                sp_x = x_dist;
            }
            // положительный диапазон
            {
                qreal pos = 0 + y_step;
                QPointF start( st_x, pos * y_skale ), stop( sp_x, start.y() );
                while ( start.y() < y_dist )
                {
                    painter.drawLine( start, stop );

                    QString t = QString::number( -1 * pos);
                    painter.drawText( -dash_lehgth - metrix.width( t ), start.y() + metrix.height()/4 , t );

                    pos += y_step;
                    start.setY( pos * y_skale );
                    stop.setY( pos * y_skale );
                }
            }
            // отрицательный диапазон
            if ( mMode == TopHalf || mMode == PlusPlus )
            {
                qreal pos = 0 - y_step;
                QPointF start( st_x, pos * y_skale ), stop( sp_x, start.y() );
                while ( start.y() > -y_dist )
                {
                    painter.drawLine( start, stop );

                    QString t = QString::number( -1 * pos);
                    painter.drawText( -dash_lehgth - metrix.width( t ), start.y() + metrix.height()/4, t );

                    pos -= y_step;
                    start.setY( pos * y_skale );
                    stop.setY( pos * y_skale );
                }
            }
        }

        // написать ед изм если есть

        painter.drawText( x_dist - metrix.width( x_label ), -4, x_label );
        painter.drawText( 4 , -y_dist + metrix.height(), y_label );


        painter.restore();
    }

    int legend_width = 0;
    int leged_height = 0;
    //нарисовать точки
    foreach (Line const& line, data)
    {
        int l_width = metrix.width(line.second.mName);
        QRect t( x_dist/100*25, legend_start_point - leged_height, l_width , metrix.height() );
        painter.drawText( t, line.second.mName );
        legend_width = std::max( legend_width, l_width );

        leged_height += metrix.height();
        painter.setPen( line.second.mColor );

        painter.drawLine( 0, t.center().y(), x_dist/100*20, t.center().y() );

        QPointF const* prevPoint = nullptr;
        foreach (QPointF const& p, line.first)
        {
            if ( !prevPoint )
                prevPoint = &p;

            QPointF start = *prevPoint;
            start.setX( start.x() * x_skale );
            start.setY( start.y() * y_skale  *-1 );
            if ( prevPoint != &p )
            {
                QPointF stop = p;
                stop.setX( stop.x() * x_skale );
                stop.setY( stop.y() * y_skale *-1 );

                painter.drawLine( start, stop );
            }
            else if ( data.size() == 1 )
            {
                painter.drawPoint( start );
            }

            prevPoint = &p;
        }
    }

    //нарисовать легенду


    return picture;
}

}//namespace ff0x
