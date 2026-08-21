-- os level dependencies
--
-- package versions match the CI runner (ubuntu-24.04, VERSION_ID "24.04");
-- the old deps.py derived them from /etc/os-release
-- the old deps.py option flags were resolved statically:
--     opt_do_kernel=false (no kernel images/headers)
--     opt_do_ddebs=false (no kernel dbgsym packages)
--     opt_do_compilers=true (gcc, clang included)
--     opt_do_kernel_tools=false (no device-tree-compiler/util-linux extras)

-- append every element of "src" onto "dst"
local function extend(dst, src)
    for _, value in ipairs(src) do
        table.insert(dst, value)
    end
    return dst
end

PACKAGES_KERNELS = {
}
PACKAGES_DOC = {
    "ncurses-doc", -- ncurses documentation
    "binutils-doc", -- binutils documentation
    "libasound2-doc",
    -- "libgnomeui-doc",
    "libsigc++-2.0-doc",
    "libgtkmm-2.4-doc",
    "libgtkmm-3.0-doc",
    "libstdc++-9-doc",
    "aspell-doc",
    -- libc
    "glibc-doc",
    "glibc-doc-reference",
    "scons-doc",
    -- "jlint-doc",
    "make-doc",
    "systemtap-doc",
    "postgresql-doc",
    "papi-examples", -- PAPI example files and test programs
    "libx11-doc",
    -- compilers (opt_do_compilers)
    "gcc-doc",
    "cpp-doc",
}

PACKAGES_TOOLS = {
    -- ruby stuff
    "ruby-bundler",
    "rbenv",
    -- for spelling
    "aspell",
    -- debugging
    "cgdb",
    "qtcreator",
    -- code measurements
    "sloccount",
    "cloc",
    -- collection of command line tools
    "bikeshed",
    -- manual pages
    "manpages",
    "manpages-dev",
    "manpages-posix",
    "manpages-posix-dev",
    -- tools for building
    "gnulib",
    -- tool for converting documents from one format to another
    "unoconv",
    "cpufrequtils",
    "cpu-checker",
    "netperf",
    -- tools
    "shellcheck", -- for checking shell scripts
    "linux-tools-common", -- perf(1)
    "python3-uno", -- soffice conversion
    -- "vnstat", -- causes performance problems
    -- "vnstati", -- causes performance problems
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
    "fdutils", -- floppy disk utilities ?!?
    "iotop", -- the iotop command
    "jnettop", -- the jnettop command
    "smartmontools",
    "gsmartcontrol",
    "lm-sensors",
    "inxi",
    "nmap",
    "cgroup-tools",
    -- doesn"t exist on 22.04
    -- "hddtemp",
    "htop",
    "btop",
    "glances",
    "sysprof",
    -- "mutrace",
    "cpulimit", -- the cpulimit command
    "bridge-utils", -- bridging utilities to demo creation of a bridge
    "iptraf-ng",
    "numactl",
    -- "pstack", -- gone
    "x86info",
    "lsscsi",
    "chrpath",
    -- "latencytop", -- gone
    "devmem2",
    "elfutils", -- manipulating elf files
    "pax-utils",
    -- "paxctl",
    -- "execstack", -- gone 25.10
    -- "prelink", -- gone 25.10
    "cpuid",
    "sysstat",
    "nmon",
    "saidar",
    -- "sysdig",
    "iperf",
    "trace-cmd",
    "kernelshark",
    "smem",
    "sysbench",
    "gnome-system-monitor",
    "xfce4-taskmanager",
    -- "mrtg", -- has errors
    "isag",
    "sdparm",
    -- "mytop",
    "cutils",
    "hlint",
    "dlint",
    -- "oprofile", -- gone, 25.10
    "powertop",
    "rt-tests",
    "procinfo",
    "wavemon",

    -- fun stuff
    "figlet",
    "sl",

    -- tools used by this package
    "libreoffice-common", -- soffice conversion
    "xutils-dev", -- makedepend(1)
    "astyle", -- indent(1) (code formatting)

    -- these packages are for the developers pleasure...:)
    "vim",
    "tofrodos",
    "pipemeter",
    "blktool",
    "scons",
    "doxygen",
    "make",

    -- memory testing tools
    "memtester",
    "memtest86+",

    -- papi
    "papi-tools", -- PAPI utilities

    -- papi

    -- kernel debugging
    "systemtap",
    "systemtap-common",
    "systemtap-runtime",

    -- kexec and kernel crash debugging
    "crash",
    "kdump-tools",
    "kexec-tools",
    "linux-crashdump",
    "makedumpfile",

    -- databases
    "postgresql-client",
    "postgresql-client-common",

    -- watchdogs
    "rtkit",
    "watchdog",
    "supervisor",
    "daemontools",
    -- "monit",

    -- firewalls
    "ufw",
    "shorewall",

    -- javascript
    "nodejs",
    "npm",

    -- openmpi
    "openmpi-bin",

    -- tracing
    "strace",
    "ltrace",
}

