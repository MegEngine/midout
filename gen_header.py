#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# MegEngine is Licensed under the Apache License, Version 2.0 (the "License")
#
# Copyright (c) 2014-2020 Megvii Inc. All rights reserved.
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT ARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

import argparse
import sys
import subprocess

MAGIC = 'midout_trace v1\n'

class MidoutHeaderGen:
    _tag_names = None
    _region_names = None

    def __init__(self):
        self._tag_names = set()
        self._region_names = set()

    def add_item(self, name: str):
        prefix = 'midout::Region<midout::tags::'
        assert name.startswith(prefix), 'bad name: {!r}'.format(name)
        comma = name.find(',', len(prefix))
        self._tag_names.add(name[len(prefix):comma])
        self._region_names.add(name)

    def write(self, fout):
        print('#define MIDOUT_GENERATED \\', file=fout)
        print('namespace midout { namespace tags { \\', file=fout)
        for i in sorted(self._tag_names):
            print('class {}; \\'.format(i), file=fout)
        print('} \\', file=fout)

        for i in self._region_names:
            i = i.replace('midout::', '')
            i = i.replace('__ndk1::', '')
            print('template<> \\', file=fout)
            print('struct {} {{ static constexpr bool enable = true; }}; \\'.
                  format(i), file=fout)

        print('}', file=fout)


def main():
    parser = argparse.ArgumentParser(
        description='generate header file from midout traces',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-o', '--output',
                        help='output header file; default to stdout')
    parser.add_argument('inputs', nargs='+', help='input trace files')
    args = parser.parse_args()
    gen = MidoutHeaderGen()
    for i in args.inputs:
        with open(i) as fin:
            assert fin.read(len(MAGIC)) == MAGIC, 'bad trace file'
            demangle = subprocess.check_output(
                ['c++filt', '-t'], input='\n'.join(list(fin)).encode('utf-8'))
            for line in demangle.decode('utf-8').split('\n'):
                line = line.strip()
                if line:
                    gen.add_item(line)

    if not args.output:
        gen.write(sys.stdout)
    else:
        with open(args.output, 'w') as fout:
            gen.write(fout)

if __name__ == '__main__':
    main()
