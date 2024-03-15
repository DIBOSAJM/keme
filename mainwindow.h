#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "diario.h"
#include <qlistwidget.h>
#include <qtreeview.h>
#include <QTreeWidget>
#include "subcuentas.h"
#include "consmayor.h"
#include "bss.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString ordenarpor(void);

private:
    Ui::MainWindow *ui;
    diario *punterodiario;
    QListWidget *estadosList;
    // QTreeView *treeView;
    QTreeWidget *treeWidget;
    QDockWidget *dockAutomaticos,*dockEstados;
    QString dirtrabajo;
    QString dirconfig;
    QString fichconfig;
    QString fichconexiones;
    QString visordvi;
    QString aplicacionimprimir;
    QString navegador;
    int filtrarpor;
    int ordendiario;
    int estilonumerico;
    bool sindecimales;
    int anchocol[16];
    QString filtropersonalizado;
    bool filtrartex;
    int idioma;
    bool noguardarpref;
    bool haydockautomaticos;
    bool haydockestados;
    bool haygestionusuarios;
    bool guardausuario;
    bool guardaconexion;
    QString cplus; // importar contaplús
    QString cdomiciliaciones;
    QString rutadocs;
    QString rutacargadocs;
    QString rutacopiapdf;
    QString ext1;
    QString vis1;
    QString ext2;
    QString vis2;
    QString ext3;
    QString vis3;
    QString ext4;
    QString vis4;
    QString usuario;
    QString claveusuario;
    QString editorlatex;
    QString ruta_seg;
    QString ruta_libros;
    QString ruta_curl;
    QString url_actu;
    QString msj_url_actu;
    QString arch_icono;
    QString titulo;
    QString arch_splash;
    QString script_copias;
    QString forzar_ver_pdf;

    int fila_presupuesto_tabla;


    bool visualizador_extendido=false;

    QString filtroactivo;

    QString orden_campo_diario;

    // TreeModel modeloarbol;

    subcuentas *s;
    consmayor *c;
    bss *b;

    bool activa_msj_tabla;

    void procesar_inicio();
    bool abrebasedatos(QString vpghost,QString vpgdbname,QString vpguser,
                         QString vpgpassword,QString vcontrolador,QString vpuerto);
    bool cargapreferencias(bool *bdabierta, bool *hayfichpref);
    int n_campo_orden();
    bool cargausuario();
    void resetprivilegios();
    void actuprivilegios();
    void activa_sinconexion();
    void activa_conexion_no_usuario();
    void activaTodo();
    void cargaarbol();
    QString condicionesfiltrodefecto(void);
    void actudatosprincipal();
    void grabapreferencias();
    bool exportaplan(QString fichero,bool subcuentas);
    bool exportaplanxml(QString fichero,bool subcuentas);
    bool importarfichplan(QString qfichero);
    void actualizadockautomaticos();
    bool importarfichasientos(QString qfichero);
    bool importarfichregul(QString qfichero);
    void actualizadockestados();
    void refresh_diario();
    void refresh_diario2();
    void icono_papelera();

