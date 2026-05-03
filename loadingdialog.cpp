#include "loadingdialog.h"
#include <QVBoxLayout>

LoadingDialog::LoadingDialog(QWidget *parent) : QDialog(parent) {
    // Configuración estética
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(this);

    m_labelGif = new QLabel(this);
    m_labelText = new QLabel("Analizando factura...", this);

    // Estilo del texto
    m_labelText->setStyleSheet("color: white; font-weight: bold; background-color: rgba(0,0,0,160); "
                               "padding: 8px; border-radius: 10px;");

    // Configuración del GIF
    m_movie = new QMovie(":/iconos/graf/trabajando.gif"); // Asegúrate de que la ruta es correcta
    m_labelGif->setMovie(m_movie);
    m_movie->start();

    layout->addWidget(m_labelGif, 0, Qt::AlignCenter);
    layout->addWidget(m_labelText, 0, Qt::AlignCenter);
}

void LoadingDialog::setMessage(const QString &text) {
    m_labelText->setText(text);
}
