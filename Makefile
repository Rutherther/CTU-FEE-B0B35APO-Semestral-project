export ROOT_DIR = $(shell pwd)
export BIN_DIR = $(ROOT_DIR)/bin

#export SSH_OPTIONS=-o "UserKnownHostsFile=/dev/null" -o "StrictHostKeyChecking=no" -o "Port=2222"
#export SSH_GDB_TUNNEL_REQUIRED=y
#export SSH_OPTIONS=-i /opt/zynq/ssh-connect/mzapo-root-key
#export SSH_OPTIONS=-o 'ProxyJump=ctu_login@postel.felk.cvut.cz'

all: image-viewer

image-viewer:
	@make -C image-viewer

copy-executables:
	@make -C image-viewer copy-executables

clean:
	@make -C image-viewer clean
	rm -rf $(BIN_DIR)
