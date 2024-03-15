/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)

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

#ifndef BASEDATOS_H_
#define BASEDATOS_H_


#include <QtSql>
#include <QDate>

class basedatos {

    public:
        // Constante que define el tipo de la base de datos
        static const char NINGUNA;
        static const char MYSQL;
        static const char POSTGRES;
        static const char SQLITE;        

        // Destructor de la clase
        virtual ~basedatos();

        // Devuelve un puntero a la instancia única del programa
        static basedatos* instancia();

        // Comprueba si está abierta la base de datos o no
        bool abierta();

        // Devuelve el tipo de base de datos que está siendo usada: MYSQL, POSTGRES, SQLITE
        virtual char cualControlador (QString db);
        virtual char cualControlador(void);

        // Devuelve la codificación del controlador designado en el parámetro
        char codigoControlador (QString controlador);

        // Devuelve el nombre del controlador
        QString controlador();

        // Devuelve el nombre de la base de datos
        QString nombre();

        // Devuelve el nombre del usuario
        QString usuario();

        // Devuelve la clave
        QString clave();

        // Devuelve el host de conexión
        QString host();

        // Devuelve el puerto de conexión
        int puerto();

        // Versión de la base de datos
        QString selectVersion();

        // Crea base de datos 
        bool crearBase(QString controlador, QString nombre, QString usuario, QString clave,
                       QString host, QString puerto, QString segunda="");

        // Abre base de datos, comprobando versión
        bool abrirBase(QString controlador, QString nombre, QString usuario, QString clave, QString host, QString puerto);

        // abre base datos sin comprobar versión
        bool abrirBase_s (QString controlador, QString nombre, QString usuario, QString clave, QString host, QString puerto);

        // Cierra la base de datos
        void cerrar();

        // Crea solo las tablas
        void solotablas(bool segunda=false, QString qbase="");

        // Crea claves foráneas
        void crearForaneas(bool segunda=false, QString qbase="");

        void copiatabla_ejercicios(QString base);
        void copiatabla_plancontable(QString base);
        void copiatabla_diario(QString base);
        void copiatabla_libroiva(QString base);
        void copiatabla_planamortizaciones(QString base);
        void copiatabla_amortcontable(QString base);
        void copiatabla_vencimientos(QString base);
        void copiatabla_tiposiva(QString base);
        void copiatabla_diarios(QString base);
        void copiatabla_datos_subcuenta(QString base);
        void copiatabla_ci(QString base);
        void copiatabla_diario_ci(QString base);
        void copiatabla_ci_amort(QString base);
        void copiatabla_configuracion(QString base);
        void copiatabla_amortfiscalycontable(QString base);
        void copiatabla_saldosubcuenta(QString base);
        void copiatabla_input_cta(QString base);
        void copiatabla_borrador(QString base);
        void copiatabla_tipos_venci(QString base);
        void copiatabla_amortpers(QString base);
        void copiatabla_paises(QString base);
        void copiatabla_claveidfiscal(QString base);
        void copiatabla_provincias(QString base);
        void copiatabla_concepto_evpn_pymes(QString base);
        void copiatabla_cols_evpn_pymes(QString base);
        void copiatabla_claveexpedidas(QString base);
        void copiatabla_claverecibidas(QString base);
        void copiatabla_clave_op_retenciones(QString base);
        void copiatabla_retenciones(QString base);
        void copiatabla_formas_pago(QString base);
        void copiatabla_externos(QString base);

        void copiatabla_series_fact(QString base);
        void copiatabla_tipos_doc(QString base);
        void copiatabla_referencias(QString base);
        void copiatabla_facturas(QString base);
        void copiatabla_lin_fac(QString base);

        // Copia tablas a segunda base de datos
        void copiatablas(QString qbase);

        // Crea las tablas iniciales de la base de datos
        // Recibe los datos de si es de longitud variable y el número de dígitos de las cuentas
        void crearTablas(bool checked, QString digitos);

        // Borra una database
        void borrarDatabase(QString database);

        // Borra de la tabla libro iva a partir de un pase
        void deletelibroivapase(QString pase);

        // Borra de la tabla retenciones el pase especificado
        void deleteretencionespase (QString pase);

        // Borra de la tabla vencimientos a partir de un pase
        void deletevencimientospase_diario_operacion(QString pase);

        // Borra del diario asientos que sean mayores o iguales
        void deleteDiarioasientomayorigualmenorigual(QString inicial, QString final,
                                                     QString ejercicio);

        // Borra de plancontable una cuenta
        void deletePlancontablecodigo(QString qcodigo);

        void borra_exteno (QString qcodigo);

        // Borra de datossubcuenta una cuenta
        void deleteDatossubcuentacuenta(QString qcodigo);

        // Borra de saldossubcuenta una cuenta
        void deleteSaldossubcuentacodigo(QString qcodigo);

        // Borra de conceptos una clave
        void deleteConceptosclave(QString qcodigo);

        // Borra de tabla una clave
        void deleteclavetabla (QString tabla, QString qcodigo);

        // Borra de ci un codigo
        void deleteCicodigo(QString codigo);

        // Borra de vencimientos un num
        void deleteVencimientosnum(QString vto);

        // Borra de cab_as_modelo un asientomodelo
        void deleteCab_as_modeloasientomodelo(QString asiento);

        // Borra de det_as_modelo un asientomodelo
        void deleteDet_as_modeloasientomodelo(QString asiento);

        // Borra de var_as_modelo un asientomodelo
        void deleteVar_as_modeloasientomodelo(QString asiento);

        // Borra de tiposiva una clave
        void deleteTiposivaclave(QString qclave);

        // Borra de cabeceraestados un titulo
        void deleteCabeceraestadostitulo(QString estado);

        // Borra de estados un titulo
        void deleteEstadostitulo(QString estado);

        // Borra todo de nuevoplan
        void deleteNuevoplan();

        // Borra de nuevoplan un código
        void deleteNuevoplancodigo(QString qcodigo);

        // Borra de dif_conciliación un codigo
        void deleteDif_conciliacioncodigo(QString qcodigo);

        // Borra de borrador un asiento
        void deleteBorrador(QString asiento);

        // Borra de ajustes_conci a partir de fechas y cuenta
        void deleteAjustes_concifechascuenta(QDate inicial, QDate final, QString cuenta);

        // 
        void deleteEjercicioscodigo(QString elcodigo);

        // 
        void deletePeriodosejercicio(QString elcodigo);

        // 
        void deleteEquivalenciasplan();

        // Borra todo de plancontable
        void deletePlancontable();

        // 
        void deleteDiarioasiento(QString asiento, QString ejercicio);

        // 
        void deleteAmortcontableejercicio(QString ejercicio);

        // 
        void deleteAmortfiscalycontableejercicio(QString ejercicio);

        void deleteAmortfiscalycontableejercicio_mes (QString ejercicio,int mes);

        // 
        void deleteAmortperscuenta(QString ctaactivo);

        // 
        void deleteCi_amortcuentaci(QString cuenta, QString ci);

        // 
        void deletePlanamortizacionescuenta_activo(QString cuenta);

        // 
        void deleteCi_amortcuenta(QString cuenta);

        // 
        void deletePresupuestoejerciciosubcuentas(QString ejercicio, bool subcuentas);

        // 
        void deletePresupuestoejerciciocuenta(QString ejercicio, QString cuenta);

        // 
        void deleteCuenta_ext_concifechacuenta(QDate fecha, QString cuenta);

        // 
        void deleteDiariodiariofechas(QString diario, QDate inicial, QDate final);

        // importaplan de creaempresa
        void insertImportaplan();

        // inserta datos en la tabla plancontable
        void insertPlancontable(QString codigo, QString descripcion, bool auxiliar);

        // Inserta datos en la tabla conceptos
        void insertConceptos(QString codigo,QString descripcion);

        // Inserta datos en tabla
        void inserttabla(QString tabla,QString codigo,QString descripcion);

      // Inserta datos en saldossubcuenta
        void insertSaldossubcuenta(QString codigo, QString saldo, QString saldomedio1, QString saldomedio2);

        // 
        void insertCi(QString codigo,QString descripcion,int nivel);

        // Inserta datos en cabeceraestados
        void insert18Cabeceraestados(QString titulo, QString cabecera, QString parte1, QString parte2,
            QString observaciones, QString formulabasepor, QString fechacalculo, QString ejercicio1, 
            QString ejercicio2, QString diarios, QString estadosmedios, QString valorbasepor1, QString valorbasepor2, 
            QString analitica, QString haycolref, QString colref, QString ci, QString grafico,
            QString desglose, QString desglose_ctas, QString descabecera, QString despie);

        // Inserta datos en tipoiva
        void insertTiposiva(QString clave,QString tipo, QString re, QString descripcion);

        // Inserta datos en estados
        void insertEstados(QString titulo,QString apartado, QString parte1, QString clave, QString nodo,
            QString formula, QString importe1, QString importe2, QString calculado, QString referencia);

        // 
        void insertNuevoplan(QString codigo, QString descripcion, bool auxiliar);

        // Introduce los datos en cab_as_modelo
        void insertCab_as_modelo(QString asientomodelo, QString fecha, QString tipo,
                                 bool aib, bool autofactura_ue, bool autofactura_no_ue,
                                 bool eib, bool eib_servicios,
                                 bool isp_op_interiores,
                                 bool importacion,
                                 bool exportacion, QString diario);

        // Introduce datos en det_as_modelo
        void insert_Det_as_modelo (QString asientomodelo,QString cuenta,QString concepto,
              QString expresion,QString d_h,QString ci,QString baseiva,QString cuentafra,
              QString cuentabaseiva,QString claveiva,QString documento, QString fecha_factura,
              QString prorrata_e, bool rectificativa,
              QString fecha_op, QString clave_op, QString bicoste,QString rectificada,
              QString numfacts, QString facini,
              QString facfinal, bool binversion);

        // Introduce datos en diarios
        void insertDiarios(QString codigo, QString descripcion);

        // Introduce datos en dif_conciliacion
        void insertDif_conciliacion(QString codigo, QString descripcion);

        // Introduce 10 datos en diario
        void insertDiario10(QString cadnumasiento, QString cadnumpase, QString cadinicioej1,
                            QString cuenta, QString debe, QString haber, QString concepto,
                            QString documento, QString usuario, QString ejercicio,QString externo="");

        void insertDiario_ic (QString cadnumasiento, QString cadnumpase, QString cadinicioej1,
                              QString cuenta, QString debe, QString haber, QString concepto,
                              QString documento, QString usuario, QString ejercicio,
                              QString nrecepcion);

        void insertDiario_con_diario (QString cadnumasiento, QString cadnumpase, QString cadinicioej1,
                                      QString cuenta, QString debe, QString haber, QString concepto,
                                      QString documento, QString qdiario, QString qusuario,
                                      QString ejercicio, QString externo="");

        void insertDiario_con_diario_ci (QString cadnumasiento, QString cadnumpase,
                                                 QString cadinicioej1, QString cuenta,
                                                 QString debe, QString haber,
                                                 QString concepto, QString documento,
                                                 QString qdiario, QString usuario, QString ci,
                                                 QString ejercicio);

        // Introduce datos de Vencimientos
        void insertVencimientosespecial(QString cadnumvto, QString qcuenta, QString qfecha, QString qdebehaber, double valor1, double valor2, QString pase);

        // Introduce datos de Vencimientos
        void insertVencimientosespecialtf(QString cadnumvto, QString qcuenta, QString qfecha, QString qdebehaber, double valor1, double valor2, QString pase);

        // Inserta datos en ajustes_conci
        void insertAjustes_conci(QString cuenta, QString fecha, QString concepto, QString debe, QString haber, QString dif_conciliacion, bool conciliado);

        // 
        void insertVar_as_modelo(QString asientomodelo, QString variable, QString tipo, QString descripcion,
                                 QString valor, int veces);

        void insertVar_as_modelo(QString asientomodelo, QString variable, QString tipo, QString descripcion,
                                 QString valor, bool inicializa,int veces);
        // 
        void insertDet_as_modelo (QString asientomodelo, QString cuenta, QString concepto,
                                  QString expresion, QString d_h, QString ci, QString baseiva,
                                  QString cuentabaseiva, QString cuentafra, QString claveiva,
                                  QString documento, QString fecha_fact,
                                  QString prorrata_e, bool rectificativa,
                                  QString fecha_op, QString clave_op, QString bicoste,QString rectificada,
                                  QString numfacts, QString facini, QString facfinal, bool binversion,
                                  QString afecto, bool agrario,
                                  QString nombre, QString cif,
                                  QString cta_retenido, bool arrendamiento,
                                  QString clave, QString base_ret,
                                  QString tipo_ret, QString retencion, QString ing_a_cta,
                                  QString ing_a_cta_rep, QString nombre_ret,
                                  QString cif_ret, QString provincia,
                                  int veces);

        // 
        void insertEquivalenciasplan(QString cod, QString equiv);

        // 
        void insertPlancontablenuevoplan();

        void insertPlancontablenuevoplan9999();

        void borradeplancontable_NO_9999();

        void duplicaplancontable_NO_9999 ();

        void borradeplancontable9999 ();

        // 
        void insert11Diario(QString cadnumasiento, qlonglong pase, QString cadfecha,
                            QString cuentagasto, double debe, double haber, QString concepto,
                            QString documento, QString usuario,QString ejercicio);

        void insert12Diario (QString cadnumasiento, qlonglong pase, QString cadfecha,
                             QString cuentagasto, double debe, double haber,
                             QString concepto, QString ci, QString usuario,
                             QString ejercicio);

        void insertpegaDiario (QString cadnumasiento, qlonglong pase, QString cadfecha,
             QString cuentagasto, double debe, double haber, QString concepto, 
             QString documento, QString ci, QString usuario, QString ejercicio);


        int recepcion_mas_datos_diario(QString pase, QString *asiento, QString *ejercicio);

        void actu_recep_asto_ejercicio(QString cadnum, QString asiento, QString ejercicio);

        void asigna_clave_ci_apunte(QString apunte, QString clave_ci);

        // 
        void insertDiario(QString cadnumasiento, qlonglong pase, QString cadfecha,
                          QString cuentagasto,
                          double debe, double haber, QString concepto, QString documento,
                          QString diario,
                          QString ci, QString usuario, QString clave_ci, QString ejercicio,
                          QString nrecepcion, bool hay_fecha_factura=false,
                          QDate fecha_factura=QDate::currentDate(), QString externo="",
                          QString concepto_sii="");

        void insertDiario_imp (QString cadnumasiento, qlonglong pase, QString cadfecha,
                                      QString cuentagasto, double debe, double haber,
                                      QString concepto, QString documento, QString diario,
                                      QString ci, QString usuario, QString clave_ci,
                                      QString ejercicio, QString codfactura);

        bool existecodigofactura_diario (QString cadena);

        bool existecodigofactura_ej_y_cta_diario (QString cadena, QString cuenta,
                                                             QString ejercicio);

        QSqlQuery codigos_factura_ej_y_cta_diario ();

        //
        void insert7Amortfiscalycontable(QString ejercicio, QString cuenta_activo, QString codigo_activo,
                                         QString cuenta_am_acum, QString cuenta_gasto,
                                         double dotacion_contable, double dotacion_fiscal);

        void insert7Amortfiscalycontable_mes(QString ejercicio, int mes, QString cuenta_activo,
                                                    QString codigo_activo, QString cuenta_am_acum,
                                                    QString cuenta_gasto, double dotacion_contable,
                                                    double dotacion_fiscal);

        // 
        void insertAmortcontable(QString ejercicio, QString asiento);

        // 
        void insert20Cabeceraestados(QString titulo, QString cabecera, QString parte1, QString parte2,
                                     QString observaciones, QString formulabasepor, QString fechacalculo,
                                     QString ejercicio1, QString ejercicio2, bool analitica, bool grafico,
                                     bool haycolref, QString colref, QString diarios, bool estadosmedios,
                                     double valorbasepor1, double valorbasepor2, bool desglose,  bool desglosectas,
                                     QString des_cabecera, QString des_pie);

        // 
        void insert8Estados(QString titulo, QString apartado, bool parte1, QString clave, QString nodo, QString formula, double importe1, double importe2);

        // 
        void insert6Estados(QString titulo, QString apartado, bool parte1, QString clave, QString nodo, QString formula);

        // 
        void insertPlanamortizaciones(QString cuenta_activo,
                                      QDate fecha_func,
                                      QString codigo_activo,
                                      QString cuenta_am_acum,
                                      QString cuenta_gasto,
                                      QString valor_residual,
                                      QString coef_amort_contable,
                                      QString coef_amort_fiscal,
                                      bool amort_fiscal_1ejer,
                                      bool amort_personalizada,
                                      bool baja,
                                      QDate fecha_baja,
                                      QString motivo_baja,
                                      QString cuenta_proveedor, QString externo);

        // 
        void insertAmortpers(QString cuenta, QString ejercicio, QString importe);

        // 
        void insertCi_amort(QString cuenta, QString ci, QString asignacion);

