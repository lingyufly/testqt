#include "mousewgt.h"
#include <QMouseEvent>
#include <QDebug>
#include <QCheckBox>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextBrowser> 
#include <QCheckBox>
#include <QPalette>

MouseWidget::MouseWidget(QWidget *parent) :
QWidget(parent)
{
    setupUi();

    pt = QTime::currentTime();
}

MouseWidget::~MouseWidget()
{
}

void MouseWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *hbox = new QHBoxLayout(NULL);
    QLabel *label = new QLabel("鼠标位置: ", this);
    m_mouseposLabel = new QLabel(this);
    m_mouseposLabel->setAlignment(Qt::AlignCenter);
    QSpacerItem *hspacer = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hbox->addWidget(label);
    hbox->addWidget(m_mouseposLabel);
    hbox->addItem(hspacer);
    mainLayout->addLayout(hbox);

    m_mouseTrackCbox = new QCheckBox("MouseTracking", this);
    mainLayout->addWidget(m_mouseTrackCbox);

    QSpacerItem *vspacer = new QSpacerItem(20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addItem(vspacer);

    m_textBrowser = new QTextBrowser(this);
    mainLayout->addWidget(m_textBrowser);

    setMouseTracking(false);

    connect(m_mouseTrackCbox, SIGNAL(toggled(bool)), this, SLOT(changeMouseTrckState(bool)));
}

void MouseWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        m_textBrowser->append("Mouse left button press");
    else
        m_textBrowser->append("Mouse right button press");
}

void MouseWidget::mouseMoveEvent(QMouseEvent *e)
{
    // 防止抖动
    ct = QTime::currentTime();
    if (pt.msecsTo(ct) <= 100)
        return;
    pt = ct;

    m_mouseposLabel->setText("X:" + QString::number(e->globalX()) + " Y:" + QString::number(e->globalY()));
    m_textBrowser->append("Mouse move");
}

void MouseWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        m_textBrowser->append("Mouse left button release");
    else
        m_textBrowser->append("Mouse right button release");
}

void MouseWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        m_textBrowser->append("Mouse double click");
    else
        m_textBrowser->append("Mouse double click");
}

void MouseWidget::changeMouseTrckState(bool stat)
{
    setMouseTracking(stat);
}
