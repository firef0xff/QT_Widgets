#include "graph_builder.h"
#include <QPainter>

namespace ff0x
{
BasicGraphBuilder::BasicGraphBuilder( int width, int height, QFont font ):
    mWidth( width ),
    mHeight( height ),
    mFont( font )
{}
BasicGraphBuilder::~BasicGraphBuilder()
{}

void BasicGraphBuilder::SetSize( int width, int height )
{
    mWidth = width;
    mHeight = height;
    emit SettingsChanged();
}

void BasicGraphBuilder::SetFont( QFont font )
{
    mFont = font;
    emit SettingsChanged();
}
void BasicGraphBuilder::SetDrawGreed( bool draw )
{
    mDrawGreed = draw;
    emit SettingsChanged();
}

void BasicGraphBuilder::SetXStep( double val )
{
    mXStep = val;
    emit SettingsChanged();
}
void BasicGraphBuilder::SetYStep( double val )
{
    mYStep = val;
    emit SettingsChanged();
}

void BasicGraphBuilder::SetXLabel( QString val )
{
    mXLabel = val;
    emit SettingsChanged();
}
void BasicGraphBuilder::SetYLabel( QString val )
{
    mYLabel = val;
    emit SettingsChanged();
}

void BasicGraphBuilder::SetXrange( QPointF const& val )
{
    mXrange = val;
    emit DataChanged();
}
void BasicGraphBuilder::SetYrange( QPointF const& val )
{
    mYrange = val;
    emit DataChanged();
}

void BasicGraphBuilder::SetData ( GraphDataLine data )
{
    mData = std::move( data );
    mAutoXRange.reset();
    mAutoYRange.reset();
    emit DataChanged();
}

QPointF GetXRange( BasicGraphBuilder::LinePoints const& points )
{
    double min_x = 0.0;
    double max_x = 0.0;

    bool init = false;
    for( QPointF const& point : points )
    {
        if( point.x() == NAN )
            continue;
        if( point.x() == INFINITY )
            continue;
        if( !init )
        {
            min_x = point.x();
            max_x = point.x();
            init = true;
            continue;
        }

        if( min_x > point.x() )
            min_x = point.x();
        if( max_x < point.x() )
            max_x = point.x();
    }
    return QPointF( max_x, min_x );
}
QPointF GetYRange( BasicGraphBuilder::LinePoints const& points )
{
    double min_y = 0.0;
    double max_y = 0.0;

    bool init = false;
    for( QPointF const& point : points )
    {
        if( point.y() == NAN )
            continue;
        if( point.y() == INFINITY )
            continue;
        if( !init )
        {
            min_y = point.y();
            max_y = point.y();
            init = true;
            continue;
        }

        if( min_y > point.y() )
            min_y = point.y();
        if( max_y < point.y() )
            max_y = point.y();
    }
    return QPointF( max_y, min_y );
}
BasicGraphBuilder::RangeInfo BasicGraphBuilder::GetAutoXrange()
{
    if( mAutoXRange )
        return *mAutoXRange;

    QPointF range;
    RangeInfo res;

    bool init = false;
    for( Line const& line: mData )
    {
        range = MergeRanges( GetXRange( line.first ), range, init  );
        init = true;
    }

    DataLength( range, res.first, res.second );

    mAutoXRange.reset( new RangeInfo( res ) );
    return res;
}

BasicGraphBuilder::RangeInfo BasicGraphBuilder::GetAutoYrange()
{
    if( mAutoYRange )
        return *mAutoYRange;

    QPointF range;
    RangeInfo res;

    bool init = false;
    for( Line const& line: mData )
    {
        range = MergeRanges( GetYRange( line.first ), range, init  );
        init = true;
    }

    DataLength( range, res.first, res.second );

    mAutoYRange.reset( new RangeInfo( res ) );
    return res;
}

GraphBuilder::GraphBuilder(int width, int height, Mode mode, QFont font ):
    BasicGraphBuilder( width, height, font ),
    mMode( mode )
{}

QPixmap GraphBuilder::Draw() const
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

    int width = mWidth - 2;
    int height = mHeight - 2;
    int x_dist = width - x_pos;
    int y_dist = std::max( height - y_pos, y_pos );


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

    qreal x_skale = x_dist / std::max( fabs( mXrange.x() ),fabs( mXrange.y() ) );
    qreal y_skale = y_dist / std::max( fabs( mYrange.x() ),fabs( mYrange.y() ) );