        //
        void borra_venci_pase_diario_operacion(QString pase);

        // 
        void insertVencimientos(QString num, QString cta_ordenante, QDate fecha_operacion,
                                 QString importe, QString cuenta_tesoreria, QDate fecha_vencimiento,
                                 bool derecho, bool pendiente, bool anulado, QString pase_diario_operacion,
                                 QString pase_diario_vencimiento, QString concepto, bool domiciliable=false,
                                QString externo="");


        void insertVencimientos_plantilla (QString num, QString cta_ordenante, QDate fecha_operacion,
                                           QString importe, QString cuenta_tesoreria,
                                           QDate fecha_vencimiento, bool derecho, bool pendiente,
                                           bool anulado, QString pase_diario_operacion,
                                           QString pase_diario_vencimiento, QString concepto,
                                           bool domiciliable, QString externo="");
        // 
        void insertPeriodos(QString ejercicio, QString codigo, QDate fechaini, QDate fechafin);

        void actuperiodos(QString ejercicio, QString etiqueta, bool inhabilitado);

        // 
        void insertPresupuestocuentaejercicio(QString cuenta, QString ejercicio);

        // 
        void insert6Ejercicios(QString codigo, QDate apertura, QDate cierre,
                               bool cerrado, bool cerrando);

        // 
        void insert6Cuenta_ext_conci(QString cuenta, QDate fecha, double debe, double haber, QString concepto, double saldo);


        void actu_libro_iva(QString pase, QString pasefactura, QString cad_importe_factura);

        // 
        void insertLibroiva(QString pase, QString cta_base_iva, QString base_iva, QString clave_iva,
                            QString tipo_iva, QString tipo_re, QString cuota_iva, QString cuenta_fra,
                            QDate fecha_fra, bool soportado, bool aib, bool eib, QString prorrata,
                            bool rectificativa, bool autofactura,
                                    QDate fechaop, QString claveop, QString bicoste,
                                QString rectificada, int nfacturas, QString finicial, QString ffinal,
                                bool autofactura_no_ue, bool pr_servicios_ue, bool binversion,
                                bool opnosujeta, QString afecto, bool agrario,
                                QString nombre, QString cif,
                                bool importacion, bool exportacion, bool exenta_no_deduc,
                                bool isp_op_interiores, bool caja_iva, bool no347=false, bool ventas_fuera_tac=false,
                            bool oro_inversion=false, bool arrto_local_ret=false, bool arrto_local_sin_ret=false, QString dua="", QString info_donacion="");

        void updateLibroiva (QString pase, QString cta_base_iva, QString base_iva, QString clave_iva,
                                        QString tipo_iva, QString tipo_re, QString cuota_iva, QString cuenta_fra,
                                        QDate fecha_fra, bool soportado, bool aib, bool eib, QString prorrata,
                                        bool rectificativa, bool autofactura,
                                        QDate fechaop, QString claveop, QString bicoste,
                                        QString rectificada, int nfacturas, QString finicial, QString ffinal,
                                        bool autofactura_no_ue, bool pr_servicios_ue, bool binversion,
                                        bool opnosujeta, QString afecto, bool agrario,
                                        QString nombre, QString cif,
                                        bool importacion, bool exportacion, bool exenta_no_deduc,
                                        bool isp_op_interiores, bool caja_iva, bool no347, bool ventas_fuera_tac,
                                        bool oro_inversion, bool arrto_local_ret, bool arrto_local_sin_ret, QString dua, QString info_donacion);

        void insertLibroivaAIB0 (QString pase, QString cta_base_iva, QString base_iva, QString clave_iva,
                                    QString tipo_iva, QString tipo_re, QString cuota_iva, QString cuenta_fra,
                                    QDate fecha_fra, QString prorrata,
                                    bool rectificativa, bool autofactura,
                                    QDate fechaop, QString claveop, QString bicoste,bool autofactura_no_ue,
                                    bool binversion);
        // 
        void insertBorrador(QString asiento, QString cuenta, QString concepto, QString debe,
                            QString haber, QString documento, QString ctabaseiva, QString baseiva,
                            QString claveiva, QString tipoiva, QString tipore, QString cuotaiva,
                            QString cuentafra, QString diafra, QString mesfra, QString anyofra,
                            QString soportado, QString ci, QString prorrata, QString rectificativa,
                            QString autofactura, QString fechaoperacion, QString claveop, QString biacoste,
                            QString rectificada, QString nfacturas, QString finicial, QString ffinal,
                            QString pr_serv_ue, QString autofactura_noue, QString bien_inversion,
                            QString sujeto, QString afecto,
                            QString col32, QString col33, QString col34, QString col35,
                            QString col36, QString col37, QString col38, QString col39,
                            QString col40, QString col41, QString col42, QString col43,
                            QString col44, QString col45, QString col46, QString col47,
                            QString col48, QString col49, QString col50, QString col51,
                            QString col52, QString col53, QString col54, QString col55,
                            QString col56, QString col57, QString col58,
                            QString externo, QString concepto_sii,
                            QString usuario, QDate fecha);

        // Devuelve el código del ejercicio de una fecha determinada
        QString selectEjerciciodelafecha(QDate fecha);

        // Devuelve el primer asiento de un ejercicio
        qlonglong selectPrimerasiento(QString ejercicio);

        // señala la fecha de inicio de ejercicio
        QDate selectAperturaejercicios(QString ejercicio);

        // primer asiento reservado para apertura de ejercicio
        bool ejercicio_primero_apertura (QString ejercicio);

            // señala la fecha de cierre de ejercicio
        QDate selectCierreejercicios(QString ejercicio);

        // Devuelve el último número de asiento del diario
        QString selectUltimonumasiento(QString filtro);

        // Devuelve la descripción de una cuenta
        QString selectDescripcionplancontable(QString qcodigo);

        // Devuelve el saldo de una cuenta en un ejercicio
        double selectSaldocuentaendiarioejercicio(QString cuenta,QString ejercicio);

        // Devuelve el saldo de una cuenta en un ejercicio
        QSqlQuery selectSaldocuentaendiariofechas(QString cuenta,QString inicial, QString final);

        // Devuelve el nombre de la empresa
        QString selectEmpresaconfiguracion();

        // devuelve fecha de constitución
        QDate fechaconst ();

        // Devuelve ciudad de la empresa
        QString ciudad ();

        // Devuelve provincia de la empresa
        QString provincia ();

        // Devuelve domicilio de la empresa
        QString domicilio ();

        // Devuelve cpostal de la empresa
        QString cpostal ();

        // Devuelve objeto de la empresa
        QString objeto();

        // Devuelve actividad de la empresa
        QString actividad();

        // Devuelve id_registral de la empresa
        QString idregistral();

        // Devuelve cif/nif de la empresa
        QString cif();

        void empresa_nif_nombre_apellidos(QString *empresa, QString *nif, QString *nombre, QString *apellidos);

        QString email();

        QString tfno();

        bool select_curl_sii();

        // Devuelve el ancho de las cuentas
        int selectAnchocuentasconfiguracion();

        // Indica si un ejercicio está cerrado o no
        int selectCerradoejercicios(QString ejercicio);

        // Indica si un ejercicio se está cerrando
        int selectCerrandoejercicios(QString ejercicio);

        // Devuelve una cola de pase debe haber y cuenta de la tabla diario desde el asiento inicial hasta el final
        QSqlQuery selectPasedebehabercuentadiario(QString inicial, QString final,
                                                  QString ejercicio);

        // Devuelve los datos de una subcuenta
        QSqlQuery selectTodoDatossubcuentacuenta(QString cuenta);

        QSqlQuery select_datos_tipo_proveedor_cta (QString codigo);

        QSqlQuery lista_datos_tipo_proveedor_cta (QString filtro, QString cuenta="");

        QSqlQuery selectTodoDatosexterno (QString codigo);

        // Hay datos accesorios ?
        bool hay_datos_accesorios(QString cuenta);

        void razon_nif_datos(QString cuenta, QString *razon, QString *nif);

        // Devuelve la foto codificada en caracteres de una subcuenta
        QString fotocuenta (QString cuenta);

        QString foto_externo (QString codigo);


        // Indica si es contabilidad analítica
        bool selectAnaliticaconfiguracion();

        // Indica si es contabilidad analítica parcial
        bool selectAnalitica_parc_configuracion ();

        // Devuelve cuentas en analítica (para analítica parcial)
        QString select_codigos_en_ctas_analitica();

        // Indica si es igic
        bool selectIgicconfiguracion();

        // Devuelve los datos de tiposiva a partir de una clave
        QSqlQuery selectTodotiposivaclave(QString cadena);

        // Devuelve la clave por defecto del IVA
        QString selectClave_iva_defectoconfiguracion();

        // Devuelve los datos del ci
        bool selectTodoCicodigo(QString codigo, QString *descripcion, QString *qnivel);

        // Devuelve la fecha de un asiento del diario
        QDate selectFechadiarioasiento(QString qasiento, QString ejercicio);

        // Devuelve el asiento de un pase
        QString selectAsientodiariopase(QString pase);

        QString select_codigo_var_evpn_pymes_diariopase (QString pase);

        // Devuelve la fecha de un pase
        QDate select_fecha_diariopase (QString pase);

            // Devuelve el diario donde está ubicado un asiento
        QString selectDiariodiarioasiento(QString qasiento, QString ejercicio);

        // Devuelve el tipo y el re de un IVA
        bool selectTiporetiposivaclave(QString cadena,QString *tipo,QString *re);

        //  Devuelve el debe-haber de una cuenta
        double selectDebehaberdiariocuenta(QString cuenta);

        // Devuelve la suma de la dotación fiscal
        double selectSumDotacion_fiscalamortfiscalycontablecuenta_activo(QString cuenta);

        // Devuelve si el vencimiento está pendiente o no
        bool selectPendientevencimientosnum(QString vto);

        // Devuelve los datos de tiposiva a partir de una clave y si existe
        int selectTodoTiposivaclave(QString cadena,QString *qtipo,QString *qre,QString *qdescrip);

        // Devuelve el nifcomunitario de la subcuenta
        // QString selectNifcomunitariodatossubcuentacuenta(QString codigo);

        // Devuelve el cif de la subcuenta
        QString selectCifdatossubcuentacuenta(QString codigo);

        // Devuelve cuenta auxiliar a parti de un cif
        QString selectCuentaCifDatossubcuenta (QString codigo);

        // Seleccina los datos de cabecera estados con una fecha especial
        QSqlQuery selectTodoespecialcabeceraestadostitulo(QString estado);

        // Devuelve una serie de campos de estados a partir de un estado
        QSqlQuery selectTituloapartadoparte1clavenodoformulaestadostitulo(QString estado);

        // 
        QSqlQuery selectCodigosaldossubcuenta();

        // Devuelve la versión de la base de datos
        QString selectVersionconfiguracion();

        QString select_url_actu_config ();

        // Devuelve la descripción del nuevoplan
        QString selectDescripcionnuevoplancodigo(QString qcodigo);

        // Devuelve asientomodelo, fecha de cab_as_modelo desde un asientomodelo
        QSqlQuery selectAsientomodelofechacab_as_modeloasientomodelo(QString nombre);

        // Devuelve variable, tipo, descripcion de var_as_modelo desde un asientomodelo
        QSqlQuery selectVariabletipodescripcionvar_as_modeloasientomodelo(QString nombre);

        // Devuelve varios datos de det_as_modelo
        QSqlQuery select_det_as_modeloasientomodelo(QString nombre);

        // Devuelve presupuesto de presupuesto a partir de ejercicio y cuenta
        QSqlQuery selectPresupuestopresupuestoejerciciocuenta(QString codigo, QString ejercicio);

        // Devuelve la suma de presupuesto de un ejercicio a partir de cuenta
        QSqlQuery selectSumpresupuestopresupuestoejerciciocuenta(QString codigo, QString ejercicio);

        // Devuelve un presupuesto de un ejercicio y cuenta
        QSqlQuery selectPresupuestopresupuestoEjercicioPosCuenta(QString codigo, QString ejercicio);

        // Devuelve un presupuesto de un presupuesto con un ejercicio una cuenta
        QSqlQuery selectPresupuestopresupuestoEjercicioCuenta(QString codigo, QString ejercicio);

        // Devuelve el código y auxiliar de plancontable a aprtir de un código
        QSqlQuery selectCodigoauxiliarplancontablecodigo(QString codigo);

        // Devuelve la dif_conciliacoin de un diario
        QSqlQuery selectDif_conciliaciondiariodif_conciliacion(QString cadena);

        // Devuelve la dif_conciliacoin de un cuenta_ext_conci
        QSqlQuery selectDif_conciliacioncuenta_ext_concidif_conciliacion(QString cadena);

        // Devuelve la dif_conciliacoin de un ajustes_conci
        QSqlQuery selectDif_conciliacionajustes_concidif_conciliacion(QString cadena);

        // Devuelve la descripción de una cuenta en dif_conciliacion
        QString selectDescripciondif_conciliacioncodigo(QString qcodigo);

        // Devuelve los códigos de los ejercicios
        QSqlQuery selectCodigoejerciciosordercodigo();

        // Devuelve datos de un diario a partir de una fecha
        QSqlQuery selectSubdebesumhabersumdebehaberdiariodiarioapertura(QDate fechainicial,QDate fechafinal,QString sub);

        // Devuelve datos del diario
        QSqlQuery selectMonthyeardebehaberdiario(QDate fechainicial, QDate fechafinal, QString sub);

        // Devuelve datos de un diario a partir de una fecha (regularización
        QSqlQuery selectSubdebesumhabersumdebehaberdiariodiarioregularizacion(QDate fechainicial,QDate fechafinal,QString sub);

        // Devuelve datos de un diario a partir de una fecha (cierre)
        QSqlQuery selectSubdebesumhabersumdebehaberdiariodiariocierre(QDate fechainicial,QDate fechafinal,QString sub);

        // Devuelve datos de tiposiva distintos de clave
        QSqlQuery selectTodoTiposivanoclave(QString Clavedefecto);

        //
        QStringList tiposiva ();

        // Devuelve lista de ejercicios de amortcontable ordenados
        QSqlQuery selectEjercicioamortcontableorderejercicio();

        // Cantidad de cuenta_activo en amortfiscalycontable para un ejercicio
        QSqlQuery selectCountcuenta_activoamortfiscalycontableejercicio(QString ejercicio);

        // Distintos datos de amortfiscalycontable
        QSqlQuery select5amortfiscalycontableejercicio(QString ejercicio);

        // Distintas sumas de amortfiscalycontable
        QSqlQuery selectSumasamortfiscalycontableejercicio(QString ejercicio);

        // Datos del diario en una fecha que sean de cierre
        QSqlQuery selectCuentadebehaberdiariofechascierre(QString cadinicioej, QString cadfinej);

        // Datos del próximo pase
        qlonglong selectProx_paseconfiguracion();

        // Códigos de los diarios ordenados
        QSqlQuery selectCodigoDiariosordercodigo();

        // Devuelve cuáles son las cuentas a pagar
        QString selectCuentasapagarconfiguracion();

        // Devuelve cuáles son las cuentas a cobrar
        QString selectCuentasacobrarconfiguracion();

        // Devuelve datos del diario  y plancontable
        QSqlQuery selectDatosDiarioPlancontable(QString cadenacuentas, QDate fechaini, QDate fechafin);

        // Cantidad de pase_diario_vencimiento de vencimiento 
        int selectCountvencimientospase_diario_vencimiento(QString pase);

        // 
        void selectSumimportevencimientospase_diario_operacion(QString pase, double v5, double v6, double *v1, double *v2);

        // Devuelve cuenta, fecha y suma de debe y haber para un pase
        bool selectCuentafechadebehaberdiariopase(QString pase, QString *qcuenta, QString *qfecha, QString *qdebehaber);

        // Cantidad de plancontable con un filtro
        int selectCountcodigoplancontablefiltro(QString filtro);

        // 
        QSqlQuery selectCodigodescripcionplancontablefiltro(QString filtro);

        // 
        QString sumadh(bool subcuenta, bool debe, QString codigo, QDate desde, QDate hasta, QString filtro, bool analitica, QString ci, bool comadecimal, bool decimales);

        QString sumadh_analitica_tabla (bool debe, QString codigo,
                                        QDate fechaini,
                                        QDate fechafin, QString ci, QString filtro,
                                        bool comadecimal, bool decimales);

        // 12 elementos de cabeceraestados desde un título
        QSqlQuery selectCabeceraestadostitulo(QString titulo);

        // Devuelve el máximo de apertura de ejercicios
        QString selectMaxaperturaejercicios();

        // 
        QString selectCodigoejercicioapertura(QString apertura);

        // Indica si los saldos medios están bloqueados
        bool selectBloqsaldosmedios();

        // Cantidad de saldossubcuenta
        int selectCountcodigosaldossubcuenta();

        // fecha, debe y haber de una cuenta en unas fechas
        QSqlQuery selectFechadebehaberdiariocuentafechasnocierreorderfecha (QString cuenta, QDate fechaini, QDate fechafin);

