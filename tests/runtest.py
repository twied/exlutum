#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright 2020 Tim Wiederhake

import argparse
import os
import subprocess
import yaml


FAIL = "\033[1;31m FAIL \033[1;m"
PASS = "\033[1;32m PASS \033[1;m"


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "program",
        help="Path to test program.",
        type=os.path.realpath)

    parser.add_argument(
        "test",
        help="Path to test file.",
        type=os.path.realpath)

    return parser.parse_args()


def compare_int(fixture, result, attribute):
    if attribute not in fixture:
        return

    expected = fixture[attribute]
    found = getattr(result, attribute)

    if expected == found:
        return

    return attribute, expected, found


def compare_string(fixture, result, attribute):
    if attribute not in fixture:
        return

    expected = b" ".join(str.encode(fixture[attribute]).split())
    found = b" ".join(getattr(result, attribute).split())

    if expected == found:
        return

    return attribute, expected, found


def main():
    args = parse_args()

    with open(args.test, "rt") as f:
        fixture = yaml.safe_load(f)

    cmd = [args.program]
    cmd.extend(fixture.get("arguments", []))

    p = subprocess.run(
        cmd,
        input=str.encode(fixture.get("stdin", "")),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE)

    failures = [
        compare_int(fixture, p, "returncode"),
        compare_string(fixture, p, "stdout"),
        compare_string(fixture, p, "stderr")]
    failures = [f for f in failures if f is not None]

    print("[%s] %s %s" % (
        FAIL if failures else PASS,
        os.path.basename(args.program),
        os.path.basename(args.test)))

    for (attribute, expected, found) in failures:
        print("\t%s: expected %s, found %s" % (
            attribute,
            repr(expected),
            repr(found)))

    exit(1 if failures else 0)


if __name__ == "__main__":
    main()
