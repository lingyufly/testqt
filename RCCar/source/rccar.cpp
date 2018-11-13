#include "rccar.h"
#include "config.h"
#include <QGridLayout>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QFile>
#include <QDebug>
#include <QLabel>
#include <QUrl>
#include <QNetworkReply>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QTimer>
#include <QSlider>
#include <QBitMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QButtonGroup>

ControlWidget::ControlWidget(QWidget *parent /*= NULL*/, Qt::WindowFlags fl/*= Qt::windowFlags()*/)
{
    setupUi();
}

ControlWidget::~ControlWidget()
{

}

void ControlWidget::setupUi()
{
    m_mainLayout = new QGridLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_picLabel = new QLabel(this);
    m_mainLayout->addWidget(m_picLabel);

    QPixmap pixmap;
    m_leftBtn = new QPushButton(this);
    m_leftBtn->setCheckable(true);
    m_leftBtn->setFlat(true);
    pixmap = QPixmap(":/image/turnleft.png");
    m_leftBtn->setIcon(pixmap);
    m_leftBtn->show();

    m_backBtn = new QPushButton(this);
    m_backBtn->setCheckable(true);
    m_backBtn->setFlat(true);
    pixmap = QPixmap(":/image/goback.png");
    m_backBtn->setIcon(pixmap);
    m_backBtn->show();

    m_rightBtn = new QPushButton(this);
    m_rightBtn->setCheckable(true);
    m_rightBtn->setFlat(true);
    pixmap = QPixmap(":/image/turnright.png");
    m_rightBtn->setIcon(pixmap);
    m_rightBtn->show();

    m_foreBtn = new QPushButton(this);
    m_foreBtn->setCheckable(true);
    m_foreBtn->setFlat(true);
    pixmap = QPixmap(":/image/gofore.png");
    m_foreBtn->setIcon(pixmap);
    m_foreBtn->show();

    m_drctBtnGroup = new QButtonGroup(this);
    m_drctBtnGroup->addButton(m_foreBtn);
    m_drctBtnGroup->addButton(m_backBtn);
    m_drctBtnGroup->addButton(m_leftBtn);
    m_drctBtnGroup->addButton(m_rightBtn);

    m_viewSlider = new QSlider(this);
    m_viewSlider->show();
    m_viewSlider->setOrientation(Qt::Horizontal);
    m_viewSlider->setMinimum(0);
    m_viewSlider->setMaximum(180);
    m_viewSlider->setValue(90);
    m_resetViewBtn = new QPushButton(this);
    pixmap = QPixmap(":/image/resetview.png");
    m_resetViewBtn->setIcon(pixmap);

    m_settingBtn = new QPushButton(this);
    m_settingBtn->setFlat(true);
    pixmap = QPixmap(":/image/setting.png");
    m_settingBtn->setIcon(pixmap);
    m_settingBtn->show();

    m_infoLabel = new QLabel(this);
    m_infoLabel->show();

    m_logLabel = new QLabel(this);
    m_logLabel->show();

    m_viewTimer = new QTimer(this);
    m_viewTimer->setInterval(20);

    connect(m_foreBtn, &QPushButton::pressed, this, &ControlWidget::slot_gofore);
    connect(m_backBtn, &QPushButton::pressed, this, &ControlWidget::slot_goback);
    connect(m_leftBtn, &QPushButton::pressed, this, &ControlWidget::slot_turnleft);
    connect(m_rightBtn, &QPushButton::pressed, this, &ControlWidget::slot_turnright);
    connect(m_drctBtnGroup, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonReleased), 
        this, &ControlWidget::slot_stop);
    connect(m_viewSlider, &QSlider::valueChanged, this, &ControlWidget::slot_viewChanged);
    connect(m_resetViewBtn, &QPushButton::clicked, this, &ControlWidget::slot_viewReset);
    connect(m_settingBtn, &QPushButton::clicked, this, &ControlWidget::slot_openSetting);

    connect(m_viewTimer, &QTimer::timeout, this, &ControlWidget::slot_viewTimer);
    connect(this, &ControlWidget::signal_logmsg, this, &ControlWidget::slot_showLog);
    m_foreBtn->setFocusPolicy(Qt::NoFocus);
    m_backBtn->setFocusPolicy(Qt::NoFocus);
    m_leftBtn->setFocusPolicy(Qt::NoFocus);
    m_rightBtn->setFocusPolicy(Qt::NoFocus);
    m_viewSlider->setFocusPolicy(Qt::NoFocus);
    m_settingBtn->setFocusPolicy(Qt::NoFocus);

    m_picManager = new QNetworkAccessManager(this);
    connect(m_picManager, &QNetworkAccessManager::finished, this, &ControlWidget::slot_showPic);

    m_size = size();
}

