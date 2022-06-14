#!/bin/sh

if [ ! -e "myfind" ]; then
    echo "myfind: No such file or directory"
    exit 1
else
    ./myfind src -print -name myfind? | tr ' ' '\n' > file1
    find src -print -name myfind? | tr ' ' '\n' > file2
    diff -u file1 file2
    res=$?
    if [ $res -eq 0 ]; then
        echo "Assert ok"
        echo ""
    else
        echo "Test failed"
        echo ""
    fi

    ./myfind src -name test? | tr ' ' '\n' > file1
    find src -name test? | tr ' ' '\n' > file2
    diff -u file1 file2
    res=$?
    if [ $res -eq 0 ]; then
        echo "Assert ok"
        echo ""
    else
        echo "Test failed"
        echo ""
    fi

    ./myfind src -type l | tr ' ' '\n' > file1
    find src -type l | tr ' ' '\n' > file2
    diff -u file1 file2
    res=$?
    if [ $res -eq 0 ]; then
        echo "Assert ok"
        echo ""
    else
        echo "Test failed"
        echo ""
    fi

    ./myfind src -user acu | tr ' ' '\n' > file1
    find src -user acu | tr ' ' '\n' > file2
    diff -u file1 file2
    res=$?
    if [ $res -eq 0 ]; then
        echo "Assert ok"
        echo ""
    else
        echo "Test failed"
        echo ""
    fi

    ./myfind src -perm 644 | tr ' ' '\n' > file1
    find src -perm 644 | tr ' ' '\n' > file2
    diff -u file1 file2
    res=$?
    if [ $res -eq 0 ]; then
        echo "Assert ok"
        echo ""
    else
        echo "Test failed"
        echo ""
    fi

    ./myfind src -name myfind? | tr ' ' '\n' > file1
    find src -name myfind? | tr ' ' '\n' > file2
    diff -u file1 file2
    res=$?
    if [ $res -eq 0 ]; then
        echo "Assert ok"
        echo ""
    else
        echo "Test failed"
        echo ""
    fi

    ./myfind src -type d | tr ' ' '\n' > file1
    find src -type d | tr ' ' '\n' > file2
    diff -u file1 file2
    res=$?
    if [ $res -eq 0 ]; then
        echo "Assert ok"
        echo ""
    else
        echo "Test failed"
        echo ""
    fi

    ./myfind src -type f | tr ' ' '\n' > file1
    find src -type f | tr ' ' '\n' > file2
    diff -u file1 file2
    res=$?
    if [ $res -eq 0 ]; then
        echo "Assert ok"
        echo ""
    else
        echo "Test failed"
        echo ""
    fi
fi
