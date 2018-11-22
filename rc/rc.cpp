//*! @file rc.cpp

#include "rc.hpp"
#include "ui_rc.h"

#include <QDebug>

using namespace DJI::OSDK;

Rc::Rc(QWidget* parent, Vehicle* vehiclePtr)
  : QWidget(parent)
  , ui(new Ui::Rc)
{
  ui->setupUi(this);
  vehicle = vehiclePtr;
}

Rc::~Rc()
{
  delete ui;
}

void
Rc::display(Telemetry::TopicName topicName)
{
  switch (topicName)
  {
    case Telemetry::TOPIC_RC                        :
    {
      Telemetry::TypeMap<Telemetry::TOPIC_RC>::type simpleRC = vehicle->subscribe->getValue<Telemetry::TOPIC_RC>();
      ui->roll->setText(QString::number(simpleRC.roll));
      ui->pitch->setText(QString::number(simpleRC.pitch));
      ui->yaw->setText(QString::number(simpleRC.yaw));
      ui->throttle->setText(QString::number(simpleRC.throttle));
      ui->mode->setText(QString::number(simpleRC.mode));
      ui->gear->setText(QString::number(simpleRC.gear));

    }
    break;
    case Telemetry::TOPIC_RC_FULL_RAW_DATA           :
    {
      Telemetry::TypeMap<Telemetry::TOPIC_RC_FULL_RAW_DATA>::type advanceRC = vehicle->subscribe->getValue<Telemetry::TOPIC_RC_FULL_RAW_DATA>();
      ui->roll->setText(QString::number(advanceRC.lb2.roll));
      ui->pitch->setText(QString::number(advanceRC.lb2.pitch));
      ui->yaw->setText(QString::number(advanceRC.lb2.yaw));
      ui->throttle->setText(QString::number(advanceRC.lb2.throttle));
      ui->mode->setText(QString::number(advanceRC.lb2.mode));
      ui->gear->setText(QString::number(advanceRC.lb2.gear));
      ui->camera->setText(QString::number(advanceRC.lb2.camera));
      ui->video->setText(QString::number(advanceRC.lb2.video));
      ui->videoPause->setText(QString::number(advanceRC.lb2.videoPause));
      ui->goHome->setText(QString::number(advanceRC.lb2.goHome));
      ui->leftWheel->setText(QString::number(advanceRC.lb2.leftWheel));
      ui->rightWheel->setText(QString::number(advanceRC.lb2.rightWheelButton));

      ui->C1->setText(QString::number(advanceRC.lb2.rcC1));
      ui->C2->setText(QString::number(advanceRC.lb2.rcC2));
      ui->D1->setText(QString::number(advanceRC.lb2.rcD1));
      ui->D2->setText(QString::number(advanceRC.lb2.rcD2));
      ui->D3->setText(QString::number(advanceRC.lb2.rcD3));
      ui->D4->setText(QString::number(advanceRC.lb2.rcD4));
      ui->D5->setText(QString::number(advanceRC.lb2.rcD5));
      ui->D6->setText(QString::number(advanceRC.lb2.rcD6));
      ui->D7->setText(QString::number(advanceRC.lb2.rcD7));
      ui->D8->setText(QString::number(advanceRC.lb2.rcD8));

      ui->sbus0->setText(QString::number(advanceRC.sbus.data[0]));
      ui->sbus1->setText(QString::number(advanceRC.sbus.data[1]));
      ui->sbus2->setText(QString::number(advanceRC.sbus.data[2]));
      ui->sbus3->setText(QString::number(advanceRC.sbus.data[3]));
      ui->sbus4->setText(QString::number(advanceRC.sbus.data[4]));
      ui->sbus5->setText(QString::number(advanceRC.sbus.data[5]));
      ui->sbus6->setText(QString::number(advanceRC.sbus.data[6]));
      ui->sbus7->setText(QString::number(advanceRC.sbus.data[7]));
      ui->sbus8->setText(QString::number(advanceRC.sbus.data[8]));
      ui->sbus9->setText(QString::number(advanceRC.sbus.data[9]));
      ui->sbus10->setText(QString::number(advanceRC.sbus.data[10]));
      ui->sbus11->setText(QString::number(advanceRC.sbus.data[11]));
      ui->sbus12->setText(QString::number(advanceRC.sbus.data[12]));
      ui->sbus13->setText(QString::number(advanceRC.sbus.data[13]));
      ui->sbus14->setText(QString::number(advanceRC.sbus.data[14]));
      ui->sbus15->setText(QString::number(advanceRC.sbus.data[15]));

    }
    break;
    default: break;
      // clang-format on
  }
}

void
Rc::pkgUnpackCallback(Vehicle*      vehiclePtr,
                                   RecvContainer rcvContainer,
                                   UserData      userData)
{
  Rc* rc = (Rc*)userData;
  rc->display(static_cast<Telemetry::TopicName>(rc->selectedTopic));
}

#include <QDebug>

void
Rc::on_buttonSimple_clicked()
{
  vehicle->subscribe->removePackage(0);

  int  pkgIndex        = 0;
  uint16_t  freq       = 10;
  bool enableTimestamp = false;

  selectedTopic = 23;

  // Then, create a list. We need to do a second pass for this :-(
  Telemetry::TopicName topicList[Telemetry::TOTAL_TOPIC_NUMBER];
  topicList[0] = static_cast<Telemetry::TopicName>(selectedTopic);

  bool pkgStatus = vehicle->subscribe->initPackageFromTopicList(
    0 , 1, topicList, enableTimestamp, freq);

  if (pkgStatus)
    vehicle->subscribe->startPackage(pkgIndex);
  QThread::msleep(100);
  vehicle->subscribe->registerUserPackageUnpackCallback(0, pkgUnpackCallback,
                                                        this);
}

void
Rc::on_buttonAdvance_clicked()
{
  vehicle->subscribe->removePackage(0);

  int  pkgIndex        = 0;
  uint16_t  freq       = 10;
  bool enableTimestamp = false;

  selectedTopic = 35;

  // Then, create a list. We need to do a second pass for this :-(
  Telemetry::TopicName topicList[Telemetry::TOTAL_TOPIC_NUMBER];
  topicList[0] = static_cast<Telemetry::TopicName>(selectedTopic);

  bool pkgStatus = vehicle->subscribe->initPackageFromTopicList(
    0 , 1, topicList, enableTimestamp, freq);

  if (pkgStatus)
    vehicle->subscribe->startPackage(pkgIndex);
  QThread::msleep(100);
  vehicle->subscribe->registerUserPackageUnpackCallback(0, pkgUnpackCallback,
                                                        this);
}

void
Rc::on_buttonStop_clicked()
{
  vehicle->subscribe->removePackage(0);
}
