#!/usr/bin/env python3

import sys
import argparse
from enum import Enum, auto
import traceback

# Always assume 16 bit width in assembler
PEEK_BITMASK = 1 << 15

def parse_opcode_schema(filename):
    """ Generates map: name -> (opcode_value, num_args) """
    print(f"Reading schema from {filename}");
    opcodeSchema = {}
    try:
        with open(filename) as f:
            for i, line in enumerate(f.readlines()):
                split = line.split(",")
                opcodeSchema[split[0].lower()] = (i, int(split[1]))
        return opcodeSchema
    except:
        print(f"{filename} could not be opened")
        sys.exit(0)

def parse_io_schema(filename):
    """ Generates list of function names from io fn schema """
    print(f"Reading io fn schema from {filename}")
    try:
        with open(filename) as f:
            return [line.strip() for line in f.readlines()]
    except:
        print(f"{filename} could not be opened")
        sys.exit(0)


def parse_int(lineno, arg):
    try:
        return int(arg)
    except:
        print(f"{lineno}: expected integer, got {arg}")
        sys.exit(1)

def get_arg_value(lineno, arg_str, io_schema):
    """
    Read an argument (label or int). If int, return int. If label, return
    (name, current_lineno)
    """
    try:
        return int(arg_str)
    except:
        if arg_str in io_schema:
            return io_schema.index(arg_str)
        else:
            return arg_str, lineno

def do_pp_directive(lineno, directive, args, program, full_line, io_schema):
    if directive == "#zeros":
        size = parse_int(lineno, args[0])
        program.extend([0] * size)
    elif directive == "#words":
        program.extend(get_arg_value(lineno, arg, io_schema) for arg in args)
    elif directive == "#string":
        assert full_line.startswith("#string "), "bad string directive"
        string = full_line.lstrip("#string ")
        chars = bytes(string, "utf-8").decode("unicode_escape")
        program.extend(ord(n) for n in chars)
    else:
        print(f"{lineno}: unknown PP directive {directive}")

def first_pass(filename, schema, io_schema):
    """ First pass, with label refs generated as placeholder tuple (name, source_lineno).
        Returns program, label_table. label_table is map name -> index
    """
    program = []
    label_table = {}
    try:
        with open(filename) as f:
            for lineno, line in enumerate(f.readlines()):
                line = line.strip()
                #skip comments
                if line.startswith("//"):
                    continue

                parts = line.split()

                #skip empty lines
                if len(parts) == 0:
                    continue

                raw_opcode = parts[0]
                opcode = raw_opcode.lower()
                if opcode.startswith("#"):
                    do_pp_directive(lineno, opcode, parts[1:], program, line, io_schema)
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
                    program.extend(get_arg_value(lineno, arg_str, io_schema) for arg_str in parts[1:])
    except Exception:
        print(traceback.format_exc())
        print(f"{filename} could not be opened")
        sys.exit(0)

    return program, label_table

def second_pass(outfile, program, label_table, big_endian):
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
            endian = "big" if big_endian else "little"
            f.write(to_write.to_bytes(2, byteorder=endian))


def main():
    parser = argparse.ArgumentParser(description="VM bytecode assembler")
    parser.add_argument("-s", "--schema", metavar="FILE", dest="schema", default="./out/opcode_schema.csv", help="opcode schema CSV file: col1=opcode_name, col2=numInlineArgs")
    parser.add_argument("-f", "--io-fn-schema", metavar="FILE", dest="io_schema", help="Schema file for associating IO function names to indices. List of fn names, one per line, in index order.")
    parser.add_argument("-e", "--endian", dest="endian", choices=["big", "little"], default="little", help="endianness of 16bit bytecode words")
    parser.add_argument("-o", "--output", metavar="FILE", dest="output", required=True, help="output file")
    parser.add_argument("-D", "--dump", action="store_true", dest="do_dump", help="Dump the parsed opcode, io schema, and program IR")
    parser.add_argument('input_file', metavar="FILE", help="input file")

    args = parser.parse_args()
    schema = parse_opcode_schema(args.schema)

    if args.io_schema is not None:
        io_schema = parse_io_schema(args.io_schema)
    else:
        io_schema = []

    program, label_table = first_pass(args.input_file, schema, io_schema)

    if args.do_dump:
        print("OPCODE SCHEMA:")
        print(schema)
        print("IO SCHEMA:")
        print(io_schema)
        print("LABEL TABLE:")
        print(label_table)
        print("PROGRAM IR:")
        print(program)
        sys.exit(0)

    is_big_endian = args.endian == "big"
    second_pass(args.output, program, label_table, is_big_endian)


if __name__ == "__main__": main()
