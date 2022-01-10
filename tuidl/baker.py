#! /usr/bin/env python

import os
import os.path

WIN32=False
MACOSX=False
LINUX=True

PROGRAMS = ("../tuidlgui",)
FLAGS="-I.. -g"

flist = os.listdir(".")

def has_suffix(filename,suffix):
    if filename.rfind(suffix)==-1:
        return False
    return filename.rfind(suffix)+len(suffix)==len(filename)

def get_headers(fname):
    f = open(fname)
    lines = f.read().splitlines()
    for line in lines:
        if line.find("#include")==0:
            to_include = line.split()[1]
            if to_include[0]=='"':
                if os.path.exists(to_include[1:len(to_include)-1]):
                    print to_include[1:len(to_include)-1]+" / "+fname
                elif os.path.exists("../"+to_include[1:len(to_include)-1]):
                    print "../"+to_include[1:len(to_include)-1]+" / "+fname
                else:
                    print to_include[1:len(to_include)-1]+" / "+fname
                print fname+" touch tuidl/"+fname

common_objects = []
for fname in flist:
    if has_suffix(fname,".y"):
        base_name = fname[:len(fname)-2]
        get_headers(fname)
        print base_name+".y / "+base_name+".tab.c"
        print base_name+".y / "+base_name+".h"
        if LINUX:
            print base_name+".tab.c / "+base_name+".tab.o"
        if WIN32:
            print base_name+".tab.c / "+base_name+".tab.obj"
        if MACOSX:
            print base_name+".tab.c / "+base_name+".tab.macho"
        print base_name+".h sh -c \"cd tuidl; bison --defines="+base_name+".h "+base_name+".y\""
        print base_name+".tab.c sh -c \"cd tuidl; bison --defines="+base_name+".h "+base_name+".y\""
        if LINUX:
            print base_name+".tab.o sh -c \"cd tuidl; g++ -std=gnu++1y -c "+FLAGS+" "+base_name+".tab.c\""
        if WIN32:
            print base_name+".tab.obj /opt/mxe/usr/bin/i686-w64-mingw32.static-g++ -std=gnu++1y -static -c "+FLAGS+" tuidl/"+base_name+".tab.c -o "+base_name+".tab.obj"
        if MACOSX:
            print base_name+'.tab.macho sh -c "source ./mac.source; o64-g++ -I/opt/osxcross/target/macports/pkgs/opt/local/include -std=gnu++1y -c '+FLAGS+' tuidl/'+base_name+'.tab.c -o '+base_name+'.tab.macho"'
        common_objects.append(base_name+".tab")
        for target in PROGRAMS:
            if LINUX:
                print base_name+".tab.o / "+target
            if WIN32:
                print base_name+".tab.obj / "+target+"_win32.exe"
            if MACOSX:
                print base_name+".tab.macho / "+target+"_mac"
    elif has_suffix(fname,".l"):
        base_name = fname[:len(fname)-2]
        get_headers(fname)
        print base_name+".l / lex.yy.c"
        print "lex.yy.c sh -c \"cd tuidl; flex "+base_name+".l\""
        if LINUX:
            print "lex.yy.c / lex.yy.o"
        if WIN32:
            print "lex.yy.c / lex.yy.obj"
        if MACOSX:
            print "lex.yy.c / lex.yy.macho"
        if LINUX:
            print "lex.yy.o sh -c \"cd tuidl; g++ -std=gnu++1y -c "+FLAGS+" lex.yy.c\""
        if WIN32:
            print "lex.yy.obj sh -c \"cd tuidl; /opt/mxe/usr/bin/i686-w64-mingw32.static-g++ -std=gnu++1y -static -c "+FLAGS+" lex.yy.c -o lex.yy.obj\""
        if MACOSX:
            print 'lex.yy.macho sh -c "source ./mac.source; cd tuidl; o64-g++ -I/opt/osxcross/target/macports/pkgs/opt/local/include -std=gnu++1y -c '+FLAGS+' lex.yy.c -o lex.yy.macho"'
        common_objects.append("lex.yy")
        for target in PROGRAMS:
            if LINUX:
                print "lex.yy.o / "+target
            if WIN32:
                print "lex.yy.obj / "+target+"_win32.exe"
            if MACOSX:
                print "lex.yy.macho / "+target+"_mac"
    elif has_suffix(fname,".cpp"):
        base_name=fname[:len(fname)-4]
        get_headers(fname)
        if LINUX:
            print base_name+".cpp / "+base_name+".o"
        if WIN32:
            print base_name+".cpp / "+base_name+".obj"
        if MACOSX:
            print base_name+".cpp / "+base_name+".macho"
        if LINUX:
            print base_name+".o sh -c \"cd tuidl; g++ -std=gnu++1y -c "+FLAGS+" "+base_name+".cpp\""
        if WIN32:
            print base_name+".obj /opt/mxe/usr/bin/i686-w64-mingw32.static-g++ -std=gnu++1y -static -c "+FLAGS+" tuidl/"+base_name+".cpp -o "+base_name+".obj"
        if MACOSX:
            print base_name+'.macho sh -c "source ./mac.source; o64-g++ -I/opt/osxcross/target/macports/pkgs/opt/local/include -std=gnu++1y -c '+FLAGS+' tuidl/'+base_name+'.cpp -o '+base_name+'.macho"'
        if not has_suffix(fname,"_main.cpp"):
            common_objects.append(base_name)
            for target in PROGRAMS:
                if LINUX:
                    print base_name+".o / "+target
                if WIN32:
                    print base_name+".obj / "+target+"_win32.exe"
                if MACOSX:
                    print base_name+".macho / "+target+"_mac"
        else:
            if LINUX:
                print base_name+".o / ../"+fname[:fname.index("_")]
            if WIN32:
                print base_name+".obj / "+fname[:fname.index("_")]+"_win32.exe"
            if MACOSX:
                print base_name+".macho / "+fname[:fname.index("_")]+"_mac"
    elif has_suffix(fname,".hpp") or has_suffix(fname,".h"):
        get_headers(fname)

for target in PROGRAMS:
    if LINUX:
        print target+' sh -c "cd tuidl; g++ -std=gnu++1y '+' '.join(map(lambda x: "tuidl/"+x+".o",common_objects))+' '+target+'_main.o -o '+target+'"'
    if WIN32:
        print target+'_win32.exe sh -c "/opt/mxe/usr/bin/i686-w64-mingw32.static-g++ -std=gnu++1y -static '+' '.join(map(lambda x: x+".obj",common_objects))+' '+target+'_main.obj /opt/mxe/usr/i686-w64-mingw32.static/lib/libfltk.a win32_bin/cyg*.dll -lws2_32 -lole32 -luuid -lcomctl32 -o '+target+'_win32.exe"'
    if MACOSX:
        print target+'_mac sh -c "source ./mac.source; o64-g++ -std=gnu++1y '+' '.join(map(lambda x: x+".macho",common_objects))+' '+target+'_main.macho -o '+target+'_mac"'