        // Devuelve datos de estados a partir de un título ordenado por nodo
        QSqlQuery select5Estadostituloordernodo(QString titulo);

        // 
        QSqlQuery select4Estadostituloordernodo(QString titulo, bool parte1);

        // Devuelve la formulabasepor de cabeceraestados para un titulo
        QString selectFormulabaseporcabeceraestadostitulo(QString titulo);

        QSqlQuery cuentas_saldo_estados_ci (QDate fechaini,
                                           QDate fechafin, QString subcadena,
                                                       QString ci);

        QSqlQuery cuentas_saldo_estados (QDate fechaini, QDate fechafin,
                                         QString subcadena, QString ci);

        QSqlQuery cuentas_saldo_estados_apertura (QDate fechaini, QDate fechafin,
                                         QString subcadena, QString ci);

        // Devuelve datos de cuenta debe y haber para una cuenta en unas fechas
        QSqlQuery selectCuentadebehaberdiariocuentafechasgroupcuenta(QString codigo, QDate fechaini, QDate fechafin, QString subcadena, QString ci);

        // Devuelve datos de cuenta debe y haber para una cuenta en unas fechas
        QSqlQuery selectCuentadebehaberdiariocuentaigualfechasgroupcuenta (QString codigo, QDate fechaini, QDate fechafin, QString subcadena, QString ci);

        // Devuelve importe1 y calculado de estados para un nodo
        QSqlQuery selectImporte1calculadonodoparte1titulo(QString nodo,
                                                          bool izquierdo,
                                                          QString titulo,
                                                          int mes=0);

        // Devuelve importe1 y calculado de estados para un nodo
        QSqlQuery selectImporte2calculadonodoparte1titulo(QString nodo, bool izquierdo, QString titulo);

        // Devuelve suma de saldomedio1 o saldomedio2 de saldossubcuenta a partir de código
        double selectSumsaldomediosaldossubcuentacodigo(bool ejercicio1, QString codigo, bool positivo, bool negaitvo);

        // Devuelve suma de saldomedio1 o saldomedio2 de saldossubcuenta a partir de código
        double selectSumsaldomediosaldossubcuentaigualcodigo(bool ejercicio1, QString codigo, bool positivo, bool negativo);

        // Devuelve datos de cabeceraestados a partir de un título
        QSqlQuery select14Cabeceraestadostitulo(QString titulo);

        // 
        QSqlQuery select5Estadostituloparte1ordernodo(QString titulo,bool parte1);

        //
        QSqlQuery select16Cabeceraestadostitulo(QString titulo);

        // 
        QSqlQuery select6Estadostituloparte1ordernodo(QString titulo,bool parte1);

        // Devuelve el código del plancontable
        QSqlQuery selectCodigoplancontablecodigolength(QString codigo);

        // Devuelve el código del plancontable para cuenta auxiliar
        QSqlQuery selectCodigoplancontablecodigoauxiliar(QString codigo);

        // 
        QSqlQuery select4Estadostitulo(QString titulo);

        // Devuelve la fecha mínima de un asiento para una cuenta
        QSqlQuery selectMinfechadiariocuenta(QString cuenta);

        // Devuelve la fecha mínima de un asiento para un ci
        QSqlQuery selectMinfechadiarioci (QString ci);

        // Devuelve la fecha mínima del diario
        QSqlQuery selectMinfechadiario();

        //
        QSqlQuery selectSumdebesumhaberdiariocuenta(QString cuenta);

        // Devuelve código y descripción de un ci para un nivel
        QSqlQuery selectCodigodescripcioncinivelordercodigo(QString cadnum);

        // Devuelve lista de códigos de plancontable
        QSqlQuery selectCodigoplancontablelengthauxiliar(QString cadanchoctas);


        QSqlQuery select_agrupacion_saldo_cuenta_externo (QString cadfin_ej);

        //
        QSqlQuery selectSumdebesumhaberdiariocuentafecha(QString codigo, QString cadinicioej, QString cadfinej);

        //
        QSqlQuery selectCodigodif_conciliacionordercodigo();

        //
        QSqlQuery select8Diariocuentafechasorder(QString cuenta, QDate inicial, QDate final);

        //
        QSqlQuery selectSumdebesumhaberdiariofechascuenta(QDate fechainiejercicio, QDate inicial, QString cuenta, QString externo="");

        //
        QSqlQuery selectSumdebesumhaberdiariofechacuenta(QDate inicial, QString cuenta, QString externo="");

        // 
        QSqlQuery select6Ajustes_concicuentafechasorderfecha(QString cuenta, QDate inicial, QDate final);

        // 
        QSqlQuery select8Cuenta_ext_concicuentafechasordernumero(QString cuenta, QDate inicial, QDate final);

        //
        QSqlQuery selectDif_conciliaciondiariocuentafechasgrouporderdif_conciliacion(QString cuenta, QDate inicial, QDate final);

        // 
        QSqlQuery select3Diariocuentafechasconciliadodif_conciliacionorderfecha(QString cuenta, QDate inicial, QDate final, QString dif_conciliacion);

        // 
        QSqlQuery selectDifconciliacioncuenta_ext_concicuentafechasgrouporderdif_conciliacion(QString cuenta, QDate inicial, QDate final);

        // 
        QSqlQuery selectDifconciliacionajustes_concicuentafechasgrouporderdif_conciliacion(QString cuenta, QDate inicial, QDate final);

        // 
        QSqlQuery select3Cuenta_ext_concicuentafechasdif_conciliacionorderfecha(QString cuenta, QDate inicial, QDate final, QString dif);

        // 
        QSqlQuery select3Ajustes_concicuentafechasdif_conciliacionorderfecha (QString cuenta, QDate inicial, QDate final, QString dif);

        // 
        QSqlQuery selectDif_conciliaciondiariocuentafechasdebegrouporderdif_conciliacion(QString cuenta, QDate inicial, QDate final);

        // 
        QSqlQuery select3Diariocuentafechasdebedif_conciliacionorderfecha(QString cuenta, QDate inicial, QDate final, QString dif);

        //
        QSqlQuery selectDif_conciliacioncuenta_ext_concicuentafechasdebegrouporderdif_conciliacion(QString cuenta, QDate inicial, QDate final);

        //
        QSqlQuery selectDif_conciliacionajustes_concicuentafechasdebegrouporderdif_conciliacion(QString cuenta, QDate inicial, QDate final);

        // 
        QSqlQuery select3Cuenta_ext_concicuentafechasdebedif_conciliacionorderfecha(QString cuenta, QDate inicial, QDate final, QString dif);

        // 
        QSqlQuery select3Ajustes_concicuentafechasdebedif_conciliacionorderfecha(QString cuenta, QDate inicial, QDate final, QString dif);

        // 
        QDate selectAperturaejerciciosaperturacierre(QString ini, QString fin);

        // 
        QDate selectCierreejerciciosaperturacierre(QString ini, QString fin);

        // 
        qlonglong selectCountasientodiariofechasfiltro(QDate inicial, QDate final, QString filtro);

        //
        QSqlQuery select8Diariofechasfiltroorderfechapase(QDate inicial, QDate final, QString filtro);

        // 
        QSqlQuery select9Diarioasientoorderpase(QString asiento);

        // 
        QSqlQuery select14Libroivapase(QString cadpase);

        // 
        QSqlQuery select7Diariofechasfiltroorderfechapase(QDate inicial, QDate final, QString filtro);

        // 
        QSqlQuery select9Diariofechascondicionorderfechapase(QDate inicial, QDate final, QString condicion);

        //
        qlonglong selectCountasientodiariofechascondicion(QDate inicial, QDate final, QString condicion);

        //
        QSqlQuery selectSumdebesumhaberdiariofechascondicion(QDate fechainiejercicio, QDate inicial, QString condicion);

        //
        QSqlQuery selectSumdebesumhaberdiariofechacondicion(QDate inicial, QString condicion);

        // 
        QSqlQuery selectCodigodescripcionplancontableauxiliarordercodigo(bool auxiliar);

        // 
        double selectSumimportevencimientospase_diario_operacion(QString elpase);

        // 
        bool selectsiPendientevencimientosnum(QString venci);

        // 
        qlonglong selectCountasientodiariofechascuenta(QDate inicial, QDate final, QString cuenta);

        // 
        QSqlQuery select5Diariofechascuentaorderfechapase(QDate inicial, QDate final, QString cuenta);
        QSqlQuery selectDiariofechascuentaorderfechapaseExterno (QDate inicial, QDate final, QString cuenta, QString externo);

        // 
        QSqlQuery selectCodigoperiodoejercicioorderinicioasc(QString ejercicio);

        // 
        QSqlQuery selectPresupuestospresupuestoejerciciocuenta(QString ejercicio, QString cuenta);

        // 
        QSqlQuery selectIniciofinperiodosejercicioorderinicio(QString ejercicio);

        // 
        QSqlQuery selectSumdebesumhaberdiariocuentafechasnocierrenoregularizacion(QString cuenta, QDate inicial, QDate final);

        // 
        int selectCountcuentapresupuestoauxiliar(bool subcuentas, QString selec, QString ejercicio);

        // 
        QSqlQuery selectCuentapresupuestopresupuestocuentaauxiliar(bool subcuentas, QString selec, QString ejercicio);

        // 
        QSqlQuery selectCuentasumdebesumhaberdiariofechas(int *totallineas, QString *periodo, QString ejercicio, int veces, int mescorriente);

        // 
        QSqlQuery selectClavetiposiva();

        // 
        QSqlQuery selectFechacab_as_modeloasientomodelo(QString asientomodelo);

        // Devuelve variable, tipo, descripcion de var_as_modelo desde un asientomodelo ordenado
        QSqlQuery selectTablaVariable(QString nombre);

        // 
        QSqlQuery selectDet_as_modeloasientomodeloorderorden(QString asientomodelo);

        // 
        QSqlQuery selectCodigodescripcionnuevoplanordercodigo();

        // 
        QSqlQuery selectCodigodescripcionplancontableordercodigo();

        // 
        QSqlQuery select10Diariodiarioasientoorderpase(QString elasiento, QString ejercicio);

        // 
        QSqlQuery selectProrratalibroivapase(QString pase);

        // 
        QSqlQuery selectConfiguracion();

        // devuelve si la empresa tiene activada la facturación
        bool con_facturacion();

        bool con_amoinv();

        QString cuentas_aa ();

        QString cuentas_ag ();

        bool hay_secuencia_recibidas();

        //
        QString provincia_def ();

        // 
        bool selectasientodiariofechas(QDate inicial, QDate final);

        // 
        bool selectEjerciciopresupuestoejercicio(QString elcodigo);

        // 
        QSqlQuery selectCodigodescripcionplancontablelengthauxiliarordercodigo(bool auxiliar);

        // 
        QSqlQuery selectCodequivequivalenciasplan();

        // 
        QSqlQuery selectCodigoplancontablecodigoordercodigo();

        // 
        int selectCountvariablevar_as_modeloasientomodelo(QString asientomodelo);

        // 
        QSqlQuery selectCodigoejercicioscerradocerrandoordercodigo(bool cerrado, bool cerrando);

        // 
        int selectCountcuenta_activoplanamortizaciones();

        // 
        QSqlQuery select10Planamortizacionesordercuenta_gasto();

        //
        QSqlQuery select_amoinv_cuenta_gasto();

        int registros_amoinv();

        void actu_amoinv(int numero, QString valor, QDate fecha_ult_amort);


        QSqlQuery select_cuenta_gasto_amoinv ();

        //
        QSqlQuery selectCiasignacionci_amortcuenta(QString cuenta);

        // 
        QString selectAsientoamortcontableejercicio(QString ejercicio);

        QString selectAsientoamortcontableejercicio_mes (QString ejercicio, int mes);

        //
        QSqlQuery selectCuentadebehaberdiarioasiento(QString cadasiento);

        // 
        QString selectImporteamortpersejerciciocuenta(QString ejercicio, QString cuenta);

        // 
        QSqlQuery select15Cabeceraestadostitulo(QString titulo);

        //
        bool existe_estado(QString titulo);

        // 
        QSqlQuery select5apartadoEstadostituloordernodo(QString titulo);

        //
        QSqlQuery select4AmortfiscalycontableEjercicioscuentaejercicioorderapertura(QString cuenta);

        // 
        int selectCountasientodiariofiltro(QString filtro);

        // 
        QSqlQuery select7DiarioPlancontablecuentafiltroorderasientopase(QString filtro);

        // 
        QSqlQuery selectCuenta_activocuenta_am_acumplanamortizacionesordercuenta_activo();

        // 
        QSqlQuery selectCodigocinivelordercodigo(int nivel);

        // 
        QDate selectMinaerturaejerciciosnocerrado();

        // 
        int selectCountcodigoejerciciosapertura(QString cadfecha);

        // 
        QSqlQuery selectAperturacodigoejerciciosaperturaorderapertura(QString cadfecha);

        // 
        QSqlQuery selectEjercicioimporteaperturaamortpersejerciciosorderapertura(QString ctaactivo);

        // 
        bool selectCici_amortcicuenta(QString ci, QString ctaactivo);

        // 
        QSqlQuery selectSumasignacion100ci_amortcuenta(QString cuenta);

        // 
        QSqlQuery selectClavetiposivanoclave(QString clave);

        // 
        int selectCountcuenta_fralibroivadiariofechaaibautoractura(QString fecha, bool aib, bool autofactura);

        // cuenta registros para facturas recibidas
        int num_registros_recibidas(QString ejercicio, bool fechacontable,
                                     QDate fechaini, QDate fechafin,
                                     bool interiores, bool aib, bool autofactura,
                                     bool autofacturanoe,
                                     bool rectificativa, bool porseries,
                                     QString serie_inicial, QString serie_final,
                                     bool filtrarbinversion, bool solobi, bool sinbi,
                                     bool filtrarnosujetas, bool solonosujetas,
                                     bool sinnosujetas, bool agrario,
                                     bool isp_interiores, bool importaciones, bool solo_caja,
                                     QString cuenta_fra, QString externo);

        // consulta registros facturas recibidas
        QSqlQuery registros_recibidas(QString ejercicio, bool fechacontable,
                                        QDate fechaini, QDate fechafin,
                                        bool interiores, bool aib, bool autofactura,
                                        bool autofacturanoue,
                                        bool rectificativa, bool porseries,
                                        QString serie_inicial, QString serie_final,
                                        bool filtrarbinversion, bool solobi, bool sinbi,
                                        bool filtrarnosujetas, bool solonosujetas,
                                        bool sinnosujetas,
                                        bool agrario,
                                        bool isp_interiores, bool importaciones, QString cuenta_fra,
                                        bool orden_fecha_fac=false, QString externo="");

        double total_factura_pase_iva(QString apunte, double *cuotaiva);

        void actu_venci_forzar_liq_iva(QString numvenci, QDate fecha);

        void actu_venci_quitar_forzar_liq_iva(QString numvenci);

        QSqlQuery datos_venci_apunte_iva_caja_liquidados(QString apunte);

        QSqlQuery datos_venci_apunte_iva_caja_no_liquidados(QString apunte);

        double ratio_iva_caja_liquidado(QString apuntediario, QDate fechaini, QDate fechafin);

        QSqlQuery pases_retenciones_ejercicio(QString ejercicio);

        // consulta registros facturas recibidas prorrata
        QSqlQuery registros_recibidas_prorrata(QString ejercicio, bool fechacontable,
                                        QDate fechaini, QDate fechafin,
                                        bool interiores, bool aib, bool autofactura,
                                        bool autofacturanoue,
                                        bool rectificativa, bool porseries,
                                        QString serie_inicial, QString serie_final,
                                        bool filtrarbinversion, bool solobi, bool sinbi,
                                        bool filtrarnosujetas, bool solonosujetas,
                                        bool sinnosujetas, bool agrario,
                                        bool isp_interiores, bool importaciones, bool solo_caja,
                                        QString cuenta_fra, QString externo);

        QSqlQuery iva_caja_recibidas_prorrata(QDate fechaini, QDate fechafin, bool binversion);

        // cuenta registros para facturas emitidas
        int num_registros_emitidas(QString ejercicio, bool fechacontable, QDate fechaini, QDate fechafin,
                                    bool interiores, bool aib, bool eib,
                                    bool autofactura, bool rectificativa, bool autofacturanoue,
                                    bool prestserviciosue,
                                    bool porseries,
                                    QString serie_inicial, QString serie_final,
                                    bool filtrarnosujetas, bool solonosujetas, bool sinnosujetas,
                                    bool exportaciones,
                                    bool isp_interiores, bool exentas_deduccion, bool ventas_fuera_tac,
                                   QString cuenta_fra, QString externo);

        void actu_enviado_sii(int pase, bool enviado);

        void actu_enviado_sii_fra(QString cod_factura, QDate fecha_fra, bool soportado);

