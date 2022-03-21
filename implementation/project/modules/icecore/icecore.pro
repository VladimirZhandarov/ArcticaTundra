include(../common.pri)

TEMPLATE = lib

RESOURCES += $$files(icecore/*.qrc)

DEFINES += BUILD_ICECORE_DLL

QMAKE_CXXFLAGS_WARN_ON -= -WX

INCLUDEPATH += "$$(OPENCV248)\include"

INCLUDEPATH += "$$(CODESYN_ODB_COMMON230)" \
			   "$$(CODESYN_ODB_PGSQL231)" \
			   "$$(CODESYN_ODB_QT231)" \
			   "$$(BOOST1470)"

INCLUDEPATH += ..\iceodb
INCLUDEPATH += ..\iceodb\iceodb\odb
INCLUDEPATH += ..\iceproc

#TODO remove
INCLUDEPATH += ..\iceimage

LIBS += -L"$$(OPENCV248)\x64\vc9\lib"

LIBS += -L"$$(CODESYN_ODB_QT231)\lib64"
LIBS += -L"$$(CODESYN_ODB_PGSQL231)\lib64"
LIBS += -L"$$(CODESYN_ODB_COMMON230)\lib64"

QMAKE_CXXFLAGS += -wd4127 -wd4512 -wd4189

CONFIG(debug) {
  LIBS += -lopencv_core248d -lopencv_highgui248d -lopencv_imgproc248d
  LIBS += -lodb-qt-d
  LIBS += -lodb-pgsql-d
  LIBS += -lodb-d
}

CONFIG(release) {
  LIBS += -lopencv_core248 -lopencv_highgui248 -lopencv_imgproc248
  LIBS += -lodb-qt
  LIBS += -lodb-pgsql
  LIBS += -lodb
}

LIBS += -liceodb$${TARGET_SUFFIX}
LIBS += -liceproc$${TARGET_SUFFIX}

RC_FILE = icecore/icecore.rc