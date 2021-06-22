export ROOT_DIR = $(shell pwd)
export BIN_DIR = $(ROOT_DIR)/bin
export INHERIT_INCLUDES = -I$(ROOT_DIR)/lib-pheripherals/include -I$(ROOT_DIR)/lib-gui/include
export LIB_DIR = -L$(ROOT_DIR)/bin

#TARGET_IP ?= 192.168.202.127
ifeq ($(TARGET_IP),)
ifneq ($(filter debug run,$(MAKECMDGOALS)),)
$(warning The target IP address is not set)
$(warning Run as "TARGET_IP=192.168.202.xxx make run" or modify Makefile)
TARGET_IP ?= 192.168.202.xxx
endif
endif
TARGET_DIR ?= /tmp/$(shell whoami)
TARGET_USER ?= root
# for use from Eduroam network use TARGET_IP=localhost and enable next line
SSH_OPTIONS=-o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -o "Port=2222"
SSH_GDB_TUNNEL_REQUIRED=y
SSH_OPTIONS+=-i ~/.ssh/mzapo-root-key
#SSH_OPTIONS=-i /opt/zynq/ssh-connect/mzapo-root-key
#SSH_OPTIONS=-o 'ProxyJump=ctu_login@postel.felk.cvut.cz'

IMAGE_VIEWER=$(BIN_DIR)/image-viewer
LIB_PHERIPHERALS=$(BIN_DIR)/libmzapo-pheripherals.so
LIB_GUI=$(BIN_DIR)/libmzapo-gui.so
TEXT_VIEWER=$(BIN_DIR)/text-viewer
FILE_BROWSER=$(BIN_DIR)/file-browser

ifdef COMPUTER
DEPENDENCIES=./.computer
else
DEPENDENCIES=./.arm
endif

all: $(DEPENDENCIES) image-viewer text-viewer file-browser

image-viewer: $(IMAGE_VIEWER)
lib-pheripherals: $(LIB_PHERIPHERALS)
lib-gui: $(LIB_GUI)
text-viewer: $(TEXT_VIEWER)
file-browser: $(FILE_BROWSER)

$(IMAGE_VIEWER): $(DEPENDENCIES) lib-pheripherals FORCE
	@make -C image-viewer

$(TEXT_VIEWER): $(DEPENDENCIES) lib-pheripherals lib-gui FORCE
	@make -C text-viewer

$(FILE_BROWSER): $(DEPENDENCIES) lib-pheripherals lib-gui FORCE
	@make -C file-browser

$(DEPENDENCIES):
	@make clean
	touch $(DEPENDENCIES)

$(LIB_PHERIPHERALS): $(DEPENDENCIES) FORCE
	@make -C lib-pheripherals

$(LIB_GUI): $(DEPENDENCIES) FORCE
	@make -C lib-gui

copy-executable: all
	ssh $(SSH_OPTIONS) -t $(TARGET_USER)@$(TARGET_IP) killall gdbserver 1>/dev/null 2>/dev/null || true
	ssh $(SSH_OPTIONS) $(TARGET_USER)@$(TARGET_IP) mkdir -p $(TARGET_DIR)
	scp -r $(SSH_OPTIONS) $(BIN_DIR) $(TARGET_USER)@$(TARGET_IP):$(TARGET_DIR)/$(NAME)
	ssh $(SSH_OPTIONS) $(TARGET_USER)@$(TARGET_IP) mv $(TARGET_DIR)/bin/* $(TARGET_DIR)

run-image-viewer: copy-executable
	ssh $(SSH_OPTIONS) -t $(TARGET_USER)@$(TARGET_IP) $(TARGET_DIR)/image-viewer $(ARG)

run-text-viewer: copy-executable
	ssh $(SSH_OPTIONS) -t $(TARGET_USER)@$(TARGET_IP) $(TARGET_DIR)/text-viewer $(ARG)

clean:
	@make -C image-viewer clean
	@make -C text-viewer clean
	@make -C file-browser clean
	@make -C lib-pheripherals clean
	@make -C lib-gui clean
	$(RM) -rf $(BIN_DIR)
	$(RM) -rf ./.computer ./.arm

FORCE:

.PHONY: all clean run-image-viewer run-text-viewer copy-executable FORCE
