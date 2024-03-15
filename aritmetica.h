/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licencia Pública General GNU publicada 
    por la Fundación para el Software Libre, ya sea la versión 3 
    de la Licencia, o (a su elección) cualquier versión posterior.

    Este programa se distribuye con la esperanza de que sea útil, pero 
    SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
    MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
    Consulte los detalles de la Licencia Pública General GNU para obtener 
    una información más detallada. 

    Debería haber recibido una copia de la Licencia Pública General GNU 
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.
  ----------------------------------------------------------------------------------*/

/***************************************************************************
    aritmetica.h  -  declaraciones de funciones para cálculo de expresiones - KEME
                             -------------------
    copyright            : (C) 2007 by José Manuel Díez Botella
 ***************************************************************************/

 #include <qstring.h>


#define MAXSIM 200  // número máximo de símbolos de una expresión */
#define MAXP 200   // tamaño máximo de una pila

/* Simbolos aritméticos */
#define SUMA -1
#define RESTA -2
#define MULT -3
#define DIV -4
#define PAR_IZQ -5
#define PAR_DER -6

/**************************************************/
/***************** Tipos de datos *****************/
typedef int Toperador;
typedef double Toperando;

/* Tipo de datos para la pila de operadores */
typedef struct {
	Toperador v[MAXP];
	int tope;
} TPila_es;


typedef struct {
	Toperando v[MAXP];
	int tope;
} TPila_os;



class evaluadorparentesis
   {
    TPila_es operadores;
    QString cadena; /* Para leer la expresión */
    int num_sim; /* numero de símbolos -paréntesis- en la expresión */
    int expresion_infija[MAXSIM]; /* guarda la expresión infija */
    public:
    void inicializa(QString qcadena);
    int apilar_es(Toperador e);
    int vaciap_es(); // Está vacía la pila ?
    int desapilar_es();
    Toperador tope_es(int *hayerror);
    void tokenize();
    int val_transforma_postfija();
    int okparentesis();
   };




class exparitmetica
  {
    TPila_es operadores;
    TPila_os operandos;
    QString cadena; /* Para leer la expresión */
    int num_sim; /* numero de símbolos en la expresión */
    double expresion_infija[MAXSIM]; /* guarda la expresión infija */
    int signoin[MAXSIM];  // signos negativos en infija
    double expresion_postfija[MAXSIM];
    int signopos[MAXSIM];  // signos negativos en postfija
    public:
    void inicializa(QString qcadena);
    int apilar_es(Toperador e);
    int vaciap_es(); // Está vacía la pila ?
    int desapilar_es();
    Toperador tope_es(int *hayerror);
    int apilar_os(Toperando e);
    int vaciap_os(); // Está vacía la pila ?
    int desapilar_os();
    Toperando tope_os(int *hayerror);
    void tokenize();
    int precedencia(Toperador op);
    int transforma_postfija();
    Toperando evalua_postfija(int *okeval);
    Toperando resultado(int *okeval);
  };

