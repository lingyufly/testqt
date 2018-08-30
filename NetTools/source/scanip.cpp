#include "scanip.h"
#include <QtNetwork/QNetworkInterface>
#include <QtCore/QList>
#include <QLayout>
#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>

ScanIPWidget::ScanIPWidget(QWidget *parent, Qt::WindowFlags fl)
    : QWidget(parent, fl)
{
    setupUi();
    m_scanBtn->click();
}

ScanIPWidget::~ScanIPWidget()
{

}

void ScanIPWidget::setupUi()
{
    m_scanIP = new GetOnlineIP();
    connect(m_scanIP, &GetOnlineIP::getNewIP, this, &ScanIPWidget::addIP);

    QHBoxLayout *hbox = NULL;
    QVBoxLayout *vbox = NULL;
    m_mainLayout = new QGridLayout(this);
    m_InterfaceCombobox = new QComboBox(this);
    connect(m_InterfaceCombobox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ScanIPWidget::changeAdapter);
    m_scanBtn = new QPushButton("Scan", this);
    connect(m_scanBtn, &QPushButton::clicked, this, &ScanIPWidget::getInterfaceList);
    hbox = new QHBoxLayout(NULL);
    hbox->addWidget(m_InterfaceCombobox);
    hbox->addWidget(m_scanBtn);
    hbox->setStretch(0, 3);
    hbox->setStretch(1, 1);
    m_mainLayout->addLayout(hbox, 0, 0);

    m_addressTreeWidget = new QTreeWidget(this);
    m_addressTreeWidget->setHeaderLabels(QStringList() << "IP Address" << "IP Mask");
    connect(m_addressTreeWidget, &QTreeWidget::currentItemChanged, this, &ScanIPWidget::scanIP);
    m_addressTreeWidget->setIndentation(0);
    m_mainLayout->addWidget(m_addressTreeWidget);

    m_scanResultTreeWidget = new QTreeWidget(this);
    m_scanResultTreeWidget->setHeaderLabels(QStringList() << "IP Address" << "Mac Address" << "Hostname");
    m_mainLayout->addWidget(m_scanResultTreeWidget);

    m_scanProgressBar = new QProgressBar(this);
    m_stopScanBtn = new QPushButton("Stop", this);
    connect(m_stopScanBtn, &QPushButton::clicked, m_scanIP, &GetOnlineIP::stop);
    hbox = new QHBoxLayout(NULL);
    hbox->addWidget(m_scanProgressBar);
    hbox->addWidget(m_stopScanBtn);
    m_mainLayout->addLayout(hbox, 3, 0);
}

void ScanIPWidget::getInterfaceList()
{
    m_interfaceList = QNetworkInterface::allInterfaces();
    
    m_InterfaceCombobox->clear();
    for each(QNetworkInterface item in m_interfaceList)
    {
        m_InterfaceCombobox->addItem(item.name());
    }
}

void ScanIPWidget::changeAdapter()
{
    int index = m_InterfaceCombobox->currentIndex();
    m_addressTreeWidget->clear();
    if (index<0 || index>m_interfaceList.length())
        return;

    QTreeWidgetItem *item = NULL;
    QNetworkInterface interface = m_interfaceList.at(index);
    qDebug() << "Adapter Name:" << interface.name();
    qDebug() << "Adapter Address:" << interface.hardwareAddress();

    QList<QNetworkAddressEntry> addressEntryList = interface.addressEntries();
    for each(QNetworkAddressEntry addressEntryItem in addressEntryList)
    {
        if (addressEntryItem.ip().protocol() == QAbstractSocket::IPv4Protocol)
        {
            qDebug() << "------------------------------------------------------------";
            qDebug() << "IP Address:" << addressEntryItem.ip().toString();
            qDebug() << "IP Mask:" << addressEntryItem.netmask().toString();
            item = new QTreeWidgetItem(m_addressTreeWidget);
            item->setText(0, addressEntryItem.ip().toString());
            item->setText(1, addressEntryItem.netmask().toString());
        }
    }
}

void ScanIPWidget::scanIP()
{
    if (m_scanIP->isRunning())
    {
        m_scanIP->stop();
    }
    m_scanResultTreeWidget->clear();
    m_scanProgressBar->setValue(0);
    QTreeWidgetItem *item = m_addressTreeWidget->currentItem();
    if (item == NULL)
        return;
    
    m_scanIP->setIpInfo(item->text(0), item->text(1));
    if (m_scanIP->checkAddress())
    {
        int max = m_scanIP->getAllIP();
        m_scanProgressBar->setMaximum(max);
        m_scanIP->start();
    }
}

void ScanIPWidget::addIP(int num, ipinfo_t ipinfo)
{
    m_scanProgressBar->setValue(num);
    if (ipinfo.ip == QString::null)
        return;
    QTreeWidgetItem *item = new QTreeWidgetItem(m_scanResultTreeWidget);
    item->setText(0, ipinfo.ip);
}