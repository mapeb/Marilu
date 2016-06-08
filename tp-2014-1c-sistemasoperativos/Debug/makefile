################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include commons/commons/parser/subdir.mk
-include commons/commons/collections/subdir.mk
-include commons/commons/subdir.mk
-include UMV/src/subdir.mk
-include Programa/src/subdir.mk
-include Kernel/src/subdir.mk
-include CPU/src/subdir.mk
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
all: aaaaaaaa

# Tool invocations
aaaaaaaa: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	gcc  -o "aaaaaaaa" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) aaaaaaaa
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