    //нарисовать риски/сетку
    {
        painter.save();
        painter.setPen( Qt::lightGray );
        painter.setPen( mDrawGreed ? Qt::DotLine : Qt::SolidLine );

        qreal dash_lehgth = std::min( y_dist, x_dist )/100;

        // x
        {
            qreal st_y = 0;
            qreal sp_y = 0;
            if ( !mDrawGreed )
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
                qreal pos = 0 + mXStep;
                QPointF start( pos * x_skale, st_y ), stop( start.x(), sp_y );
                while ( start.x() < x_dist )
                {
                    painter.drawLine( start, stop );

                    QString t = QString::number(pos);
                    painter.drawText( stop.x() - metrix.width( t )/2 , dash_lehgth + metrix.height(), t );

                    pos += mXStep;
                    start.setX( pos * x_skale );
                    stop.setX( pos * x_skale );
                }
            }
            // отрицательный диапазон
            if ( mMode != PlusPlus )
            {
                qreal pos = 0 - mXStep;
                QPointF start( pos * x_skale, st_y ), stop( start.x(), sp_y );
                while ( start.x() > -x_dist )
                {
                    painter.drawLine( start, stop );

                    QString t = QString::number(pos);
                    painter.drawText( stop.x() - metrix.width( t )/2 , dash_lehgth + metrix.height(), t );

                    pos -= mXStep;
                    start.setX( pos * x_skale );
                    stop.setX( pos * x_skale );
                }
            }
        }

        // y
        {
            qreal st_x = 0;
            qreal sp_x = 0;
            if ( !mDrawGreed )
            {
                sp_x = dash_lehgth;
                st_x = -dash_lehgth;
            }
            else
            {
                st_x = -x_dist;
                if ( mMode == PlusPlus )
                {
                    st_x = 0;
                }
                sp_x = x_dist;
            }
            // положительный диапазон
            {
                qreal pos = 0 - mYStep;
                QPointF start( st_x, pos * y_skale ), stop( sp_x, start.y() );
                while ( start.y() > -y_dist )
                {
                    painter.drawLine( start, stop );

                    QString t = QString::number( -1 * pos);
                    painter.drawText( -dash_lehgth - metrix.width( t ), start.y() + metrix.height()/4, t );

                    pos -= mYStep;
                    start.setY( pos * y_skale );
                    stop.setY( pos * y_skale );
                }
            }
            // отрицательный диапазон
            if ( !(mMode == TopHalf || mMode == PlusPlus) )
            {
                qreal pos = 0 + mYStep;
                QPointF start( st_x, pos * y_skale ), stop( sp_x, start.y() );
                while ( start.y() < y_dist )
                {
                    painter.drawLine( start, stop );

                    QString t = QString::number( -1 * pos);
                    painter.drawText( -dash_lehgth - metrix.width( t ), start.y() + metrix.height()/4 , t );

                    pos += mYStep;
                    start.setY( pos * y_skale );
                    stop.setY( pos * y_skale );
                }
            }
        }

        // написать ед изм если есть

        painter.drawText( x_dist - metrix.width( mXLabel ), -4, mXLabel );
        painter.drawText( 4 , -y_dist + metrix.height(), mYLabel );


        painter.restore();
    }

    //нарисовать точки
    foreach (Line const& line, mData)
    {
        painter.setPen( line.second.mColor );
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
            else if ( mData.size() == 1 )
            {
                painter.drawPoint( start );
            }

            prevPoint = &p;
        }
    }

    //нарисовать легенду
    painter.save();
    int legend_y_ofset = 5;
    int legend_x_ofset = 5;
    painter.translate ( QPoint( legend_x_ofset-window_center.x(), legend_y_ofset-window_center.y() ) );
    int legend_width = 0;
    int legend_height = 0;
    foreach (Line const& line, mData)
    {
        int l_width = metrix.width(line.second.mName);
        QRect t( std::max( 20, x_dist/100*15 ), legend_height, l_width , metrix.height() );

        painter.setPen( Qt::black );
        painter.drawText( t, line.second.mName );
        legend_width = std::max( legend_width, l_width );

        legend_height += metrix.height();
        painter.setPen( line.second.mColor );

        painter.drawLine( 0, t.center().y(), std::max( 15, x_dist/100*10 ), t.center().y() );
    }
    painter.restore();

    return picture;
}


NoAxisGraphBuilder::NoAxisGraphBuilder( int width, int height, QFont font ):
    BasicGraphBuilder( width, height, font)
{
}


