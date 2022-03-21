include(../common.pri)

QT += network

TEMPLATE = app

DEFINES += VERSION=\\\"$$(VERSION)\\\" 

DEFINES += QWT_DLL

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

INCLUDEPATH += ..\iceproc
INCLUDEPATH += ..\icecore
INCLUDEPATH += ..\iceqwt
INCLUDEPATH += ..\icegui
INCLUDEPATH += ..\icemdi
INCLUDEPATH += ..\iceodb
INCLUDEPATH += ..\iceimage

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
  TARGET = tundraapp_d
}

CONFIG(release) {
  LIBS += -L"$$(GDAL1111)\Release\lib" -lgdal_i
  LIBS += -lopencv_core248 -lopencv_highgui248 -lopencv_imgproc248
  LIBS += -lodb-qt
  LIBS += -lodb-pgsql
  LIBS += -lodb
  LIBS += -lqwt
  TARGET = tundraapp
}

LIBS += -lcudart_static
LIBS += -licemdi$${TARGET_SUFFIX}
LIBS += -liceproc$${TARGET_SUFFIX}
LIBS += -licecore$${TARGET_SUFFIX}
LIBS += -liceqwt$${TARGET_SUFFIX}
LIBS += -licegui$${TARGET_SUFFIX}
LIBS += -liceodb$${TARGET_SUFFIX}
LIBS += -liceimage$${TARGET_SUFFIX}

RESOURCES += $$files(iceapp/*.qrc)
FORMS += $$files(iceapp/ui/*.ui)
UI_HEADERS_DIR = iceapp
UI_SOURCES_DIR = ./

RC_FILE = iceapp/iceapp.rc
