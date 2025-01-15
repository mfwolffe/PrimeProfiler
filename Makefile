#
# Simple (but not too simple) Makefile
# Matt Wolffe, James Madison University, 2025*
#
# (*I'm not like a prof, this is just a nod to the 
# way Dr. Lam documented his makefiles for 261)
#
# This makefile builds the C binary to instrument with specified
# optimization level, Or builds the pin-tool c++ binary (once I write it)
# by invoking their respective makefiles.
#
#

.PHONY: all clean opt0 opt1 opt2 opt3 optfast pin_tool

all: opt0 opt1 opt2 opt3 optfast pin_tool
Conly: opt0 opt1 opt2 opt3 optfast
Cpponly: pin_tool

opt0:
    $(MAKE) -f makefiles/Makefile.opt0

opt1:
    $(MAKE) -f makefiles/Makefile.opt1

opt2:
    $(MAKE) -f makefiles/Makefile.opt2

opt3:
    $(MAKE) -f makefiles/Makefile.opt3

optfast:
    $(MAKE) -f makefiles/Makefile.optfast

pin_tool:
    $(MAKE) -f makefiles/Makefile.pin

clean:
    $(MAKE) -f makefiles/Makefile.opt0 clean
    $(MAKE) -f makefiles/Makefile.opt1 clean
    $(MAKE) -f makefiles/Makefile.opt2 clean
    $(MAKE) -f makefiles/Makefile.opt3 clean
    $(MAKE) -f makefiles/Makefile.optfast clean
    $(MAKE) -f makefiles/Makefile.pin clean

