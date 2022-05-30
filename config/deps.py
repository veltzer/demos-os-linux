import os # for uname
import sys # for exit
import subprocess # for check_output
import platform

##############
# parameters #
##############
# debug this script?
opt_debug=False
# exit after debug?
opt_exit=False
# install kernels?
opt_do_kernel=False
# install debug packages for kernels?
opt_do_ddebs=False
# do we want to install compilers?
opt_do_compilers=False

# version of the papi library
opt_papiversion="6.0"
# what version of wxgtk to install?
opt_wxgtk_ver="3.0"
# what boost to install?
opt_boost_version_short="1.74"
opt_boost_version=opt_boost_version_short+".0"

release=os.uname().release
short_release=release[:release.rfind("-")]
source_release=short_release[:short_release.rfind("-")]
release_lowlatency=short_release
release_generic=short_release

if opt_debug:
    print(f"release is [{release}]")
    print(f"short_release is [{short_release}]")
    print(f"source_release is [{source_release}]")
    print(f"release_lowlatency is [{release_lowlatency}]")
    print(f"release_generic is [{releae_generic}]")

if opt_exit:
    sys.exit(0)

if hasattr(platform, "freedesktop_os_release"):
    desktop = platform.freedesktop_os_release()
    VERSION_ID = desktop["VERSION_ID"]
    # what version of urcu?
    opt_urcu_ver = None
    if VERSION_ID == "22.04":
        opt_urcu_ver = 8
else:
    opt_urcu_ver = 6

packages_kernels=[
]
packages_doc=[
    "gcc-doc", # gcc documentation
    "cpp-doc", # gcc documentation
    "ncurses-doc", # ncurses documentation
    "binutils-doc", # binutils documentation
    "libasound2-doc",
    #"libgnomeui-doc",
    "libsigc++-2.0-doc",
    "libgtkmm-2.4-doc",
    "libgtkmm-3.0-doc",
    "libstdc++-9-doc",
    "aspell-doc",
    # libc
    "glibc-doc",
    "glibc-doc-reference",
    "scons-doc",
    #"jlint-doc",
    "make-doc",
    "systemtap-doc",
    "postgresql-doc",
    "papi-examples", # PAPI example files and test programs
]

packages_tools=[
    # debugging
    "cgdb",
    "qtcreator",
    # code measurements
    "sloccount",
    "cloc",
    # collection of command line tools
    "bikeshed",
    # manual pages
    "manpages",
    "manpages-dev",
    "manpages-posix",
    "manpages-posix-dev",
    # tools for building
    "gnulib",
    # tool for converting documents from one format to another
    "unoconv",
    "cpufrequtils",
    "netperf",
    # tools
    "linux-tools-common", # for perf(1)
    "python3-uno", # for soffice conversion
    #"vnstat", # causes performance problems
    #"vnstati", # causes performance problems
    "wireshark-common",
    "wireshark",
    "ngrep",
    "iftop",
    "traceroute",
    "valgrind",
    "dwarves",
    "kerneltop",
    "tshark",
    "google-perftools",
    "pv",
    "splint",
    "patchelf",
    "schedtool",
    "blktrace",
    "fdutils", # floppy disk utilities ?!?
    "iotop", # for the iotop command
    "jnettop", # for the jnettop command
    "smartmontools",
    "gsmartcontrol",
    "lm-sensors",
    "inxi",
    # doesn"t exist on 22.04
    # "hddtemp",
    "htop",
    "glances",
    "sysprof",
    "mutrace",
    "cpulimit", # for the cpulimit command
    "bridge-utils", # for bridging utilities to demo creation of a bridge
    "iptraf",
    "numactl",
    "pstack",
    "x86info",
    "lsscsi",
    "chrpath",
    "latencytop",
    "devmem2",
    "elfutils", # for manipulating elf files
    "pax-utils",
    "paxctl",
    "execstack",
    "prelink",
    "dstat",
    "cpuid",
    "sysstat",
    "nmon",
    "saidar",
    "sysdig",
    "iperf",
    "smem",
    "sysbench",
    "gnome-system-monitor",
    "xfce4-taskmanager",
    "mrtg",
    "isag",
    "sdparm",
    "mytop",
    "cutils",
    "hlint",
    "dlint",
    "oprofile",
    "powertop",
    "rt-tests",
    "procinfo",
    "wavemon",

    # fun stuff
    "figlet",
    "sl",

    # tools used by this package
    "libreoffice-common", # for soffice conversion
    "xutils-dev", # for makedepend(1)
    "astyle", # for indent(1) (code formatting)

    # these packages are for the developers pleasure...:)
    "vim",
    "tofrodos",
    "pipemeter",
    "blktool",
    "scons",
    "doxygen",
    "make",

    # memory testing tools
    "memtester",
    "memtest86+",
    
    # papi
    "papi-tools", # PAPI utilities

    # papi

    # assmebly
    "nasm",
    "yasm",

    # kernel debugging
    "systemtap",
    "systemtap-common",
    "systemtap-runtime",

    # kexec and kernel crash debugging
    "crash",
    "kdump-tools",
    "kexec-tools",
    "linux-crashdump",
    "makedumpfile",

    # databases
    "postgresql-client",
    "postgresql-client-common",

    # watchdogs
    "rtkit",
    "watchdog",
    "supervisor",
    "daemontools",
    "ruby-god",
    #"monit",

    # firewalls
    "ufw",
    "shorewall",

    # javascript
    "nodejs",
    "npm",
]

