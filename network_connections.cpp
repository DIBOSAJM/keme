#include "network_connections.h"
#include <QNetworkRequest>
#include <QApplication>
#include <QEventLoop>
#include "funciones.h"
#include <QFile>
#include <QSslKey>
#include <QMessageBox>

vies_vatNumber::vies_vatNumber() {

}

void vies_vatNumber::validate(QString vatnumber) {

    loaded=false;

    mDataBuffer= new QByteArray;

    fault=false;
    fault_string.clear();

    valid=false;
    name.clear();
    address.clear();

    QString soapRequest = "<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"\n"
            "xmlns:tns1=\"urn:ec.europa.eu:taxud:vies:services:checkVat:types\"\n"
            "xmlns:impl=\"urn:ec.europa.eu:taxud:vies:services:checkVat\">\n"
            "<soap:Header>\n"
            "</soap:Header>\n"
            "<soap:Body>\n"
            "<tns1:checkVat xmlns:tns1=\"urn:ec.europa.eu:taxud:vies:services:checkVat:types\"\n"
            "xmlns=\"urn:ec.europa.eu:taxud:vies:services:checkVat:types\">\n"
            "<tns1:countryCode>%1</tns1:countryCode>\n"
            "<tns1:vatNumber>%2</tns1:vatNumber>\n"
              "</tns1:checkVat>\n"
            "</soap:Body>\n"
          "</soap:Envelope>\n";

    // %1 --> country code
    // %2 --> vat number
    QString url = "http://ec.europa.eu/taxation_customs/vies/services/checkVatService";
    //QString url = "http://ec.europa.eu/taxation_customs/vies/checkVatTestService";

    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"text/xml;charset=UTF-8");

    QString send_soap_request=soapRequest.arg(vatnumber.left(2)).arg(vatnumber.mid(2));

    // qDebug() << send_soap_request;

    mNetReply = mNetMan->post(request,send_soap_request.toUtf8());
    connect(mNetReply, &QIODevice::readyRead, this, &vies_vatNumber::OnDataReadyToRead);
    connect(mNetReply, &QNetworkReply::finished, this, &vies_vatNumber::OnProcessFinished);

}

void vies_vatNumber::OnDataReadyToRead() {
   mDataBuffer->append(mNetReply->readAll());
}

void vies_vatNumber::OnProcessFinished() {

    QString reply= *mDataBuffer;

    // qDebug() << reply;

    QDomDocument doc("RESPONSE");
    doc.setContent(reply);
    QDomNodeList fault_list = doc.elementsByTagName("soap:Fault");
    QDomNodeList response_list = doc.elementsByTagName("ns2:checkVatResponse");
    if (response_list.isEmpty()) response_list = doc.elementsByTagName("ns2:checkVatResponse");
    if (fault_list.isEmpty() && response_list.isEmpty()) {fault_string=tr("bad connection"); fault=true;}
    if (!fault_list.isEmpty()) {
        fault=true;
        QDomNode response_node=fault_list.at(0);
        QDomNode fault_string_node=response_node.namedItem("faultstring");
        if (!fault_string_node.isNull()) {
            QDomElement e = fault_string_node.toElement();
            if (!e.isNull()) fault_string=e.text();
        }
    }
     else {
        if (!response_list.isEmpty()) {
            QDomNode response_node=response_list.at(0);
            QDomNode valid_node=response_node.namedItem("ns2:valid");
            if (!valid_node.isNull()) {
                QDomElement e = valid_node.toElement();
                if (!e.isNull()) {
                    if (e.text()=="true") valid=true;
                }
            }
            QDomNode name_node=response_node.namedItem("ns2:name");
            if (!name_node.isNull()) {
                QDomElement e = name_node.toElement();
                if (!e.isNull()) {
                    name=e.text();
                }
            }
            QDomNode address_node=response_node.namedItem("ns2:address");
            if (!address_node.isNull()) {
                QDomElement e = address_node.toElement();
                if (!e.isNull()) {
                   address=e.text();
                }
            }
        }
       }

     // qDebug() << (fault ? "FALLO" : "NO FALLO");
     // qDebug() << fault_string;
    // qDebug() << (valid ? "VÁLIDO" : "NO VALIDO");
    // qDebug() << name;
    // qDebug() << address;

    delete mDataBuffer;
    delete mNetReply;
    loaded=true;
}


bool vies_vatNumber::query_processed() {

    QTime _Timer = QTime::currentTime().addMSecs(3000);
       while( (QTime::currentTime() < _Timer) && (!loaded))
           QCoreApplication::processEvents(QEventLoop::AllEvents, 5);

   return loaded;

}


bool vies_vatNumber::soap_fault() {
    return fault;
}

QString vies_vatNumber::fault_error() {
    return fault_string;
}


bool vies_vatNumber::valid_code() {
    return valid;
}

QString vies_vatNumber::response_name() {
    return name;
}

