#pragma once
#include <QPixmap>
#include <QFont>
#include <memory>

namespace ff0x
{

class BasicGraphBuilder : public QObject
{
    Q_OBJECT

public:
    struct LabelInfo
    {
        LabelInfo():
            mName( "" ),
            mColor( Qt::black )
        {}
        LabelInfo( QString name, Qt::GlobalColor color ):
            mName( name ),
            mColor( color )
        {}

        QString mName;
        Qt::GlobalColor mColor;
    };
    typedef QVector< QPointF > LinePoints;
    typedef std::pair< LinePoints, LabelInfo > Line;
    typedef QVector< Line > GraphDataLine;
    typedef std::pair< QPointF, double > RangeInfo;

    BasicGraphBuilder( int width, int height, QFont font );
    virtual ~BasicGraphBuilder();

    void SetSize( int width, int height );
    void SetFont( QFont font );

    void SetDrawGreed( bool draw );

    void SetXStep( double val = 1.0 );
    void SetYStep( double val = 1.0 );

    void SetXLabel( QString val = "X" );
    void SetYLabel( QString val = "Y" );

    void SetData ( GraphDataLine data );

    void SetXrange( QPointF const& val );
    void SetYrange( QPointF const& val );

    virtual QPixmap Draw() const = 0;

    RangeInfo GetAutoXrange();
    RangeInfo GetAutoYrange();

signals:

    void SettingsChanged();
    void DataChanged();

protected:
    int mWidth;
    int mHeight;
    bool mDrawGreed = false;
    double mXStep = 1.0;
    double mYStep = 1.0;

    QPointF mXrange;
    QPointF mYrange;

    QString mXLabel = "X";
    QString mYLabel = "Y";
    QFont mFont;
    GraphDataLine mData;

    std::unique_ptr< RangeInfo > mAutoXRange;
    std::unique_ptr< RangeInfo > mAutoYRange;
};
class GraphBuilder : public BasicGraphBuilder
{
public:
    enum Mode
    {
        PlusPlus,
        TopHalf,
        BottomHalf,
        Full,
    };
    GraphBuilder( int width, int height, Mode mode, QFont font = QFont() );
    ~GraphBuilder() = default;

    QPixmap Draw() const;
private:
    Mode mMode;
};

class NoAxisGraphBuilder :public BasicGraphBuilder
{
public:
    NoAxisGraphBuilder( int width, int height, QFont font = QFont() );
    ~NoAxisGraphBuilder() = default;

    void DrawBorderCeil( bool );

    QPixmap Draw() const;
protected:
    virtual qreal TranclateToXAxis( qreal value, QPointF x_range, qreal garph_range ) const;
    virtual qreal TranclateToYAxis( qreal value, QPointF y_range, qreal garph_range ) const;
    bool mDrawBorderCeil = true;
};

class Log10GraphBuilder : public NoAxisGraphBuilder
{
public:
    Log10GraphBuilder( int width, int height, QFont font = QFont() );
    ~Log10GraphBuilder() = default;

    QPixmap Draw() const;

protected:
    virtual qreal TranclateToXAxis( qreal value, QPointF x_range, qreal garph_range ) const;
    virtual qreal TranclateToYAxis( qreal value, QPointF y_range, qreal garph_range ) const;

};

void DataLength( QPointF const& range, QPointF &out_range, double &out_step );

QPointF MergeRanges( QPointF const& range1, QPointF const& range2, bool use_r2 = true );

}//namespace ff0x
