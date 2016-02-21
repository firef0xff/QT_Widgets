#include "graph_builder.h"
#include <QPainter>

namespace ff0x
{
BasicGraphBuilder::BasicGraphBuilder( int width, int height, QFont font ):
    mWidth( width ),
    mHeight( height ),
    mFont( font )
{}


GraphBuilder::GraphBuilder(int width, int height, Mode mode, QFont font ):
    BasicGraphBuilder( width, height, font ),
    mMode( mode )
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
        QRect t( x_dist/100*25, legend_start_point + leged_height, l_width , metrix.height() );

        painter.setPen( Qt::black );
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


NoAxisGraphBuilder::NoAxisGraphBuilder( int width, int height, QFont font ):
    BasicGraphBuilder( width, height, font)
{
}


QPixmap NoAxisGraphBuilder::Draw(GraphDataLine const& data,
              QPointF x_range,
              QPointF y_range,
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

    //габариты зоны легенды
    int legend_y_ofset = 5;
    int legend_start_point = legend_y_ofset;

    // нарисовать рамку графика
    QRect border( 0, 0, mWidth - 1, mHeight - 1 );
    painter.fillRect( border, Qt::white );
    painter.drawRect( border );

    // нарисовать легенду
    foreach (Line const& line, data)
    {
        int l_width = metrix.width(line.second.mName);
        QRect t( mWidth/100*25, legend_start_point, l_width , metrix.height() );

        painter.setPen( Qt::black );
        painter.drawText( t, line.second.mName );

        legend_start_point += metrix.height();
        painter.setPen( line.second.mColor );

        painter.drawLine( mWidth/100*5, t.center().y(), mWidth/100*20, t.center().y() );
    }


    // переместить рисовальщик в начало изображения
    int legend_height = legend_start_point + legend_y_ofset;
    QPoint graph_start_point ( 0, legend_height );
    painter.setPen( Qt::black );
    painter.setPen( Qt::SolidLine );
    painter.translate ( graph_start_point );

    //вычислить количество шагов от 0 до минимального и максимального значения
    if ( !x_step || !y_step )
        return picture;

    int top_x_steps = ceil(x_range.x()/x_step) + 1;
    int top_y_steps = ceil(y_range.x()/y_step) + 1;
    int bottom_x_steps = floor(x_range.y()/x_step) - 1;
    int bottom_y_steps = floor(y_range.y()/y_step) - 1;

    QPointF out_x_range( top_x_steps * x_step, bottom_x_steps * x_step );
    QPointF out_y_range( top_y_steps * y_step, bottom_y_steps * y_step );

    //габариты подписей по осям x и y
    int y_marks_width = metrix.width("12345") * 2;
    int x_marks_height = metrix.height() * 2;

    //габариты зоны графика
    int gr_width = mWidth - 2 - y_marks_width - 10;
    int gr_heigth = mHeight - 2 - legend_height - x_marks_height;


    //нарисовать зону графика
    //|_
    {
        painter.save();
        painter.setPen( Qt::black );
        painter.setPen( Qt::SolidLine );
        painter.drawLine( y_marks_width, gr_heigth, y_marks_width + gr_width, gr_heigth); // _
        painter.drawLine( y_marks_width, 0, y_marks_width, gr_heigth); // |
        painter.restore();
    }

    //нарисовать риски/сетку
    {
        painter.save();
        painter.setPen( Qt::lightGray );
        painter.setPen( draw_greed ? Qt::DotLine : Qt::SolidLine );

        qreal dash_lehgth = std::min( gr_width, gr_heigth )/100;

        // x
        {
            qreal st_y = 0;
            qreal sp_y = 0;
            if ( !draw_greed )
            {
                sp_y = gr_heigth + dash_lehgth;
                st_y = gr_heigth -dash_lehgth;
            }
            else
            {
                st_y = 0;
                sp_y = gr_heigth;
            }

            for ( qreal pos = out_x_range.y(); pos <= out_x_range.x(); pos += x_step  )
            {
                QPointF start( y_marks_width + TranclateToXAxis( pos, out_x_range, gr_width ), st_y ), stop( start.x(), sp_y );
                painter.drawLine( start, stop );
                QString t = QString::number( round( pos * 1000 )/1000 );
                painter.drawText( stop.x() - metrix.width( t )/2 , gr_heigth + dash_lehgth + metrix.height(), t );

            }
        }

        // y
        {
            qreal st_x = 0;
            qreal sp_x = 0;
            if ( !draw_greed )
            {
                sp_x = y_marks_width + dash_lehgth;
                st_x = y_marks_width - dash_lehgth;
            }
            else
            {
                st_x = y_marks_width;
                sp_x = y_marks_width + gr_width;
            }

            for ( qreal pos = out_y_range.y(); pos <= out_y_range.x(); pos += y_step  )
            {
                QPointF start( st_x, gr_heigth - TranclateToYAxis( pos, out_y_range, gr_heigth ) ), stop( sp_x, start.y() );
                painter.drawLine( start, stop );
                QString t = QString::number( round( pos * 1000 )/1000 );
                painter.drawText( y_marks_width - dash_lehgth - metrix.width( t ), start.y() + metrix.height()/4 , t );

            }
        }

        // написать ед изм если есть

        painter.drawText( y_marks_width + gr_width - metrix.width( x_label ), gr_heigth - 4, x_label );
        painter.drawText( y_marks_width + 4 , metrix.height(), y_label );
        painter.restore();
    }

    //нарисовать точки
    foreach (Line const& line, data)
    {
        painter.setPen( line.second.mColor );
        QPointF const* prevPoint = nullptr;
        foreach (QPointF const& p, line.first)
        {
            if ( !prevPoint )
                prevPoint = &p;

            QPointF start = *prevPoint;
            start.setX( y_marks_width + TranclateToXAxis( start.x(), out_x_range, gr_width ) );
            start.setY( gr_heigth - TranclateToYAxis( start.y(), out_y_range, gr_heigth ) );
            if ( prevPoint != &p )
            {
                QPointF stop = p;
                stop.setX( y_marks_width + TranclateToXAxis( stop.x(), out_x_range, gr_width ) );
                stop.setY( gr_heigth - TranclateToYAxis( stop.y(), out_y_range, gr_heigth ) );

                painter.drawLine( start, stop );
            }
            else if ( data.size() == 1 )
            {
                painter.drawPoint( start );
            }

            prevPoint = &p;
        }
    }
    return picture;
}

qreal NoAxisGraphBuilder::TranclateToXAxis( qreal value, QPointF x_range, qreal garph_range ) const
{
    qreal data_range = x_range.x() - x_range.y();
    qreal value_ofset = value - x_range.y();
    return value_ofset * garph_range / data_range;
}
qreal NoAxisGraphBuilder::TranclateToYAxis( qreal value, QPointF y_range, qreal garph_range ) const
{
    return TranclateToXAxis( value, y_range, garph_range );
}

Log10GraphBuilder::Log10GraphBuilder( int width, int height, QFont font ):
    NoAxisGraphBuilder( width, height, font )
{}

QPixmap Log10GraphBuilder::Draw(GraphDataLine const& data,
              QPointF x_range,
              QPointF y_range,
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

    //габариты зоны легенды
    int legend_y_ofset = 5;
    int legend_start_point = legend_y_ofset;

    // нарисовать рамку графика
    QRect border( 0, 0, mWidth - 1, mHeight - 1 );
    painter.fillRect( border, Qt::white );
    painter.drawRect( border );

    // нарисовать легенду
    foreach (Line const& line, data)
    {
        int l_width = metrix.width(line.second.mName);
        QRect t( mWidth/100*25, legend_start_point, l_width , metrix.height() );

        painter.setPen( Qt::black );
        painter.drawText( t, line.second.mName );

        legend_start_point += metrix.height();
        painter.setPen( line.second.mColor );

        painter.drawLine( mWidth/100*5, t.center().y(), mWidth/100*20, t.center().y() );
    }


    // переместить рисовальщик в начало изображения
    int legend_height = legend_start_point + legend_y_ofset;
    QPoint graph_start_point ( 0, legend_height );
    painter.setPen( Qt::black );
    painter.setPen( Qt::SolidLine );
    painter.translate ( graph_start_point );

    //вычислить количество шагов от 0 до минимального и максимального значения
    if ( !y_step )
        return picture;

    int top_x_steps = ceil( x_range.x() > 0 ? log10( x_range.x() ) : 1.0 );
    int top_y_steps = ceil(y_range.x()/y_step) + 1;
    int bottom_x_steps = floor( x_range.y() > 0 ? log10( x_range.y() ) : 0.0 );
    int bottom_y_steps = floor(y_range.y()/y_step) - 1;

    QPointF out_x_range( pow( 10, top_x_steps), pow( 10, bottom_x_steps ) );
    QPointF out_y_range( top_y_steps * y_step, bottom_y_steps * y_step );

    //габариты подписей по осям x и y
    int y_marks_width = metrix.width("12345") * 2;
    int x_marks_height = metrix.height() * 2;

    //габариты зоны графика
    int gr_width = mWidth - 2 - y_marks_width - 10;
    int gr_heigth = mHeight - 2 - legend_height - x_marks_height;


    //нарисовать зону графика
    //|_
    {
        painter.save();
        painter.setPen( Qt::black );
        painter.setPen( Qt::SolidLine );
        painter.drawLine( y_marks_width, gr_heigth, y_marks_width + gr_width, gr_heigth); // _
        painter.drawLine( y_marks_width, 0, y_marks_width, gr_heigth); // |
        painter.restore();
    }

    //нарисовать риски/сетку
    {
        painter.save();
        painter.setPen( Qt::lightGray );
        painter.setPen( draw_greed ? Qt::DotLine : Qt::SolidLine );

        qreal dash_lehgth = std::min( gr_width, gr_heigth )/100;

        // x
        {
            qreal st_y = 0;
            qreal sp_y = 0;
            if ( !draw_greed )
            {
                sp_y = gr_heigth + dash_lehgth;
                st_y = gr_heigth -dash_lehgth;
            }
            else
            {
                st_y = 0;
                sp_y = gr_heigth;
            }

            double old_mark = -1.0;
            for ( qreal pos = out_x_range.y(); pos <= out_x_range.x(); pos +=  pow( 10, floor( log10( pos ) ) )  )
            {
                QPointF start( y_marks_width + TranclateToXAxis( pos, out_x_range, gr_width ), st_y ), stop( start.x(), sp_y );
                painter.drawLine( start, stop );

                double mark = floor( log10( pos ) );
                if ( mark != old_mark )
                {
                    old_mark = mark;
                    QString t = QString::number( round( pos * 1000 )/1000 );
                    painter.drawText( stop.x() - metrix.width( t )/2 , gr_heigth + dash_lehgth + metrix.height(), t );
                }

            }
        }

        // y
        {
            qreal st_x = 0;
            qreal sp_x = 0;
            if ( !draw_greed )
            {
                sp_x = y_marks_width + dash_lehgth;
                st_x = y_marks_width - dash_lehgth;
            }
            else
            {
                st_x = y_marks_width;
                sp_x = y_marks_width + gr_width;
            }

            for ( qreal pos = out_y_range.y(); pos <= out_y_range.x(); pos += y_step  )
            {
                QPointF start( st_x, gr_heigth - TranclateToYAxis( pos, out_y_range, gr_heigth ) ), stop( sp_x, start.y() );
                painter.drawLine( start, stop );
                QString t = QString::number( round( pos * 1000 )/1000 );
                painter.drawText( y_marks_width - dash_lehgth - metrix.width( t ), start.y() + metrix.height()/4 , t );

            }
        }

        // написать ед изм если есть

        painter.drawText( y_marks_width + gr_width - metrix.width( x_label ), gr_heigth - 4, x_label );
        painter.drawText( y_marks_width + 4 , metrix.height(), y_label );
        painter.restore();
    }

    //нарисовать точки
    foreach (Line const& line, data)
    {
        painter.setPen( line.second.mColor );
        QPointF const* prevPoint = nullptr;
        foreach (QPointF const& p, line.first)
        {
            if ( !prevPoint )
                prevPoint = &p;

            QPointF start = *prevPoint;
            start.setX( y_marks_width + TranclateToXAxis( start.x(), out_x_range, gr_width ) );
            start.setY( gr_heigth - TranclateToYAxis( start.y(), out_y_range, gr_heigth ) );
            if ( prevPoint != &p )
            {
                QPointF stop = p;
                stop.setX( y_marks_width + TranclateToXAxis( stop.x(), out_x_range, gr_width ) );
                stop.setY( gr_heigth - TranclateToYAxis( stop.y(), out_y_range, gr_heigth ) );

                painter.drawLine( start, stop );
            }
            else if ( data.size() == 1 )
            {
                painter.drawPoint( start );
            }

            prevPoint = &p;
        }
    }
    return picture;
}

qreal Log10GraphBuilder::TranclateToXAxis( qreal value, QPointF x_range, qreal garph_range ) const
{
    if ( value <= 0 )
        return 0;
    qreal data_range = log10( x_range.x() ) - log10( x_range.y() );
    qreal value_ofset = log10( value ) - log10( x_range.y() );
    return value_ofset * garph_range / data_range;
}
qreal Log10GraphBuilder::TranclateToYAxis( qreal value, QPointF y_range, qreal garph_range ) const
{
    qreal data_range = y_range.x() - y_range.y();
    qreal value_ofset = value - y_range.y();
    return value_ofset * garph_range / data_range;
}

void DataLength( QPointF const& range, QPointF &out_range, double &out_step )
{
    double range_len = range.x() - range.y();
    if ( range_len <= 0.001 )
    {
        out_range.setX( range.x() + 5 );
        out_range.setY( range.y() - 5 );
        out_step = 1;
        return;
    }

    bool set = false;
    for ( double st = 0.01, prev_st = st/10.0; !set; prev_st = st, st *= 10 )
    {
        if ( range_len < st )
        {
            out_step = prev_st;
            set = true;
        }
        else if ( range_len < 2.5 * st )
        {
            out_step = 2.5 * prev_st;
            set = true;
        }
        else if ( range_len < 5 * st )
        {
            out_step = 5 * prev_st;
            set = true;
        }
    }

    out_range.setX( ceil( range.x() / out_step ) * out_step );
    out_range.setY( floor( range.y() / out_step ) * out_step );
}
QPointF MergeRanges( QPointF const& range1, QPointF const& range2, bool use_r2 )
{
    if ( use_r2 )
    {
        QPointF range;
        range.setX( std::max( range1.x(), range2.x() ) );
        range.setY( std::min( range1.y(), range2.y() ) );
        return std::move( range );
    }
    else
    {
        return range1;
    }
}

}//namespace ff0x