        // consulta registros facturas emitidas
        QSqlQuery registros_emitidas(QString ejercicio, bool fechacontable, QDate fechaini, QDate fechafin,
                                     bool interiores, bool aib, bool eib,
                                     bool autofactura, bool rectificativa,  bool autofacturanoue,
                                     bool prestserviciosue, bool porseries,
                                     QString serie_inicial, QString serie_final,
                                     bool filtrarnosujetas, bool solonosujetas, bool sinnosujetas,
                                     bool exportaciones,
                                     bool isp_interiores, bool exentas_deduccion, bool ventas_fuera_tac, QString cuenta_fra,
                                     QString externo,bool ordenfechafac=false,
                                     bool ordendocumento=false);

        QSqlQuery registros_emitidas_liq_caja(QDate fechaini,
                                                QDate fechafin);

        // 
        QSqlQuery select10DiarioLibroivaPlancontablefechaaibautofacturaorderfechaasiento(QString fecha, bool aib, bool autofactura);

        // 
        int selectCountcuenta_fralibroivadiariofechaeibaibautofacturarectificativa(QString fecha, bool eib, bool aib, bool autofactura, bool rectificativa);

        // 
        QSqlQuery select12DiarioLibroivaPlancontablefechaaibeibautofacturarectivicativa(QString fecha, bool aib, bool eib, bool autofactura, bool rectificativa);

        // 
        int selectCountcuenta_fralibroivadiariofechaaib(QString fecha, bool aib);

        // 
        QSqlQuery select11DiarioLibroivaPlancontablefechaaiborderfechaasientodocumento(QString fecha, bool aib);

        // 
        QSqlQuery libroivafechas_sop_corrientes(QDate inicial, QDate final, bool cajaiva=false);

        //
        QSqlQuery libroivafechas_sop_inversion (QDate inicial, QDate final, bool cajaiva=false);

        QSqlQuery libroivafechas_sop_corrientes_importacion (QDate inicial, QDate final);

        QSqlQuery libroivafechas_sop_inversion_importacion (QDate inicial, QDate final);

        QSqlQuery libroivafechas_sop_agrario (QDate inicial, QDate final);
        //
        QSqlQuery select7DiarioLibroivafechasgroupcuentatipo_ivatipo_re(QDate inicial, QDate final,
                                                                        bool caja_iva=false);

        QSqlQuery baserecibidasexentas_corrientes (QDate inicial, QDate final);

        QSqlQuery baserecibidasexentas_corrientes_importacion (QDate inicial, QDate final);

        QSqlQuery baserecibidasexentas_inversion (QDate inicial, QDate final);

        QSqlQuery baserecibidasexentas_inversion_importacion (QDate inicial, QDate final);

        QSqlQuery baseemitidasexentasnoeib (QDate inicial, QDate final);

        QSqlQuery baseemitidasexentas_noeib_noexport (QDate inicial, QDate final);

        QSqlQuery baseemitidasexentas_noeib_noexport_nodeduc (QDate inicial, QDate final);

        QSqlQuery selectSumbase_emitidas_isp (QDate inicial, QDate final);

        // 
        QSqlQuery libroiva_aib_corrientes(QDate inicial, QDate final);

        //
        QSqlQuery libroiva_aib_inversion (QDate inicial, QDate final);

        //
        QSqlQuery select4DiarioLibroivafechasgroupcuentatipo_ivasinprorrata(QDate inicial, QDate final);

        // 
        QSqlQuery selectSumbase_ivalibroivadiariofechas(QDate inicial, QDate final);

        // suma base prestación servicios ue
        QSqlQuery selectSumbase_serv_ue_libroivadiariofechas (QDate inicial, QDate final);

        QSqlQuery selectSumbase_expor_fechas (QDate inicial, QDate final);

        QSqlQuery selectSumbase_expor_fechas_no_sujetas (QDate inicial, QDate final);

        //
        double selectSumimportevendimientosfiltroderecho(QString filtro, bool derecho);

        double selectSumimpagados (QString filtro, bool derecho);
        // 
        QDate selectMaxcierrecountcierreejercicios();

        // 
        bool selectCodigoejercicioscodigo(QString codigo);

        // 
        QSqlQuery selectAperturacierreejercicios();

        // 
        QSqlQuery selectCodigoiniciofinperiodosejercicioorrderinicio(QString ejercicio);

        // 
        QSqlQuery select9Planamortizacionesordercuenta_activo();

        QSqlQuery select_inv_inmovilizado ();

        QSqlQuery select_reg_inv_inmovilizado (QString numero);

        void borra_reg_inv_inmovilizado (QString numero);

        void inserta_item_inmov(QString cta_aa,
                                           QString cta_ag,
                                           QString concepto,
                                           QDate fecha_inicio,
                                           QDate fecha_compra,
                                           QDate fecha_ult_amort,
                                           QString coef,
                                           QString valor_adq,
                                           QString valor_amortizado,
                                           QString factura,
                                           QString proveedor);

        void modifica_item_imov(QString num,
                                           QString cta_aa,
                                           QString cta_ag,
                                           QString concepto,
                                           QDate fecha_inicio,
                                           QDate fecha_compra,
                                           QDate fecha_ult_amort,
                                           QString coef,
                                           QString valor_adq,
                                           QString valor_amortizado,
                                           QString factura,
                                           QString proveedor);

        // 
        QSqlQuery selectBasesPresupuestospresupuestoejerciciocuenta(QString ejercicio, QString cuenta);

        // 
        QSqlQuery select4Ejercicioscodigo(QString qejercicio);

        // 
        QSqlQuery select6casepresupuestoplancontableejercicio(QString ejercicio, bool subcuentas);

        // 
        QSqlQuery selectCuentapresupuestoejercicio(QString ejercicio);

        // 
        QSqlQuery selectPresupuestospresupuestocuentaejercicio(QString cuenta, QString ejercicio);

        // 
        QSqlQuery selectCodigodescripcionauxiliarplancontablesubcuentasordercodigo(bool subcuentas);

        // 
        QSqlQuery select20Borradorasiento(qlonglong AstoEspera);

        // 
        QDate selectMaxfechacuenta_ext_concicuenta(QString cuenta);

        // 
        QSqlQuery select9Vencimientosnum(QString numvenci);

        // 
        QSqlQuery selectAsientodocumentodiariopase(QString pase);

        // 
        QSqlQuery select4Cabeceraestadostitulo(QString titulo);

        // 
        int selectCountnodoestadostituloparte1(QString titulo, bool parte1);

        // 
        QSqlQuery select3Estadostituloparte1ordernodo(QString titulo, bool parte1);

        // 
        QSqlQuery selectclavescuenta_pygconfiguracion();

        // 
        QSqlQuery selectCuenta_frabase_ivafechasgroupordercuenta_fra(
                QDate inicial, QDate final, bool aib, bool eib,
                bool radio1, bool radio2, bool autofacuraue, bool autofacturanoue,
                bool prestserviciosue, bool isp_op_interiores);

        QSqlQuery info349_recibidas(QDate inicial, QDate final);

        QSqlQuery info349_emitidas(QDate inicial, QDate final);

        QSqlQuery datos_donaciones (QDate inicial, QDate final);


        // Datos 347
        QSqlQuery datos347 (QDate inicial, QDate final, QString cadenacuentas);

        // volumen operaciones en libroiva por cuenta
        double vol_op_iva_cuenta (QString cuenta, QDate inicial, QDate final,
                                  bool aib, bool eib, bool radio1, bool radio2,
                                  bool autofacturaue, bool autofacturanoue,
                                  bool prestserviciosue, bool isp_op_interiores, QString externo="");

        // volumen operaciones 347
        double vol_op_347_cuenta (QString cuenta, QDate inicial, QDate final, QString externo, bool soportado, bool isp_op_interiores);

        // 
        QSqlQuery select7Datossubcuentacuenta(QString cuenta);

        QSqlQuery select7datos_externo (QString externo);

        // 
        QSqlQuery selectsumdiarionoaperturanocierrefechas(bool radio1, QString ejercicio, QString cadenacuentas);

        // devuelve volumen de operaciones a partir de los datos del diario primer trimestre
        double operaciones_diario_1T (bool radio1, QString ejercicio,
                                                     QString cadenacuentas);

        // devuelve volumen de operaciones a partir de los datos del diario segundo trimestre
        double operaciones_diario_2T (bool radio1, QString ejercicio,
                                                     QString cadenacuentas);

        // devuelve volumen de operaciones a partir de los datos del diario tercer trimestre
        double operaciones_diario_3T (bool radio1, QString ejercicio,
                                                     QString cadenacuentas);

        // devuelve volumen de operaciones a partir de los datos del diario tercer trimestre
        double operaciones_diario_4T (bool radio1, QString ejercicio,
                                                     QString cadenacuentas);

        // 
        QSqlQuery select6Datossubcuentacuenta(QString cuenta);

        // 
        qlonglong selectMaxasientofecha(QDate fecha);

        qlonglong min_asiento (QString ejercicio);

        //
        qlonglong selectMaxpasediariofecha(QDate fecha);

        // 
        int selectCountasientodiariofecha(QDate fecha);

        // 
        QSqlQuery selectAsientopasediariofecha(QDate fecha);

        // 
        QSqlQuery selectAsientopasediariofechaorderfechaasientopase(QDate fecha);

        // 
        QSqlQuery selectAsientomodelocab_as_modeloorderasientomodelo();

        // 
        QSqlQuery selectTitulocabeceraestadosordertitulo();

        // 
        QSqlQuery select7Vencimientosfechasanuladoordernum(QDate inicial, QDate final);

        // 
        QSqlQuery select8Borradororderasiento();

        // 
        QSqlQuery select9Vencimientospase_diario_operacion(QString qpase);

        // 
        QSqlQuery select11Diariofiltro(QString filtro);

        QSqlQuery selectDiariofiltro_nomsj_error (QString filtro, bool *correcto);

        QSqlQuery select6ejerciciosorderapertura();

        void actu_bloquea_ejercicio(QString codigo, bool bloqueado);

        bool ejercicio_bloqueado(QString codigo);

        // 
        QSqlQuery select10Planamortizacionesordercuenta_activo();

        // 
        double selectSaldosaldossubcuentacodigo(QString codigo, bool *ok);

        // 
        int selectMaxasientoborrador();

        // Actualiza vencimientos a partir de pase_diario_vencimiento
        void updateVencimientospase_diario_vencimientopendientepase_diario_vencimiento(QString pase);

        // Actualiza saldos subcuenta a partir del saldo y el código
        void updateSaldossubcuentasaldomenosmascodigo(QString menos, QString mas, QString codigo);

        // Actualiza la descripción de una cuenta del plancontable
        void updatePlancontabledescripcioncodigo(QString codigo,QString descripcion);

        // Actualiza la descripción de la clave de conceptos
        void updateConceptosdescripcionclave(QString codigo,QString descripcion);

        // Actualiza la descripción del código de la tabla
        void updatetabladescripcionclave (QString tabla, QString codigo,QString descripcion);

        // Actualiza la descripción y el nivel de un ci
        void updateCidescripcioncodigo(QString codigo,QString descripcion,int nivel);

        // Actualiza saldos subcuenta a partir del importe y el código
        void updateSaldossubcuentasaldocargocodigo(QString cuenta,QString importe,bool cargo);

        // Actualiza los vencimientos con un numero
        void updateVencimientosanuladonum(QString vto,bool b);

        // Actualiza los tipos de iva con una clave
        void updateTiposivatiporedescripcionclave(QString clave,QString tipo,QString re,QString descripcion);

        // Desbloquea saldos medios
        void updateConfiguracionbloqsaldosmedios();

        // Actualiza los saldossubcuenta con un codigo
        void updateSaldossubcuentasaldocodigo(QString cuenta,QString importe,bool cargo);

        // Establece todos los saldos a 0
        void updateSaldossubcuentasaldo0();

        bool esta_en_saldossubcuenta(QString codigo);

        void actucodigos_saldossubcuenta();

        // Actualiza nuevoplan con un código
        void updateNuevoplandescripcioncodigo(QString codigo,QString descripcion);

        // Actualiza amortfiscalycontable con una cuenta_activo
        void updateAmortfiscalycontablecuenta_activocuenta_activo(QString origen, QString destino);

        // Actualiza amortfiscalycontable con una cuenta_am_acum
        void updateAmortfiscalycontablecuenta_am_acumcuenta_am_acum(QString origen, QString destino);

        // Actualiza amortfiscalycontable con una cuenta_gasto
        void updataAmortfiscalycontablecuenta_gastocuenta_gasto(QString origen, QString destino);

        // Actualiza borrador con una cuenta
        void updateBorradorcuentacuenta(QString origen, QString destino);

        // Actualiza datossubcuenta con una cuenta
        void updateDatossubcuentacuentacuenta(QString origen, QString destino);

        // Actualiza el diario con una cuenta
        void updateDiariocuentacuenta(QString origen, QString destino);

        // Actualiza libroiva con una cuenta base iva
        void updateLibroivacta_base_ivacta_base_iva(QString origen, QString destino);
    
        // Actualiza libroiva con una cuenta_fra
        void updatelibroivacuenta_fracuenta_fra (QString origen, QString destino);

        // Actualiza planamortizaciones con una cuenta_activo
        void updateplanamortizacionescuenta_activocuenta_activo(QString origen, QString destino);

        // Actualiza planamortizacoines con una cuenta_am_acum
        void updatePlanamortizacionescuenta_am_acumcuenta_am_acum(QString origen, QString destino);

        // Actualiza planamortizacioens con una cuenta_gasto
        void updatePlanamortizacionescuenta_gastocuenta_gasto(QString origen, QString destino);

        // Actualiza amortpers con una cuenta
        void updateAmortperscuentacuenta(QString origen, QString destino);

        // Actualiza saldossubcuenta con un codigo
        void updateSaldossubcuentacodigocodigo(QString origen, QString destino);

        // Actualiza vencimientos desde cta-ordenante
        void updateVencimientoscta_ordenantecta_ordenante(QString origen, QString destino);

        // Actualiza vencimientos desde cta_tesoreria
        void updateVencimientoscta_tesoreriacta_tesoreria(QString origen, QString destino);

        // Actualiza Diarios desde un código
        void updateDiariosdescripcioncodigo(QString codigo, QString descripcion);

        // Actualiza dif_conciliacion desde un código
        void updateDif_conciliaciondescripcioncodigo(QString codigo, QString descripcion);

        // Actualiza el prox_asiento de configuracion
        void update_ejercicio_prox_asiento(QString ejercicio, QString cadnum);

        // Actualiza el prox_pase de configuracion
        void updateConfiguracionprox_pase(QString cadnumpase);

        // Actualiza el prox_pase de configuracion
        void updateConfiguracionprox_vencimiento(QString cadnumpase);

        // Actualiza el diario de diario en un asiento
        void updateDiariodiarioasiento(QString diario, QString asiento, QString ejercicio);

        // Bloquea o desbloquea saldos medios
        void updateConfiguracionbloqsaldosmedios(bool b);

        // Actualiza saldomedio1 de saldossubcuenta
        void updateSaldossubcuentasaldomedio1codigo(QString cadnum, QString codigo);

        // Actualiza saldomedio2 de saldossubcuenta
        void updateSaldossubcuentasaldomedio2codigo(QString cadnum, QString codigo);

        // Actualiza estados con calcularo falso para un título
        void updateEstadoscalculadotitulo(bool b, QString titulo);

        // Actualiza Estados con importe1 importe2 calculado desde parte1, nodo y título
        void updateEstadoscalcularestado(double resultado1, double resultado2, bool calculado, bool parte1, QString qnodo, QString titulo);

        void actu_rdos_mes (double m1, double m2, double m3, double m4, double m5,
                            double m6,
                            double m7, double m8, double m9, double m10, double m11,
                            double m12,
                            bool calculado, bool parte1,
                            QString qnodo, QString titulo);

        // Actualiza cabeceraestados con valores base para un título
        void updateCabeceraestadosvalorbaseportitulo(double resultado1, double resultado2, QString titulo);

        // Actualiza datos de cabeceraestados
        void updateCabeceraestadosfechacalculoejercicio1ciejercicio2titulo(QDate fechaactual, QString ejercicio1, QString ci, QString ejercicio2, QString titulo);

        //
        void updateDiariocuentafechasnodiario_apertura(QString por, QString origen, bool fechas, QDate inicial, QDate final);

        // Actualiza el diario a partir de cuenta
        void updateDiariocuentapase(QString codigo, QString apunte);

        void updateconcepto_evpn_pymes (QString apunte, QString concepto);

        // Actualiza el ejercicio a partir de código, campo cerrando
        void updateEjercicioscerrandocodigo(bool cerrando, QString codigo);

        // Actualiza campo cerrado
        void updateEjercicioscerradocodigo (bool cerrado, QString codigo);

        //
        void updateDiarioconciliadodif_conciliacionpase(bool conciliado, QString cad, QString pase);

        // 
        void updateCuenta_ext_conciconciliadodif_conciliacionnumero(bool conciliado, QString dif, QString numero);

        // 
        void updateDiariofechaasiento(QDate fecha, QString gasiento);

        // 
        void updateEjercicioscierrecodigo(QDate fecha, QString ejercicio);

