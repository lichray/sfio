# This shell script attempts to find the actual lexical spelling
# of the following objects:
#	_iob : the traditional name of the stdio FILE structure
#	_filbuf: the traditional name of the function to fill a FILE buffer
#	_flsbuf: the traditional name of the function to flush a FILE buffer
#
#	_uflow: _filbuf on Linux
#	_overflow: _flsbuf on Linux
#
#	_sF, _srget, _swbuf, _sgetc, _sputc: BSD stuffs
#
# Written by Kiem-Phong Vo, 12/11/93

# Clean up on error or exit
trap "eval 'rm kpv.xxx.* >/dev/null 2>&1'" 0 1 2

# Take cross-compiler name as an argument
if test "$1" = ""
then	CC=cc
else	CC="$*"
fi

# clear the generated header file
>sfstdhdr.h

# Get full path name for stdio.h
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

# determine the right names for the given objects
for name in _iob _filbuf _flsbuf _uflow _overflow _sf _srget _swbuf _sgetc _sputc
do
	{ 
	  echo "#include <stdio.h>"
	  case $name in
	  _iob)		echo "kpvxxx: stdin;" ;;
	  _filbuf)	echo "kpvxxx: getc(stdin);" ;;
	  _flsbuf)	echo "kpvxxx: putc(0,stdout);" ;;
	  _uflow)	echo "kpvxxx: getc(stdin);" ;;
	  _overflow)	echo "kpvxxx: putc(0,stdout);" ;;
	  _sf)		echo "kpvxxx: stdin;" ;;
	  _srget)	echo "kpvxxx: getc(stdin);" ;;
	  _swbuf)	echo "kpvxxx: putc(0,stdout);" ;;
	  _sgetc)	echo "kpvxxx: getc(stdin);" ;;
	  _sputc)	echo "kpvxxx: putc(0,stdout);" ;;
	  esac
	} >kpv.xxx.c

	case $name in
	_iob)		pat='__*iob[a-zA-Z0-9_]*' ;;
	_filbuf)	pat='__*fi[a-zA-Z0-9_]*buf[a-zA-Z0-9_]*' ;;
	_flsbuf)	pat='__*fl[a-zA-Z0-9_]*buf[a-zA-Z0-9_]*' ;;
	_uflow)		pat='__*u[a-z]*flow[a-zA-Z0-9_]*' ;;
	_overflow)	pat='__*o[a-z]*flow[a-zA-Z0-9_]*' ;;
	_sf)		pat='__*s[fF][a-zA-Z0-9_]*' ;;
	_srget)		pat='__*sr[a-zA-Z0-9_]*' ;;
	_swbuf)		pat='__*sw[a-zA-Z0-9_]*' ;;
	_sgetc)		pat='__*sg[a-zA-Z0-9_]*' ;;
	_sputc)		pat='__*sp[a-zA-Z0-9_]*' ;;
	esac

	rm kpv.xxx.name >/dev/null 2>&1
	$CC -E kpv.xxx.c > kpv.xxx.cpp 2>/dev/null
	grep kpvxxx kpv.xxx.cpp | grep "$pat" > kpv.xxx.name
	if test "`cat kpv.xxx.name`" = ""
	then	echo "${name}_kpv" > kpv.xxx.name
	else
ed kpv.xxx.name >/dev/null 2>&1 <<!
s/$pat/::&/
s/.*:://
s/$pat/&::/
s/::.*//
w
!
	fi

	NAME="`cat kpv.xxx.name`"
	{ if test "$NAME" != "${name}_kpv"
	  then	echo ""
		echo "#define NAME$name	\"$NAME\""
		if test "$NAME" != "$name"
		then	echo "#undef $name"
			echo "#define $name	$NAME"
		fi
	  fi
	} >>sfstdhdr.h
done

# hack for BSDI
{ echo "#ifdef NAME_sgetc"
  echo "#undef NAME_sgetc"
  echo "#define NAME_srget	\"__srget\""
  echo "#endif"
  echo "#ifdef NAME_sputc"
  echo "#undef NAME_sputc"
  echo "#define NAME_swbuf	\"__swbuf\""
  echo "#endif"
} >>sfstdhdr.h

exit 0
