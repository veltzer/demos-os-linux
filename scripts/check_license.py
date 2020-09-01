#!/usr/bin/python3

"""
this script will check that all files that match
'*.c' '*.cc' '*.h' '*.hh' '*.S'
have, at their begining, the license specified in
'support/license.txt'
"""

import sys
import os


def main():
    if len(sys.argv)>1:
        raise ValueError('no parameters on the cmd line')

    f_new=open("support/license.txt")
    new_lic=f_new.read()
    f_new.close()

    suffixes=['.c','.cc','.h','.hh','.S']
    bad_suffixes=['.mod.c']
    bad_prefixes=['./.venv']
    error=False
    root_folder='.'
    for root,dirs,files in os.walk(root_folder):
        for file in files:
            current_file=os.path.join(root,file)
            doit=False
            for suf in suffixes:
                if current_file.endswith(suf):
                    doit=True
            for suf in bad_suffixes:
                if current_file.endswith(suf):
                    doit=False
            for pref in bad_prefixes:
                if current_file.startswith(pref):
                    doit=False
            if doit:
                #print('doing',current_file)
                with open(current_file) as f:
                    content=f.read()
                    if content.startswith(new_lic):
                        continue
                    # if first line is SPDX license (for kernel patches) then skip it
                    first_line = content.split("\n")[0]
                    # print(f"first_line is {first_line}")
                    rest = "\n".join(content.split("\n")[1:])
                    if first_line=='// SPDX-License-Identifier: GPL-2.0':
                        if rest.startswith(new_lic):
                            continue
                    print("bad license for",current_file)
                    error=True;
    if error:
        raise ValueError("had errors")


if __name__ == '__main__':
    main()