QString vies_vatNumber::response_address() {
    return address;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

aeat_soap::aeat_soap() {
    if (mNetMan!=NULL) delete(mNetMan);
    QNetworkAccessManager *manager=new QNetworkAccessManager();
    mNetMan=manager;

}


void aeat_soap::process(QString file, QString url, QString certificate_file, QString pw_key) {
    loaded=false;

    mDataBuffer= new QByteArray;

    fault=false;
    fault_string.clear();

    valid=false;

    QNetworkRequest request;;
    request.setUrl(url);

    //QSslConfiguration defaultConfig = QSslConfiguration::defaultConfiguration();
    //request->setSslConfiguration(defaultConfig);

    request.setHeader(QNetworkRequest::ContentTypeHeader,"text/xml;charset=UTF-8");
    QSslConfiguration configuration;

    QFile pkcs(certificate_file);
    pkcs.open(QFile::ReadOnly);

    QSslKey key;
    QSslCertificate cert;
    QList<QSslCertificate> imported_certs;
    QSslCertificate::importPkcs12(&pkcs,&key,&cert,&imported_certs,QByteArray::fromStdString(pw_key.toStdString()));
    pkcs.close();

    configuration.setLocalCertificate(cert);
    configuration.setCaCertificates(imported_certs);
    configuration.setPrivateKey(key);
    // configuration.setProtocol(QSsl::TlsV1_0);
    configuration.setProtocol(QSsl::TlsV1_2OrLater);
    // configuration.setProtocol(QSsl::TlsV1_1);
    // configuration.setPeerVerifyMode(QSslSocket::QueryPeer);
    configuration.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(configuration);

    QFile contentFile(file);
    if (!contentFile.open(QFile::ReadOnly | QFile::Text)) return;
    QTextStream in(&contentFile);
    QString contents= in.readAll();
    // qDebug() << contents;
    mNetReply = mNetMan->post(request,contents.toUtf8());
    // QMessageBox::information( 0, tr("INFO"),"prueba");

    connect(mNetReply, &QIODevice::readyRead, this, &aeat_soap::OnDataReadyToRead);
    connect(mNetReply, &QNetworkReply::finished, this, &aeat_soap::OnProcessFinished);

}


void aeat_soap::OnDataReadyToRead() {

    mDataBuffer->append(mNetReply->readAll());     

}

void aeat_soap::OnProcessFinished() {

    QString reply= *mDataBuffer;

    // qDebug() << reply;
    // qDebug() << mNetReply->errorString();
    QDomDocument wdoc("RESPONSE");
    wdoc.setContent(reply);
    doc=wdoc;


    delete mDataBuffer;
    delete mNetReply;
    loaded=true;

}

QDomDocument aeat_soap::response() {
    return doc;
}

bool aeat_soap::processed() {
    QTime _Timer = QTime::currentTime().addMSecs(60000);
       while( (QTime::currentTime() < _Timer) && (!loaded))
           QCoreApplication::processEvents(QEventLoop::AllEvents, 5);

   return loaded;
}

// --------------------------------------------------------------------------------

currency_exchange::currency_exchange()
{
    mNetReply=nullptr;
}

void currency_exchange::restrictions(QDate wdate, QString wbase)
{
    date=wdate;
    base=wbase;
}

void currency_exchange::load_data()
{
    loaded=false;
    mDataBuffer= new QByteArray;
    QUrl url;
    url.setScheme("https");
    url.setHost("api.vatcomply.com");
    //url.setPort(int);
    url.setPath("/rates");
    url.setQuery("base="+base+"&date="+date.toString("yyyy-MM-dd"));
    QNetworkRequest q(url);

    mNetReply = mNetMan->get(QNetworkRequest(q));
    connect(mNetReply, &QIODevice::readyRead, this, &currency_exchange::OnDataReadyToRead);
    connect(mNetReply, &QNetworkReply::finished, this, &currency_exchange::OnProcessFinished);
}

bool currency_exchange::data_loaded()
{
    QTime _Timer = QTime::currentTime().addMSecs(3000);
    while( (QTime::currentTime() < _Timer) && (!loaded))
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);

    return loaded;

}

bool currency_exchange::conn_error()
{
    return connection_error;
}

QString currency_exchange::conn_error_string()
{
    return network_reply_error_string;
}

int currency_exchange::conn_error_number()
{
    return network_reply_error;
}

QJsonObject currency_exchange::values()
{
    return exchanges;
}

void currency_exchange::OnDataReadyToRead()
{
    mDataBuffer->append(mNetReply->readAll());
}

void currency_exchange::OnProcessFinished()
{
    QJsonDocument doc = QJsonDocument::fromJson(*mDataBuffer);
    // QJsonArray array = doc.array();

    exchanges = doc.object();
    // qDebug() << *mDataBuffer;
    delete mDataBuffer;
    delete mNetReply;
    loaded=true;

}
