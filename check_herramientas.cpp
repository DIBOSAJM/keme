#include "check_herramientas.h"
#include "ui_check_herramientas.h"
#include <QMovie>
#include "funciones.h"
#include "basedatos.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>


Check_herramientas::Check_herramientas(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Check_herramientas), m_networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    QString cad=ui->editor_latex_t_label->text();
    QString ed=editor_latex();
    cad.append(" ");
    if (ed.isEmpty()) cad.append(tr("No existente en preferencias"));
    else cad.append(ed);
    ui->editor_latex_t_label->setText(cad);
}


Check_herramientas::~Check_herramientas()
{
    delete ui;
}


void Check_herramientas::evalua()
{
    QMovie *movie1 = new QMovie(":/iconos/graf/trabajando.gif");
    movie1->setScaledSize(ui->latex_label->size());
    ui->latex_label->setMovie(movie1);
    ui->editor_text_label->setMovie(movie1);
    ui->pdf_images_label->setMovie(movie1);
    ui->tesseract_label->setMovie(movie1);
    ui->IA_config_label->setMovie(movie1);
    // ui->editor_text_label->setMovie(movie1);
    movie1->start();



    QEventLoop loop;
    QTimer::singleShot(1000, &loop, &QEventLoop::quit); // 1000 ms
    loop.exec();

    QString cadexec="pdflatex";
    if (es_os_x()) cadexec="/Library/TeX/texbin/xelatex";
    QString cadarg="--help";

    QObject *parent=NULL;

    QStringList arguments;
    arguments << adapta(cadarg);

    QProcess *myProcess = new QProcess(parent);

    myProcess->start(cadexec,arguments);

    // system(cadexec);
    QString ruta;
    if (!myProcess->waitForStarted ())
    {
        ruta=":/iconos/graf/borrar.png";
    }
    else {
        ruta=":/iconos/graf/ok-256x256.png";
    }
    delete myProcess;
    QPixmap icono(ruta);
    QSize size(30, 30); // Tamaño deseado
    QPixmap scaledPixmap = icono.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->latex_label->setPixmap(scaledPixmap);

    QTimer::singleShot(1000, &loop, &QEventLoop::quit); // 1000 ms
    loop.exec();

    cadexec=editor_latex();
    if (!cadexec.isEmpty()) {
       myProcess = new QProcess(parent);
       myProcess->start(cadexec,arguments);
       if (!myProcess->waitForStarted ())
       {
           ruta=":/iconos/graf/borrar.png";
       }
       else {
           ruta=":/iconos/graf/ok-256x256.png";
       }
    } else ruta=":/iconos/graf/borrar.png";
    icono.load(ruta);
    scaledPixmap = icono.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->editor_text_label->setPixmap(scaledPixmap);

    QTimer::singleShot(1000, &loop, &QEventLoop::quit); // 1000 ms
    loop.exec();

    cadexec="pdfimages";
    if (es_os_x()) cadexec="/opt/homebrew/bin/pdfimages";
    myProcess = new QProcess(parent);
    myProcess->start(cadexec,arguments);
    if (!myProcess->waitForStarted ())
        ruta=":/iconos/graf/borrar.png";
    else
        ruta=":/iconos/graf/ok-256x256.png";
    icono.load(ruta);
    scaledPixmap = icono.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->pdf_images_label->setPixmap(scaledPixmap);

    QTimer::singleShot(1000, &loop, &QEventLoop::quit); // 1000 ms
    loop.exec();

    cadexec="tesseract";
    if (es_os_x()) cadexec="/opt/homebrew/bin/tesseract";
    myProcess = new QProcess(parent);
    myProcess->start(cadexec,arguments);
    if (!myProcess->waitForStarted ())
        ruta=":/iconos/graf/borrar.png";
    else
        ruta=":/iconos/graf/ok-256x256.png";
    icono.load(ruta);
    scaledPixmap = icono.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->tesseract_label->setPixmap(scaledPixmap);

    // cargarmos url y apikey de IA
    QString api_url, api_key, prompt_factura, prompt_fact_emitida, prompt_prov_cli;
    bool ia_no_gestion_iva, ia_no_gestion_isp;
    bool mostrar_respuesta_ia;
    basedatos::instancia()->parametrosIA(&api_url, &api_key, &prompt_factura, &prompt_fact_emitida, &prompt_prov_cli,
                                         &ia_no_gestion_iva, &ia_no_gestion_isp, &mostrar_respuesta_ia);
    QString mensaje_error;
    if (evalua_ia(api_url,api_key,&mensaje_error,3000)) ruta=":/iconos/graf/ok-256x256.png";
      else ruta=":/iconos/graf/borrar.png";
      icono.load(ruta);
      scaledPixmap = icono.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
      ui->IA_config_label->setPixmap(scaledPixmap);
      ui->IA_config_t_label->setText(mensaje_error);
}