        // 
        void updateDiarioconceptodocumentopase(QString concepto, QString documento, QString numpase, bool asiento=false);


        bool hay_criterio_caja_config();

        bool hay_externos();

        bool guardar_borrados_consol();

        QString msj_vto_dcho();

        QString msj_vto_obl();

        bool tipo_proveedor_config();

        //
        void updateConfiguracion(QString empresa , QString nif, QString domicilio, QString poblacion,
                                 QString cpostal, QString provincia, QString email, QString web,
                                 QString cuenta_iva_soportado, QString cuenta_iva_repercutido,
                                 QString retirpf,
                                 QString cuenta_ret_irpf, QString cuentasapagar, QString cuentasacobrar,
                                 QString clave_iva_defecto, QString clave_gastos, QString clave_ingresos,
                                 QString cuenta_pyg, QString prox_pase,
                                 QString prox_vencimiento, bool igic, bool analitica,
                                 QString id_registral, QString prorrata_iva, bool prorrata_especial,
                                 QDate fechaconst, QString objeto, QString actividad, bool analitica_parc,
                                 QString ctas_analitica,
                                 bool usuarios, QString proxdoc, bool analitica_tabla,
                                 QString provincia_def,
                                 QString cuentas_aa, QString cuentas_ag, bool amoinv,
                                 bool sec_recibidas, QString prox_domicilia, QString cod_ine_plaza,
                                 bool caja_iva, bool borrados_consol, QString msj_vto_dcho, QString msj_vto_obl,
                                 bool externos, QString tfno, bool gd_bd, bool tipo_proveedor, QString cod_homol_pruebas,
                                 QString nombre, QString apellidos, bool curl, QString url_actu);

        // 
        void updateDiarioasientoasiento(QString nuevo, QString antiguo, QString ejercicio);

        // 
        void updatePlanamortizaciones9cuenta_activo(QDate fecha_func,
                                                    QString codigo_activo,
                                                    QString cuenta_am_acum,
                                                    QString cuenta_gasto,
                                                    bool amort_personalizada,
                                                    QString valor_residual,
                                                    QString coef_amort_contable,
                                                    QString coef_amort_fiscal,
                                                    bool amort_fiscal_1ejer,
                                                    bool baja, QDate fecha_baja,
                                                    QString motivo_baja,
                                                    QString cuenta_proveedor,
                                                    QString cuenta_activo, QString externo);

        // 
        void updateCi_amortasignacioncuentaci(QString asignacion, QString cuenta, QString ci);

        // 
        void updateVencimientos7num(QString cta_ordenante, QDate fecha_operacion,
                                    QString importe, QString cuenta_tesoreria,
                                    QDate fecha_vencimiento, QString concepto,
                                    QString num, bool domiciliable, QString externo);

        // 
        void updatePresupuestopresupuestosejerciciocuenta(QString presupuesto, QStringList lista, QString ejercicio, QString cuenta);

        // 
        void updateEjercicios4codigo(bool sibase_ejercicio, QString base_ejercicio, bool sibase_presupuesto, QString base_presupuesto, bool presupuesto_base_cero, QString incremento, QString ejercicio);

        // 
        void updatePresupuestobaseejerciciobaseejerciciocuenta(QString base, QString ejerciciobase, QString ejercicio, QString cuenta);

        // 
        void updatePresupuestobaseejerciciocuenta(QString basen, QString base, QString ejercicio, QString cuenta);

        // 
        void updatePresupuestobasespresupuestobaseejerciciocuenta(QStringList lista, QString presupuestobase, QString ejercicio, QString cuenta);

        // 
        void updatePresupuestopresupuestoincrementoejerciciocuenta(QString incremento, QString ejercicio, QString cuenta);

        // 
        void updateVencimientos4num(QString cuenta_tesoreria,
                                    QDate fecha_vencimiento,
                                    bool pendiente,
                                    QString pase_diario_vencimiento,
                                    QString medio_realizacion,
                                    QString cuenta_bancaria,
                                    QString num);

        // 
        void updateEstadosreferencianodotituloparte1(QString referencia, QString nodo, QString titulo, bool parte1);

        // 
        void updateLibroivapasepase(QString antiguo, QString nuevo);

        // 
        void updateVencimientospase_diario_operacionpase_diario_operacion(QString antiguo, QString nuevo);

        // 
        void updateVencimientospase_diario_vencimientopase_diario_vencimiento(QString antiguo, QString nuevo);

        // 
        void updateDiariopase5000000fecha(QDate inicioej);

        // 
        void updateDiariosetasientopaseasientopase(QString asientoA, QString paseA, QString asientoN, QString paseN);

        // Devuelve el primer asiento a partir de una fecha
        qlonglong primasejerciciodelafecha(QDate fecha);

        //
        void cambia_asiento_a_pase (QString pase, QString asientoN);

        // 
        void calculaprimerasientoejercicio(QString ejercicio);

        // 
        QString numrelativa(QString numabsoluta);

        // Indica si se sobreescribe el próximo asiento
        bool sobreescribeproxasiento(QString ejercicio);

        // Indica si existe un código en el plan contable
        int existecodigoplan(QString cadena,QString *qdescrip);

        QString externo_pase(QString pase);

        QString concepto_sii_pase(QString pase);

        bool existe_externo(QString codigo);

        QString razon_externo(QString codigo);

        QString cuenta_externo(QString codigo);

        QString cif_externo(QString codigo);

        bool hayivaasociado_externo(QString codigo, QString &cuentaiva, QString &tipoiva, QString &cta_base_iva);

        bool hayretasociada_externo(QString codigo, QString *cuentaret, QString *ret, bool *arrend,
                                       QString *tipo_oper_ret);

        bool hayvenciasociado_externo(QString codigo);

        bool hayconcepto_externo_asociado(QString codigo, QString &concepto);

        bool carga_externo(QString codigo, QString *cuenta, QString *razon, QString *ncomercial, QString *cif,
                           QString *nombre, QString *apellidos,
                           QString *nifrprlegal, QString *domicilio, QString *poblacion,
                           QString *codigopostal, QString *provincia, QString *pais,
                           QString *tfno, QString *fax, QString *email, QString *observaciones,
                           QString *ccc, QString *cuota, bool *venciasoc, QString *codvenci,
                           QString *tesoreria, bool *ivaasoc, QString *cuentaiva, QString *cta_base_iva, QString *tipoiva,
                           QString *conceptodiario, QString *web, QString *claveidfiscal, bool *procvto,
                           QString *paisdat, int *vfijo, QString *cuenta_ret_asig,
                           bool *es_ret_asig_arrend, QString *tipo_ret_asig, QString *tipo_oper_ret_asig,
                           bool *dom_bancaria, QString *IBAN, QString *nriesgo, QString *cedente1,
                           QString *cedente2, QString *bic, QString *sufijo,
                           QString *ref_mandato, QDate *firma_mandato,
                           bool *caja_iva, QString *forma_pago,
                           QString *imagen);

        void guarda_datos_externo (QString codigo, QString cuenta, QString razon, QString ncomercial, QString cif,
            QString nifrprlegal, QString nombre, QString apellidos, QString domicilio, QString poblacion,
            QString codigopostal, QString provincia, QString pais,
            QString tfno, QString fax, QString email, QString observaciones,
            QString ccc, QString cuota, bool ivaasoc, QString tipoiva, QString cuentaiva, QString cta_base_iva,
            bool venciasoc, QString tipovenci, QString cuentatesor, QString conceptodiario,
            QString web, QString codclaveidfiscal, bool procvto,
            QString pais_dat, int vdiafijo,
            QString cuenta_ret_asig, bool es_ret_asig_arrend,
            QString tipo_ret_asig, QString tipo_oper_ret_asig,
            bool dom_bancaria, QString iban, QString nriesgo, QString cedente1, QString cedente2,
            QString bic, QString sufijo, bool caja_iva, QString ref_mandato, QDate firma_mandato,
            QString forma_pago,
            QString fototexto);

        QSqlQuery consulta_busca_externos(QString campo, int criterio, QString contenido);

        int maxcodcta(QString cuenta);

        // Indica si una subcuenta es cuenta de iva soportado
        int escuentadeivasoportado (const QString subcuenta);

        // Indica si una subcuenta es cuenta de iva repercutido
        int escuentadeivarepercutido (const QString subcuenta);

        // Indica se es cuenta de retención irpf practicada
        bool escuenta_de_ret_irpf (const QString subcuenta);

        // Indica se es cuenta de retención irpf a ingresar
        bool escuenta_de_ret_ing (const QString subcuenta);

        QSqlQuery select_ppdiario ();

        // Borra los asientos desde el inicial hasta el final con transacción
        void borraasientos(QString asientoinicial, QString asientofinal, QString ejercicio, bool papelera=true);

        // Borra los asientos desde el inicial hasta el final sin transacción
        void borraasientosnotransac (QString asientoinicial, QString asientofinal, QString ejercicio, bool papelera=true);

        // Registra apunte borrado con marca temporal
        void registra_apunte_borrado(QString pase);

        void recupera_pp_asiento(QString asiento, QString ejercicio);

        void borra_pp_asiento(QString asiento, QString ejercicio);

        void borrar_papelera();

        bool papelera_vacia();

        QSqlQuery select_pase_ppdiario (QString asiento,
                                        QString ejercicio);

        void borraasientosnotransac_ni (QString asientoinicial,QString asientofinal,
                                        QString ejercicio);

        void borraasientosnotransac_no_copiadoc (QString asientoinicial,QString asientofinal,
                                                 QString ejercicio);

        // Indica si hay un asiento en amortcontable entre el inicial y el final
        bool amortenintasientos(QString inicial,QString final,QString ejercicio);

        //Indica si un pase corresponde a un asiento
        bool paseenasiento(QString pase, QString asiento);
    
        // Indica si la cuenta pertenece a un grupo de cuentas
        int subgrupocongrupo(QString cadena);

        // Indica si una cuenta pertenece a un subgrupo
        int cuentaconsubgrupo(QString cadena);

        // Indica si una cuenta coincide con un subgrupo de 3 dígitos
        int codigoconcuenta3digitos(QString cadena);

        // Indica si una cuenta coincide con un subgrupo de N dígitos
        int codigoconcuentandigitos(QString cadena,QString digitos);

        // Elimina un elemento del plancontable si no tiene "hijos"
        int itemplaneliminado(QString qcodigo);

        // Indica si una cuenta tiene apuntes en el diario
        int cuentaendiario(QString cadena);

        bool externoendiario (QString cadena);

        void guardadatos_tipo_proveedor(QString cuenta, QString tipo_proveedor, QString homologacion,
                                        QString actividad, QDate fecha_homologacion,
                                        bool proveedor_activo, bool acceso_lopd, bool cumpli_lopd,
                                        bool acceso_prl, bool coord_prl, bool sol_cert_ss,
                                        bool sol_cert_hda, bool no_correo_pub);

        // Guarda los datos accesorios de una subcuenta
        void guardadatosaccesorios(QString cuenta, QString razon, QString ncomercial, QString cif,
            QString nifrprlegal, QString domicilio, QString poblacion,
            QString codigopostal, QString provincia, QString pais,
            QString tfno, QString fax, QString email, QString observaciones,
            QString ccc, QString cuota, bool ivaasoc, QString tipoiva, QString cuentaiva, QString cta_base_iva,
            bool venciasoc, QString tipovenci, QString cuentatesor, QString conceptodiario,
            QString web, QString codclaveidfiscal, bool procvto,
            QString pais_dat, QString tipo_ret, bool ret_arrendamiento, QString tipo_operacion_ret,
            int vdiafijo,
            QString cuenta_ret_asig, bool es_ret_asig_arrend,
            QString tipo_ret_asig, QString tipo_oper_ret_asig,
            bool dom_bancaria, QString iban, QString nriesgo, QString cedente1, QString cedente2,
            QString bic, QString sufijo, bool caja_iva, QString ref_mandato, QDate firma_mandato, QString forma_pago,
            QString fototexto);

        // Indica si existe un concepto y devuelve su descripción en puntero
        int existecodigoconcepto(QString cadena,QString *qdescrip);
        bool existecodigotabla (QString tabla, QString cadena,QString *qdescrip);

        // Indica si la subcuenta es de gasto
        int escuentadegasto(const QString subcuenta);

        // Indica si la subcuenta es de ingresos
        int escuentadeingreso(const QString subcuenta);

        // Devuelve si existe una fila en saldossubcuenta para una cuenta
        int existecodigosaldosubcuenta(QString cadena);

        // Devuelve la cadena de inicio del iva soportado
        QString cuentadeivasoportado();

        // Devuelve la cadena de inicio del iva repercutido
        QString cuentadeivarepercutido();

        // Indica si un ci existe en el diario
        bool existeciendiario(QString codigo, int nivel);

        // Indica si un ci existe en ci_amort
        bool existecienplanamort(QString codigo, int nivel);

        // Devuelve la subcuenta anterior a una dada
        QString subcuentaanterior(QString qcodigo);

        // Devuelve la subcuenta posterior a una dada
        QString subcuentaposterior(QString qcodigo);

        // Indica si un asiento tiene aib
        bool asientoenaib(QString asiento, QString ejercicio);

        // Indica si un asiento tiene autofactura
        bool asientoenautofactura (QString asiento, QString ejercicio);

        // Indica si un asiento tiene isp operaciones interiores
        bool asiento_isp_op_interiores(QString asiento, QString ejercicio);

        // Indica si un asiento tiene autofactura no ue
        bool asientoenautofactura_no_ue (QString asiento, QString ejercicio);

        // Indica si un asiento tiene pr_servicios_ue
        bool asientoenpr_servicios_ue (QString asiento, QString ejercicio);

        // Indica si un asiento tiene aib
        bool asientoeneib(QString asiento, QString ejercicio);

        bool asientoenopnosujeta (QString asiento, QString ejercicio);

        // Asiento con apunte de iva exento no aib, no eib
        bool asiento_exento_noaib_noeib (QString asiento, QString ejercicio);

        // Devuelve si existe un asiento en el diario
        int existeasiento(QString asiento, QString ejercicio);

        // Devuelve si existe apunte en el diario
        bool existe_apunte (QString apunte);

        // Indica si hay una cuenta_activo en plan amortizaciones con un codigo_activo
        bool cuentaycodigoenplanamort(QString cuenta,QString codigo);

        // Indica si un ejercicio está en amortcontable
        bool practamort(QString ejercicio);

        bool practamort_mes(QString ejercicio, int mes);

        void asig_amort_mes_asiento(QString ejercicio, int mes, QString cadasiento);
        void borra_amort_mes_asiento(QString ejercicio, int mes);
        bool amortmensual_practicada(QString ejercicio);

        // Indica si es pase de vencimiento procesado
        bool espasevtoprocesado(QString qpase);

        // 
        QString ejercicio_igual_o_maxigual(QDate fecha);

        // Devuelve el número de asiento de vencimiento
        QString numasientovto(QString vto);

        // 
        QString numasientovtovto(QString vto);

        QDate fecha_pase_diario_vto(QString vto);

        // Indica si existe vencimiento
        bool existevencimiento(QString vto);

        QString documento_num_operacion_vencimiento(QString numvenci);

        // Devuelve si hay algún título en cabeceraestados
        int sihaytitulocabeceraestados(QString titulo);

        // 
        bool int_asientos_ej_cerrado(QString asientoinicial,QString asientofinal);

        // 
        bool cuentaenlibroiva(QString cuenta);

        // Indica si es un pase de apertura
        bool paseapertura(QString pase);

        //
        bool paseconvto(QString pase);

        // 
        bool pasepagovto(QString pase);

        // 
        bool paseenlibroivaeib(QString pase);

        bool paseenlibroiva_isp (QString pase);

        // Indica si una cuenta pertenece a un grupo
        int subgrupocongruponuevoplan(QString cadena);

        // Indica si una cuenta pertenece a un grupo
        int cuentaconsubgruponuevoplan(QString cadena);

        // Indica si una cuenta pertenece a un grupo
        int codigoconcuenta3digitosnuevoplan(QString cadena);

        // Indica si una cuenta pertenece a un grupo
        int codigoconcuentandigitosnuevoplan(QString cadena, QString n);

        // Si existe un nuevo plan y su descripción
        int existecodigoplannuevo(QString cadena,QString *qdescrip);

        // 
        int siCodigoennuevoplanconlength(QString codigo);

        // Si el código posee subcódigos que no sean subcuentas
        bool codsubdivis(QString qcodigo,QString tabla);

        //
        int asientomodeloencab_as_modelo (QString titulo);

        // Indica si existe un código en el diario y devuelve su descripción
        int existecodigodiario(QString cadena,QString *qdescrip);

        // Borra un diario si no tiene asientos
        int eliminadiario(QString qcodigo);

        // Indica si existe cuenta con presupuesto en el ejercicio
        bool existectapres(QString cuenta,QString ejercicio);

        // Indica si la empresa tiene prorrata especial
        bool prorrata_especial();

        // Indica la prorrata iva de la empresa
        double prorrata_iva();