PACKAGES = {
    -- packages needed for the build
    "ccache",
    "uncrustify", -- uncrustify(1) (code formatting)
    "indent", -- indent(1) (code formatting)
    "electric-fence",

    -- gcc plugins
    -- "gcc-15-plugin-dev", -- cant install on github
    -- dialog really has header files and that is why it is here
    "dialog",
    "binutils-dev",
    "libxtables-dev",
    "libevent-dev",
    "libevent-2.1-7t64",
    "libiberty-dev",
    "libncurses6",
    "libncurses-dev",
    "libncursesw6",
    -- "libncursesw6-dev",
    -- libprocps is gone
    -- "libprocps8",
    -- "libprocps-dev",
    "libsystemd0",
    "libsystemd-dev",
    "libsigc++-2.0-0v5",
    "libsigc++-2.0-dev",
    "libgtkmm-2.4-dev",
    "libgtkmm-3.0-dev",
    "libpq-dev",
    "liblog4cpp5-dev",
    "libmysqlclient-dev",
    "libwxgtk3.2-dev",
    "libmysql++-dev",
    "libsdl1.2-dev",
    "libace-dev",
    "libboost1.83-dev",
    "libboost-thread1.83.0",
    "libboost-system1.83.0",
    "libboost-all-dev",
    "libpcap-dev",
    -- asound lib
    "libasound2t64",
    "libasound2-dev",
    "libdmalloc5",
    "libdmalloc-dev",
    "libcpufreq-dev", -- cpufreq.h
    -- netfilter library
    "libnetfilter-queue1",
    "libnetfilter-queue-dev",
    -- capability.h
    "libcap-dev",
    -- rcu library
    "liburcu8t64",
    "liburcu-dev",
    "libcds2.3.3t64",
    "libcds-dev",
    "libunwind8", -- unwind library
    "libunwind-setjmp0", -- unwind library
    "libunwind-dev", -- unwind library
    "libunwind-setjmp0-dev", -- unwind library
    "libelf1t64", -- reading elf files
    -- dw lib
    "libdw1t64",
    "libdw-dev",
    -- asm lib
    "libasm1t64",
    "libasm-dev",
    "libaspell-dev",
    "libacl1-dev",
    "libattr1-dev",
    "libdaemon-dev",
    "libsystemd0",
    "libpapi-dev",
    "libpapi7.1t64",
    "libpopt-dev",
    "systemtap-sdt-dev",
    "liburing-dev",
    -- "libffi7",
    "libffi8",
    "libffi-dev",
    "libopenmpi3t64",
    "libopenmpi-dev",
    "liburing2",
    "liburing-dev",
    -- qemu stuff
    "qemu-system-x86",
    "virt-manager",
    "virtinst",
    "libvirt-clients",
    "bridge-utils",
    "libvirt-daemon-system",
    -- qt stuff
    -- this is for qt6
    "qt6-base-dev",
    -- this is for qt5
    "qtbase5-dev",
    -- linters
    "cppcheck",
    "cpplint",
    "clang-tidy",
    -- X11 and OpenGL headers
    "libx11-dev",
    "libxrandr-dev",
    "libgl-dev",
    "libglu1-mesa-dev",
    "libglx-dev",
    -- extra packages for this VERSION_ID (the old "add" list)
    "dialog",
    -- compilers (opt_do_compilers)
    "gcc",
    -- "gcc-multilib",
    "clang",
}
extend(PACKAGES, PACKAGES_TOOLS)
