# This shell script attempts to find the actual lexical spelling
# of the following objects:
#	_iob : the traditional name of the stdio FILE structure
#	_filbuf: the traditional name of the function to fill a FILE buffer
#	_flsbuf: the traditional name of the function to flush a FILE buffer
# Written by Kiem-Phong Vo, 12/11/93

# The next three patterns should be defined so that they can
# match _iob, _filbuf and _flsbuf. There are variations on different
# systems as to what these are. If necessary, they should be changed.
IOB='__*iob[a-zA-Z0-9_]*'			# This should match _iob...
FILBUF='__*fi[a-zA-Z0-9_]*buf[a-zA-Z0-9_]*'	# This should match _fi...buf...
FLSBUF='__*fl[a-zA-Z0-9_]*buf[a-zA-Z0-9_]*'	# This should match _fl...buf...

# Clean up on error or exit
trap "eval 'rm kpv.xxx.* >/dev/null 2>&1'" 0 1 2

# Take cross-compiler name as an argument
if test "$1" = ""
then	CC=cc
else	CC=$1
fi

# Get full path name for stdio.h
rm sfstdhdr.h >/dev/null 2>&1

# make sure that the right stdio.h file will be included
echo "#include <stdio.h>" > kpv.xxx.c
$CC -E kpv.xxx.c > kpv.xxx.cpp 2>/dev/null

ed kpv.xxx.cpp >/dev/null 2>&1 <<!
$
/stdio.h"/p
.w sfstdhdr.h
E sfstdhdr.h
1
s/stdio.h.*/stdio.h/
s/.*"//
s/.*/#include "&"/
w
!

# determine the right names for _iob, _filbuf, and _flsbuf
for name in _iob _filbuf _flsbuf
do
echo "#include <stdio.h>" >kpv.xxx.c
echo "main() {" >>kpv.xxx.c
case $name in
_iob)	echo "stdin;" >>kpv.xxx.c
	pat=$IOB
	;;
_filbuf) echo "getc(stdin);" >>kpv.xxx.c
	pat=$FILBUF
	;;
_flsbuf) echo "putc(0,stdout);" >>kpv.xxx.c
	pat=$FLSBUF
	;;
esac
echo "}" >>kpv.xxx.c

rm kpv.xxx.name >/dev/null 2>&1
$CC -E kpv.xxx.c > kpv.xxx.cpp 2>/dev/null
echo "kpv" >> kpv.xxx.cpp
ed kpv.xxx.cpp >/dev/null 2>&1 <<!
$
?$pat?
s/$pat/====&/
s/.*====//
s/$pat/&====/
s/====.*//
.w kpv.xxx.name
!

NAME="`cat kpv.xxx.name`"
echo "#define NAME$name \"$NAME\""	>> sfstdhdr.h
if test "$NAME" != "$name"
then	echo "#ifdef $name"		>> sfstdhdr.h
	echo "#undef $name"		>> sfstdhdr.h
	echo "#endif"			>> sfstdhdr.h
	echo "#define $name	$NAME"	>> sfstdhdr.h
fi

done

exit 0