bool Check_herramientas::evalua_ia(const QString &apiUrl, const QString &apiKey, QString *mensajeError, int timeoutMs)
{
    // 1. Validaciones previas
    if (apiKey.trimmed().isEmpty()) {
        if (mensajeError) *mensajeError = "La API Key está vacía.";
        return false;
    }

    // URL por defecto si viene vacía
    QString finalUrlStr = apiUrl.trimmed();
    if (finalUrlStr.isEmpty()) {
        finalUrlStr = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent";
    }

    // Inyectar la API Key en los parámetros de consulta (query string)
    QUrl url(finalUrlStr);
    QUrlQuery query(url);
    query.addQueryItem("key", apiKey.trimmed());
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Construir el payload mínimo
    QJsonObject textPart;
    textPart["text"] = "ping";

    QJsonObject partsObject;
    QJsonArray partsArray;
    partsArray.append(textPart);
    partsObject["parts"] = partsArray;

    QJsonArray contentsArray;
    contentsArray.append(partsObject);

    QJsonObject payload;
    payload["contents"] = contentsArray;

    QJsonDocument doc(payload);

    // 2. Enviar petición HTTP
    QNetworkReply *reply = m_networkManager->post(request, doc.toJson());

    // 3. Crear EventLoop y Timer para el Timeout
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    // Salir del bucle si termina la red O si vence el temporizador
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    // Iniciar temporizador (por defecto 5000ms = 5 segundos)
    timer.start(timeoutMs);
    loop.exec(); // Bloquea de forma segura manteniendo viva la UI

    bool resultado = false;

    // 4. Comprobar si la salida ocurrió por timeout
    if (timer.isActive()) {
        // La petición terminó antes de agotar el tiempo
        timer.stop();

        if (reply->error() != QNetworkReply::NoError) {
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (mensajeError) {
                *mensajeError = QString("Error de red (%1): %2").arg(statusCode).arg(reply->errorString());
            }
        } else {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

            if (!jsonDoc.isObject()) {
                if (mensajeError) *mensajeError = "Respuesta de la IA con formato JSON no válido.";
            } else {
                QJsonObject rootObj = jsonDoc.object();

                if (rootObj.contains("error")) {
                    QJsonObject errorObj = rootObj["error"].toObject();
                    if (mensajeError) {
                        *mensajeError = errorObj["message"].toString("Error devuelto por la API de Gemini.");
                    }
                } else if (rootObj.contains("candidates")) {
                    if (mensajeError) *mensajeError = "Conexión y API Key correctas.";
                    resultado = true;
                } else {
                    if (mensajeError) *mensajeError = "Respuesta no reconocida de la API.";
                }
            }
        }
    } else {
        // Se agotó el temporizador (Timeout)
        reply->abort(); // Cancelar la petición HTTP pendiente
        if (mensajeError) {
            *mensajeError = QString("Tiempo de espera agotado (%1 ms). Sin respuesta de la IA.").arg(timeoutMs);
        }
    }

    reply->deleteLater();
    return resultado;
}

void Check_herramientas::on_comprobar_pushButton_clicked()
{
    evalua();
}

