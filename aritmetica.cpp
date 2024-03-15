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
    aritmetica.cpp  -  cálculo de expresiones para KEME
                             -------------------
    copyright            : (C) 2007 by José Manuel Díez Botella
 ***************************************************************************/

#include "aritmetica.h"
   
void evaluadorparentesis::inicializa(QString qcadena)
{
  operadores.tope = -1;
  cadena=qcadena;
}

int evaluadorparentesis::apilar_es(Toperador e)
{
  if((operadores.tope +1) >= MAXP)
    {
      //ERROR: La Pila de operadores se ha llenado
      return(0);
    }
  else
      {
        operadores.tope++;
        operadores.v[operadores.tope] = e;
      }
  return(1);
}

int evaluadorparentesis::vaciap_es()
{
  if(operadores.tope <= -1) return(1);
   else return(0);
}

int evaluadorparentesis::desapilar_es()
{
  if(!vaciap_es())
     operadores.tope--;
     else return (0);
         // ERROR: Intentas desapilar en la pila de operadores, pero esta vacía

  return(1);
}

Toperador evaluadorparentesis::tope_es(int *hayerror)
{
  *hayerror=0;
  if(!vaciap_es())
     return(operadores.v[operadores.tope]);
     else
         // ERROR: Intentas consultar el tope de la pila de operadores, pero esta vacía
         *hayerror=1;
  return(0);
}

void evaluadorparentesis::tokenize()
{
 int i;
 num_sim = 0;
 i = 0;
 while(i<cadena.length())
   {
    if (cadena[i]=='(') expresion_infija[num_sim]=PAR_IZQ; break;
    if (cadena[i]==')') expresion_infija[num_sim]=PAR_DER; break;
    num_sim++;
    i++;
   }
}

int evaluadorparentesis::val_transforma_postfija()
{
 int i, op;
 int hayerror=0;
	for(i = 0; i < num_sim; i++) {
		switch(expresion_infija[i]) {
			case PAR_IZQ: apilar_es(PAR_IZQ);
				break;
			case PAR_DER: op = tope_es(&hayerror);
			        if (hayerror) return 0;
				if (!desapilar_es()) return 0;
				while(op != PAR_IZQ) {
					op = tope_es(&hayerror);
					if (hayerror) return 0;
					if (!desapilar_es()) return 0;
				}
				break;
		}
	}
	/* Vaciamos los posibles operadores que queden en la pila */
	while(!vaciap_es()) {
		op = tope_es(&hayerror);
		if (hayerror) return 0;
		if (!desapilar_es()) return 0;
	}
  return 1;

}

int evaluadorparentesis::okparentesis()
{
 if (cadena.contains('(')!=cadena.contains(')')) return 0;
 if (cadena.contains('(')==0) return 1;
 tokenize();
 if (val_transforma_postfija()) return 1;
 return 0;
}


  
void exparitmetica::inicializa(QString qcadena)
{
  operadores.tope = -1;
  operandos.tope=-1;
  cadena=qcadena;
}

int exparitmetica::apilar_es(Toperador e)
{
  if((operadores.tope +1) >= MAXP)
    {
      //ERROR: La Pila de operadores se ha llenado
      return(0);
    }
  else
      {
        operadores.tope++;
        operadores.v[operadores.tope] = e;
      }
  return(1);
}

int exparitmetica::vaciap_es()
{
  if(operadores.tope <= -1) return(1);
   else return(0);
}

int exparitmetica::desapilar_es()
{
  if(!vaciap_es())
     operadores.tope--;
     else return (0);
         // ERROR: Intentas desapilar en la pila de operadores, pero esta vacía

  return(1);
}

Toperador exparitmetica::tope_es(int *hayerror)
{
  *hayerror=0;
  if(!vaciap_es())
     return(operadores.v[operadores.tope]);
     else
         // ERROR: Intentas consultar el tope de la pila de operadores, pero esta vacía
         *hayerror=1;
  return(0);
}


