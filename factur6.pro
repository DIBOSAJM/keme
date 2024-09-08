greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RESOURCES = keme.qrc

QT += sql
QT += xml
QT += charts
QT += network

CONFIG += c++17

CODECFORTR = UTF-8
CONFIG += qt
RC_FILE = factur.rc
CONFIG += release

ICON = factur.icns


include(./QtRptProject/QtRPT/QtRPT.pri)
include(./QtRptProject/3rdparty/QtXlsx/qtxlsx.pri)
include(./QtRptProject/3rdparty/zint-2.4.4/backend_qt4/Zint.pri)



HEADERS += factur/main_factur.h \
           factur/listaseries.h \
           funciones.h \
           basedatos.h \
           aritmetica.h \
           factur/inicio.h \
           factur/inicial.h \
           creaempresa.h \
           conexion.h \
    factur/serie.h \
    factur/listadocs.h \
    factur/edita_doc.h \
    factur/listarefs.h \
    factur/edita_ref.h \
    tiposivacta.h \
    buscasubcuenta.h \
    buscasubcuenta_input.h \
    asignainputcta.h \
    factur/facturas.h \
    factur/factura.h \
    ret_irpf.h \
    ajuste_imagen_logo.h \
    subcuentas.h \
    ajuste_imagen.h \
    arbol.h \
    datos_accesorios.h \
    factur/traspasos.h \
    editarasiento.h \
    tabla_asientos.h \
    procesavencimiento.h \
    login.h \
    aux_express.h \
    factur/buscaref.h \
    asignainputdiario.h \
    buscaconcepto.h \
    conceptos.h \
    ivasoportado.h \
    ivarepercutido.h \
    aib.h \
    eib.h \
    introci.h \
    consmayor.h \
    ivadatosadic.h \
    ivasop_aut.h \
    ivarep_aut.h \
    exento.h \
    consultavencipase.h \
    pidenombre.h \
    retencion.h \
    cuadimprimayor.h \
    introfecha.h \
    nuevo_vencimiento.h \
    factur/lista_fact_predef.h \
    factur/lotes_fact.h \
    factur/lotes.h \
    acumuladosmes.h \
    asigna_fichdoc.h \
    pidecuenta.h \
    factur/ed_cta_lote.h \
    vies_msj.h \
    graf_recorte.h \
    busca_externo.h \
    externos.h \
    notas.h \
    pide_fich.h \
    concilia.h \
    editafechaasiento.h \
    editapaseconci.h \
    listavenci.h \
    pidefecha.h \
    contrapartidas.h \
    etiqueta_fich.h \
    pide_info_unif.h \
    edit_conc_doc.h \
    cambiacuentapase.h \
    network_connections.h \
    mensaje_url.h

SOURCES += factur/amain.cpp \
    factur/main_factur.cpp \
    factur/listaseries.cpp \
    funciones.cpp \
    basedatos.cpp \
    aritmetica.cpp \
    factur/inicio.cpp \
    factur/inicial.cpp \
    creaempresa.cpp \
    conexion.cpp \
    factur/serie.cpp \
    factur/listadocs.cpp \
    factur/edita_doc.cpp \
    factur/listarefs.cpp \
    factur/edita_ref.cpp \
    tiposivacta.cpp \
    buscasubcuenta.cpp \
    buscasubcuenta_input.cpp \
    asignainputcta.cpp \
    factur/facturas.cpp \
    factur/factura.cpp \
    ret_irpf.cpp \
    ajuste_imagen_logo.cpp \
    subcuentas.cpp \
    ajuste_imagen.cpp \
    arbol.cpp \
    datos_accesorios.cpp \
    factur/traspasos.cpp \
    editarasiento.cpp \
    tabla_asientos.cpp \
    procesavencimiento.cpp \
    login.cpp \
    aux_express.cpp \
    factur/buscaref.cpp \
    asignainputdiario.cpp \
    buscaconcepto.cpp \
    conceptos.cpp \
    ivasoportado.cpp \
    ivarepercutido.cpp \
    aib.cpp \
    eib.cpp \
    introci.cpp \
    consmayor.cpp \
    ivadatosadic.cpp \
    ivasop_aut.cpp \
    ivarep_aut.cpp \
    exento.cpp \
    consultavencipase.cpp \
    pidenombre.cpp \
    retencion.cpp \
    cuadimprimayor.cpp \
    introfecha.cpp \
    nuevo_vencimiento.cpp \
    factur/lista_fact_predef.cpp \
    factur/lotes_fact.cpp \
    factur/lotes.cpp \
    acumuladosmes.cpp \
    asigna_fichdoc.cpp \
    graf_recorte.cpp \
    pidecuenta.cpp \
    factur/ed_cta_lote.cpp \
    vies_msj.cpp \
    busca_externo.cpp \
    externos.cpp \
    notas.cpp \
    pide_fich.cpp \
    concilia.cpp \
    editafechaasiento.cpp \
    editapaseconci.cpp \
    listavenci.cpp \
    pidefecha.cpp \
    contrapartidas.cpp \
    etiqueta_fich.cpp \
    pide_info_unif.cpp \
    edit_conc_doc.cpp \
    cambiacuentapase.cpp \
    network_connections.cpp \
    mensaje_url.cpp


FORMS += factur/main_factur.ui \
         factur/listaseries.ui \
         factur/inicio.ui \
         factur/inicial.ui \
         creaempresa.ui \
         conexion.ui \
    factur/serie.ui \
    factur/listadocs.ui \
    factur/edita_doc.ui \
    factur/listarefs.ui \
    factur/edita_ref.ui \
    tiposivacta.ui \
    buscasubcuenta.ui \
    buscasubcuenta_input.ui \
    asignainputcta.ui \
    factur/facturas.ui \
    factur/factura.ui \
    ret_irpf.ui \
    ajuste_imagen_logo.ui \
    subcuentas.ui \
    ajuste_imagen.ui \
    datos_accesorios.ui \
    factur/traspasos.ui \
    tabla_asientos.ui \
    procesavencimiento.ui \
    login.ui \
    aux_express.ui \
    factur/buscaref.ui \
    asignainputdiario.ui \
    buscaconcepto.ui \
    conceptos.ui \
    ivasoportado.ui \
    ivarepercutido.ui \
    aib.ui \
    eib.ui \
    introci.ui \
    consmayor.ui \
    ivadatosadic.ui \
    ivasop_aut.ui \
    ivarep_aut.ui \
    exento.ui \
    consultavencipase.ui \
    pidenombre.ui \
    retencion.ui \
    cuadimprimayor.ui \
    introfecha.ui \
    nuevo_vencimiento.ui \
    graf_recorte.ui \
    pidecuenta.ui \
    factur/lista_fact_predef.ui \
    factur/lotes_fact.ui \
    factur/lotes.ui \
    acumuladosmes.ui \
    asigna_fichdoc.ui \
    factur/ed_cta_lote.ui \
    busca_externo.ui \
    externos.ui \
    vies_msj.ui \
    notas.ui \
    pide_fich.ui \
    concilia.ui \
    editafechaasiento.ui \
    editapaseconci.ui \
    listavenci.ui \
    pidefecha.ui \
    contrapartidas.ui \
    edit_conc_doc.ui \
    cambiacuentapase.ui \
    pide_info_unif.ui \
    mensaje_url.ui
