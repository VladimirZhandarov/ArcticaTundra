include(../common.pri)

QT += xml

TEMPLATE = lib

DEFINES += BUILD_ICEIMAGE_DLL _CRT_SECURE_NO_WARNINGS

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

INCLUDEPATH += ..\iceodb

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
LIBS += -liceodb$${TARGET_SUFFIX}

#
# Дополнительный CUDA-компилятор
#
SOURCES_CUDA = $$files(*.cu)

cuda_compiler.output = $${OBJECTS_DIR}/${QMAKE_FILE_BASE}_cuda.obj

cuda_compiler.input = SOURCES_CUDA
cuda_compiler.name = "CUDA Files"

CONFIG(debug) {
  cuda_compiler.commands = nvcc.exe \
			-gencode=arch=compute_10,code=\\\"sm_10,compute_10\\\" \
			-gencode=arch=compute_20,code=\\\"sm_20,compute_20\\\" \
			-gencode=arch=compute_30,code=\\\"sm_30,compute_30\\\" \
			-gencode=arch=compute_35,code=\\\"sm_35,compute_35\\\" \
			--machine 64 \
			-ccbin \"%VS90COMNTOOLS%/../../VC/bin\" \
			-DWIN32 -Xcompiler \"/EHsc /W3 /nologo /Od /Zi /MDd\" \
			-I\"$(CUDA_TOOL55)/include\" -I\"$(OPENCV248)/include\" \
			-maxrregcount=0 --compile \
			-o \"${QMAKE_FILE_OUT}\" ${QMAKE_FILE_NAME} -Xcompiler \"/wd 4819\"
}
CONFIG(release) {
  cuda_compiler.commands = nvcc.exe \
			-gencode=arch=compute_10,code=\\\"sm_10,compute_10\\\" \
			-gencode=arch=compute_20,code=\\\"sm_20,compute_20\\\" \
			-gencode=arch=compute_30,code=\\\"sm_30,compute_30\\\" \
			-gencode=arch=compute_35,code=\\\"sm_35,compute_35\\\" \
			--machine 64 \
			-ccbin \"%VS90COMNTOOLS%/../../VC/bin\" \
			-DWIN32 -Xcompiler \"/EHsc /W3 /nologo /O2 /Zi /MD\" \
			-I\"$(CUDA_TOOL55)/include\" -I\"$(OPENCV248)/include\" \
			-maxrregcount=0 --compile \
			-o \"${QMAKE_FILE_OUT}\" ${QMAKE_FILE_NAME} -Xcompiler \"/wd 4819\"
}

cuda_compiler.dependency_type = TYPE_C
cuda_compiler.clean = $${OBJECTS_DIR}/${QMAKE_FILE_BASE}_cuda.obj

QMAKE_EXTRA_COMPILERS += cuda_compiler
