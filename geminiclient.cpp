#include "geminiclient.h"

GeminiClient::GeminiClient(QObject *parent)
    : QObject(parent) {
    m_manager = new QNetworkAccessManager(this);
}

void GeminiClient::requestJson(const QByteArray &fileData, const QString &mimeType) {
    //QUrl url(QString("https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=%1").arg(m_apiKey));
    QUrl url(QString(model_url+"?key="+m_apiKey));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Construcción del Payload siguiendo la estructura de Gemini
    QJsonObject inlineData;
    inlineData["mime_type"] = mimeType;
    inlineData["data"] = QString(fileData.toBase64()); // Codificación vital

    QJsonObject parts;
    parts["text"] = prompt;

    QJsonObject mediaPart;
    mediaPart["inline_data"] = inlineData;

    QJsonArray partsArray;
    partsArray.append(parts);
    partsArray.append(mediaPart);

    QJsonObject contents;
    contents["parts"] = partsArray;

    QJsonObject root;
    root["contents"] = QJsonArray{contents};

    // Forzamos respuesta en JSON puro
    QJsonObject genConfig;
    genConfig["response_mime_type"] = "application/json";
    root["generationConfig"] = genConfig;

    // Conectamos la señal de finalización a nuestro manejador
    QNetworkReply *reply = m_manager->post(request, QJsonDocument(root).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReplyFinished(reply);
    });
}

void GeminiClient::get_url(const QString qurl)
{
    model_url=qurl;
}

void GeminiClient::get_apiKey(const QString qapiKey)
{
    m_apiKey=qapiKey;
}

void GeminiClient::get_prompt(const QString qprompt)
{
    prompt=qprompt;
}

void GeminiClient::onReplyFinished(QNetworkReply *reply) {
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit error(reply->errorString() + ": " + reply->readAll());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (doc.isNull() || !doc.isObject()) {
        emit error("Respuesta no válida del servidor.");
        return;
    }

    // Extraemos el texto del JSON que Gemini mete dentro de su estructura de candidatos
    QJsonObject responseObj = doc.object();
    QString rawJsonText = responseObj["candidates"].toArray()[0].toObject()["content"].toObject()["parts"].toArray()[0].toObject()["text"].toString();

    // Convertimos ese string de vuelta a un QJsonObject real para el usuario
    QJsonDocument finalJson = QJsonDocument::fromJson(rawJsonText.toUtf8());
    emit finished(finalJson.object());
}
