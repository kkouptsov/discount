__tests=0
__passed=0
__failed=0
__title=

___COLS=60

title() {
    __title="$*"
    if [ "$VERBOSE" ]; then
	./echo "$__title"
    else
	./echo -n "$__title" \
'.................................................................' | ./cols $(( $___COLS + 4 ))
    fi
}


summary() {
    if [ -z "$VERBOSE" ]; then
	if [ $__failed -eq 0 ]; then
	    ./echo " OK"
	else
	    ./echo
	    ./echo "$1: $__tests tests; $__failed failed/$__passed passed"
	    ./echo
	fi
    fi
}


try_header() {
    testcase=`./echo -n "  $1" '........................................................' | ./cols $___COLS `
    __tests=`expr $__tests + 1`

    test "$VERBOSE" && ./echo -n "$testcase"
}



try() {
    unset FLAGS
    while [ "$1" ]; do
	case "$1" in
	-*) FLAGS="$FLAGS $1"
	    shift ;;
	*) break ;;
	esac
    done

    try_header "$1"

    case "$2" in
    -t*) Q=`./markdown $FLAGS "$2"` ;;
    *)   Q=`./echo "$2" | ./markdown $FLAGS` ;;
    esac

    if [ "$3" = "$Q" ]; then
	__passed=`expr $__passed + 1`
	test $VERBOSE && ./echo " ok"
    else
	__failed=`expr $__failed + 1`
	if [ -z  "$VERBOSE" ]; then
	    ./echo
	    ./echo "$1"
	fi
	./echo "source:"
	./echo "$2" | sed -e 's/^/	/'
	./echo "diff:"
	(./echo "$3"  >> $$.w
	./echo "$Q"  >> $$.g
	diff  $$.w $$.g ) | sed -e 's/^/	/'
	rm -f $$.w $$.g
	rc=1
    fi
}

match() {
    testcase=`./echo -n "  $1" '........................................................' | ./cols $___COLS`

    test $VERBOSE && ./echo -n "$testcase"

    if ./echo "$2" | ./markdown | grep "$3" >/dev/null; then
	test $VERBOSE && ./echo " ok"
    else
	if [ -z "$VERBOSE" ]; then
	    ./echo
	    ./echo "$testcase"
	fi
	rc=1
    fi
}
