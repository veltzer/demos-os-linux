import os
import sys
import platform

# debug this script?
opt_debug=False
# exit after debug?
opt_exit=False
# install kernels?
opt_do_kernel=False
# install debug packages for kernels?
opt_do_ddebs=False
# do we want to install compilers?
opt_do_compilers=True

ver_boost_short = "1.74"
ver_boost=ver_boost_short+".0"
ver_urcu = "8"
ver_unwind = "8"

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
    print(f"release_generic is [{release_generic}]")

if opt_exit:
    sys.exit(0)

libevent_ver = None
desktop = platform.freedesktop_os_release()
VERSION_ID = desktop["VERSION_ID"]
if VERSION_ID == "22.04":
    ver_papi = "6.0"
    libevent_ver = "2.1-7"
    ver_wxgtk = "3.0"
    lib_wx=f"libwxgtk{ver_wxgtk}-gtk3-dev"
    libncurses_ver="5"
if VERSION_ID == "22.10":
    ver_papi = "6.0"
    ver_unwind = "-15"
    libevent_ver = "2.1-7a"
    ver_wxgtk = "3.0"
    lib_wx=f"libwxgtk{ver_wxgtk}-gtk3-dev"
    libncurses_ver="5"
if VERSION_ID == "23.04":
    ver_papi = "7.0"
    libevent_ver = "2.1-7a"
    ver_wxgtk = "3.2"
    lib_wx=f"libwxgtk{ver_wxgtk}-dev"
    libncurses_ver="5"
if VERSION_ID == "23.10":
    ver_papi = "7.0"
    libevent_ver = "2.1-7"
    ver_wxgtk = "3.2"
    lib_wx=f"libwxgtk{ver_wxgtk}-dev"
    libncurses_ver="6"
assert libevent_ver is not None

packages_kernels=[
]
packages_doc=[
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
    # ruby stuff
    "ruby-bundler",
    "rbenv",
    # for spelling
    "aspell",
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
    "cpu-checker",
    "netperf",
    # tools
    "shellcheck", # for checking shell scripts
    "linux-tools-common", # perf(1)
    "python3-uno", # soffice conversion
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
    "iotop", # the iotop command
    "jnettop", # the jnettop command
    "smartmontools",
    "gsmartcontrol",
    "lm-sensors",
    "inxi",
    "nmap",
    # doesn"t exist on 22.04
    # "hddtemp",
    "htop",
    "btop",
    "glances",
    "sysprof",
    # "mutrace",
    "cpulimit", # the cpulimit command
    "bridge-utils", # bridging utilities to demo creation of a bridge
    "iptraf-ng",
    "numactl",
    "pstack",
    "x86info",
    "lsscsi",
    "chrpath",
    "latencytop",
    "devmem2",
    "elfutils", # manipulating elf files
    "pax-utils",
    # "paxctl",
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
    # "mytop",
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
    "libreoffice-common", # soffice conversion
    "xutils-dev", # makedepend(1)
    "astyle", # indent(1) (code formatting)

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

    # openmpi
    "openmpi-bin",

    # tracing
    "strace",
    "ltrace",
]

packages=[
    # packages needed for the build
    "ccache",
    "uncrustify", # uncrustify(1) (code formatting)
    "indent", # indent(1) (code formatting)
    "electric-fence",

    # dialog really has header files and that is why it is here
    "dialog",
    "gcc-9-plugin-dev",
    "binutils-dev",
    "libxtables-dev",
    "libevent-dev",
    f"libevent-{libevent_ver}",
    "libiberty-dev",
    f"libncurses{libncurses_ver}",
    "libncurses-dev",
    f"libncursesw{libncurses_ver}",
    # f"libncursesw{libncurses_ver}-dev",
    # libprocps is gone
    # "libprocps8",
    # "libprocps-dev",
    "libsystemd0",
    "libsystemd-dev",
    "libsigc++-2.0-0v5",
    "libsigc++-2.0-dev",
    "libgtkmm-2.4-dev",
    "libgtkmm-3.0-dev",
    "libpq-dev",
    "liblog4cpp5-dev",
    "libmysqlclient-dev",
    lib_wx,
    "libmysql++-dev",
    "libsdl1.2-dev",
    "libace-dev",
    f"libboost{ver_boost_short}-dev",
    f"libboost-thread{ver_boost}",
    f"libboost-system{ver_boost}",
    "libpcap-dev",
    "libasound2",
    "libasound2-dev",
    "libdmalloc5",
    "libdmalloc-dev",
    "libcpufreq-dev", # cpufreq.h
    "libnetfilter-queue1",
    "libnetfilter-queue-dev",
    "libcap-dev", # capability.h
    f"liburcu{ver_urcu}", # rcu library
    "liburcu-dev", # rcu library
    f"libunwind{ver_unwind}", # unwind library
    "libunwind-setjmp0", # unwind library
    "libunwind-dev", # unwind library
    "libunwind-setjmp0-dev", # unwind library
    "libelf1", # reading elf files
    "libdw1",
    "libdw-dev",
    "libasm1",
    "libasm-dev",
    "libaspell-dev",
    "libacl1-dev",
    "libattr1-dev",
    "libdaemon-dev",
    "libsystemd0",
    "libpapi-dev",
    f"libpapi{ver_papi}",
    "libpopt-dev",
    "systemtap-sdt-dev",
    "liburing-dev",
    # "libffi7",
    "libffi8",
    "libffi-dev",
    "libopenmpi3",
    "libopenmpi-dev",
    "liburing2",
    "liburing-dev",
    # qemu stuff
    "qemu-system-x86",
    "virt-manager",
    "virtinst",
    "libvirt-clients",
    "bridge-utils",
    "libvirt-daemon-system",
    # qt stuff
    # this is for qt6
    "qt6-base-dev",
    # this is for qt5
    "qtbase5-dev",
    # linters
    "cppcheck",
    "cpplint",
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
        # systemtap - this one does not exist in the regular ubuntu archives
        # ubuntu does not always carry the "-lowlatency" dbgsym and that why
        # it is commented out...
        # TODO: make this script add sources to the dbgsyms if need be
        f"linux-image-{release_generic}-generic-dbgsym",
        f"linux-image-{release_lowlatency}-lowlatency-dbgsym",
    ])

if opt_do_compilers:
    packages.extend([
        "gcc",
        "gcc-multilib",
        "clang",
    ])
    packages_doc.extend([
        "gcc-doc",
        "cpp-doc",
    ])

packages.extend(packages_tools)