QPixmap NoAxisGraphBuilder::Draw() const
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
    foreach (Line const& line, mData)
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
    if ( !mXStep || !mYStep )
        return picture;

    int c = 0;
    if ( mDrawBorderCeil )
       c = 1;
    int top_x_steps = ceil(mXrange.x()/mXStep) + c;
    int top_y_steps = ceil(mYrange.x()/mYStep) + c;
    int bottom_x_steps = floor(mXrange.y()/mXStep) - c;
    int bottom_y_steps = floor(mYrange.y()/mYStep) - c;

    QPointF out_mXrange( top_x_steps * mXStep, bottom_x_steps * mXStep );
    QPointF out_mYrange( top_y_steps * mYStep, bottom_y_steps * mYStep );

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
        painter.setPen( mDrawGreed ? Qt::DotLine : Qt::SolidLine );

        qreal dash_lehgth = std::min( gr_width, gr_heigth )/100;

        // x
        {
            qreal st_y = 0;
            qreal sp_y = 0;
            if ( !mDrawGreed )
            {
                sp_y = gr_heigth + dash_lehgth;
                st_y = gr_heigth -dash_lehgth;
            }
            else
            {
                st_y = 0;
                sp_y = gr_heigth;
            }

            for ( qreal pos = out_mXrange.y(); pos <= out_mXrange.x(); pos += mXStep  )
            {
                double mark = round( pos * 1000 )/1000;
                QString t = QString::number( mark );
                painter.save();
                if( mark == 0.0 )
                {
                    auto pen = painter.pen();
                    pen.setColor( Qt::black );
                    pen.setStyle( Qt::SolidLine );
                    pen.setWidth( 2 );
                    painter.setPen( pen );
                }
                QPointF start( y_marks_width + TranclateToXAxis( pos, out_mXrange, gr_width ), st_y ), stop( start.x(), sp_y );
                painter.drawLine( start, stop );
                painter.drawText( stop.x() - metrix.width( t )/2 , gr_heigth + dash_lehgth + metrix.height(), t );
                painter.restore();
            }
        }

        // y
        {
            qreal st_x = 0;
            qreal sp_x = 0;
            if ( !mDrawGreed )
            {
                sp_x = y_marks_width + dash_lehgth;
                st_x = y_marks_width - dash_lehgth;
            }
            else
            {
                st_x = y_marks_width;
                sp_x = y_marks_width + gr_width;
            }

            for ( qreal pos = out_mYrange.y(); pos <= out_mYrange.x(); pos += mYStep  )
            {
                double mark = round( pos * 1000 )/1000;
                QString t = QString::number( mark );
                painter.save();
                if( mark == 0.0 )
                {
                    auto pen = painter.pen();
                    pen.setColor( Qt::black );
                    pen.setStyle( Qt::SolidLine );
                    pen.setWidth( 2 );
                    painter.setPen( pen );
                }
                QPointF start( st_x, gr_heigth - TranclateToYAxis( pos, out_mYrange, gr_heigth ) ), stop( sp_x, start.y() );
                painter.drawLine( start, stop );
                painter.drawText( y_marks_width - dash_lehgth - metrix.width( t ), start.y() + metrix.height()/4 , t );
                painter.restore();
            }
        }

        // написать ед изм если есть

        painter.drawText( y_marks_width + gr_width - metrix.width( mXLabel ), gr_heigth - 4, mXLabel );
        painter.drawText( y_marks_width + 4 , metrix.height(), mYLabel );
        painter.restore();
    }

    //нарисовать точки
    foreach (Line const& line, mData)
    {
        painter.save();
        auto pen = painter.pen();
        pen.setColor( line.second.mColor );
        pen.setStyle( Qt::SolidLine );
        pen.setWidth( 2 );
        painter.setPen( pen );

        QPointF const* prevPoint = nullptr;
        foreach (QPointF const& p, line.first)
        {
            if ( !prevPoint )
                prevPoint = &p;

            QPointF start = *prevPoint;
            start.setX( y_marks_width + TranclateToXAxis( start.x(), out_mXrange, gr_width ) );
            start.setY( gr_heigth - TranclateToYAxis( start.y(), out_mYrange, gr_heigth ) );
            if ( prevPoint != &p )
            {
                QPointF stop = p;
                stop.setX( y_marks_width + TranclateToXAxis( stop.x(), out_mXrange, gr_width ) );
                stop.setY( gr_heigth - TranclateToYAxis( stop.y(), out_mYrange, gr_heigth ) );

                painter.drawLine( start, stop );
            }
            else if ( mData.size() == 1 )
            {
                painter.drawPoint( start );
            }

            prevPoint = &p;
        }
        painter.restore();
    }
    return picture;
}