int exparitmetica::apilar_os(Toperando e)
{
  if((operandos.tope +1) >= MAXP)
    {
      //ERROR: La Pila de operadores se ha llenado
      return(0);
    }
  else
      {
        operandos.tope++;
        operandos.v[operandos.tope] = e;
      }
  return(1);
}

int exparitmetica::vaciap_os()
{
  if(operandos.tope <= -1) return(1);
   else return(0);
}

int exparitmetica::desapilar_os()
{
  if(!vaciap_os())
     operandos.tope--;
     else return (0);
         // ERROR: Intentas desapilar en la pila de operadores, pero esta vacía

  return(1);
}

Toperando exparitmetica::tope_os(int *hayerror)
{
  *hayerror=0;
  if(!vaciap_os())
     return(operandos.v[operandos.tope]);
     else
         // ERROR: Intentas consultar el tope de la pila de operadores, pero esta vacía
         *hayerror=1;
  return(0);
}

void exparitmetica::tokenize()
{
	int i, j;
	QString sim;
	char caracter;

	num_sim = 0;
	i = 0;
	while(i<cadena.length()) {
	        signoin[num_sim]=0;
		caracter=' ';
		if (cadena[i]=='+') caracter='+';
		if (cadena[i]=='-') caracter='-';
		if (cadena[i]=='*') caracter='*';
		if (cadena[i]=='/') caracter='/';
		if (cadena[i]=='(') caracter='(';
		if (cadena[i]==')') caracter=')';
		switch(caracter) {
			case '+': expresion_infija[num_sim]=SUMA; break;
			case '-': 
			         {
				  if (i>0)
				     if (cadena[i-1]!='(' && cadena[i-1]!='*' && cadena[i-1]!='/')
			                { expresion_infija[num_sim]=RESTA; break; }
				 }
				 num_sim--;
				 // i++; loop;
				 break;
			case '*': expresion_infija[num_sim]=MULT; break;
			case '/': expresion_infija[num_sim]=DIV; break;
			case '(': expresion_infija[num_sim]=PAR_IZQ; break;
			case ')': expresion_infija[num_sim]=PAR_DER; break;
			default: j=i;
				sim="";
				while((j<cadena.length()) && (((cadena[j]>='0') && (cadena[j]<='9')) || cadena[j]=='.')) {
					// sim[j-i] = cadena[j];
					j++;
				}
				sim=cadena.mid(i,j-i);
				// sim[j-i]='\0';
				// sscanf(sim,"%lf15.2",&expresion_infija[num_sim]); mod
                                expresion_infija[num_sim]=sim.toDouble();
				if (i>0)
				   if (cadena[i-1]=='-')
				      {
				       if (i-1==0) signoin[num_sim]=1;
				           else
				              if (cadena[i-2]=='(' || cadena[i-2]=='*' || cadena[i-2]=='/')
				                 signoin[num_sim]=1;
				      }
				i=j-1;
				/* expresion_infija[*num_sim] = atoi(sim); */
		}
		num_sim++;
		i++;
	}

}


int exparitmetica::precedencia(Toperador op)
{
	int p=0;
	switch(op) {
		case SUMA: p=1; break;
		case RESTA: p=1; break;
		case MULT: p=2; break;
		case DIV: p=2; break;
		case PAR_IZQ: p=0; break;
	}
	return(p);
}