void ControlWidget::setVideoUrl(QString url)
{
    m_picRequest = QNetworkRequest(url);
    m_picManager->get(m_picRequest);
}

void ControlWidget::setControlUrl(QString url)
{

}

void ControlWidget::setShowInfo(bool shown)
{
    if (shown)
    {
        m_infoLabel->show();
    }
    else
    {
        m_infoLabel->setText("");
        m_infoLabel->hide();
    }
}

void ControlWidget::setShowLog(bool shown)
{
    if (shown)
    {
        m_logLabel->show();
    }
    else
    {
        m_logLabel->setText("");
        m_logLabel->hide();
    }
}

void ControlWidget::repaintCtl()
{
    int w = width();
    int h = height();

    int lth = w / 14;// The width and height of Btn
    int mng = 30;
    int space = 10;// The margin between btns

    int x = 0, y = 0;
    QPixmap pixmap;

    x = mng;
    y = h - (mng + lth + space + lth);
    m_leftBtn->resize(lth, lth);
    m_leftBtn->setIconSize(m_leftBtn->size());
    m_leftBtn->move(x, y);

    x = mng + lth + space;
    y = h - (mng + lth);
    m_backBtn->resize(lth, lth);
    m_backBtn->setIconSize(m_backBtn->size());
    m_backBtn->move(x, y);

    x = mng + lth + space + lth + space;
    y = h - (mng + lth + space + lth);
    m_rightBtn->resize(lth, lth);
    m_rightBtn->setIconSize(m_rightBtn->size());
    m_rightBtn->move(x, y);

    x = mng + lth + space;
    y = h - (mng + lth + space + lth + space + lth);
    m_foreBtn->resize(lth, lth);
    m_foreBtn->setIconSize(m_foreBtn->size());
    m_foreBtn->move(x, y);

    m_settingBtn->resize(20, 20);
    m_settingBtn->setIconSize(m_settingBtn->size());
    m_settingBtn->move(10, 10);

    m_infoLabel->resize(100, 50);
    m_infoLabel->move(w - (100 + 10), 10);

    m_logLabel->resize(100, 50);
    m_logLabel->move(10, 60);

    m_viewSlider->resize(200, 10);
    x = w - mng - 200;
    y = h - mng - 10 - 10;
    m_viewSlider->move(x, y);

    lth = 20;
    m_resetViewBtn->resize(lth, lth);
    m_resetViewBtn->setIconSize(m_resetViewBtn->size());
    x = w - mng - 100 - lth / 2;
    y = h - mng - 10 - 10 - lth;
    m_resetViewBtn->move(x, y);
    if (m_viewSlider->value() == 90)
        m_resetViewBtn->hide();
}

void ControlWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    if (m_size != size())
    {
        repaintCtl();
        m_size = size();
    }
}

void ControlWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void ControlWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;
    QWidget::keyPressEvent(event);
    if (event->key() == Qt::Key_W || event->key() == Qt::Key_Up)
    {
        m_ctrlaction &= 0xF0;
        m_ctrlaction |= GOFORE;
        m_foreBtn->setChecked(true);
        slot_gofore();
    }
    else if (event->key() == Qt::Key_A || event->key() == Qt::Key_Left)
    {
        m_ctrlaction &= 0xF0;
        m_ctrlaction |= TURNLEFT;
        m_leftBtn->setChecked(true);
        slot_turnleft();
    }
    else if (event->key() == Qt::Key_S || event->key() == Qt::Key_Down)
    {
        m_ctrlaction &= 0xF0;
        m_ctrlaction |= GOBACK;
        m_backBtn->setChecked(true);
        slot_goback();
    }
    else if (event->key() == Qt::Key_D || event->key() == Qt::Key_Right)
    {
        m_ctrlaction &= 0xF0;
        m_ctrlaction |= TURNRIGHT;
        m_rightBtn->setChecked(true);
        slot_turnright();
    }

    if (event->key() == Qt::Key_Q)
    {
        m_ctrlaction &= 0x0F;
        m_ctrlaction |= VIEWLEFT;
        if (!m_viewTimer->isActive())
            m_viewTimer->start();
    }
    else if (event->key() == Qt::Key_E)
    {
        m_ctrlaction &= 0x0F;
        m_ctrlaction |= VIEWRIGHT;
        if (!m_viewTimer->isActive())
            m_viewTimer->start();
    }
}

void ControlWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;
    QWidget::keyReleaseEvent(event);
    if (event->key() == Qt::Key_W || event->key() == Qt::Key_A || event->key() == Qt::Key_S || event->key() == Qt::Key_D)
    {
        if (m_drctBtnGroup->checkedButton() != NULL)
        {
            m_drctBtnGroup->checkedButton()->setChecked(false);
        }
        m_ctrlaction &= 0xF0;
        slot_stop();
    }
    else if (event->key() == Qt::Key_Q || event->key() == Qt::Key_E)
    {
        m_ctrlaction &= 0x0F;
        m_viewTimer->stop();
    }
}

void ControlWidget::slot_showPic(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        m_picLabel->setText("Connection error");
        m_picManager->get(m_picRequest);
        return;
    }
    QByteArray jpgdata = reply->readAll();
    QPixmap pix;
    pix.loadFromData(jpgdata);
    pix = pix.scaled(m_picLabel->size());

    m_picLabel->setPixmap(pix);

    m_picManager->get(m_picRequest);
}

void ControlWidget::slot_gofore()
{
    emit signal_logmsg("Go fore");
}

void ControlWidget::slot_goback()
{
    emit signal_logmsg("Go back");
}

void ControlWidget::slot_turnleft()
{
    emit signal_logmsg("Turn left");
}

void ControlWidget::slot_turnright()
{
    emit signal_logmsg("Turn right");
}

void ControlWidget::slot_stop()
{
    emit signal_logmsg("Stop");
}

void ControlWidget::slot_viewChanged(int angle)
{
    if (angle == 90)
        m_resetViewBtn->hide();
    else
        m_resetViewBtn->show();
    emit signal_logmsg(QString("View %1").arg(angle));
}

void ControlWidget::slot_viewReset()
{
    m_viewTimer->stop();
    m_viewSlider->setValue(90);
}

void ControlWidget::slot_viewTimer()
{
    int angle = m_viewSlider->value();
    if (m_ctrlaction&VIEWLEFT && angle > 0)
        angle -= 1;
    else if (m_ctrlaction&VIEWRIGHT && angle < 180)
        angle += 1;
    else
        m_viewTimer->stop();
    m_viewSlider->setValue(angle);
}

void ControlWidget::slot_showinfo(QString msg)
{
    if (!m_showInfo)
        return;
    m_logLabel->setText(msg);
    qDebug() << msg.toLocal8Bit().data();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.toLocal8Bit().data());
    QJsonObject jsonObj = jsonDoc.object();
    QStringList keys= jsonObj.keys();
    for (QList<QString>::iterator key = keys.begin(); key != keys.end(); key++)
        m_infoLabel->setText((*key) + ":" + jsonObj.value(*key).toString() + "\n");
}

void ControlWidget::slot_showLog(QString msg)
{
    if (!m_showLog)
        return;
    QString logmsg = msg + "\n" + m_logLabel->text();
    m_logLabel->setText(logmsg);
}

void ControlWidget::slot_openSetting()
{
    ConfigWidget config(this);
    QString url = QString::null;
    url = m_controlUrl.length() > 0 ? m_controlUrl : "http://192.168.2.199:8081";
    config.setControlUrl(url);
    url = m_videoUrl.length() > 0 ? m_videoUrl : "http://192.168.2.199:8080/?action=snapshot";
    config.setVideoUrl(url);
    config.setShowInfo(m_showInfo);
    config.setShowLog(m_showLog);
    if (config.exec() == QDialog::Accepted)
    {
        m_controlUrl = config.getControlUrl();
        m_videoUrl = config.getVideoUrl();
        m_showInfo = config.getShowInfo();
        m_showLog = config.getShowLog();

        setVideoUrl(m_videoUrl);
        setControlUrl(m_controlUrl);
        setShowInfo(m_showInfo);
        setShowLog(m_showLog);
    }
}
