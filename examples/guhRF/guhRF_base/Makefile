ifeq ($(TARGET), guhRF)
PLATFORM_FILES= avr-size-server er-example-server.guhRF.hex \
    er-example-server.guhRF.eep                             \

endif


# By default we use the manually-assigned VMWare range of MAC addresses
# VMWare Range is 00:50:56:XX:YY:ZZ
# Where XX is a between 00 and 3F and YY and ZZ are between 00 and FF.
# From this we generate the EUI64.
# But you really should assign the address that is printed on your Board!
SED              = sed
EUI64_ADDRESS    = 00:50:56:FF:FF:03:04:05
PARAMS_EUI64ADDR =                                              \
    $(shell echo ${EUI64_ADDRESS}                               \
     | $(SED) -e 's/^/{0x/' -e 's/[:-]/, 0x/g' -e 's/$$/}/'     \
     )


CFLAGS += -DPARAMS_EUI64ADDR='${PARAMS_EUI64ADDR}'




all: er-example-server $(PLATFORM_FILES)

# use target "er-plugtest-server" explicitly when requried 

CONTIKI=../../..

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

#LDFLAGS += -Wl,--section-start=.jumps=0x3FF80

# automatically build RESTful resources including common resources
ifndef TARGET
    REST_EXCLUDE=
else
ifeq ($(TARGET), native)
    REST_EXCLUDE=
else
    REST_EXCLUDE= ! -name 'res-plugtest*'
endif
endif
REST_RESOURCES_DIR = ./resources
REST_RESOURCES_DIR_COMMON = ../resources-common
REST_RESOURCES_FILES = $(notdir \
    $(shell find $(REST_RESOURCES_DIR) -name '*.c' $(REST_EXCLUDE)) \
    $(shell find $(REST_RESOURCES_DIR_COMMON) -name '*.c') \
    )

PROJECTDIRS += $(REST_RESOURCES_DIR) $(REST_RESOURCES_DIR_COMMON)
PROJECT_SOURCEFILES += $(REST_RESOURCES_FILES)

# linker optimizations
SMALL=1

# REST Engine shall use Erbium CoAP implementation
APPS += er-coap
APPS += rest-engine



# optional rules to get assembly
#CUSTOM_RULE_C_TO_OBJECTDIR_O = 1
#CUSTOM_RULE_S_TO_OBJECTDIR_O = 1

CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include


#optional rules to get assembly
#$(OBJECTDIR)/%.o: asmdir/%.S
#	$(CC) $(CFLAGS) -MMD -c $< -o $@
#	@$(FINALIZE_DEPENDENCY)
#
#asmdir/%.S: %.c
#	$(CC) $(CFLAGS) -MMD -S $< -o $@



#-----------------------------------------------------------------------
#			guh is good for you
#------------------------------------------------------------------------
avr-size-server: er-example-server.guhRF
	avr-size -C --mcu=MCU=atmega256rfr2 er-example-server.guhRF

er-example-server.guhRF.hex: er-example-server.guhRF
	avr-objcopy -j .text -j .data -O ihex er-example-server.guhRF \
	    er-example-server.guhRF.hex

er-example-server.guhRF.eep: er-example-server.guhRF
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	    --change-section-lma .eeprom=0 -O ihex                      \
	    er-example-server.guhRF er-example-server.guhRF.eep

flash-server: er-example-server.guhRF.hex er-example-server.guhRF.eep
	avrdude -pm256rfr2 -cstk500 -b57600 -P/dev/ttyUSB0 -D -U \
	    flash:w:er-example-server.guhRF.hex:a -U \
	    eeprom:w:er-example-server.guhRF.eep:a

flash-server-jtag: er-example-server.guhRF.hex er-example-server.guhRF.eep
	avrdude -pm256rfr2 -catmelice -P usb -U \
	    flash:w:er-example-server.guhRF.hex:a -U \
	    eeprom:w:er-example-server.guhRF.eep:a


.PHONY: flash-server avr-size-server
