/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

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

#ifndef PREFERENCIAS_H
#define PREFERENCIAS_H

#include <QtGui>
#include "ui_preferencias.h"

#define NADA 0
#define EJERCICIOACTUAL 1
#define ULTIMOS30DIAS 2
#define ULTIMOS30ASIENTOS 3

#define FECHA 0
#define ASIENTO 1

#define COMADECIMAL 1
#define PUNTODECIMAL 0

// #define NUMERACIONORIGINAL 0
// #define PRIMERASIENTO1 1

class preferencias : public QDialog {
    Q_OBJECT
	public:
                preferencias();
                void pasadirtrabajo(QString directorio);
                void pasavisordvi(QString visor);
                void pasaaplicimprimir(QString aplic);
                void pasanavegador(QString navegador);
                void pasafiltropor(int filtro);
                void pasaordenarpor(int orden);
                void pasaestilonumerico(int estilo);
                void pasasindecimales(bool sindecimales);
                // void pasanumeracion(int numeracion);
                void pasafiltrodiariopersonalizado(QString filtro);
                void pasafiltrartex(bool filtrartext);
                void pasaidioma(int qidioma);
                void pasahaydockautomaticos(bool activo);
                void pasahaydockestados(bool activo);
                void pasaeditorlatex(QString qeditor);
                void pasa_ruta_seg(QString qruta);
                void pasa_ruta_curl(QString qruta);
                void pasa_repetidos(bool rep);
                bool sihaydockautomaticos();
                bool sihaydockestados();
                QString dirtrabajo();
                QString visordvi();
                QString aplicimprimir();
                QString navegador();
                QString ruta_curl();
                QString editorlatex();
                int filtropor();
                int ordenarpor();
                int estilonumerico();
                bool sindecimales();
                int numeracion();
                bool esfiltrartex();
                bool hay_control_docs_repetidos();
                QString filtrodiariopersonalizado();
                int qidioma();
                QString rutadocs();
                QString rutacopiapdf();
                QString ruta_seg();
                QString ext1();
                QString vis1();
                QString ext2();
                QString vis2();
                QString ext3();
                QString vis3();
                QString ext4();
                QString vis4();

                QString url_actu();
                QString msj_url_actu();

                void pasarutadocs(QString cadena);
                void pasarutacopiapdf(QString cadena);
                void pasaext1(QString cadena);
                void pasavis1(QString cadena);
                void pasaext2(QString cadena);
                void pasavis2(QString cadena);
                void pasaext3(QString cadena);
                void pasavis3(QString cadena);
                void pasaext4(QString cadena);
                void pasavis4(QString cadena);

                QString rutacargadocs();

                void pasarutacargadocs(QString cadena);
                void pasarutalibros(QString cadena);
                QString rutalibros();
                void pasaguardausuario(bool guarda);
                void pasaguardaconexion(bool guarda);
                void pasa_visualizador_extendido(bool visexten);
                bool visualizador_extendido();
                bool seguardausuario();
                bool seguardaempresa();
                void pasa(int x, int y, int qalto, int qancho);

                void pasa_listas_info(QStringList qconex_rutas, QStringList qrutas_trabajo, QStringList qrutas_docs);
                void recupera_listas_info(QStringList *qconex_rutas, QStringList *qrutas_trabajo, QStringList *qrutas_docs);
                void pasa_url_actu(QString cadena);
                void pasa_msj_url_actu(QString cadena);
                void pasa_import_remota(bool importrem);
                bool import_remota();


        private:
                Ui::preferencias ui;
                QStringList conex_rutas, rutas_trabajo, rutas_docs;
	private slots:
	    void filtrarporcambiado();
 	    void filtrarinicambiado();
	    void accept();
            void dirtrab_pulsado();
            void rutadoc_pulsado();
            void cargadoc_pulsado();
            void copiapdf_pulsado();
            void ruta_seg_pulsado();
            void ruta_libros_pulsado();
            void rutas_conexion();


};

#endif 
