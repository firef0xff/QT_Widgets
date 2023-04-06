#include "plotter.h"
#include "ui_plotter.h"
#include <QResizeEvent>
#include <QPaintEvent>
#include <QMouseEvent>

ff0x::BasicGraphBuilder::GraphDataLine MakeGraph()
{
    ff0x::BasicGraphBuilder::GraphDataLine data;
    data.resize(5);

    for( int x = -100; x <= 100; ++x )
    {
        data[0].first.push_back( QPointF( x, x*x-100 ) );
        data[1].first.push_back( QPointF( x, -x*x+100 ) );
        data[2].first.push_back( QPointF( x, sin( x*3.14/180 ) * 100 ) );
        data[3].first.push_back( QPointF( x, cos( x*3.14/180 ) * 100 ) );
        data[4].first.push_back( QPointF( x, 100.0/x ) );
    }

    data[0].second.mName = "1";
    data[1].second.mName = "2";
    data[2].second.mName = "3";
    data[3].second.mName = "4";
    data[4].second.mName = "5";

    data[0].second.mColor = Qt::red;
    data[1].second.mColor = Qt::green;
    data[2].second.mColor = Qt::blue;
    data[3].second.mColor = Qt::magenta;
    data[4].second.mColor = Qt::darkMagenta;

    return data;
}
Plotter::Plotter( std::unique_ptr<ff0x::BasicGraphBuilder> builder,
                  std::unique_ptr<QPointF> mXDefaults,
                  std::unique_ptr<QPointF> mYDefaults,
                  std::unique_ptr<double> DX  ,
                  std::unique_ptr<double> DY  ,
                  QWidget *parent ) :
    QWidget(parent),
    ui(new Ui::Plotter),
    mBuilder( std::move( builder ) ),
    mXDefaults( std::move( mXDefaults ) ),
    mYDefaults( std::move( mYDefaults ) ),
    mDX  ( std::move( DX ) ),
    mDY  ( std::move( DY ) )
{
    ui->setupUi(this);
    mBuilder->SetDrawGreed( true );
}

Plotter::~Plotter()
{
    delete ui;
}

void Plotter::resizeEvent(QResizeEvent *event)
{
    Repaint();
    QWidget::resizeEvent( event );
}
void Plotter::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent( event );
}
void Plotter::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::RightButton )
    {
        auto curs = ui->pixmap->cursor();
        curs.setShape( Qt::CrossCursor );
        ui->pixmap->setCursor( curs );

       mSelection.reset( new SeletionState );
       mSelection->mStartPoint = event->pos();
       mSelection->mEndPoint = event->pos();
    }
    else if( event->button() == Qt::LeftButton )
    {
        auto curs = ui->pixmap->cursor();
        curs.setShape( Qt::PointingHandCursor );
        ui->pixmap->setCursor( curs );

        mMove.reset( new QPoint( event->pos() ) );
    }
    QWidget::mousePressEvent( event );
}
void Plotter::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::RightButton )
    {
       if( !mSelection )
          return;
       mSelection->mEndPoint = event->pos();


//       auto start_point = RealPoint( mSelection->mStartPoint.X, mSelection->mStartPoint.Y );
//       auto end_point = RealPoint( mSelection->mEndPoint.X, mSelection->mEndPoint.Y );

//       AxisLimits x_limit;
//       x_limit.mMin = start_point.mX < end_point.mX ? start_point.mX : end_point.mX;
//       x_limit.mMax = start_point.mX > end_point.mX ? start_point.mX : end_point.mX;

//       AxisLimits y_limit;
//       y_limit.mMin = start_point.mY < end_point.mY ? start_point.mY : end_point.mY;
//       y_limit.mMax = start_point.mY > end_point.mY? start_point.mY : end_point.mY;

//       SetLimitsImpl( x_limit, y_limit );
//       mCurrentLine->mMoveX = 0;
//       mCurrentLine->mMoveY = 0;
//       mSelection.reset();
//       Draw();
    }
    else if( event->button() == Qt::LeftButton )
    {
        mMove.reset();
    }
    auto curs = ui->pixmap->cursor();
    curs.setShape( Qt::ArrowCursor );
    ui->pixmap->setCursor( curs );
    QWidget::mouseReleaseEvent( event );
}
void Plotter::mouseMoveEvent(QMouseEvent *event)
{
//    std::unique_lock<std::mutex>lock(mDataMutex);
//    if( !mCurrentLine )
//       return;

//    LinePoint p = RealPoint( X, Y );
//    xLabel->Caption = mCurrentLine->mXname + L": " + StringValue( p.mX ) + L" " + mCurrentLine->mYname + L": " + StringValue( p.mY );

//    auto point = TPoint(X,Y);
    if( mSelection )
       mSelection->mEndPoint = event->pos();
    if( mMove )
    {
       QPoint current = event->pos();
       mXMove += ( mMove->x() - current.x() );
       mYMove += ( mMove->y() - current.y() );
       mMove.reset( new QPoint( current ) );
    }

    if( mMove || mSelection )
        Repaint();
    QWidget::mouseReleaseEvent( event );
}