packages=[
    # packages needed for the build
    "ccache",
    "uncrustify", # for uncrustify(1) (code formatting)
    "indent", # for indent(1) (code formatting)
    "electric-fence",

    # dialog really has header files and that is why it is here
    "dialog",
    "gcc-9-plugin-dev",
    "binutils-dev",
    "libxtables-dev",
    "libevent-dev",
    "libevent-2.1-7",
    "libiberty-dev",
    "libncurses5",
    "libncurses5-dev",
    "libncursesw5",
    "libncursesw5-dev",
    "libprocps8",
    "libprocps-dev",
    "libsystemd0",
    "libsystemd-dev",
    "libsigc++-2.0-0v5",
    "libsigc++-2.0-dev",
    "libgtkmm-2.4-dev",
    "libgtkmm-3.0-dev",
    "libpq-dev",
    "liblog4cpp5-dev",
    "libmysqlclient-dev",
    f"libwxgtk{opt_wxgtk_ver}-gtk3-dev",
    "libmysql++-dev",
    "libsdl1.2-dev",
    "libace-dev",
    f"libboost{opt_boost_version_short}-dev",
    f"libboost-thread{opt_boost_version}",
    f"libboost-system{opt_boost_version}",
    "libpcap-dev",
    "libasound2",
    "libasound2-dev",
    "libdmalloc5",
    "libdmalloc-dev",
    "libcpufreq-dev", # for cpufreq.h
    "libnetfilter-queue1",
    "libnetfilter-queue-dev",
    "libcap-dev", # for capability.h
    f"liburcu{opt_urcu_ver}", # rcu library
    "liburcu-dev", # rcu library
    "libunwind8", # unwind library
    "libunwind-setjmp0", # unwind library
    "libunwind8-dev", # unwind library
    "libunwind-setjmp0-dev", # unwind library
    "libelf1", # for reading elf files
    "libdw1",
    "libdw-dev",
    "libasm1",
    "libasm-dev",
    "libaspell-dev",
    "libacl1-dev",
    "libattr1-dev",
    "libdaemon-dev",
    "libsystemd-dev",
    "libsystemd0",
    "libpapi-dev",
    f"libpapi{opt_papiversion}",
    "libpopt-dev",
    "systemtap-sdt-dev",
    "liburing-dev",
    "libffi7",
    "libffi8",
    "libffi-dev",
]

if opt_do_kernel:
    packages.extend([
        "linux-headers-generic",
        "linux-headers-lowlatency",
        f"linux-headers-{release_generic}-generic",
        f"linux-headers-{release_lowlatency}-lowlatency",
    ])
    packages_kernels.extend([
        f"linux-image-{release_generic}-generic",
        f"linux-image-{release_lowlatency}-lowlatency",
    ])
    packages_tools.extend([
        "linux-tools-generic",
        "linux-tools-lowlatency",
        f"linux-tools-{release_generic}-generic",
        f"linux-tools-{release_lowlatency}-lowlatency",
    ])
    packages_doc.extend([
        "linux-source",
        f"linux-source-{source_release}",
    ])

if opt_do_ddebs:
    packages_kernels.extend([
        # for systemtap - this one does not exist in the regular ubuntu archives
        # ubuntu does not always carry the "-lowlatency" dbgsym and that why
        # it is commented out...
        # TODO: make this script add sources to the dbgsyms if need be
        f"linux-image-{release_generic}-generic-dbgsym",
        f"linux-image-{release_lowlatency}-lowlatency-dbgsym",
    ])

if opt_do_compilers:
    packages.extend([
        # compilers
        "gcc-4.7",
        "g++-4.7",
        "gcc-4.8",
        "g++-4.8",
        "gcc-4.8-plugin-dev",
        "g++-4.9",
        "gcc-snapshot",
        "clang",
    ])
    packages_doc.extend([
        "gcc-4.7-doc",
        "gcc-4.8-doc",
    ])
