include $(TEE_TOP_REL_PATH)/build/tee/tee_client_api.mk

APP_CROSS_COMPILE ?= arm-marvell-linux-gnueabi-

ifeq ($(APP_CC), clang)
CC	= $(APP_CC)
else
CC	= $(APP_CROSS_COMPILE)gcc
endif
AR	= $(APP_CROSS_COMPILE)ar

CFLAGS	+= $(TEEC_CFLAGS)
CFLAGS	+= -I. $(addprefix -I, $(sort $(dir $(TEEC_REL_INCLUDES))))
CFLAGS	+= $(addprefix -I, $(CA_C_INCLUDES))

CFILES	:= $(CA_SRC_FILES) $(TEEC_REL_CFILES)
COBJS	:= $(patsubst %.c,%.o,$(CFILES))

all: $(CA_MODULE).so $(CA_MODULE).a

$(CA_MODULE).so: $(CFILES)
	mkdir -p $(dir $(ODIR)/$@)
	$(CC) $(CFLAGS) $(CA_CFLAGS) -fPIC -shared -Wl,-soname,$(CA_MODULE).so -o $(ODIR)/$@ $^ $(CA_LDFLAGS)

$(CA_MODULE).a: $(COBJS)
	$(AR) cr $(ODIR)/$@ $(addprefix $(ODIR),$(subst $(TEE_TOP_REL_PATH),,$^))

%.o : %.c
	mkdir -p $(ODIR)$(dir $(subst $(TEE_TOP_REL_PATH),,$@))
	$(CC) $(CFLAGS) $(CA_CFLAGS) -fPIC -c -o $(ODIR)$(subst $(TEE_TOP_REL_PATH),,$@) $(abspath $^)

clean:
	rm -f $(COBJS)
	rm -f $(CA_MODULE)
	rm -f $(CA_MODULE).*
