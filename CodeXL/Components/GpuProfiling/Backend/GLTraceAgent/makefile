#MAKE FILE FOR GLTraceAgent

DEPTH = ../..
include $(DEPTH)/Build/Common.mk

TARGETSO = $(BACKEND_OUTPUT_DIR)/lib$(GPU_PROFILER_LIB_PREFIX)GLTraceAgent$(TARGET_SUFFIX).so

ADDL_DEFINES = -DNON_BLOCKING_TIMEOUT

INCLUDES = \
	-I. \
	-I$(BACKEND_SYCLPROFILEAGENT_DIR) \
	-I$(BACKEND_COMMON_DIR) \
	-I$(COMMON_PROJ_DIR) \
	-isystem$(ATI_STREAM_SDK_DIR)/include \
	-isystem$(CAL_DIR)/include \
	-I$(DYNAMICLIBRARYMODULE_DIR) \
	-I$(DEVICEINFO_DIR) \
	-I$(TSINGLETON_DIR) \
	-I$(UTF8CPP_DIR) \
	-I$(AMDTMUTEX_DIR)

LIBS = \
	$(COMMON_LIBS) \
	$(FRAMEWORK_LIBS) \
	$(STANDARD_LIBS)

LIBPATH = \
	-L/usr/lib \
	$(COMMON_LIB_PATHS)


SO_OBJS =  \
	./$(OBJ_DIR)/AtpFile.o \
	./$(OBJ_DIR)/GLAtpFile.o 


include $(DEPTH)/Build/CommonTargets.mk

# END OF MAKE FILE
