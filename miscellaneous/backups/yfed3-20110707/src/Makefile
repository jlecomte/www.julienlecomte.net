PLATFORMS = i386-rhel4-gcc3
YAPACHES = 1.3
YPHPS = 5.2 5.3

include /home/y/share/yahoo_cfg/yphp.defs

CXXSRC = php_yahoo_yfed3.cpp WorkflowScheduler.cpp AsyncDispatcher.cpp
CXXFLAGS += -fno-strict-aliasing -I../src
LIB_NAME = yahoo_yfed3
SHLIB_VERSION = handler
LDEF = -DCOMPILE_DL_YAHOO_YFED3=1
LDFLAGS += -L/home/y/lib -L/home/y/lib64
LLDLIBS += -lcurl -llog4cpp_y -lst
LD_UNDEFINED_OPT += -Wl,--unresolved-symbols=ignore-all

include /home/y/share/yahoo_cfg/yphp.rules

