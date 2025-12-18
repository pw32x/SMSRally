#change to your preferred name
#pass the PROGNAME name as a parameter to set a new rom name. "make PROGNAME=my_new_name"
PROGNAME?=RacingGame

#change to your devkitSMS folder
DEVKITSMS_BASE=../../../devkitSMS-master

LOCAL_LIB_DIR = lib

CC=sdcc
IHX2SMS=$(DEVKITSMS_BASE)/ihx2sms/Linux/ihx2sms
SMSLIB_BASE=$(LOCAL_LIB_DIR)
PSGLIB_BASE=$(LOCAL_LIB_DIR)
SMSLIB_INCDIR=$(LOCAL_LIB_DIR)
PSGLIB_INCDIR=$(LOCAL_LIB_DIR)
PEEP_RULES=$(SMSLIB_BASE)/src/peep-rules.txt
CRT0=$(DEVKITSMS_BASE)/crt0/crt0_sms.rel
SMSLIB_LIB=$(SMSLIB_BASE)/SMSlib.lib
PSGLIB_LIB=$(PSGLIB_BASE)/PSGlib.lib
ASSET_BANKS_DIR=assets/banks

SOURCE_DIR = source
EXPORTED_SRC_DIR = $(SOURCE_DIR)/exported
OUTPUT_DIR = out

SOURCE_OUTPUT_DIR = $(OUTPUT_DIR)/$(SOURCE_DIR)

CFLAGS=-mz80 -I$(SMSLIB_INCDIR) -I$(PSGLIB_INCDIR) \
       -I$(SOURCE_DIR) -I$(EXPORTED_SRC_DIR) -I$(ASSET_BANKS_DIR) \
       --peep-file $(PEEP_RULES) --opt-code-speed

LDFLAGS=-mz80 --no-std-crt0 --data-loc 0xC000 -Wl-b_BANK2=0x8000

# collect sources
CS := $(wildcard $(SOURCE_DIR)/*.c)
EXPORTED_CS := $(shell find $(EXPORTED_SRC_DIR) -name '*.c')

# map sources to objects (preserve folder structure)
SOURCE_OBJS := $(patsubst $(SOURCE_DIR)/%.c,$(OUTPUT_DIR)/$(SOURCE_DIR)/%.rel,$(CS))
EXPORTED_OBJS := $(patsubst $(SOURCE_DIR)/%.c,$(OUTPUT_DIR)/$(SOURCE_DIR)/%.rel,$(EXPORTED_CS))

OBJS = $(SOURCE_OBJS) $(EXPORTED_OBJS)

ASSET_BANKS_OBJS = $(wildcard $(ASSET_BANKS_DIR)/*.rel)

all: $(OUTPUT_DIR)/$(PROGNAME).sms

# compile rules (auto-create directories)
$(OUTPUT_DIR)/$(SOURCE_DIR)/%.rel: $(SOURCE_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUTPUT_DIR)/$(PROGNAME).ihx: $(OBJS)
	$(CC) -o $@ $(LDFLAGS) \
	$(CRT0) $(ASSET_BANKS_OBJS) $(SMSLIB_LIB) $(PSGLIB_LIB) $^

$(OUTPUT_DIR)/$(PROGNAME).sms: $(OUTPUT_DIR)/$(PROGNAME).ihx
	$(IHX2SMS) $< $@

clean:
	rm -rf $(OUTPUT_DIR)

# force relink on every run if linking fails
.PHONY: clean
