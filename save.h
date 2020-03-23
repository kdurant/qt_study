#ifndef SAVE_H
#define SAVE_H

#include <QWidget>

namespace Ui {
class save;
}

class save : public QWidget
{
    Q_OBJECT

public:
    explicit save(QWidget *parent = 0);
    ~save();

private:
    Ui::save *ui;
};

#endif // SAVE_H
