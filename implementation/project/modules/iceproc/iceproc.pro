include(../common.pri)

TEMPLATE = lib

RESOURCES += $$files(iceproc/*.qrc)

DEFINES += BUILD_ICEPROC_DLL _CRT_SECURE_NO_WARNINGS

QMAKE_CXXFLAGS_WARN_ON -= -WX

INCLUDEPATH += "$$(OPENCV248)\include"
INCLUDEPATH += "$$(CUDA_TOOL55)\include"
INCLUDEPATH += "$$(QJSON)\include"

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

INCLUDEPATH += ..\cuda_helper
INCLUDEPATH += ..\iceimage
INCLUDEPATH += ..\iceodb

LIBS += -L"$$(OPENCV248)\x64\vc9\lib"
LIBS += -L"$$(CUDA_TOOL55)\lib\x64"
LIBS += -L"$$(QJSON)\src"

LIBS += -L"$$(CODESYN_ODB_QT231)\lib64"
LIBS += -L"$$(CODESYN_ODB_PGSQL231)\lib64"
LIBS += -L"$$(CODESYN_ODB_COMMON230)\lib64"

QMAKE_CXXFLAGS += -wd4127 -wd4512 -wd4189

CONFIG(debug) {
  LIBS += -L"$$(GDAL1111)\Debug\lib" -lgdal_i
  LIBS += -lopencv_core248d -lopencv_highgui248d -lopencv_imgproc248d
  LIBS += -lqjsond
  LIBS += -lodb-qt-d
  LIBS += -lodb-pgsql-d
  LIBS += -lodb-d
}

CONFIG(release) {
  LIBS += -L"$$(GDAL1111)\Release\lib" -lgdal_i
  LIBS += -lopencv_core248 -lopencv_highgui248 -lopencv_imgproc248
  LIBS += -lqjson
  LIBS += -lodb-qt
  LIBS += -lodb-pgsql
  LIBS += -lodb
}

LIBS += -lcudart_static
LIBS += -liceimage$${TARGET_SUFFIX}
LIBS += -liceodb$${TARGET_SUFFIX}

FORMS += $$files(iceproc/ui/*.ui)
UI_HEADERS_DIR = iceproc
UI_SOURCES_DIR = ./

RC_FILE = iceproc/iceproc.rc
