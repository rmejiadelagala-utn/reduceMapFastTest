################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: seudoMarta

dependents:
	-cd /home/rmejia/Repositories/tp-2015-1c-socketes-planificados/Socketes/Debug && $(MAKE) clean all

# Tool invocations
seudoMarta: $(OBJS) $(USER_OBJS) /home/rmejia/Repositories/tp-2015-1c-socketes-planificados/Socketes/Debug/libsocketes.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/rmejia/Repositories/tp-2015-1c-socketes-planificados/Socketes/Debug" -o "seudoMarta" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) seudoMarta
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/rmejia/Repositories/tp-2015-1c-socketes-planificados/Socketes/Debug/libsocketes.so:

-include ../makefile.targets
