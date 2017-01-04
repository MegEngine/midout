#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import sys

MAGIC = 'midout_trace v0\n'

class MidoutHeaderGen:
    _tag_names = None
    _region_names = None

    def __init__(self):
        self._tag_names = set()
        self._region_names = set()

    def add_item(self, name: str):
        prefix = 'midout::Region<midout::tags::'
        assert name.startswith(prefix), 'bad name: {}'.format(name)
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
            for line in fin:
                gen.add_item(line.strip())

    if not args.output:
        gen.write(sys.stdout)
    else:
        with open(args.output, 'w') as fout:
            gen.write(fout)

if __name__ == '__main__':
    main()