        // Indica si hay cuenta_activo en planamortizaciones
        bool cuentaenplanamort(QString codigo);

        // Indica si existe codigo en dif_conciliacion para una cuenta
        bool existecodigodif_conc(QString cadena);

        // Indica si una cuenta está en el presupuesto
        bool cuentaenpresupuesto(QString cadena);

        // Indica si una cuenta está en ajustes_conci
        bool cuentaen_ajustes_conci(QString cadena);

        // Indica si una cuenta está en cuenta_ext_conci
        bool cuentaencuenta_ext_conci(QString cadena);

        // Devuelve la cantidad de pases de un diario en una fecha que sea de cierre
        int registroscierrediarioenfechas(QString cadinicioej, QString cadfinej);

        // para bloquear las tablas para asiento apertura
        void bloquea_para_asientoapertura();

        // para bloquear las tablas para asiento de cierre
        void bloquea_para_asientocierre();

        // para bloquear las tablas para asignavencimientos
        void bloquea_para_asignavencimientos();

        void bloquea_configuracion();

        // bloquea tablas para genamort
        void bloquea_para_genamort();

        // bloquea las tablas para listavenci
        void bloquea_para_listavenci();

        // bloquea las tablas para nuevo_vencimiento
        void bloquea_para_nuevo_vencimiento();

        // bloquea las tablas para importar asientos
        void bloquea_para_importar_asientos();

        // bloquea las tablas para procesavencimientos
        void bloquea_para_procesavencimiento();

        // bloquea las tablas para regularización
        void bloquea_para_regularizacion();

        // bloquea para regularización a medida
        void bloquea_para_regularizacion2();

        // bloquea las tablas para tabla asientos
        void bloquea_para_tabla_asientos();

        // bloquea para borrador
        void bloquea_para_borrador();

        // efectúa commit y desbloquea tablas en mysql
        void desbloquea_y_commit();

        // Indica cuál es el próximo asiento
        qlonglong proximoasiento(QString ejercicio);

        // próximo número recepción fras. recibidas
        qlonglong proximo_nrecepcion (QString ejercicio);

        // Indica cuál es el próximo vencimiento
        qlonglong proximovencimiento();

        // Filtro para bss
        QString filtrobss(bool grupos, bool subgrupos, bool cuentas3dig, bool cuentas,
                          bool subcuentas,
                          bool cuentasgroup,QString desde, QString hasta);

        // 
        bool calcestadohaymedias(QString titulo);

        // Devuelve la cantidad de códigos en plancontable
        int cantidadplancontable(QString cadanchoctas);

        // Devuelve la cantidad de códigos en plancontable con filtro
        int cantidadplancontablefiltro(QString cadanchoctas, QString filtro);

        // 
        bool siCuentaenpresupuestoejercicio(QString ejercicio, bool subcuentas);

        // Actualiza las tablas de la base de datos desde la versión 2.0 a 2.1
        void actualizade20();

        // Actualiza las tablas de la base de datos desde la versión 2.1 a 2.2
        void actualizade21();

        // actualizade22 actualiza de 2.2 a 2.2.1
        void actualizade22();

        void actualizade221();

        void actualizade23();

        void actualizade24();

        void actualizade25();

        void actualizade251();

        void actualizade26();

        void actualizade27();

        void actualizade28();

        void actualizade29();

        void actualizade293();

        void actualizade293E();

        void actualizade294();

        void actualizade295();

        void actualizade296();

        void actualizade297();

        void actualizade2973();

        void actualizade2974();

        void actualizade2975();

        void actualizade2976();

        void actualizade2977();

        void actualizade2978();

        void actualizade2979();

        void actualizade298();

        void actualizade2981();

        void actualizade2982();

        void actualizade2983();

        void actualizade2984();

        void actualizade2985();

        void actualizade3000();

        void actualizade3001();

        void actualizade3002();

        void actualizade3020();

        void actualizade3021();

        void actualizade3023();

        void actualizade3030();

        void actualizade3033();

        void actualizade3034();

        void actualizade3035();

        void actualizade3036();

        void actualizade3100();

        void actualizade3110();

        void actualizade3111();

        void actualizade3112();

        void actualizade3113();

        void actualizade3114();

        void actualizade3115();

        void actualizade3116();

        void actualizade3117();

        void actualizade3200();

        void actualizade3201();

        void actualizade3202();

        void actualizade3203();

        void actualizade3210();

        void actualizade3211();

        void actualizade3212();

        void actualizade3213();

        void actualizade3215();

        void actualizade3216();

        void actualizade3217();

        void actualizade3218();

        void actualizade3219();

        void actualizade3220();

        void actualizade3221();

        void actualizade3222();

        void actualizade3223();

        void actualizade3224();

        void introclave_op_retenciones();

        void intro_provincias();

        void introclaveexpedidas();

        void introclaverecibidas();

        void intropaises();

        void introclaveidfiscal();

        void intro_conceptos_evpn();

        void intro_cols_evpn_pymes();

        // Devuelve la lista de los drivers
        QStringList drivers();

        int anchocuentas(QString db);

        int anchocuentas();

        bool cod_longitud_variable(QString db);

        bool cod_longitud_variable();

        bool codsubdivis2db (QString qcodigo,QString db);

        QSqlQuery select_codigo_descrip_no_aux_2db (QString db);

        QSqlQuery select_codigo_descrip_aux_2db (QString db);

        bool posee_cuenta_aux_2db (QString cta,QString db);

        // se utiliza para insertar información en la contabilidad consolidada
        void insert_diario_consolidacion (QString cadnumasiento, qlonglong pase, QDate fecha,
          QString cuenta, double debe, double haber, QString concepto, QString documento,
          QString diario, QString usuario, QString ejercicio);


        qlonglong selectMaxpase_origen_entrefechas (QString cx, QDate inicio, QDate fin);

        bool ejercicio_vacio(QString ejercicio, QString cx);

        void insert_diario_consolidacion_conex (QString cadnumasiento, qlonglong pase,
                                            QDate fecha, QString cuenta, double debe,
                                            double haber, QString concepto,
                                            QString documento,
                                            QString diario, QString usuario,
                                            QString ejercicio, qlonglong pase_origen);


        // abre segunda base de datos (no default)
        bool abre2db(QString nombredb,QString nombreusuario,QString clave,
                    QString host, QString controlador, QString puerto, QString base);

        // información del diario de la base de datos no default
        QSqlQuery consulta_diario_db(QDate fechainicial, QDate fechafinal, QString base);

        // información del diario de la base de datos no default a partir de número de apunte
        QSqlQuery consulta_diario_db_ap_apunte(QDate fechainicial,
                                               QDate fechafinal,
                                               QString apunte,
                                               QString base);

        // información número apuntes en diario de la base de datos no default
        // a partir de número de apunte
        int consulta_diario_num_apuntes(QDate fechainicial,
                                        QDate fechafinal,
                                        QString apunte,
                                        QString base);

        // cierre base de datos no default
        void cierra_no_default(QString base);

        // devuelve el saldo hasta la fecha
        double saldo_hasta_fecha (QString cuenta,QDate fecha);

        void insertDiarioRegularizacion (QString cadnumasiento, QString cadnumpase,
               QString cadinicioej1, QString cuenta, QString debe, QString haber,
               QString usuario, QString ejercicio);

        void insertDiarioRegularizacion_ci (QString cadnumasiento, QString cadnumpase,
                                                    QString cadinicioej1, QString cuenta,
                                                    QString debe, QString haber, QString usuario,
                                                    QString ci,
                                                    QString ejercicio);

        // Devuelve lista de códigos de plancontable con filtro
        QSqlQuery selectCodigoplancontablelengthauxiliarfiltro (QString cadanchoctas,QString filtro);

        QSqlQuery selec_auxiliares_codigo(QString codigo);

        // para cierre, regularización con analítica (sin tabla aparte)
        QSqlQuery selectDiario_campoci_filtro (QString filtro);

        QSqlQuery select_cuenta_sumadebe_sumahaber_diario (QString filtro);

        QSqlQuery select_desglose_ci_tabla_positivos (QString inicioej, QString finej,
                                            QString cuenta);

        QSqlQuery select_desglose_ci_tabla_negativos (QString inicioej, QString finej,
                                                       QString cuenta);

        void haz_venci_procesado(QString numvenci, QString pase_diario_vencimiento);

        // comprueba si el asiento posee pases vinculados con la tabla de vencimientos
        bool asiento_con_pases_vencimientos(QString elasiento, QString ejercicio);

        bool asiento_con_pases_vencimientos_realizados(QString elasiento, QString ejercicio);

        // comprueba si el asiento posee pases vinculados con la tabla de facturas
        bool asiento_con_pases_factura(QString elasiento, QString ejercicio);

        bool asiento_con_enlace(QString elasiento, QString ejercicio);

                // devuelve el importe del estado con nodo y ejercicio suministrado
        QString cadvalorestado(QString estado,QString nodo,QString ejercicio, bool parte1);

        bool existecodigoregularizacion (QString cadena,QString *qdescrip);

        // Actualiza la descripción del código de regularización
        void guardacambiosregulariz (QString codigo,QString descripcion);

        // Inserta datos en la tabla cab_regulariz
        void insertcab_regulariz (QString codigo,QString descripcion);

        // Borra un código de regularización
        void eliminaregulariz (QString qcodigo);

        // borra secuencia de regularización
        void borra_regul_sec(QString codigo);

        // inserta registro de secuencia de regularización
        void inserta_reg_sec(QString codigo, int orden, QString descripcion, QString cuenta_deudora,
                             QString cuenta_acreedora, QString cuentas, QString condicion,
                             QString diario);

        QSqlQuery carga_regul_sec(QString codigo);

        QSqlQuery carga_cols_evpn_pymes();
        void borra_cols_evpn_pymes();
        void inserta_reg_cols_evpn_pymes(QString codigo, QString descripcion,
                                         QString cuentas);

        double calculasaldocuentas(QString cadena, QDate inicial, QDate final);

        // inserta apunte en diario para regularización parcial
        void insertDiarioRegulParc (QString cadnumasiento, QString cadnumpase, QString cadfecha,
         QString cuenta, QString debe, QString haber, QString concepto, QString ejercicio,
         QString diario);

        // crea tablas copia de libroiva y vencimientos y borra las originales
        void proctablas_libroiva_venci();

        // cambian números de pase en libroiva2 y vencimentos2
        void updateLibroivapasepase2 (QString antiguo, QString nuevo);
        void updateVencimientospase_diario_operacionpase_diario_operacion2(QString antiguo, QString nuevo);
        void updateVencimientospase_diario_vencimientopase_diario_vencimiento2 (QString antiguo, QString nuevo);

        // copia el contenido de libroiva2 y vencimientos2 a sus tablas originales y elimina las réplicas
        void rettablas_libroiva_venci();
        QString ejerciciodeasientoamort(QString asiento);
        bool esasientodeamort(QString asiento, QString ejercicio);
        void renum_amortiz (QString asientoA, QString asientoN, QString ejercicio);
        bool essqlite();

        bool existecodigotipovenci (QString cadena,QString *qdescrip);
        bool existecodigotipo_automatico (QString cadena,QString *qdescrip);

        void guardacambiostipovenci (QString codigo,QString descripcion);
        void guardacambiostipo_automatico (QString codigo,QString descripcion);
        void inserttipovenci (QString codigo,QString descripcion);
        void inserttipo_automatico (QString codigo,QString descripcion);
        void eliminatipovenci (QString qcodigo);
        void eliminatipo_automatico (QString qcodigo);
        QSqlQuery infocopiardepase (QString pase);
        QStringList listatiposdescripvenci();
        QStringList listaclaveidfiscal();
        QStringList listapaises();
        QStringList listaoperaciones_recibidas();
        QStringList listaoperaciones_expedidas();
        QStringList listaoperaciones_ret();
        QStringList listacodprovincias();
        QStringList lista_concepto_evpn();
        bool hayconceptoasociado(QString cuenta, QString &concepto);
        bool hayconceptoasociado_cta(QString cuenta, QString &concepto);
        bool hayivaasociado(QString cuenta, QString &cuentaiva, QString &tipoiva, QString &cta_base_iva);
        bool hayivaasociado_cta(QString cuenta, QString &cuentaiva, QString &tipoiva, QString &cta_base_iva);

        bool hayivaasociado_cta_iva(QString cuenta, QString &tipoiva);
        bool hayretasociada(QString cuenta, QString *cuentaret, QString *ret, bool *arrend,
                                       QString *tipo_oper_ret);
        // retención asociada a cuenta principal (junto con IVA)
        bool hayretasociada_cta(QString cuenta, QString *cuentaret, QString *ret, bool *arrend,
                                       QString *tipo_oper_ret);

        bool hay_ret_asociada_cta_ret(QString cuenta, double *tipo_ret,
                                      QString *tipo_operacion_ret,
                                      bool *ret_arrendamiento);
        bool hayvenciasociado(QString cuenta);
        void datos_cuenta_banco(QString cuenta, QString *ccc, QString *iban,
                                QString *cedente1, QString *cedente2, QString *nriesgo,
                                QString *sufijo);
        QString bic_cuenta_banco(QString cuenta);
        QString iban_cuenta_banco(QString cuenta);

        bool cargadatospase(QString pase, QString &cuenta,QDate &fecha,
                            QString &importedebe,
                            QString &importehaber, QString &documento);
        bool pase_en_vencimientos(QString pase);
        QString cadvenci_subcuenta (QString cuenta);

        QString cadvenci_externo (QString codigo);


        int dia_fijo_vto_subcuenta (QString cuenta);

        int dia_fijo_vto_externo (QString codigo);

        bool cuenta_dom_bancaria (QString cuenta);
        bool externo_dom_bancaria (QString codigo);

        QString cta_tesoreria_venci (QString cuenta);
        QString cta_tesoreria_venci_externo (QString codigo);

        QString asientodepase(QString pase);
        void borra_bloqueo_cta(QString cuenta);
        bool existe_bloqueo_cta_gen(QString cuenta);
        bool existe_bloqueo_cta(QString cuenta, QDate fecha);
        bool existe_venci_cta(QString cuenta);
        bool existe_iva_ret_cta(QString cuenta);
        void borra_venci_cta(QString cuenta);
        void borra_iva_ret_cta(QString cuenta);
        QString tipo_venci_asignado_cta(QString cuenta);
        bool hay_venci_cta_asociado_ver(QString cuenta);
        bool hay_venci_cta_asociado(QString cuenta, QString *cadena_venci,
                                    QString *cuenta_tesoreria, bool *procesavenci,
                                    int *dia_vto_fijo);
        QString descrip_tiposvenci(QString codigo);

        QSqlQuery datos_venci_asignado_cta(QString cuenta);
        QSqlQuery datos_iva_ret_asignado_cta(QString cuenta);
        void actu_venci_asignado_cta(QString cuenta, QString tipo,
                                     QString cta_tesoreria, bool procesavenci,
                                     int vto_dia_fijo);

        void actu_iva_ret_asignado_cta(QString cuenta, bool ivaasoc,
                                                  QString cuentaiva, QString tipoiva,
                                                  QString conceptodiario,
                                                  QString cuenta_ret_asig,
                                                  bool es_ret_asig_arrend,
                                                  QString tipo_ret_asig,
                                                  QString tipo_oper_ret_asig, QString cta_base_iva);


        void insert_bloqueo_cta (QString cuenta);
        void update_venci_cta (QString cuenta, QString venci);
        void insert_venci_cta (QString cuenta, QString venci);
        void insert_iva_ret_cta (QString cuenta);
        void bloqueos_mes_cta(QString cuenta, bool *enero, bool *febrero, bool *marzo,
                                         bool *abril, bool *mayo, bool *junio, bool *julio,
                                         bool *agosto, bool *septiembre, bool *octubre,
                                         bool *noviembre, bool *diciembre);
        void ed_bloqueos_mes_cta(QString cuenta, bool enero, bool febrero, bool marzo,
                                         bool abril, bool mayo, bool junio, bool julio,
                                         bool agosto, bool septiembre, bool octubre,
                                         bool noviembre, bool diciembre);
        bool fecha_periodo_bloqueado(QDate fecha);
        bool asiento_en_periodo_bloqueado(QString elasiento, QString ejercicio);
        bool asiento_pp_en_periodo_bloqueado(QString elasiento,QString ejercicio);
        bool asiento_con_cuentas_bloqueadas(QString elasiento, QString ejercicio);
        bool asiento_pp_con_cuentas_bloqueadas(QString elasiento,QString ejercicio);
        bool int_asientos_per_bloqueado (QString asientoinicial,QString asientofinal,
                                         QString ejercicio);
        bool int_asientos_con_enlace (QString asientoinicial,QString asientofinal,
                                                    QString ejercicio);
        bool int_asientos_cuentas_bloqueadas (QString asientoinicial,QString asientofinal,
                                              QString ejercicio);
        bool periodo_bloqueado_entre_fechas(QDate inicial, QDate final);
        bool hayperiodosbloqueados();
        QString documento_pase(QString pase);
        void updateDiariocipase (QString codigo, QString apunte);
        void agregados340(QDate inicial, QDate final, int *nregistros,
                          double *sumabi, double *sumacuota, double *itotal);
        QSqlQuery datoslibroiva(QDate inicial, QDate final);

