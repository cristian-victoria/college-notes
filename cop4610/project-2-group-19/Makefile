obj-m += my_timer.o

# Get the kernel build directory
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target: build the module
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean target: remove generated files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f *.o *.ko *.mod.c *.mod *.order *.symvers

# Install target (optional)
install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

# make 						= Build the module
# make clean 				= Clean up
# sudo insmod my_timer.ko 	= Load the module
# sudo rmmod my_timer 		= Unload the module
# cat /proc/timer			= Read the timer