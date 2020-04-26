#!/usr/bin/python3

'''
This script wraps the running of the compiler with the right flags.
You must pass the compiler and the flags to it.
It will scan the source file in order to find specific flags
to be added to the compilation or linkage.
'''
import sys # for exit, argv
import subprocess # for Popen, PIPE, stderr, stdout

#############
# functions #
#############
def system_check_output(cmd, input=None, cwd=None, env=None):
    pipe=subprocess.Popen(cmd, shell=True, cwd=cwd, env=env, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (out_stdout,out_stderr)=pipe.communicate(input=input)
    status=pipe.returncode
    if status:
        #raise ValueError('error in executing',cmd)
        print('could not run [%s]' % (cmd), file=sys.stderr)
        print('error follows...', file=sys.stderr)
        print(out_stderr.decode(), file=sys.stderr)
        print('output follows...', file=sys.stderr)
        print(out_stdout.decode(), file=sys.stderr)
        sys.exit(status)
    return out_stdout.decode()

# start of code
script=sys.argv.pop(0)
doDebug=int(sys.argv.pop(0))
ccache=int(sys.argv.pop(0))
link=int(sys.argv.pop(0))
source=sys.argv.pop(0)
target=sys.argv.pop(0)
args=sys.argv

# substitutions

if doDebug:
    print('script is',script)
    print('source is',source)
    print('target is',target)
subs=dict()
subs['arch']=subprocess.check_output(['arch']).decode().rstrip()
subs['deb_build_arch']=subprocess.check_output([
    'dpkg-architecture',
    '-qDEB_BUILD_ARCH',
]).decode().rstrip()
subs['deb_build_gnu_cpu']=subprocess.check_output([
    'dpkg-architecture',
    '-qDEB_BUILD_GNU_CPU',
]).decode().rstrip()
subs['deb_build_multiarch']=subprocess.check_output([
    'dpkg-architecture',
    '-qDEB_BUILD_MULTIARCH',
]).decode().rstrip()
subs['source']=source
subs['target']=target
# scan the source code
for line in open(source):
    line=line.strip()
    if link:
        f=line.find('EXTRA_LINK_CMDS=')
        if f!=-1:
            f=line.find('EXTRA_LINK_CMDS=')+len('EXTRA_LINK_CMDS=')
            cmd=line[f:]
            cmd=cmd.format(**subs)
            cmd=cmd.split()
            out=subprocess.check_output(cmd).decode()
            out=out.split()
            args.extend(out)
        f=line.find('EXTRA_LINK_FLAGS=')
        if f!=-1:
            f=line.find('EXTRA_LINK_FLAGS=')+len('EXTRA_LINK_FLAGS=')
            cmd=line[f:]
            cmd=cmd.format(**subs)
            cmd=cmd.split()
            args.extend(cmd)
        f=line.find('LINKER=')
        if f!=-1:
            f=line.find('LINKER=')+len('LINKER=')
            args[0]=line[f:]
    else:
        f=line.find('EXTRA_COMPILE_CMDS=')
        if f!=-1:
            f=line.find('EXTRA_COMPILE_CMDS=')+len('EXTRA_COMPILE_CMDS=')
            cmd=line[f:]
            cmd=cmd.format(**subs)
            out=system_check_output(cmd)
            if doDebug:
                print('out is',out)
            out=out.split()
            args.extend(out)
        f=line.find('EXTRA_COMPILE_FLAGS=')
        if f!=-1:
            f=line.find('EXTRA_COMPILE_FLAGS=')+len('EXTRA_COMPILE_FLAGS=')
            cmd=line[f:]
            cmd=cmd.format(**subs)
            args[1:1]=cmd.split()
        f=line.find('EXTRA_COMPILE_FLAGS_AFTER=')
        if f!=-1:
            f=line.find('EXTRA_COMPILE_FLAGS_AFTER=')+len('EXTRA_COMPILE_FLAGS_AFTER=')
            cmd=line[f:]
            cmd=cmd.format(**subs)
            args.extend(cmd.split())
        f=line.find('COMPILER=')
        if f!=-1:
            f=line.find('COMPILER=')+len('COMPILER=')
            args[0]=line[f:]
if ccache and not link:
    args.insert(0,'ccache')
if doDebug:
    print('running',args)
try:
    subprocess.check_call(args)
except Exception as e:
    #print('e is',e,file=sys.stderr)
    #print(dir(e),file=sys.stderr)
    #print(e.returncode,file=sys.stderr)
    print(e)
    # fix the following line
    #sys.exit(e.returncode)
    sys.exit(1)
