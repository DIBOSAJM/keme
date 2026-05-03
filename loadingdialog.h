#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QMovie>

class LoadingDialog : public QDialog {
    Q_OBJECT // Importante para las funciones de Qt

public:
    explicit LoadingDialog(QWidget *parent = nullptr);
    void setMessage(const QString &text); // Por si quieres cambiar el texto dinámicamente

private:
    QLabel *m_labelGif;
    QLabel *m_labelText;
    QMovie *m_movie;
};

#endif // LOADINGDIALOG_H
