/*! @file qtosdk.cpp
 *  @version 3.4
 *  @date Dec 2017
 *
 *
 *  @Copyright (c) 2017 DJI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "qtosdk.hpp"
#include "ui_qtosdk.h"

using namespace DJI;
using namespace DJI::OSDK;

qtOsdk::qtOsdk(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::qtOsdk)
{
  ui->setupUi(this);
  refreshPort();
  vehicle = 0;
}

qtOsdk::~qtOsdk()
{
  delete ui;
  if (vehicle)
    delete vehicle;
}

void
qtOsdk::readAppIDKey()
{
  QFile f(":/UserConfig.txt");
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    DSTATUS ("No UserConfig.txt file found");
  else
  {
    while (!f.atEnd())
    {
      QByteArray line = f.readLine();
      if (line.startsWith("ID:"))
        ui->appIDInput->setText(line.remove(0, 3));
      else if (line.startsWith("KEY:"))
        ui->keyInput->setText(line.remove(0, 4));
    }
    f.close();
  }
}

void
qtOsdk::refreshPort()
{
  QString currentPort;

  if (ui->portSelection->currentText() != "Connect Serial")
  {
   currentPort = ui->portSelection->currentText();
  }

  ui->portSelection->clear();
  QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
  QStringList            list;
  for (int i = 0; i < ports.length(); ++i)
  {
    list.append(ports[i].portName());
  }
  if (ports.length()== 0)
      list.append("Connect Serial");

  ui->portSelection->addItems(list);

  if (ports.length() != 0)
  {
    ui->portSelection->setCurrentIndex(ui->portSelection->findText(currentPort));
  }

  if (currentPort.isEmpty())
  {
    ui->portSelection->setCurrentIndex(0);
  }
}

void
qtOsdk::on_initVehicle_clicked()
{
  if (ui->portSelection->currentText() == "Connect Serial")
    return;

  readAppIDKey();

  vehicle = new Vehicle(ui->portSelection->currentText().toStdString().c_str(),
                        ui->baudRateInput->text().toInt(), true);
  if (vehicle)
  {
    emit changeInitButton("Vehicle Initialized", true);
    QThread::msleep(100);

  }
  else
  {
    emit changeInitButton("Init Failed", false);
  }
}

void
qtOsdk::on_activateButton_clicked()
{
  Vehicle::ActivateData activateData;
  activateData.ID = static_cast<uint32_t>(ui->appIDInput->text().toUInt());
  QByteArray key  = ui->keyInput->text().toLocal8Bit();
  char       keyArray[65];
  activateData.encKey = keyArray;
  strcpy(activateData.encKey, key.toStdString().c_str());
  activateData.version = vehicle->getFwVersion();
  vehicle->activate(&activateData, activateCallback, this);
}

void
qtOsdk::activateCallback(Vehicle* vehicle, RecvContainer recvFrame,
                         UserData userData)
{
  qtOsdk*        sdk = (qtOsdk*)userData;
  ACK::ErrorCode ack_data;
  if (recvFrame.recvInfo.len - OpenProtocol::PackageMin <= 2)
  {
    ack_data.data = recvFrame.recvData.ack;
    ack_data.info = recvFrame.recvInfo;

    if (ACK::getError(ack_data))
    {
      emit sdk->changeActivateButton(
        QString("Activation Error: %1").arg(ack_data.data), false);
      ACK::getErrorCodeMessage(ack_data, __func__);
    }
    else
    {
      emit sdk->changeActivateButton(QString("Activation Successful"), true);

    }
  }
  else
  {
    emit sdk->changeActivateButton(QString("ACK Decode Error"), false);
  }

  // Do the stuff the OSDK callback does, since it is private and we cannot call
  // it here
  if (ack_data.data == OpenProtocolCMD::ErrorCode::ActivationACK::SUCCESS &&
      vehicle->getAccountData().encKey)
  {
    vehicle->protocolLayer->setKey(vehicle->getAccountData().encKey);
  }
}

void
qtOsdk::initComponents()
{
  this->rc = new Rc(0, this->vehicle);
  ui->componentTabs->addTab(rc, QString("RC Data"));
}

void
qtOsdk::initFinished(QString initStatus, bool initResult)
{
  if (initResult)
  {
    ui->initVehicle->setStyleSheet("background-color: qlineargradient(x1: 0, "
                                   "y1: 0, x2: 0, y2: 1, stop: 0 #44a8f2, "
                                   "stop: 1 #44a8f2); color:white");
    ui->initVehicle->setText(initStatus);

  }
  else
  {
    ui->initVehicle->setStyleSheet("background-color: qlineargradient(x1: 0, "
                                   "y1: 0, x2: 0, y2: 1, stop: 0 red, stop: 1 "
                                   "red); color:white");
    ui->initVehicle->setText(initStatus);
  }
}

void
qtOsdk::activateFinished(QString activateStatus, bool activateResult)
{
  if (!activateResult)
  {
    ui->activateButton->setText(activateStatus);
    ui->activateButton->setStyleSheet("background-color: qlineargradient(x1: "
                                      "0, y1: 0, x2: 0, y2: 1, stop: 0 "
                                      "#B4B4B4, stop: 1 #B4B4B4); color:white");
  }
  else
  {
    ui->activateButton->setText(activateStatus);
    ui->activateButton->setStyleSheet("background-color:qlineargradient(x1: 0, "
                                      "y1: 0, x2: 0, y2: 1, stop: 0 #44a8f2, "
                                      "stop: 1 #44a8f2); color:white");
    ui->hwVersionDisplay->setText(QString(vehicle->getHwVersion()));
    Version::FirmWare fwVersion = vehicle->getFwVersion();
    uint8_t           ver1      = (fwVersion >> 24) & 0x000000ff;
    uint8_t           ver2      = (fwVersion >> 16) & 0x000000ff;
    uint8_t           ver3      = (fwVersion >> 8) & 0x000000ff;
    uint8_t           ver4      = fwVersion & 0x000000ff;
    ui->fwVersionDisplay->setText(
      QString("%1.%2.%3.%4").arg(ver1).arg(ver2).arg(ver3).arg(ver4));
    // Initialize all the other parts of the SDK
    initComponents();
  }
}

void
qtOsdk::on_componentTabs_currentChanged(int index)
{
  for (int i = 0; i < ui->componentTabs->count(); i++)
    if (i != index)
      ui->componentTabs->widget(i)->setSizePolicy(QSizePolicy::Ignored,
                                                  QSizePolicy::Ignored);

  ui->componentTabs->widget(index)->setSizePolicy(QSizePolicy::Preferred,
                                                  QSizePolicy::Preferred);
  ui->componentTabs->widget(index)->resize(
    ui->componentTabs->widget(index)->sizeHint());
  ui->componentTabs->widget(index)->adjustSize();
  resize(sizeHint());
  adjustSize();
}

void qtOsdk::on_portSelection_activated(const QString &arg1)
{
    refreshPort();
}
