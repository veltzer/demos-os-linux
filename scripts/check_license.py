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
                with open(current_file) as in_f:
                    f=in_f.read()
                    if not f.startswith(new_lic):
                        error=True;
                        print("bad license for",current_file)
    if error:
        raise ValueError("had errors")


if __name__ == '__main__':
    main()
