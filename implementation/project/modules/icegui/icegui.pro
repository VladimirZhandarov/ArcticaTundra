include(../common.pri)

QT += network webkit

TEMPLATE = lib

RESOURCES += $$files(icegui/*.qrc)

DEFINES += BUILD_ICEGUI_DLL

QMAKE_CXXFLAGS_WARN_ON -= -WX

INCLUDEPATH += "$$(OPENCV248)\include"
INCLUDEPATH += "$$(CUDA_TOOL55)\include"
INCLUDEPATH += "$$(QWT610)\src"

CONFIG(debug) {
  INCLUDEPATH += "$$(GDAL1111)\Debug\include"
}

CONFIG(release) {
  INCLUDEPATH += "$$(GDAL1111)\Release\include"
}

INCLUDEPATH += "$$(CODESYN_ODB_COMMON230)" \
			   "$$(CODESYN_ODB_PGSQL231)" \
			   "$$(CODESYN_ODB_QT231)" \
			   "$$(BOOST1470)"

INCLUDEPATH += ..\iceimage
INCLUDEPATH += ..\iceodb
INCLUDEPATH += ..\icecore
INCLUDEPATH += ..\iceproc
INCLUDEPATH += ..\iceqwt

LIBS += -L"$$(OPENCV248)\x64\vc9\lib"
LIBS += -L"$$(CUDA_TOOL55)\lib\x64"
LIBS += -L"$$(QWT610)\lib"

LIBS += -L"$$(CODESYN_ODB_QT231)\lib64"
LIBS += -L"$$(CODESYN_ODB_PGSQL231)\lib64"
LIBS += -L"$$(CODESYN_ODB_COMMON230)\lib64"

QMAKE_CXXFLAGS += -wd4127 -wd4512 -wd4189

CONFIG(debug) {
  LIBS += -L"$$(GDAL1111)\Debug\lib" -lgdal_i
  LIBS += -lopencv_core248d -lopencv_highgui248d -lopencv_imgproc248d
  LIBS += -lodb-qt-d
  LIBS += -lodb-pgsql-d
  LIBS += -lodb-d
  LIBS += -lqwtd
}

CONFIG(release) {
  LIBS += -L"$$(GDAL1111)\Release\lib" -lgdal_i
  LIBS += -lopencv_core248 -lopencv_highgui248 -lopencv_imgproc248
  LIBS += -lodb-qt
  LIBS += -lodb-pgsql
  LIBS += -lodb
  LIBS += -lqwt
}

LIBS += -lcudart_static
LIBS += -liceimage$${TARGET_SUFFIX}
LIBS += -liceodb$${TARGET_SUFFIX}
LIBS += -licecore$${TARGET_SUFFIX}
LIBS += -liceproc$${TARGET_SUFFIX}
LIBS += -liceqwt$${TARGET_SUFFIX}

FORMS += $$files(icegui/ui/*.ui)
UI_HEADERS_DIR = icegui
UI_SOURCES_DIR = ./

RC_FILE = icegui/icegui.rc
