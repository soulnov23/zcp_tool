# mms server Makefile����������
#
# 2002-07-19 Song

CC = g++
CXX = g++
RANLIB = ranlib
AR = ar
AROPT=-scurv

#==============================================================================
#	BUILD:	��������
#------------------------------------------------------------------------------
#		BUILD_DEV:		�����汾
#		BUILD_DEBUG:		���԰汾
#		BUILD_NORMAL:		һ�㿪���汾
#		BUILD_RELEASE:		���а汾
#------------------------------------------------------------------------------
#BUILD=BUILD_NORMAL
BUILD=BUILD_DEV

AAADEF= -DSYSTEM_Q -D_MODULE_APP 

ifeq ($(BUILD), BUILD_DEV)
CFLAGS =  -Wall -fPIC -g -DDEBUG $(AAADEF)
endif
ifeq ($(BUILD), BUILD_DEBUG)
CFLAGS =  -Wall -fPIC -g -DDEBUG $(AAADEF)
endif
ifeq ($(BUILD), BUILD_NORMAL)
CFLAGS =  -Wall -fPIC -g -DINFO  $(AAADEF)
endif
ifeq ($(BUILD), BUILD_RELEASE)
CFLAGS =  -Wall -fPIC -O2 -g -DINFO  $(AAADEF)
endif



COMM_PROJECT_PATH=/data/home/kevintzhang/179/kevintzhang/comm_project_r043
#COMM_PROJECT_PATH=/data/home/harlylei/publiclib
MYSQL_PATH=$(COMM_PROJECT_PATH)/mysql-5.0.33
GNP_PATH=$(COMM_PROJECT_PATH)/gnp_oss
PNG_PATH=$(COMM_PROJECT_PATH)/png_oss
#PNG_PATH=$(COMM_PROJECT_PATH)/png
WBL_PATH=$(COMM_PROJECT_PATH)/wbl_oss
TTC_PATH=$(COMM_PROJECT_PATH)/ttc-4.1.3-suse32
DNS_PATH=$(COMM_PROJECT_PATH)/dns_suse_32_4.1.2

WBL_PATH=$(COMM_PROJECT_PATH)/wbl_oss
WBL_INC=$(WBL_PATH)/include
WBL_LIB=$(WBL_PATH)/lib/$(BUILD_ARCH)

NCS_PATH=$(COMM_PROJECT_PATH)/ncs_api
NCS_INC=$(NCS_PATH)/include
NCS_LIB=$(NCS_PATH)/lib

SSP_PATH=$(COMM_PROJECT_PATH)/ssp_api
SSP_INC=$(SSP_PATH)/include
SSP_LIB=$(SSP_PATH)/$(BUILD_ARCH)

PNG_PATH=$(COMM_PROJECT_PATH)/png_oss
PNG_INC=$(PNG_PATH)/include
PNG_LIB=$(PNG_PATH)/lib


# �Զ������ļ���������ϵ
.%.d: %.cpp
	$(CC) $(INCLUDE) -MM $< > $@
	@$(CC) $(INCLUDE) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@
%.o: %.cpp 
	$(CXX) $(CFLAGS) $(INCLUDE) -c $<

.%.d: %.c
	$(CC) $(INCLUDE) -MM $< > $@
	@$(CC) $(INCLUDE) -MM $< | sed s/"^"/"\."/  |  sed s/"^\. "/" "/  | \
                sed s/"\.o"/"\.d"/  >> $@
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $<


