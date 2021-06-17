export ROOT_DIR = $(shell pwd)
export BIN_DIR = $(ROOT_DIR)/bin
export INHERIT_INCLUDES = -I$(ROOT_DIR)/lib-pheripherals/include
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
#SSH_OPTIONS=-o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -o "Port=2222"
#SSH_GDB_TUNNEL_REQUIRED=y
SSH_OPTIONS+=-i ~/.ssh/mzapo-root-key
#SSH_OPTIONS=-i /opt/zynq/ssh-connect/mzapo-root-key
#SSH_OPTIONS=-o 'ProxyJump=ctu_login@postel.felk.cvut.cz'

all: image-viewer FORCE

image-viewer: FORCE lib-pheripherals
	@make -C image-viewer

lib-pheripherals: FORCE
	@make -C lib-pheripherals

copy-executable: all
	ssh $(SSH_OPTIONS) -t $(TARGET_USER)@$(TARGET_IP) killall gdbserver 1>/dev/null 2>/dev/null || true
	ssh $(SSH_OPTIONS) $(TARGET_USER)@$(TARGET_IP) mkdir -p $(TARGET_DIR)
	scp -r $(SSH_OPTIONS) $(BIN_DIR) $(TARGET_USER)@$(TARGET_IP):$(TARGET_DIR)/$(NAME)
	ssh $(SSH_OPTIONS) $(TARGET_USER)@$(TARGET_IP) mv $(TARGET_DIR)/bin/* $(TARGET_DIR)

run-image-viewer: copy-executable all 
	ssh $(SSH_OPTIONS) -t $(TARGET_USER)@$(TARGET_IP) $(TARGET_DIR)/image-viewer $(ARG)

clean:
	@make -C image-viewer clean
	@make -C lib-pheripherals clean
	$(RM) -rf $(BIN_DIR)

FORCE: ;

.PHONY: clean all image-viewer FORCE copy-executable run-image-viewer