void Plotter::OnDataUpdated( ff0x::BasicGraphBuilder::GraphDataLine data )
{
    if( !mBuilder )
        return;
    ff0x::BasicGraphBuilder& builder = *mBuilder;
    builder.SetData( std::move( data ) );
    on_bt_0_scale_clicked();
}
void Plotter::Repaint()
{
    if( !mBuilder )
        return;

    ff0x::BasicGraphBuilder& builder = *mBuilder;

    auto *pix = ui->pixmap;
    builder.SetSize( pix->width(), pix->height() );

    auto auto_x_range = builder.GetAutoXrange();
    auto auto_y_range = builder.GetAutoYrange();

    if( mXDefaults )
        auto_x_range.first = ff0x::MergeRanges( auto_x_range.first, *mXDefaults );
    if( mYDefaults )
        auto_y_range.first = ff0x::MergeRanges( auto_y_range.first, *mXDefaults );

    if( mDX )
        auto_x_range.second = std::max( auto_x_range.second, *mDX );
    if( mDY )
        auto_y_range.second = std::max( auto_y_range.second, *mDY );

    builder.SetXStep( auto_x_range.second * pow( 2, -mXScale ) );
    builder.SetYStep( auto_y_range.second * pow( 2, -mYScale ) );
    QPointF x_range = QPointF( auto_x_range.first.x() * pow( 2, -mXScale ), auto_x_range.first.y() * pow( 2, -mXScale ) );
    QPointF y_range = QPointF( auto_y_range.first.x() * pow( 2, -mYScale ), auto_y_range.first.y() * pow( 2, -mYScale ) );

    double x_dist = x_range.x() - x_range.y();
    double y_dist = y_range.x() - y_range.y();
    double x_move = static_cast<double>( mXMove ) / static_cast<double>( pix->width() ) * static_cast<double>( x_dist );
    double y_move = static_cast<double>( mYMove ) / static_cast<double>( pix->height() ) * static_cast<double>( y_dist );
    x_range.setX( x_range.x() + x_move );
    x_range.setY( x_range.y() + x_move );
    y_range.setX( y_range.x() - y_move );
    y_range.setY( y_range.y() - y_move );


    builder.SetXrange( x_range );
    builder.SetYrange( y_range );

    auto pix_map = builder.Draw();
    pix->setScaledContents( false );
    pix->setMinimumSize( 10, 10 );
    pix->setPixmap( pix_map );
}

void Plotter::on_bt_x_scale_up_clicked()
{
    ++mXScale;
    Repaint();
}
void Plotter::on_bt_x_scale_down_clicked()
{
    --mXScale;
    Repaint();
}
void Plotter::on_bt_y_scale_up_clicked()
{
    ++mYScale;
    Repaint();
}
void Plotter::on_bt_y_scale_down_clicked()
{
    --mYScale;
    Repaint();
}
void Plotter::on_bt_0_scale_clicked()
{
    mXScale = 0;
    mYScale = 0;
    mXMove = 0;
    mYMove = 0;
    Repaint();
}
