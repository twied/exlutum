#!/bin/sh

set -e -u

EXLUTUM_HEX2BIN="${EXLUTUM_HEX2BIN:-hex2bin}"
EXLUTUM_LABEL2HEX="${EXLUTUM_LABEL2HEX:-label2hex}"
EXLUTUM_MACRO2LABEL="${EXLUTUM_MACRO2LABEL:-macro2label}"
EXLUTUM_ARABILIS="${EXLUTUM_ARABILIS:-arabilis}"


src="$(dirname "${0}")"
comp_hex2bin="../src/${EXLUTUM_HEX2BIN}/${EXLUTUM_HEX2BIN}"
comp_label2hex="../src/${EXLUTUM_LABEL2HEX}/${EXLUTUM_LABEL2HEX}"
comp_macro2label="../src/${EXLUTUM_MACRO2LABEL}/${EXLUTUM_MACRO2LABEL}"
comp_arabilis2label="../src/${EXLUTUM_ARABILIS}/${EXLUTUM_ARABILIS}"


compare() {
    local testname="${1}"
    local actual_code="${2}"
    local actual_output="${3}"
    local expect_code="${4}"
    local expect_output="${5}"

    if [ "${expect_code}" != "${actual_code}" ]
    then
        echo "${testname}: wrong return code, got ${actual_code} instead of ${expect_code}"
        exit 1
    fi

    if [ "${expect_output}" != "${actual_output}" ]
    then
        echo "${testname}: wrong output, got \"${actual_output}\" instead of \"${expect_output}\""
        exit 1
    fi
}


test_hex() {
    ( "${comp_hex2bin}" ) \
        < "${src}/exit42.hex" \
        > "exit42"
    chmod +x exit42

    if output="$(./exit42)"
    then
        retcode="0"
    else
        retcode="${?}"
    fi

    compare hex "${retcode}" "${output}" 42 ""
}


test_label() {
    ( "${comp_label2hex}" | "${comp_hex2bin}" ) \
        < "${src}/hello_world.label" \
        > "hello_world"
    chmod +x hello_world
        
    if output="$(./hello_world)"
    then
        retcode="0"
    else
        retcode="${?}"
    fi

    compare label "${retcode}" "${output}" 0 "Hello World!"
}


test_macro() {
    ( "${comp_macro2label}" | "${comp_label2hex}" | "${comp_hex2bin}" ) \
        < "${src}/hello_macro.macro" \
        > "hello_macro"
    chmod +x hello_macro

    if output="$(./hello_macro)"
    then
        retcode="0"
    else
        retcode="${?}"
    fi

    compare macro "${retcode}" "${output}" 0 "Hello Macro!"
}


test_arabilis() {
    ( "${comp_arabilis2label}" | "${comp_macro2label}" | "${comp_label2hex}" | "${comp_hex2bin}" ) \
        < "${src}/fizzbuzz.arabilis" \
        > "fizzbuzz"
    chmod +x fizzbuzz

    if output="$(./fizzbuzz)"
    then
        retcode="0"
    else
        retcode="${?}"
    fi

    compare \
        arabilis \
        "${retcode}" \
        "${output}" \
        0 \
        "1 2 Fizz 4 Buzz Fizz 7 8 Fizz Buzz 11 Fizz 13 14 `
            `FizzBuzz 16 17 Fizz 19 " 
}


test_hex
test_label
test_macro
test_arabilis
