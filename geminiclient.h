#ifndef GEMINICLIENT_H
#define GEMINICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class GeminiClient : public QObject {
    Q_OBJECT
public:
    explicit GeminiClient(QObject *parent = nullptr);

    // Método principal: Envía el PDF/Imagen y el Prompt
    void requestJson(const QByteArray &fileData, const QString &mimeType);
    void get_url(const QString qurl);
    void get_apiKey(const QString qapiKey);
    void get_prompt(const QString qprompt);

signals:
    // Señal que se emite al recibir el JSON con éxito
    void finished(const QJsonObject &jsonResponse);
    // Señal en caso de error
    void error(const QString &errorMessage);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QString m_apiKey;
    QString model_url;
    QString prompt;
    QNetworkAccessManager *m_manager;
};

#endif // GEMINICLIENT_H
