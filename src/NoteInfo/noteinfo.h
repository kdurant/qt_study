#ifndef NOTEINFO_H
#define NOTEINFO_H

#include <QWidget>

namespace Ui
{
class NoteInfo;
}

class NoteInfo : public QWidget
{
    Q_OBJECT

public:
    explicit NoteInfo(QWidget *parent = 0);
    ~NoteInfo();

private:
    Ui::NoteInfo *ui;
};

#endif  // NOTEINFO_H
