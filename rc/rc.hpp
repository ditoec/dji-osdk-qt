// @file rc.hpp

#ifndef RC_H
#define RC_H

#include <QWidget>
#include <dji_vehicle.hpp>

namespace Ui
{
class Rc;
}

class Rc : public QWidget
{
  Q_OBJECT

public:
  explicit Rc(QWidget* parent = 0,DJI::OSDK::Vehicle* vehiclePtr = 0);
  ~Rc();

  void display(DJI::OSDK::Telemetry::TopicName topicName);

private slots:
  void on_buttonSimple_clicked();
  void on_buttonAdvance_clicked();
  void on_buttonStop_clicked();

public:
  static void pkgUnpackCallback(DJI::OSDK::Vehicle*      vehiclePtr,
                                 DJI::OSDK::RecvContainer rcvContainer,
                                 DJI::OSDK::UserData      userData);

private:
  Ui::Rc* ui;
  int selectedTopic;
  DJI::OSDK::Vehicle* vehicle;
};

#endif // RC_H
