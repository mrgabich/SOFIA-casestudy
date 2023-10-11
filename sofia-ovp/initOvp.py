#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from argparse import ArgumentParser
import os
import subprocess

def main():

    parser = ArgumentParser()

    parser.add_argument(
        '-l',
        '--license',
        action='store',
        dest='license',
        required=True,
        help="License origin")

    parser.add_argument(
        '-p',
        '--path',
        action='store',
        dest='path',
        required=True,
        help="Path to Imperas/OVP")

    parser.add_argument(
        '-v',
        '--version',
        action='store',
        dest='version',
        required=True,
        help="Imperas version")

    args = parser.parse_args()

    with open('ovp.sh', 'w+') as ovpFile:
        ovpFile.write("#!/bin/bash\n")
        ovpFile.write("export IMPERASD_LICENSE_FILE=2700@" + args.license + "\n")
        ovpFile.write(
            "source "+ args.path +"Imperas." +
            args.version +
            "/bin/setup.sh\n")
        ovpFile.write("setupImperas "+ args.path +"Imperas." + args.version + "\n")
        ovpFile.write("eval \"$@\"\n")

    subprocess.call(['chmod', '+x', 'ovp.sh'])

if __name__ == "__main__":
    main()
