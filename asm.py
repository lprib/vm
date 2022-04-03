#!/usr/bin/env python3

import sys
import argparse
from enum import Enum, auto

# Always assume 16 bit width in assembler
PEEK_BITMASK = 1 << 15

def parse_schema(filename):
    """ Generates map: name -> (opcode_value, num_args) """
    print(f"Reading schema from {filename}");
    opcodeSchema = {}
    try:
        with open(filename) as f:
            for i, line in enumerate(f.readlines()):
                split = line.split(",")
                opcodeSchema[split[0].lower()] = (i, int(split[1]))
    except:
        print(f"{filename} could not be opened")
        sys.exit(0)
    return opcodeSchema

def parse_int(lineno, arg):
    try:
        return int(arg)
    except:
        print(f"{lineno}: expected integer, got {arg}")
        sys.exit(1)

def get_arg_value(lineno, arg_str):
    try:
        return int(arg_str)
    except:
        return arg_str, lineno

def do_pp_directive(lineno, directive, args, program):
    if directive == "#zeros":
        size = parse_int(lineno, args[0])
        program.extend([0] * size)
    elif directive == "#data":
        program.extend(get_arg_value(lineno, arg) for arg in args)
    else:
        print(f"{lineno}: unknown PP directive {directive}")

def first_pass(filename, schema):
    """ First pass, with label refs generated as placeholder tuple (name, source_lineno).
        Returns program, label_table. label_table is map name -> index
    """
    program = []
    label_table = {}
    try:
        with open(filename) as f:
            for lineno, line in enumerate(f.readlines()):
                line = line.strip()
                parts = line.split()
                raw_opcode = parts[0]
                opcode = raw_opcode.lower()
                if opcode.startswith("#"):
                    do_pp_directive(lineno, opcode, parts[1:], program)
                elif opcode.startswith(":"):
                    label_name = opcode.lstrip(":")
                    label_table[label_name] = len(program)
                else:
                    peek_mask = 0
                    if opcode.startswith("@"):
                        opcode = opcode.lstrip("@")
                        peek_mask = PEEK_BITMASK

                    if opcode not in schema:
                        print(f"{lineno}: unknown opcode {raw_opcode}")
                        sys.exit(1)
                    num_args = len(parts) - 1
                    if schema[opcode][1] != num_args:
                        print(f"{lineno}: opcode {raw_opcode} expected {schema[opcode][1]} args, got {num_args}")
                        # TODO don't exit early?
                        sys.exit(1)

                    program.append(schema[opcode][0] | peek_mask)
                    program.extend(get_arg_value(lineno, arg_str) for arg_str in parts[1:])
    except Exception as e:
        print(e)
        print(f"{filename} could not be opened")
        sys.exit(0)

    return program, label_table

def second_pass(outfile, program, label_table):
    with open(outfile, "wb") as f:
        for n in program:
            to_write = 0
            if type(n) is tuple:
                # label
                label_name = n[0]
                label_lineno = n[1]
                if label_name in label_table:
                    to_write = label_table[label_name]
                else:
                    print(f"{label_lineno}: undefined label {label_name}")
            else:
                to_write = n
            f.write(to_write.to_bytes(2, byteorder="little"))


def main():
    parser = argparse.ArgumentParser(description="VM bytecode assembler")
    parser.add_argument("-s", "--schema", metavar="FILE", dest="schema", default="./out/opcode_schema.csv", help="opcode schema csv file")
    parser.add_argument("-o", "--output", metavar="FILE", dest="output", required=True, help="output file")
    parser.add_argument('input_file', metavar="FILE", help="input file")

    args = parser.parse_args()
    schema = parse_schema(args.schema)
    print(schema)
    program, label_table = first_pass(args.input_file, schema)
    print(label_table)
    print(program)
    second_pass(args.output, program, label_table)


if __name__ == "__main__": main()