        void updateDiarioCI (QString por, QString origen, bool fechas, QDate inicial, QDate final);

        qlonglong select_prox_doc ();

        qlonglong select_prox_domiciliacion ();

        // Actualiza el prox_documento de configuracion
        void updateConfiguracionprox_documento (QString cadnumpase);
        void updateConfiguracionprox_domiciliacion (QString cadnum);
        void updatecopia_docdiario (QString cadnumasiento, QString copia_doc, QString ejercicio);

        // Devuelve campo copia_doc en diario
        QString copia_docdiario (QString cadnumasiento, QString ejercicio);

        // Borra campo copia_doc en diario
        void borra_docdiario (QString cadnumasiento, QString ejercicio);

        // Devuelve consulta de código y nombre de usuarios
        QSqlQuery select_codigo_nombre_usuarios();

        bool existecodigousuario(QString contenido);

        void adcodigousuario(QString contenido);

        void borrausuario(QString contenido);

        void grabarusuario(QString codigo, QString nombre, QString clave, QString nif,
                              QString domicilio, QString poblacion, QString codigopostal,
                              QString provincia, QString pais, QString tfno, QString email,
                              QString observaciones, QString privilegios);
        void grabaimagenusuario(QString codigo, QString fototexto);
        void grabaimagenlogo(QString codigo, QString fototexto);

        void graba_config_imagenlogo(QString fototexto);

        void cambia_clave_usuario(QString codigo,QString clave);

        QSqlQuery recuperarusuario(QString codigo);

        void actu_privilegios_admin(QString cad);

        bool gestiondeusuarios();

        bool claveusuariook(QString usuario, QString clave);

        QString privilegios(QString usuario);

        QString imagenusuario(QString codigo);

        QSqlQuery select_usuarios(QString db);

        double saldocuentaendiarioejercicio (QString cuenta,QString ejercicio,QString diario);

        QSqlQuery saldo_auxs_endiarioejercicio (QString ejercicio,QString diario,
                                                bool intervalo, QString inicial, QString final);

        QSqlQuery saldo_auxs_hasta_fecha (QDate fecha,
                                          bool intervalo,
                                          QString inicial, QString final);

        QSqlQuery sumas_aux_dh ( QDate desde,
                                            QDate hasta,
                                            QString filtro,
                                            bool intervalo,
                                            QString inicial, QString final,
                                            bool analitica,
                                            QString ci);

        QSqlQuery sumas_aux_dh_analitica_tabla (QDate fechaini,
                                                QDate fechafin,
                                                QString ci,
                                                QString filtro,
                                                bool intervalo,
                                                QString inicial, QString final);

        // idem que anterior solo que para cuentas no auxiliares
        double saldocuenta_princi_endiarioejercicio (QString cuenta,QString ejercicio,
                                                                QString diario);


        bool hayprocvenci(QString cuenta);

        bool hayprocvenci_externo(QString codigo);

        bool asiento_es_vencimiento_procesado(QString elasiento, QString ejercicio,
                                              QString *qpase);

        QString numvto_paseprocesado(QString qpase);

        QSqlQuery baseemitidasexentasaib_corrientes (QDate inicial, QDate final);

        QSqlQuery baseemitidasexentasaib_inversion (QDate inicial, QDate final);

        QSqlQuery baseautofacturasrecibidasexentas (QDate inicial, QDate final);

        QSqlQuery baseautofacturasnouerecibidasexentas (QDate inicial, QDate final);

        QSqlQuery base_isp_interiores_recibidasexentas (QDate inicial, QDate final);

        bool aperturaendiarioenfechas (QString cadinicioej, QString cadfinej);

        QString clavegastos();
        QString claveingresos();
        QSqlQuery listacuentasauxiliares(QString filtro);

        bool existe_input_cuenta (QString ci, QString cta);
        void updateCi_input_cta (QString asignacion, QString cuenta, QString ci);
        void insertCi_input_cta (QString cuenta, QString ci, QString asignacion);
        void clona_input(QString origen, QString destino);

        QSqlQuery selectSumasignacion100input_cta (QString cuenta);
        bool analitica_tabla();
        bool gestor_doc_bd();
        QSqlQuery ci_input(QString cuenta);
        qlonglong prox_num_ci();
        void incrementa_prox_num_ci();
        void cambia_clave_ci_apunte_diario(QString pase, QString clave_ci);
        void inserta_diario_ci(QString cadproxci, QString ci, QString importe);
        void deletediarioci_clave (QString clave);
        QSqlQuery carga_de_tabla_ci(QString qclave_ci);
        QSqlQuery cons_diario_ci (QDate inicial, QDate final, QString filtro);
        qlonglong count_regs_diario_ci (QDate inicial, QDate final, QString filtro);
        QSqlQuery cons_diario_ci_tex (QDate inicial, QDate final, QString filtro);
        QSqlQuery lista_inputs_cuenta(QString cuenta, QDate fecha_ini,
                                     QDate fecha_fin, bool esdebe);
        QSqlQuery debehaber_ci (QString codigo, QDate fechaini,
                                QDate fechafin, QString subcadena, QString ci);
        bool ci_input_ok (QString cuenta);
        QSqlQuery pase_bi_libroiva();
        double anotacion_pase(QString pase);
        QSqlQuery debe_haber_pase(QString pase);
        QSqlQuery ctaord_pase_vtos();

        // descuadres en diario (asientos)
        QSqlQuery descuadres_diario();

        QString cuenta_pase(QString pase);
        QString notas();
        void grabarnotas(QString notas);
        QSqlQuery pase_libroiva();
        QDate fecha_pase_diario(QString pase);
        void borra_reg_libroiva(QString pase);

        // Devuelve número y pase en diario de vencimientos
        QSqlQuery num_pase_vencimientos();

        // Devuelve números de pase correspondientes a la tabla diario
        QSqlQuery num_pase_retenciones();

        // devuelve clave y fechas de facturas
        QSqlQuery clave_fechafac_facturas();

        // borra líneas de facturas con clave determinada
        QSqlQuery borra_lin_fact_clave(QString clave);

        // borra cabecera de factura
        QSqlQuery borra_facturas_clave(QString clave);

        // Borra registro de vencimientos
        void borra_reg_vencimientos(QString num);

        // Borra registro de retenciones
        void borra_reg_retenciones(QString pase);

        // Borra ejercicio de presupuesto
        void borra_ej_presupuesto(QString ejercicio);

        // Borra ejercicio ajustes conciliación
        void borra_ej_ajustesconci(QDate inicial, QDate final);

        // Borra ejercicio ajustes cuenta_ext_conci
        void borra_ej_cuenta_ext_conci(QDate inicial, QDate final);

        // devuelve el campo clave_ci de la tabla diario_ci
        QSqlQuery clave_ci_diario_ci();

        // devuelve la fecha correspondiente al registro del diario con clave_ci
        QDate fecha_clave_ci_diario(QString clave_ci);

        // borra registros en tabla diario_ci
        void borra_clave_ci_diario_ci(QString clave_ci);

        // borra registros del diario entre fechas
        void borra_diario_entrefechas(QDate inicial, QDate final);

        void delete_input_cta (QString cuenta, QString ci);

        bool existecodigoserie (QString cadena,QString *qdescrip);

        void guardacambiosserie (QString codigo,QString descripcion);

        void insertserie (QString codigo,QString descripcion);

        void eliminaserie (QString qcodigo);

        bool existecodigoserienum (QString cadena,QString *qdescrip, qlonglong *num);

        void actualizaserie(QString codigo, QString descrip, QString proxdoc);

        bool existecodigo_fact_predef (QString cadena,QString *qdescrip);

        bool existecodigo_lote (QString cadena,QString *qdescrip);

        void insertfacturas_predef (QString codigo,QString descripcion);

        // Inserta datos en la tabla lote_fact
        void insertfacturas_lote (QString codigo,QString descripcion);

        bool existecodigotipos_doc (QString cadena,QString *qdescrip);

        QSqlQuery datos_cabecera_fact_predef(QString codigo);

        void guardacambiostipos_doc (QString codigo,QString descripcion);

        void inserttipos_doc (QString codigo,QString descripcion);

        void eliminatipo_doc (QString qcodigo);

        void eliminafra_predefinida (QString qcodigo);

        void eliminalote_fact (QString qcodigo);

        void guardacambios_facturas_predef (QString codigo,QString descripcion);

        void guardacambios_lote (QString codigo,QString descripcion);

        void elimina_factura_predef (QString qcodigo);

        QStringList listacodseries();

        QString imagendoc(QString codigo);

        QString logo_empresa();

        void transac_fich_import();

        void fintransac_fich_import();

        void carga_tipo_doc(QString codigo,
                                       QString *descrip,
                                       QString *serie,
                                       QString *pie1,
                                       QString *pie2,
                                       QString *moneda,
                                       QString *codigo_moneda,
                                       bool *contabilizable,
                                       bool *rectificativo,
                                       int *tipo_operacion,
                                       QString *documento,
                                       QString *cantidad,
                                       QString *referencia,
                                       QString *descripref,
                                       QString *precio,
                                       QString *totallin,
                                       QString *bi,
                                       QString *tipoiva,
                                       QString *tipore,
                                       QString *cuota,
                                       QString *cuotare,
                                       QString *totalfac,
                                       QString *notas,
                                       QString *venci,
                                       QString *notastex,
                                       QString *cif_empresa,
                                       QString *cif_cliente,
                                       QString *numero,
                                       QString *fecha,
                                       QString *cliente,
                                       QString *descuento,
                                       QString *totallineas, QString *suplidos,
                                       QString *lineas_doc,
                                       QString *nombre_emisor,
                                       QString *domicilio_emisor,
                                       QString *cp_emisor,
                                       QString *poblacion_emisor,
                                       QString *provincia_emisor,
                                       QString *pais_emisor,
                                       QString *cif_emisor,
                                       QString *id_registral, QString *fichreport, QString *concepto_sii,
                                       QString *fe_idioma, QString *fe_libro, QString *fe_registro,
                                       QString *fe_hoja, QString *fe_folio, QString *fe_seccion, QString *fe_volumen,
                                       QString *fe_datos_adic, QString *retencion, QString *clave_donacion, QString *comunidad_autonoma,
                                       bool *donacion_especie, bool *donacion_2ejer, double *porcent_deduc_autonomia,
                                       QString *imagen);

        bool es_euro_tipo_doc(QString codigo);

        void msjs_tipo_doc(QString codigo,
                                       QString *documento,
                                       QString *cantidad,
                                       QString *referencia,
                                       QString *descripref,
                                       QString *precio,
                                       QString *totallin,
                                       QString *bi,
                                       QString *tipoiva,
                                       QString *tipore,
                                       QString *cuota,
                                       QString *cuotare,
                                       QString *totallineas,
                                       QString *totalfac,
                                       QString *notas,
                                       QString *venci,
                                       QString *cif_empresa,
                                       QString *cif_cliente,
                                       QString *numero,
                                       QString *fecha,
                                       QString *cliente,
                                       QString *descuento, QString *suplidos, QString *retencion);

        void datos_facturae_tipo_doc(QString codigo, QString *fe_idioma,
                                                QString *fe_libro, QString *fe_registro,
                                                QString *fe_hoja, QString *fe_folio,
                                                QString *fe_seccion, QString *fe_volumen, QString *fe_datos_adic);

        void datos_empresa_tipo_doc(QString codigo,
                                    QString *nombre_emisor,
                                    QString *domicilio_emisor,
                                    QString *cp_emisor,
                                    QString *poblacion_emisor,
                                    QString *provincia_emisor,
                                    QString *pais_emisor,
                                    QString *cif_emisor,
                                    QString *id_registral
                                        );


        QString descrip_doc(QString codigo);

        int lineas_doc(QString codigo);

        void guarda_tipo_doc(QString codigo,
                                       QString descrip,
                                       QString serie,
                                       QString pie1,
                                       QString pie2,
                                       QString moneda,
                                       QString codigo_moneda,
                                       bool contabilizable,
                                       bool rectificativo,
                                       QString tipo_operacion,
                                       QString documento,
                                       QString cantidad,
                                       QString referencia,
                                       QString descripref,
                                       QString precio,
                                       QString totallin,
                                       QString bi,
                                       QString tipoiva,
                                       QString tipore,
                                       QString cuota,
                                       QString cuotare,
                                       QString totalfac, QString suplidos,
                                       QString notas,
                                       QString venci,
                                       QString notastex,
                                       QString cif_empresa,
                                       QString cif_cliente,
                                       QString numero,
                                       QString fecha,
                                       QString cliente,
                                       QString descuento,
                                       QString totallineas,
                                       QString lineas_doc,  // tipo int
                                       QString nombre_emisor,
                                       QString domicilio_emisor,
                                       QString cp_emisor,
                                       QString poblacion_emisor,
                                       QString provincia_emisor,
                                       QString pais_emisor,
                                       QString cif_emisor,
                                       QString id_registral,
                                       QString fichjasper, QString concepto_sii, QString fe_idioma,
                                       QString fe_libro, QString fe_registro, QString fe_hoja, QString fe_folio,
                                       QString fe_seccion, QString fe_volumen, QString fe_datos_adic,
                                       QString retencion, QString clave_donacion="", QString comunidad_autonoma="",
                                       bool donacion_especie=false, bool donacion_2ejer=false, QString porcent_deduc_autonomia="0");

        int tipo_operacion_tipo_doc(QString codigo);


        int nuevacabecerafactura(QString serie,
                                            QString numero,
                                            QString cuenta, QDate fecha_asiento,
                                            QDate fecha_fac,
                                            QDate fecha_op,
                                            bool contabilizado,
                                            bool contabilizable,
                                            bool con_ret,
                                            bool re,
                                            QString tipo_ret,
                                            QString retencion,
                                            QString tipo_doc,
                                            QString notas,
                                            QString pie1,
                                            QString pie2,
                                            QString pase_diario_cta,
                                            QString cta_anticipos, QString externo="", QString concepto_sii="",
                                            QString c_a_rol1="", QString c_a_rol2="", QString c_a_rol3="");

        int modificacabecerafactura(QString serie,
                                            QString numero,
                                            QString cuenta, QDate fecha_asiento,
                                            QDate fecha_fac,
                                            QDate fecha_op,
                                            bool contabilizado,
                                            bool contabilizable,
                                            bool con_ret,
                                            bool re,
                                            QString tipo_ret,
                                            QString retencion,
                                            QString tipo_doc,
                                            QString notas,
                                            QString pie1,
                                            QString pie2,
                                            QString pase_diario_cta,
                                            QString cta_anticipos, QString externo="", QString concepto_sii="",
                                            QString c_a_rol1="", QString c_a_rol2="", QString c_a_rol3="");


        void modificacabecerafactura_predef(QString codigo,
                bool contabilizable,
                bool con_ret,
                bool re,
                QString tipo_ret,
                QString tipo_doc,
                QString notas,
                QString pie1,
                QString pie2
                , QString concepto_sii);

        QSqlQuery select_cab_facturas (QString filtro);

        QSqlQuery select_cabecera_doc (QString serie, QString numero);

        QString fichreport(QString tipo_doc);

        bool existe_fact(QString serie,
                                    QString numero);

        bool existe_fact_predef(QString codigo);

        bool doc_cerrado(QString serie,
                         QString numero);

        bool doc_contabilizado(QString serie,
                                      QString numero);

        int tipo_operacion_doc(QString codigo);

        QString info_donacion_doc(QString codigo);

        bool tipo_doc_rectificativo(QString codigo);

        bool tipo_doc_contabilizable(QString codigo);

        void update_contabilizado_doc (QString serie, QString numero, QString pasediario);

        bool pase_en_facturas(QString pase, QString *serie, QString *numero);

        bool doc_contabilizable(QString serie,
                                QString numero);

        void cierra_doc(QString serie,
                        QString numero);


        QString doc_cliente(QString serie,
                            QString numero);

        QString cuenta_ref(QString ref);

        void bloquea_para_facturas();

        void borralineas_doc (int clave);

        void borralineas_fac_predef (QString codigo);

        void borra_cabecera_doc (QString serie,
                                 QString numero);

        void insert_lin_doc(int clave, QString codigo, QString descrip,
                            QString cantidad, QString precio, QString clave_iva,
                            QString tipo_iva, QString tipo_re, QString dto, QString ci, QString notas,
                            QString pedido="", QString albaran="", QString expediente="", QString contrato="");

        void insert_lin_doc_predef(QString cod_fact, QString codigo, QString descrip,
                                       QString cantidad, QString precio, QString clave_iva,
                                       QString tipo_iva, QString tipo_re, QString dto, QString ci, QString notas);

        QSqlQuery select_lin_doc (int clave);

        QSqlQuery select_lin_doc_predef (QString cod_predef);

        QSqlQuery select_iva_lin_doc (int clave);

        int clave_doc_fac(QString serie,
                          QString numero);

