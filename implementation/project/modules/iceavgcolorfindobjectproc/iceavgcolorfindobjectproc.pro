include(../common.pri)

TEMPLATE = lib

RESOURCES += $$files(iceavgcolorfindobjectproc/*.qrc)

DEFINES += BUILD_ICEAVGCOLORFINDOBJECTPROC_DLL _CRT_SECURE_NO_WARNINGS

QMAKE_CXXFLAGS_WARN_ON -= -WX

INCLUDEPATH += "$$(OPENCV248)\include"
INCLUDEPATH += "$$(CUDA_TOOL55)\include"

INCLUDEPATH += "$$(CODESYN_ODB_COMMON230)" \
			   "$$(CODESYN_ODB_PGSQL231)" \
			   "$$(CODESYN_ODB_QT231)" \
			   "$$(BOOST1470)"

INCLUDEPATH += ..\iceimage
INCLUDEPATH += ..\iceodb
INCLUDEPATH += ..\iceproc
INCLUDEPATH += ..\icegui
INCLUDEPATH += ..\iceavgcolorproc
INCLUDEPATH += ..\icefindobjectproc

LIBS += -L"$$(OPENCV248)\x64\vc9\lib"
LIBS += -L"$$(CUDA_TOOL55)\lib\x64"

CONFIG(debug) {
  INCLUDEPATH += "$$(GDAL1111)\Debug\include"
}

CONFIG(release) {
  INCLUDEPATH += "$$(GDAL1111)\Release\include"
}

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
LIBS += -liceavgcolorproc$${TARGET_SUFFIX}
LIBS += -licefindobjectproc$${TARGET_SUFFIX}

FORMS += $$files(iceavgcolorfindobjectproc/ui/*.ui)
UI_HEADERS_DIR = iceavgcolorfindobjectproc
UI_SOURCES_DIR = ./

RC_FILE = iceavgcolorfindobjectproc/iceavgcolorfindobjectproc.rc