qreal NoAxisGraphBuilder::TranclateToXAxis( qreal value, QPointF mXrange, qreal garph_range ) const
{
    qreal data_range = mXrange.x() - mXrange.y();
    qreal value_ofset = value - mXrange.y();
    return value_ofset * garph_range / data_range;
}
qreal NoAxisGraphBuilder::TranclateToYAxis( qreal value, QPointF mYrange, qreal garph_range ) const
{
    return TranclateToXAxis( value, mYrange, garph_range );
}

void NoAxisGraphBuilder::DrawBorderCeil( bool v )
{
   mDrawBorderCeil = v;
}

Log10GraphBuilder::Log10GraphBuilder( int width, int height, QFont font ):
    NoAxisGraphBuilder( width, height, font )
{}

QPixmap Log10GraphBuilder::Draw() const
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
    foreach (Line const& line, mData)
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
    if ( !mYStep )
        return picture;

    int top_x_steps = ceil( mXrange.x() > 0 ? log10( mXrange.x() ) : 1.0 );
    int top_y_steps = ceil(mYrange.x()/mYStep) + 1;
    int bottom_x_steps = floor( mXrange.y() > 0 ? log10( mXrange.y() ) : 0.0 );
    int bottom_y_steps = floor(mYrange.y()/mYStep) - 1;

    QPointF out_mXrange( pow( 10, top_x_steps), pow( 10, bottom_x_steps ) );
    QPointF out_mYrange( top_y_steps * mYStep, bottom_y_steps * mYStep );

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
        painter.setPen( mDrawGreed ? Qt::DotLine : Qt::SolidLine );

        qreal dash_lehgth = std::min( gr_width, gr_heigth )/100;

        // x
        {
            qreal st_y = 0;
            qreal sp_y = 0;
            if ( !mDrawGreed )
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
            for ( qreal pos = out_mXrange.y(); pos <= out_mXrange.x(); pos +=  pow( 10, floor( log10( pos ) ) )  )
            {
                QPointF start( y_marks_width + TranclateToXAxis( pos, out_mXrange, gr_width ), st_y ), stop( start.x(), sp_y );
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
            if ( !mDrawGreed )
            {
                sp_x = y_marks_width + dash_lehgth;
                st_x = y_marks_width - dash_lehgth;
            }
            else
            {
                st_x = y_marks_width;
                sp_x = y_marks_width + gr_width;
            }

            for ( qreal pos = out_mYrange.y(); pos <= out_mYrange.x(); pos += mYStep  )
            {
                QPointF start( st_x, gr_heigth - TranclateToYAxis( pos, out_mYrange, gr_heigth ) ), stop( sp_x, start.y() );
                painter.drawLine( start, stop );
                QString t = QString::number( round( pos * 1000 )/1000 );
                painter.drawText( y_marks_width - dash_lehgth - metrix.width( t ), start.y() + metrix.height()/4 , t );

            }
        }

        // написать ед изм если есть

        painter.drawText( y_marks_width + gr_width - metrix.width( mXLabel ), gr_heigth - 4, mXLabel );
        painter.drawText( y_marks_width + 4 , metrix.height(), mYLabel );
        painter.restore();
    }

    //нарисовать точки
    foreach (Line const& line, mData)
    {
        painter.setPen( line.second.mColor );
        QPointF const* prevPoint = nullptr;
        foreach (QPointF const& p, line.first)
        {
            if ( !prevPoint )
                prevPoint = &p;

            QPointF start = *prevPoint;
            start.setX( y_marks_width + TranclateToXAxis( start.x(), out_mXrange, gr_width ) );
            start.setY( gr_heigth - TranclateToYAxis( start.y(), out_mYrange, gr_heigth ) );
            if ( prevPoint != &p )
            {
                QPointF stop = p;
                stop.setX( y_marks_width + TranclateToXAxis( stop.x(), out_mXrange, gr_width ) );
                stop.setY( gr_heigth - TranclateToYAxis( stop.y(), out_mYrange, gr_heigth ) );

                painter.drawLine( start, stop );
            }
            else if ( mData.size() == 1 )
            {
                painter.drawPoint( start );
            }

            prevPoint = &p;
        }
    }
    return picture;
}

qreal Log10GraphBuilder::TranclateToXAxis( qreal value, QPointF mXrange, qreal garph_range ) const
{
    if ( value <= 0 )
        return 0;
    qreal data_range = log10( mXrange.x() ) - log10( mXrange.y() );
    qreal value_ofset = log10( value ) - log10( mXrange.y() );
    return value_ofset * garph_range / data_range;
}
qreal Log10GraphBuilder::TranclateToYAxis( qreal value, QPointF mYrange, qreal garph_range ) const
{
    qreal data_range = mYrange.x() - mYrange.y();
    qreal value_ofset = value - mYrange.y();
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