private slots:
    void conex();
    void qordendiario(bool numero);
    void activaWidgets();
    void imgdiario();
    void creaDockWindows();
    void qpreferencias();
    void met_acercade();
    void salir();
    void configuracion();
    void ed_ejercicios();
    void nuevaempresa();
    void testigic();
    void eliminaempresaactual();
    void copiaempresa();
    void editanotas();
    void consultanotas();
    void deshab_periodos();
    void cambiousuario();
    void gestionusuarios();
    void cambiaclave();
    void iusuarios();
    void tabla_codigo_pais();

    void tabla_id_fiscal();
    void tabla_op_expedidas();

    void tabla_op_recibidas();

    void tabla_clave_retenciones();

    void tabla_provincias();

    void tabla_concepto_evpn_pymes();
    void tabla_tipo_proveedor();
    void tabla_homologacion();
    void tabla_actividades();

    void contenido347();

    void edplan();
    void edsubcuentas();
    void datos_acc();
    void ctas_iva_ret();

    void bloqueactas();
    void exportplan();
    void exportplansubcuentas();

    void actualizaplan();
    void nuevoasiento();
    void apunteaib();
    void apunteeib();

    void gestautomaticos();
    void refrescardiario();
    void filtrar_inicio();
    void editaasiento();

    void editanumasiento();
    void consultaasiento();
    void consultanumasiento();

    void activa_revisado();

    void asigna_ficherodocumento();

    void borraasiento();
    void filtrardiario();

    void visdoc();

    void copiar();
    void pegar();
    void cortar();

    void asientosespera();

    void impridiario();

    void edicionconceptos();

    void regulariza();
    void cierra();
    void suprimircierre();
    void apert();
    void reg_cierre_apertura();
    void ordenarpor_campo();

    void tiposdiario();
    void regulparc();
    void ejecuta_regul();
    void importa_reg();
    void consultamayor();
    void consultamayor_permanente();
    void imprimemayor();
    void genbss();
    void acummes();
    void consultaplan();
    void dresumen();
    void roper();
    void conciliacion();
    void conc_diferencias();
    void importac43();
    void importacsv();
    void importa_conci(bool csv);
    void borraimportac43();
    void conciliacion_imp();
    void contabiliza_venci_c43();
    void retencion();
    void consulta_retenciones();
    void roper182();

    void consvencipase();
    void procvencimientos();
    void tipos_venci_a_cuentas();
    void ejecuta_tiposvenci();
    void vanul();
    void apunteivasoportado();
    void apunteivarepercutido();
    void qtiposiva();
    void cuentasiva_repercutido();
    void cuentasiva_soportado();
    void chequea_libro_facturas();
    void relacion_apuntes_no_libroiva();
    void relacion_cuotas_descuadre();
    void liqiva();
    void relopiva();
    void librorecibidas();
    void libroemitidas();
    void m340();
    void modelo347();
    void ci1();
    void ci2();
    void ci3();
    void cinput();
    void imputctas();
    void verinput_apunte();
    void cambiacidiario();
    void reasig_input();
    void cambia_input_cta_diario();
    void gestestados();
    void liberacalcmedios();
    void ecpn_pymes();
    void concepto_ecpn_pymes();
    void qcolumnas_ecpn_pymes();
    void genxbrl();
    void genis();
    void procmemoria();
    void presupuestos();
    void presupuestoscta();
    void desv();
    void desvctas();
    void plamort();
    void asamort();
    void difbi();
    void hamort();
    void ineto();
    void importasientos();
    void imp_cplus();
    void marcarcerrado();
    void importafich();
    void asignardiario();
    void librosagr();
    void consultatex();
    void generarpdf();
    void seguridad();
    void consolid_sinc();
    void renumera();
    void intercambiaas();
    void borraint();
    void cambiacuen();
    void cambiacuenpase();
    void cambiaciapunte();
    void editafechaasien();
    void edcondoc();
/*    void carganuevoplan();
    void ednuevoplan();
    void equivalenciasplan();
    void susticuenta();
    void borranuevoplan();*/
    void verificaBD();
    void actusaldos();
    void infoconex();
    void acuerdolicencia();
    void acuerdolicenciaesp();
    void domicilia();
    void ejecuta_automatico();
    void cal_estado(QListWidgetItem *item);
    void m349();
    void informacion_venci();
    void ctas_inversion_sp();
    void edformas_pago();
    void import_ctas_datos();
    void regenera_arch_config();
    void edpapelera();
    void gest_externos();
    void gen_sii_recibidas();
    void gen_sii_emitidas();
    void import_externos();

    void direc_soap_sii();

    void asigna_externo_apunte();
    void util_carga_docs_bd();

    void inv_inmov();
    void on_actionModelo_182_registro_donaciones_triggered();
    void on_actionFicheros_de_transferencias_triggered();
    void on_action_Tipos_de_Cambio_triggered();
};

#endif // MAINWINDOW_H
