
# user enviorn var: MPICXX and MPICC
# or, use the definition in file $CHARMINC/MPIOPTS
MPICXX_DEF=mpicxx
MPICC_DEF=mpicc

test "$MPICXX" != "$MPICXX_DEF" && /bin/rm -f $CHARMINC/MPIOPTS
if test ! -f "$CHARMINC/MPIOPTS"
then
  echo MPICXX_DEF=$MPICXX > $CHARMINC/MPIOPTS
  echo MPICC_DEF=$MPICC >> $CHARMINC/MPIOPTS
  chmod +x $CHARMINC/MPIOPTS
fi

CMK_REAL_COMPILER=`$MPICXX -show 2>/dev/null | cut -d' ' -f1 `
case "$CMK_REAL_COMPILER" in
g++)   CMK_AMD64="-m64 -fPIC" ;;
icpc)  CMK_AMD64="-m64";;
pgCC)  CMK_AMD64="-DCMK_FIND_FIRST_OF_PREDICATE=1 --no_using_std " ;;
FCC)   CMK_AMD64="-Kfast -DCMK_FIND_FIRST_OF_PREDICATE=1 --variadic_macros";;
esac
CMK_REAL_C_COMPILER=`$MPICC -show 2>/dev/null | cut -d' ' -f1 `



CMK_CPP_CHARM="cpp -P"
CMK_CPP_C="$MPICC -E"
CMK_CC="$MPICC $CMK_AMD64 "
CMK_CXX="$MPICXX $CMK_AMD64 "
CMK_CXXPP="$MPICXX -E $CMK_AMD64 "
CMK_LD="$CMK_CC "
CMK_LDXX="$CMK_CXX "

CMK_NATIVE_CC="$CMK_REAL_C_COMPILER $CMK_AMD64 "
CMK_NATIVE_LD="$CMK_REAL_C_COMPILER $CMK_AMD64 "
CMK_NATIVE_CXX="$CMK_REAL_COMPILER $CMK_AMD64 "
CMK_NATIVE_LDXX="$CMK_REAL_COMPILER $CMK_AMD64 "
CMK_NATIVE_LIBS=""

case "$CMK_REAL_COMPILER" in
FCC) CMK_NATIVE_LD="$CMK_REAL_C_COMPILER";;
esac



# fortran compiler 
# for Intel Fortran compiler 8.0 and higher which is renamed to ifort from ifc
# does not work for ifc 7.0
CMK_CF77="mpif77 -auto -fPIC "
CMK_CF90="mpif90 -auto -fPIC "
CMK_CF90_FIXED="$CMK_CF90 -132 -FI "
F90DIR=`which ifort 2> /dev/null`
if test -h "$F90DIR"
then
  F90DIR=`readlink $F90DIR`
fi
if test -x "$F90DIR"
then
  F90LIBDIR="`dirname $F90DIR`/../lib"
  F90MAIN="$F90LIBDIR/for_main.o"
fi
# for_main.o is important for main() in f90 code
CMK_F90MAINLIBS="$F90MAIN "
CMK_F90LIBS="-L$F90LIBDIR -lifcore -lifport -lifcore "
CMK_F77LIBS="$CMK_F90LIBS"

CMK_F90_USE_MODDIR=""