        qlonglong proxnum_serie(QString serie);

        bool existecodigoref(QString cadena,QString *qdescrip);

        void guardacambiosref (QString codigo,QString descripcion);

        void insertref (QString codigo,QString descripcion);

        void eliminaref (QString qcodigo);

        bool ref_precision(QString codigo);

        void carga_ref(QString codigo,
                       QString *descrip,
                       QString *tipoiva,
                       QString *tipoivare,
                       QString *cuenta,
                       double *pvp, bool *suplido, bool *anticipo, bool *dobleprecision);

        void guarda_ref(QString codigo,
                        QString descrip,
                        QString tipoiva,
                        QString tipoivare,
                        QString cuenta,
                        QString pvp, bool suplido, bool anticipo, bool precision);

        QStringList lista_ref_suplidos();

        int cuentatipoiva (QString cadena, QString *cuenta, bool soportado=false);
        void updatecuentatipoiva (QString clave,QString cuenta);
        void updatecuentatipoiva_soportado (QString clave,QString cuenta);
        QStringList listacoddocs();
        QStringList lista_cod_predef();
        void carga_irpf(QString *cuenta,
                       double *tipo);
        void actuconfig_irpf (QString cuenta,QString valor);
        void insertadatosaccesorios_cplus(QString cuenta, QString razon, QString nif,
                                          QString domicilio, QString poblacion,
                                          QString provincia, QString codpostal,
                                          QString telef, QString fax, QString email);

        void guardadatosaccesorios2 (QString cuenta, QString razon, QString ncomercial,
                                                QString cif,
            QString nifrprlegal, QString domicilio, QString poblacion,
            QString codigopostal, QString provincia, QString pais, QString pais_dat,
            QString tfno, QString fax, QString email, QString observaciones,
            QString ccc, QString cuota,
            bool venciasoc, QString tipovenci, QString cuentatesor,
            QString web, QString codclaveidfiscal, bool procvto, int diasfijo,
            bool dom_bancaria, QString IBAN, QString BIC, QString formapago,
            QString fototexto);

        void datosnumeracion_ejercicio (QString ejercicio,
                                        QString *prox_asiento,
                                        bool *primero_apertura);

        void pasanumeracion_ejercicio (QString ejercicio,
                                       QString prox_asiento,
                                       bool primero_apertura);
        void pasa_prox_nrecepcion_ejercicio (QString ejercicio,
                                                  QString prox_nrecepcion);
        bool diario_vacio();

        QDate maxfecha_diario ();
        QSqlQuery libroiva_autofacturas_ue (QDate inicial, QDate final);
        QSqlQuery libroiva_autofacturas_ue_emitidas (QDate inicial, QDate final);
        QSqlQuery libroiva_autofacturas_no_ue (QDate inicial, QDate final);
        QSqlQuery libroiva_autofacturas_no_ue_emitidas (QDate inicial, QDate final);
        QSqlQuery libroiva_isp_interiores_recibidas (QDate inicial, QDate final);
        QSqlQuery libroiva_isp_interiores_emitidas (QDate inicial, QDate final);
        QStringList lista_cod_ejercicios();
        void actu_ejercicio_diario(QString ejercicio);
        void asigna_prox_asiento_ejercicio(QString ejercicio);
        QStringList tipos_automaticos();
        QString cuenta_tipo_iva(QString clave);
        QString cuenta_tipo_iva_sop(QString clave);
        // elimina contabilizado y el registro de apunte del diario para la tabla facturas
        void quitacontab_fra_de_pase (QString pase);
        QString descrip_pais (QString codigo);
        double tipo_ret_cta(QString cuenta);
        bool ret_arrendamiento(QString cuenta);
        QString tipo_operacion_ret(QString cuenta);
        void insert_retencion (QString pase,
                               QString cta_retenido, bool ret_arrendamiento,
                               QString clave, QString base_percepciones,
                               QString tipo_ret, QString retencion,
                               QString ing_cta, QString ing_cta_repercutido,
                               QString nombre_ret, QString cif_ret, QString provincia);
        void modifica_retencion (QString pase,
                               QString cta_retenido, bool ret_arrendamiento,
                               QString clave, QString base_percepciones,
                               QString tipo_ret, QString retencion,
                               QString ing_cta, QString ing_cta_repercutido,
                               QString nombre_ret, QString cif_ret, QString provincia);
        // devuelve consulta registro de un pase determinado en retenciones
        QSqlQuery datos_ret_pase (QString cadpase);
        // devuelve informe agrupado de info retenciones
        QSqlQuery retenciones_agrup (QDate fechaini, QDate fechafin,
                                                bool soloarrendamientos,
                                                bool excluyearrendamientos);
        QSqlQuery listaret (QDate fechaini, QDate fechafin,
                                                bool soloarrendamientos,
                                                bool excluyearrendamientos);
        QSqlQuery listaretagrup (QDate fechaini, QDate fechafin,
                                 bool soloarrendamientos,
                                 bool excluyearrendamientos);


        int primer_venci_pte_de_pase(QString apunte);

        void actualiza_prox_asiento_ejercicios();

        void actualiza_prox_pase();

        void insert_datossubcuenta_import(QString codigo, QString razon,
                                          QString nombrecomercial, QString cif,
                                          QString web, QString domicilio,
                                          QString poblacion, QString cp,
                                          QString provincia, QString telefono,
                                          QString pais, QString fax,
                                          QString email, QString observaciones,
                                          QString ccc, QString iban,
                                          QString bic, QString sufijo,
                                          QString domiciliable,
                                          QString ref_mandato,
                                          QDate firma_mandato, QString codpais, QString claveidfiscal);

        void update_datossubcuenta_import(QString codigo, QString razon,
                                          QString nombrecomercial, QString cif,
                                          QString web, QString domicilio,
                                          QString poblacion, QString cp,
                                          QString provincia, QString telefono,
                                          QString pais, QString fax,
                                          QString email, QString observaciones,
                                          QString ccc, QString iban,
                                          QString bic, QString sufijo,
                                          QString domiciliable,
                                          QString ref_mandato,
                                          QDate firma_mandato, QString codpais, QString claveidfiscal);

        void insert_libroiva_import(QString apunte_diario,
                QString cuenta_base_iva,
                QString base_imponible,
                QString clave_iva,
                QString tipo_iva,
                QString tipo_recargo,
                QString cuota,
                QString cuenta_factura,
                QDate fecha_factura,
                bool soportado,
                bool aib,
                bool ais,
                bool adq_serv_extranjero,
                bool prest_serv_ue,
                bool bien_inversion,
                bool eib,
                QDate fecha_operacion,
                bool exportacion,
                bool simplificada,
                QString nfacturas,
                QString finicial,
                QString ffinal,
                bool rectificativa,
                QString tipo_rectificativa,
                bool agrario, bool exento, bool importacion);


        void insertavenci_import(QString numvencimiento,
                                    QString cuenta_ordenante,
                                    QDate fecha_operacion,
                                    QString importe,
                                    QString cuenta_tesoreria,
                                    QDate fecha_vencimiento,
                                    bool derecho,
                                    bool pendiente,
                                    QString apunte_diario,
                                    QString apunte_vto,
                                    bool domiciliable
                                    );

        // Indica si existe un código de ejercicio
        bool existe_codigo_ejercicio (QString cadena);

        bool existen_datos_accesorios (QString cuenta);

        QSqlQuery selec_diario_vr(QString pase);

        QSqlQuery select_cods_evpn_pymes_diario (QString ejercicio,
                                                 QString cadenacuentas);
        QSqlQuery select_apertura_diario (QString ejercicio,
                                          QString cadenacuentas);
        void asignadiarios_segun_concepto();
        bool hayerror_consulta();
        void reset_error_consulta();


        QSqlQuery apuntes_borrados(QDate desdefecha, QString base);
        void borra_apunte_consolidada(QString apunte_origen, int codigo);
        void borra_datos_consolidada(int codigo);
        QSqlQuery select_consolidables();
        void borra_consolidables();
        void inserta_reg_consolidable(QString controlador, QString basedatos,
                                      QString usuario, QString clave, QString host,
                                      QString puerto, int codigo,
                                      QString descripcion, bool infodiario,
                                      QDate fecha_actu, QString hora_actu,
                                      long ultimo_apunte);


        void marca_apunte_revisado(qlonglong asiento, QString ejercicio, bool marcado);
        // devuelve consulta para filtrar por vencimientos en domiciliacion
        QSqlQuery select_venci_filtrados ( QString importe1, QString importe2,
                                           QString cuenta1, QString cuenta2, bool venci,
                                           QDate venci1, QDate venci2, bool oper,
                                           QDate op1, QDate op2,
                                           QString cta_banco, int orden, bool pagos=false);
        void transac_bloquea_config();
        void pasa_domiciliacion_vto(QString num, QString domiciliacion);

        void guardadomiciliacion(QString id_remesa, QString aux_banco, QString descrip,
                            QString ccc, QString iban, QString sufijo, QString cedente1,
                            QString cedente2, QString riesgo, QDate fecha_conf, QDate fecha_cargo,
                            bool emitido19, bool emitido58, bool emitidosepa1914, bool procesada, bool pagos=false);

        QSqlQuery carga_domiciliacion(QString id);

        void elimina_domiciliacion(QString id);

        int cuenta_regs_domiciliacion(QString id);

        int cuenta_regs_domiciliacion_fecha(QString id, QDate fecha);
        QList<QDate> lista_fechas_domiciliacion(QString id);
        QSqlQuery datos_domiciliacion_fecha(QString id, QDate fecha);

        QSqlQuery datos_domiciliacion(QString id);

        QSqlQuery datos_externo_dom(QString cod);

        QString config_codigo_ine_plaza();

        void marca_vto_impagado(QString num, bool impagado);

        bool cuenta_domiciliable(QString cuenta);

        void borra_tabla_apuntes_borrados();

        int registros_plan();

        bool hay_criterio_caja_aux(QString codigo);

        QSqlQuery registros_libro_facturas(QDate inicio, QDate final);

        bool mostrar_advert();

        void datos_advert(QString *mensaje, QString *nombre, QString *cargo,
                          QString *organizacion);

        void pasa_datos_advert(QString mensaje, QString nombre, QString cargo,
                               QString organizacion, bool acepta);

        void actualiza_cabecera_lote_fact(QString codigo, bool cuotas,
                                                     QString cod_factura_predef,
                                                     QString presupuesto);
        void borra_lins_lote_predef(QString codigo);
        void inserta_lin_lote_predef(QString codigo, QString cuenta,
                                     QString importe,
                                     QString cuota, QString externo);

        QSqlQuery cabecera_lote_fact(QString codigo);

        QSqlQuery lineas_lin_lote_predef(QString codigo);

        QSqlQuery select_facturas_predef(QString codigo);

        QSqlQuery select_lin_facturas_predef(QString codigo);

        void update_cuenta_gasto_vto_config(QString codigo);

        QString selec_cuenta_gasto_vto_config();

        QString select_cuenta_tesoreria_configuracion ();

        void update_cuenta_tesoreria_config(QString codigo);

        bool hay_reg_duplicado_diario(QString cuenta, QString documento);

        QString apuntes_no_libroiva(bool soportado);

        QString apuntes_descuadre_libroiva();

        QSqlQuery datos_carta347();

        void actu_config_carta347(QString cabecera347, QString cuerpo347, bool desglose347,
                                  QString cuerpo2_347, QString pie347, QString saludo347, QString firma347);

        QStringList listacuentas();

        QStringList listacuentas_datos(QStringList &razon,
                                       QStringList &nombrecomercial, QStringList &cif,
                                       QStringList &web, QStringList &domicilio,
                                       QStringList &poblacion, QStringList &cp,
                                       QStringList &provincia, QStringList &telefono,
                                       QStringList &pais, QStringList &fax,
                                       QStringList &email, QStringList &observaciones,
                                       QStringList &ccc, QStringList &iban,
                                       QStringList &bic, QStringList &sufijo,
                                       QStringList &domiciliable,
                                       QStringList &ref_mandato,
                                       QStringList &firma_mandato,
                                       QStringList &codpais,
                                       QStringList &claveidfiscal);

        QStringList lista_codigos_plan();

        QStringList listacuentas_aux();

        void reemplaza_aux_plan(QString origen, QString por);

        QSqlQuery datos_venci_fechas(QString cuenta_tesor, QDate fechaini, QDate fechafin, int margen);

        QSqlQuery datos_ext_conci_fechas(QString cuenta, QDate inicial, QDate final);

        QSqlQuery select_apuntes_analitica_diario (QString cuenta,QDate fechaini, QDate fechafin);
        int registros_select_apuntes_analitica_diario (QString cuenta,QDate fechaini, QDate fechafin);

        QSqlQuery cuentas_diario_grupos(QStringList grupos);

        QSqlQuery datos347_agrup_nif (QDate inicial, QDate final, QString cadenacuentas);

        double vol_op_347_nif (QString nif, QDate inicial, QDate final);

        QStringList auxiliares_entre_codigos(QString inicial, QString final);

        QSqlQuery pases_diario_operacion_vencimiento(QString cuenta, QDate fechaini, QDate fechafin);

        QSqlQuery pases_diario_vencimiento(QString cuenta, QDate fechaini, QDate fechafin);

        QSqlQuery vencimientos_info(QDate fechaini, QDate fechafin, QString cuentas);

        void guarda_cuentas_inv_sp(QString cta_iva_sop_AIB, QString cta_iva_rep_AIB,
                                   QString cta_iva_sop_AIS, QString cta_iva_rep_AIS,
                                   QString cta_iva_sop_AES, QString cta_iva_rep_AES,
                                   QString cta_iva_sop_OI,  QString cta_iva_rep_OI );
        void recupera_cuentas_inv_sp(QString *cta_iva_sop_AIB, QString *cta_iva_rep_AIB,
                                     QString *cta_iva_sop_AIS, QString *cta_iva_rep_AIS,
                                     QString *cta_iva_sop_AES, QString *cta_iva_rep_AES,
                                     QString *cta_iva_sop_OI,  QString *cta_iva_rep_OI);

        void inserta_formapago(QString fpago, bool mostrar_cuenta_propia, bool mostrar_cuenta_cliente);
        void borra_tabla_formas_pago();
        QSqlQuery carga_formas_pago();

        void cuenta_aux_forma_pago(QString cuenta, QString *forma_pago,
                                   bool *mostrar_cuenta_propia, bool *mostrar_cuenta_cliente);
        int selectCount_registros_papelera ();

        void actualiza_cta_anticipo(QString cuenta);

        QString cuenta_anticipo();

        bool ref_con_anticipo(QString codigo);

        QSqlQuery campos_libroiva(QString pase);

        QSqlQuery select_direc_SII();

        void actualiza_direc_SII(QString ruta_sii_emit_test, QString ruta_sii_rec_test,
                                 QString ruta_sii_emit_real, QString ruta_sii_rec_real, QString siilr, QString sii);

        void actualiza_asiento_externo(QString externo, QString asiento, QString ejercicio);

        void inserta_doc_gestor(qlonglong num, QString nombre_fich, QByteArray inByteArray);

        void actu_img_tipos_doc(QString documento, QByteArray barray);

        void elimina_img_tipos_doc(QString documento);

        QByteArray img_tipos_doc(QString codigo);

        QByteArray documento_bd(qlonglong num);

        bool existe_doc_gestor(qlonglong num);

        void borra_doc_gestor(qlonglong num);
        void actualiza_nombrefich_doc_gestor(qlonglong num, QString archivo);
        QSqlQuery asientos_documentos_ejercicio(QString ejercicio);

        QStringList tipos_proveedor();
        QStringList homologaciones();
        QStringList actividades();

        QSqlQuery asientos_cuenta_libroiva(QString cuenta );

        QString cod_homol_pruebas_config();

        QSqlQuery cuenta_max_fecha_diario(QString filtro);

        QSqlQuery select_sumDH_externos_cuenta_hasta_fecha (QString cuenta, QDate final);

        QSqlQuery ejecutar_publica(QString cadena);

    private:

        // Constructor de la clase
        basedatos();

        // ejecuta query (no cadena)
        QSqlQuery ejecuta (QSqlQuery query);

        QSqlQuery ejecutar(QString cadena);

        // ejecuta sentencia en la base de datos designada por db
        QSqlQuery ejecutar(QString cadena, QString db);

        // Comprueba la versión de las tablas
        bool comprobarVersion();

        // Añade el motor a utilizar según cuál sea el sistema gestor de base de datos
        QString anadirMotor(QString cadena, QString db);
        QString anadirMotor(QString cadena);

        // Crea las database para MYSQL y POSTGRES
        bool crearDatabase(QString controlador, QString nombre);

        // Crea las database para MYSQL y POSTGRES
        bool crearDatabase(QString controlador, QString nombre, QString db);

        QString mensajes_iniciales_347();

        // Puntero de la clase única
        static basedatos *base;
        // Base de datos sobre la que se trabaja
        // QSqlDatabase database;

        bool comadecimal,decimales;
};

#endif
