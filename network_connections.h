#ifndef NETWORK_CONNECTIONS_H
#define NETWORK_CONNECTIONS_H

#include <QtNetwork/QNetworkReply>
#include <QDomDocument>

class vies_vatNumber : public QObject {

  public:
          vies_vatNumber();
          void validate(QString vatnumber);
          bool query_processed();
          bool soap_fault();
          QString fault_error();

          bool valid_code();
          QString response_name();
          QString response_address();

  private:

          QByteArray *mDataBuffer;
          QNetworkReply *mNetReply;

          bool loaded=false;

          bool fault=false;
          QString fault_string;

          bool valid=false;
          QString name;
          QString address;

  private slots:
          void OnDataReadyToRead();
          void OnProcessFinished();

};


class aeat_soap : public QObject {

  public:
          aeat_soap();
          void process(QString file, QString url, QString certificate_file, QString pw_key);
          bool processed();
          QDomDocument response();

  private:

          QByteArray *mDataBuffer;
          QNetworkReply *mNetReply;
          QDomDocument doc;

          bool loaded=false;
          bool fault=false;
          QString fault_string;

          bool valid=false;

  private slots:
          void OnDataReadyToRead();
          void OnProcessFinished();

};



#endif // NETWORK_CONNECTIONS_H
