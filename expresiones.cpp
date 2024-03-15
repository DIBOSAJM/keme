#include "funciones.h"
#include "expresiones.h"
#include "aritmetica.h"
#include "basedatos.h"

double valorexpresion( QString formula,QDate fecha1, QDate fecha2 )
{
  //empezamos por reemplazar corchetes de cuentas
  if (formula.length()==0) return 0;
  QString cadpru;
  QString extract;
  QString cadval;
  cadpru=formula;
  // eliminamos espacios de cadpru
  cadpru.remove(' ');
  double valor;
  int final;
  int inicio;
  int indice=0;
  while (indice<cadpru.length())
      {
         if (cadpru[indice]=='[')
           {
              inicio=indice;
              final=inicio;
              while (final<cadpru.length() && cadpru[final]!=']') final++;
              // extraemos la cadena de la cuenta sin corchetes
              extract=cadpru.mid(inicio+1,final-inicio-1);
              // habría que calcular el valor de la cuenta
              // QMessageBox::warning( this, tr("Estados Contables"),extract);
              // -------------------------------------------------------------------------------------
              if (extract.contains(':')==1) valor=cuentaespec(extract,fecha1,fecha2);
                else
	            {
                      valor=calculacuenta(extract,fecha1,fecha2);
	            }
	     if (valor>-0.004 && valor < 0.005) valor=0;
             cadval.setNum(valor,'f',2);
             if (valor<0 && inicio>=1 && cadpru[inicio-1]=='+' ) inicio--;
             if (valor<0 && inicio>=1 && cadpru[inicio-1]=='-' )
               {cadpru[inicio-1]='+'; cadval.setNum(valor*-1,'f',2); }
             cadpru.remove(inicio,final-inicio+1);
             cadpru.insert(inicio,cadval);
             continue;
           }
       indice++;
      }
  int okeval;
  exparitmetica expresion;
  if (cadpru[0]=='+') { cadpru[0]=' '; cadpru.remove(' ');}
  if (cadpru[0]=='-' && cadpru[1]=='(') cadpru=cadpru.insert(1,"1*");
  cadpru.left(-1).replace("(+","(");
  // QMessageBox::warning( this, tr("Estados Contables"),cadpru);
  expresion.inicializa(cadpru);
  valor=expresion.resultado(&okeval);
  if (!okeval) valor=0;
 cadval.setNum(valor,'f',2);
  // QMessageBox::warning( this, tr("Estados Contables"),cadval);
  // QMessageBox::warning( this, tr("Estados Contables"),/*formula*/cadval+" "+ cadpru);
 return valor;
}

 
double calculacuenta(QString codigo, QDate fechaini, QDate fechafin)
{
  bool positivo=false;
  bool negativo=false;
  positivo=codigo.contains('+');
  negativo=codigo.contains('-');
  if (positivo && negativo) return 0;
  if (positivo) codigo.remove('+');
  if (negativo) codigo.remove('-');

  QString cadena;
  double resultado=0;

  QString subcadena="";

  QSqlQuery query = basedatos::instancia()->selectCuentadebehaberdiariocuentafechasgroupcuenta(codigo, fechaini, fechafin, subcadena, "" );

  if ( query.isActive() ) {
     while ( query.next() ) 
           {
            if (positivo || negativo)
               {
                if (positivo && query.value(1).toDouble()>0) resultado+=query.value(1).toDouble();
                if (negativo && query.value(1).toDouble()<0) resultado+=query.value(1).toDouble();
               }
               else
	           resultado+=query.value(1).toDouble();
           }
     }
  return resultado;
}



double cuentaespec( QString extract, QDate fecha1, QDate fecha2 )
{
   double resultado=0;
  // separamos código de cuenta y especificador
   QString codigo=extract.section(':',0,0);
   QString espec=extract.section(':',1,1);
   resultado=calculacuentadiario(codigo,espec,fecha1,fecha2);
   return resultado;
}


double calculacuentadiario( QString codigo, QString qdiario,QDate fechaini, QDate fechafin )
{
    double resultado=0;
    bool positivo=false;
    bool negativo=false;
    positivo=codigo.contains('+');
    negativo=codigo.contains('-');
    if (positivo && negativo) return 0;
    if (positivo) codigo.remove('+');
    if (negativo) codigo.remove('-');

    QString subcadena = " and diario='";
    if (qdiario != diario_no_asignado())
    subcadena += qdiario.left(-1).replace("'","''");
    subcadena += "' ";

    QSqlQuery query = basedatos::instancia()->selectCuentadebehaberdiariocuentafechasgroupcuenta(codigo, fechaini, fechafin, subcadena, "");

    resultado = 0;
    if ( query.isActive() ) {
        while ( query.next() ) 
        {
            if (positivo || negativo)
            {
                if (positivo && query.value(1).toDouble()>0) resultado += query.value(1).toDouble();
                if (negativo && query.value(1).toDouble()<0) resultado += query.value(1).toDouble();
            }
            else
	            resultado += query.value(1).toDouble();
        }
    }
    return resultado;
}