int exparitmetica::transforma_postfija()
{
	int i, op, entero=0;
	int hayerror;
	int num_sim_out; /* numero de simbolos que quedan en la salida */

	num_sim_out = 0;
	for(i = 0; i < num_sim; i++) {
	     if (expresion_infija[i]<0)
	       {
	        if (expresion_infija[i]==SUMA) entero=SUMA;
	        if (expresion_infija[i]==RESTA) entero=RESTA;
	        if (expresion_infija[i]==MULT) entero=MULT;
	        if (expresion_infija[i]==DIV) entero=DIV;
	        if (expresion_infija[i]==PAR_IZQ) entero=PAR_IZQ;
	        if (expresion_infija[i]==PAR_DER) entero=PAR_DER;
		switch(entero) {
			case SUMA:
			case RESTA:
			case MULT:
			case DIV:
				if(!vaciap_es()) {  // Algunas veces se intentaba sacar datos de la pila estando vacia
					op = tope_es(&hayerror);
					if (hayerror) return 0;
					while((precedencia(op) >= precedencia(entero)) &&
						(!vaciap_es())) {
						if (!desapilar_es()) return 0;
						expresion_postfija[num_sim_out] = op;
						num_sim_out++;
						if(vaciap_es()) break; // Algunas veces se intentaba sacar datos de la pila estando vacia
						op = tope_es(&hayerror);
						if (hayerror) return 0;
					}
				}
				if (!apilar_es(entero)) return 0;
				break;
			case PAR_IZQ: if (!apilar_es(PAR_IZQ)) return 0;
				break;
			case PAR_DER: op = tope_es(&hayerror);
			        if (hayerror) return 0;
				if (!desapilar_es()) return 0;
				if(op != PAR_IZQ) {
					expresion_postfija[num_sim_out] = op;
					num_sim_out++;
				}
				while(op != PAR_IZQ) {
					op = tope_es(&hayerror);
					if (hayerror) return 0;
					if (!desapilar_es()) return 0;
					if(op != PAR_IZQ) {
						expresion_postfija[num_sim_out] = op;
						num_sim_out++;
					}
				}
				break;
			}
		}
		  else /* Es un operando */
		     {
				expresion_postfija[num_sim_out] = expresion_infija[i];
				signopos[num_sim_out]=signoin[i];
				num_sim_out++;
		     }
		}
	
	/* Vaciamos los posibles operadores que queden en la pila */
	while(!vaciap_es()) {
		op = tope_es(&hayerror);
		if (hayerror) return 0;
		if (!desapilar_es()) return 0;
		expresion_postfija[num_sim_out] = op;
		num_sim_out++;
	}
	num_sim = num_sim_out;
 return 1;
}


Toperando exparitmetica::evalua_postfija(int *okeval)
{
	int i,entero=0,hayerror;
	Toperando op1, op2;

	i = num_sim;
	while(i) {
		if(expresion_postfija[num_sim-i] < 0) {
			op2 = tope_os(&hayerror);
			if (hayerror) {*okeval=0; return 0;}
			if (!desapilar_os()) {*okeval=0; return 0;}
			op1 = tope_os(&hayerror);
			if (hayerror) {*okeval=0; return 0;}
			if (!desapilar_os()) {*okeval=0; return 0;}
	                if (expresion_postfija[num_sim-i]==SUMA) entero=SUMA;
	                if (expresion_postfija[num_sim-i]==RESTA) entero=RESTA;
	                if (expresion_postfija[num_sim-i]==MULT) entero=MULT;
	                if (expresion_postfija[num_sim-i]==DIV) entero=DIV;
	                if (expresion_postfija[num_sim-i]==PAR_IZQ) entero=PAR_IZQ;
	                if (expresion_postfija[num_sim-i]==PAR_DER) entero=PAR_DER;
			switch(entero) {
				case SUMA: op1 = op1 + op2; break;
				case RESTA: op1 = op1 - op2; break;
				case MULT: op1 = op1 * op2; break;
				case DIV: op1 = op1 / op2; break;
			}
			if (!apilar_os(op1)) {*okeval=0; return 0;}
		} else {
			if (signopos[num_sim-i]>0) { if (!apilar_os(expresion_postfija[num_sim-i]*-1)) {*okeval=0; return 0;} }
			 else { if (!apilar_os(expresion_postfija[num_sim-i])) {*okeval=0; return 0;} };
			 
		}
		i--;
	}
        *okeval=1;
	op1=tope_os(&hayerror);
	if (!hayerror) {*okeval=1; return(op1);} else {*okeval=0; return(0);} 
}


Toperando exparitmetica::resultado(int *okeval)
{
 Toperando elresultado;
 tokenize();
 if (!transforma_postfija()) {*okeval=0; return 0;}
 elresultado=evalua_postfija(okeval);
 return elresultado; 
}
