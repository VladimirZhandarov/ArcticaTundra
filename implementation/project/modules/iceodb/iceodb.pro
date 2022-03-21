include(../common.pri)

TEMPLATE = lib

CONFIG += staticlib

QT -= gui

INCLUDEPATH += "$$(CODESYN_ODB_COMMON230)" \
			   "$$(CODESYN_ODB_PGSQL231)" \
			   "$$(CODESYN_ODB_QT231)" \
			   "$$(BOOST1470)" \
			   "iceodb\odb"

LIBS += -L"$$(CODESYN_ODB_QT231)\lib64"
LIBS += -L"$$(CODESYN_ODB_PGSQL231)\lib64"
LIBS += -L"$$(CODESYN_ODB_COMMON230)\lib64"

QMAKE_CXXFLAGS += -wd4127 -wd4512 -wd4189

CONFIG(debug) {
  LIBS += -lodb-qt-d
  LIBS += -lodb-pgsql-d
  LIBS += -lodb-d
}

CONFIG(release) {
  LIBS += -lodb-qt
  LIBS += -lodb-pgsql
  LIBS += -lodb
}


#
# Дополнительный ODB-компилятор
#
HEADERS_HXX = $$files(iceodb/odb/*.hxx)
HEADERS_H = $$files(iceodb/odb/*.h)

for( SRC, HEADERS_HXX ) {
  SRC = $$basename(SRC)
  SOURCES_CXX += $$replace(SRC, "\.hxx", ".cxx")
  # Исходные коды, генерируемые ODB-компилятором из файлов, сгенерированных ODBCPPGEN-компилятором
  SOURCES_ODB += $$replace(SRC, "\.hxx", "-odb.cxx")
  HEADERS_ODB += iceodb/$$replace(SRC, "\.hxx", "-odb.hxx")
  HEADERS_ODB += iceodb/$$replace(SRC, "\.hxx", "-odb.ixx")
}

SOURCES += $${SOURCES_CXX} \
           $${SOURCES_ODB}

HEADERS += $${HEADERS_ODB} \
           $${HEADERS_H}

odb_compiler.output = ${QMAKE_FILE_BASE}-odb.cxx iceodb/${QMAKE_FILE_BASE}-odb.hxx iceodb/${QMAKE_FILE_BASE}-odb.ixx

odb_compiler.input = HEADERS_HXX
odb_compiler.name = "ODB Files"

odb_compiler.commands = odb.exe \
			--database pgsql \
			--profile qt \
			--generate-query \
			--generate-schema \
			--output-dir "iceodb" \
			-I "$(QTDIR)\include" -I "$(BOOST1470)"\
			"${QMAKE_FILE_IN}" \
			&& move /Y iceodb\\${QMAKE_FILE_BASE}-odb.cxx ./

odb_compiler.clean = ${QMAKE_FILE_BASE}-odb.cxx \
			 iceodb\\${QMAKE_FILE_BASE}-odb.hxx \
			 iceodb\\${QMAKE_FILE_BASE}-odb.ixx \
			 iceodb\\${QMAKE_FILE_BASE}.sql

odb_compiler.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += odb_compiler
