#include "noteinfo.h"
#include "ui_noteinfo.h"

NoteInfo::NoteInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteInfo)
{
    ui->setupUi(this);
}

NoteInfo::~NoteInfo()
{
    delete ui;
}
