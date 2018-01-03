all:
	cd kernel && make && cd -
	make -C user
clean:
	cd kernel && make clean && cd -
	make -C user clean
test:all
	-@sudo rmmod netlinkKernel
	insmod kernel/netlinkKernel.ko
	sudo user/netlinkUser
