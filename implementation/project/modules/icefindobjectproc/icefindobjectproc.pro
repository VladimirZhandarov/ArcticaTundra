include(../common.pri)

TEMPLATE = lib

RESOURCES += $$files(icefindobjectproc/*.qrc)

DEFINES += BUILD_ICEFINDOBJECTPROC_DLL _CRT_SECURE_NO_WARNINGS

QMAKE_CXXFLAGS_WARN_ON -= -WX

INCLUDEPATH += "$$(OPENCV248)\include"
INCLUDEPATH += "$$(CUDA_TOOL55)\include"

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
INCLUDEPATH += ..\iceproc
INCLUDEPATH += ..\icegui

LIBS += -L"$$(OPENCV248)\x64\vc9\lib"
LIBS += -L"$$(CUDA_TOOL55)\lib\x64"

LIBS += -L"$$(CODESYN_ODB_QT231)\lib64"
LIBS += -L"$$(CODESYN_ODB_PGSQL231)\lib64"
LIBS += -L"$$(CODESYN_ODB_COMMON230)\lib64"

QMAKE_CXXFLAGS += -wd4127 -wd4512 -wd4189

CONFIG(debug) {
  LIBS += -L"$$(GDAL1111)\Debug\lib" -lgdal_i
  LIBS += -lopencv_core248d -lopencv_highgui248d -lopencv_imgproc248d -lopencv_gpu248d
  LIBS += -lodb-qt-d
  LIBS += -lodb-pgsql-d
  LIBS += -lodb-d
}

CONFIG(release) {
  LIBS += -L"$$(GDAL1111)\Release\lib" -lgdal_i
  LIBS += -lopencv_core248 -lopencv_highgui248 -lopencv_imgproc248 -lopencv_gpu248
  LIBS += -lodb-qt
  LIBS += -lodb-pgsql
  LIBS += -lodb
}

LIBS += -lcudart_static
LIBS += -liceimage$${TARGET_SUFFIX}
LIBS += -liceodb$${TARGET_SUFFIX}
LIBS += -liceproc$${TARGET_SUFFIX}
LIBS += -licegui$${TARGET_SUFFIX}

FORMS += $$files(icefindobjectproc/ui/*.ui)
UI_HEADERS_DIR = icefindobjectproc
UI_SOURCES_DIR = ./

RC_FILE = icefindobjectproc/icefindobjectproc.rc
