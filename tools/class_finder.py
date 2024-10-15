#!/usr/bin/env python3

import argparse
import re

from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional

# --- DTK HELPERS ---

@dataclass
class Section:
    name: str
    start: str = "0x00000000"
    end: str = "0x00000000"
    attributes: list[str] = field(default_factory=list)


class Sections:
    def __init__(self):
        self.init = Section(".init")
        self.text = Section(".text")
        self.ctors = Section(".ctors")
        self.dtors = Section(".dtors")
        self.rodata = Section(".rodata")
        self.data = Section(".data")
        self.bss = Section(".bss")
        self.sdata = Section(".sdata")
        self.sbss = Section(".sbss")
        self.sdata2 = Section(".sdata2")
        self.sbss2 = Section(".sbss2")

    def types(self) -> list[Section]:
        return [
            getattr(self, "init"),
            getattr(self, "text"),
            getattr(self, "ctors"),
            getattr(self, "dtors"),
            getattr(self, "rodata"),
            getattr(self, "data"),
            getattr(self, "bss"),
            getattr(self, "sdata"),
            getattr(self, "sbss"),
            getattr(self, "sdata2"),
            getattr(self, "sbss2"),
        ]


class Splits:
    def __init__(self):
        # filename: sections
        self.entries: dict[str, Sections] = {}
        self.selected_file: Optional[str] = None

    def select(self, filename: str):
        self.selected_file = filename

    def add(self, filename: str, start: Optional[str] = None, end: Optional[str] = None):
        if filename not in self.entries.keys():
            self.entries[filename] = Sections()

    def get_section(self, section_name: str):
        section: Section = getattr(self.entries[self.selected_file], section_name.removeprefix("."))
        return section
    
    def set_start(self, section_name: str, address: str):
        section = self.get_section(section_name)
        section.start = address
    
    def set_end(self, section_name: str, address: str):
        section = self.get_section(section_name)
        section.end = address

    def add_attribute(self, section_name: str, attr: str):
        section = self.get_section(section_name)
        section.attributes.append(attr)
    
    def get_txt(self):
        return "\n".join(
            f"{filename}:\n"
            + "\n".join(
                f"\t{section.name:11} start:{section.start} end:{section.end}"
                for section in sections.types() if int(section.start, 16) > 0 and int(section.end, 16) > 0
            ) 
            + "\n"
            for filename, sections in self.entries.items()
        )

    @staticmethod
    def from_file(file: Path):
        new_splits = Splits()

        file = file.resolve()
        assert file.exists()

        with file.open("r") as f:
            filedata = f.readlines()

        found = False
        cur_file = ""
        for line in filedata:
            line = line.strip()

            if not found and line.endswith(".c:") or line.endswith(".cpp:"):
                found = True
                cur_file = line.removesuffix(":")
                new_splits.entries[cur_file] = Sections()
                continue

            if found and len(cur_file) > 0:
                if line != "":
                    section: Section = getattr(new_splits.entries[cur_file], line[1:10].strip())
                    section.start = line[18:28]
                    section.end = line[33:43]
                    section.attributes = line[44:]
                else:
                    found = False
                    cur_file = ""

        return new_splits


@dataclass
class Symbol:
    name: str
    section: Section
    attributes: str


class Symbols:
    def __init__(self):
        self.entries: list[Symbol] = []
    
    def get(self, name: str):
        for sym in self.entries:
            if sym.name == name:
                return sym
        return None

    def get_next(self, cur_name: str):
        found = False
        for sym in self.entries:
            if not found and sym.name == cur_name:
                found = True
                continue
            if found:
                return sym
        return None

    def get_first(self, section: str):
        """Returns the first symbol of the section"""
        for sym in self.entries:
            if sym.section.name == section:
                return sym

    def replace(self, old: str, new: str):
        for sym in self.entries:
            if sym.name == old:
                sym.name = new

    def get_txt(self):
        return "\n".join(
            f"{sym.name} = {sym.section.name}:{sym.section.start}; // {sym.attributes}"
            for sym in self.entries
        )

    @staticmethod
    def from_file(file: Path):
        new_syms = Symbols()

        file = file.resolve()
        assert file.exists()

        with file.open("r") as f:
            filedata = f.readlines()

        for i, line in enumerate(filedata):
            split = line.split(" = ")
            cur_symbol = Symbol(
                split[0],
                Section(split[1].split(":")[0], split[1].split(":")[1].split(";")[0]),
                line.split("; // ")[1].removesuffix("\n")
            )

            new_syms.entries.append(cur_symbol)

        return new_syms


# --- MAIN ---


CLASS_REGEX = r".obj lbl_[a-fA-F0-9]*, global\n\t.4byte lbl_[a-fA-F0-9]*\n\t.4byte 0x[a-fA-F0-9]*\n\t.4byte 0x[a-fA-F0-9]*\n\t.4byte fn_[a-fA-F0-9]*\n.endobj lbl_[a-fA-F0-9]*"
SEPARATOR = "-" * 80

@dataclass
class EmulatorClass:
    name: str
    struct_size: int
    class_base_ptr: str
    event_func_name: str
    new_event_func_name: str

    def is_valid(self):
        if self.name == "Unknown":
            return False
        if self.struct_size <= 0:
            return False
        if self.event_func_name == "unknown":
            return False
        return True

    @staticmethod
    def new():
        return EmulatorClass("Unknown", 0, "NULL", "unknown", "")


def get_section_paths(asm_folder: Path, section: str):
    paths: list[Path] = []

    for path in asm_folder.iterdir():
        if path.name.startswith("auto_") and path.name.endswith(f"_{section.removeprefix('.')}.s"):
            paths.append(path)

    return paths


def get_string_from_bytes(input: list[str]):
    # convert them to utf-8 (TODO: handle japanese characters)
    cur_str = ""
    for chars in input:
        cur_str += bytes.fromhex(chars).decode()

    if len(cur_str) == 0:
        cur_str = "(string decode error)"

    # return without the extra zeroes
    return cur_str.replace("\x00", "")


def get_string_from_asm(input: str):
    # parse the string's bytes
    str_bytes: list[str] = []
    for l in input.split("\n"):
        if ".4byte" in l:
            str_bytes.append(l.strip().removeprefix(".4byte").strip().removeprefix("0x"))
    
    return get_string_from_bytes(str_bytes)


# def string_analysis(base_path: Path, version: str):
#     # create the path to the asm folder of for this version
#     asm_folder = Path(f"{base_path}/build/{version}/asm/").resolve()

#     # create paths for the different data sections (ignoring .sdata2 as it doesn't contains strings)
#     data_paths = get_section_paths(asm_folder, "data")
#     rodata_paths = get_section_paths(asm_folder, "rodata")
#     sdata_paths = get_section_paths(asm_folder, "sdata")
#     section_paths = data_paths + rodata_paths + sdata_paths

#     for path in section_paths:
#         with path.open("r") as file:
#             match = re.findall(fr"\.obj (lbl_[a-fA-F0-9]*,).*\.endobj $1", file.read(), re.DOTALL)



def get_classes(base_path: Path, version: str):
    # create the path to the asm folder of for this version
    asm_folder = Path(f"{base_path}/build/{version}/asm/").resolve()

    # create paths for the different data sections (ignoring .sdata2 as it doesn't contains strings)
    data_paths = get_section_paths(asm_folder, "data")
    rodata_paths = get_section_paths(asm_folder, "rodata")
    sdata_paths = get_section_paths(asm_folder, "sdata")
    section_paths = data_paths + rodata_paths + sdata_paths

    # find the classes as asm
    class_matches: list[str] = []
    for path in data_paths:
        with path.open("r") as file:
            class_matches.extend(re.findall(CLASS_REGEX, file.read(), re.DOTALL))

    if len(class_matches) == 0:
        return None

    # convert the asm to readable data
    emu_classes: list[EmulatorClass] = []
    for match in class_matches:
        lines = match.split("\n")
        emu_class = EmulatorClass.new()

        for i, line in enumerate(lines):
            # the relevant data always start by ".4byte" in this case
            if ".4byte" in line:
                value = line.strip().removeprefix(".4byte").strip()

                if value.startswith("lbl_"):
                    # the class' name
                    match = None

                    # since this is a pointer to a string we need to get the actual string's data
                    # we're searching in .data, .rodata and .sdata sections
                    for path in section_paths:
                        with path.open("r") as file:
                            match = re.search(fr"\.obj {value}.*\.endobj {value}", file.read(), re.DOTALL)

                        if match is not None:
                            emu_class.name = get_string_from_asm(match.group(0))
                            break
                elif value.startswith("0x") and i == 1:
                    # the pointer, it's always NULL but just in case we parse it anyway
                    emu_class.class_base_ptr = "NULL" if value == "0x00000000" else value
                elif value.startswith("0x") and i == 2:
                    # the struct's size
                    emu_class.struct_size = int(value, base=16)
                elif value.startswith("fn_"):
                    # the event function symbol
                    emu_class.event_func_name = value
            elif line.startswith(".endobj"):
                emu_classes.append(emu_class)
                emu_class = EmulatorClass.new()

    if len(emu_classes) == 0:
        return None

    return emu_classes


def get_report(base_path: Path, version: str):
    emu_classes = get_classes(base_path, version)

    if emu_classes is None:
        return f"Version: '{version}': (not available, splits already done?)"

    # validate classes and check if there's anything interesting
    length = 0
    for emu_class in emu_classes:
        if not emu_class.is_valid():
            emu_classes.remove(emu_class)
            print("not valid, ignoring this one")

        if emu_class.class_base_ptr != "NULL":
            print(f"{emu_class.name} is interesting :eyes:")
        
        if len(emu_class.name) > length:
            length = len(emu_class.name) + 2

    # create and return the report for this version
    return f"Version: '{version}':\n\t" + "\n\t".join(
        f"Class {repr(emu_class.name):{length}} // event_func={repr(emu_class.event_func_name)} struct_size=0x{emu_class.struct_size:X}"
        for emu_class in emu_classes
    )


NAME_TO_DATA = {
    'SYSTEM': {"event_func": "systemEvent", "filename": "emulator/system.c"},
    'AI': {"event_func": "aiEvent", "filename": "emulator/ai.c"},
    'VI': {"event_func": "viEvent", "filename": "emulator/vi.c"},
    'SI': {"event_func": "siEvent", "filename": "emulator/si.c"},
    'PI': {"event_func": "piEvent", "filename": "emulator/pi.c"},
    'MI': {"event_func": "miEvent", "filename": "emulator/mi.c"},
    'DD': {"event_func": "diskEvent", "filename": "emulator/disk.c"},
    'CPU': {"event_func": "cpuEvent", "filename": "emulator/cpu.c"},
    'RAM': {"event_func": "ramEvent", "filename": "emulator/ram.c"},
    'ROM': {"event_func": "romEvent", "filename": "emulator/rom.c"},
    'RDB': {"event_func": "rdbEvent", "filename": "emulator/rdb.c"},
    'MEMORY-PAK': {"event_func": "pakEvent", "filename": "emulator/pak.c"},
    'SRAM': {"event_func": "sramEvent", "filename": "emulator/sram.c"},
    'FLASH': {"event_func": "flashEvent", "filename": "emulator/flash.c"},
    'Frame': {"event_func": "frameEvent", "filename": "emulator/frame.c"},
    'OS-LIBRARY': {"event_func": "libraryEvent", "filename": "emulator/library.c"},
    'Code': {"event_func": "codeEvent", "filename": "emulator/codeRVL.c"},
    'AUDIO': {"event_func": "audioEvent", "filename": "emulator/soundRVL.c"},
    'Video': {"event_func": "videoEvent", "filename": "emulator/video.c"},
    'Store': {"event_func": "storeEvent", "filename": "emulator/store.c"},
    'Controller': {"event_func": "controllerEvent", "filename": "emulator/controller.c"},
    'HelpMenu': {"event_func": "helpMenuEvent", "filename": "emulator/helpRVL.c"},
    'PIF': {"event_func": "pifEvent", "filename": "emulator/pif.c"},
    'RSP': {"event_func": "rspEvent", "filename": "emulator/rsp.c"},
    'FILE': {"event_func": "xlFileEvent", "filename": "emulator/xlFileRVL.c"},
}

def update_config(base_path: Path, version: str):
    emu_classes = get_classes(base_path, version)
    syms = Symbols.from_file(Path(f"config/{version}/symbols.txt"))
    splits = Splits()

    for i, ec in enumerate(emu_classes):
        data_map = NAME_TO_DATA.get(ec.name)
        filename = ""

        if data_map is not None:
            ec.new_event_func_name = data_map["event_func"]
            filename = data_map["filename"]
        else:
            name = ec.name.replace("-", "_").lower()
            ec.new_event_func_name = f"{name}Event"
            filename = f"emulator/{name}.c"

        # replace the symbol's name
        syms.replace(ec.event_func_name, ec.new_event_func_name)

        # get the current symbol and the next one
        # sym = syms.get(ec.new_event_func_name)
        next_sym = syms.get_next(ec.new_event_func_name)

        if i > 0:
            prev_sym = syms.get_next(emu_classes[i - 1].new_event_func_name)
        else:
            prev_sym = syms.get_first(".text")

        if prev_sym is not None and next_sym is not None:
            # create a new split and select it
            splits.add(filename)
            splits.select(filename)

            # set the addresses of the split
            splits.set_start(".text", prev_sym.section.start)
            splits.set_end(".text", next_sym.section.start)

    with Path(f"config/{version}/splits.txt").resolve().open("a") as file:
        file.write(splits.get_txt())

    with Path(f"config/{version}/symbols.txt").resolve().open("w") as file:
        file.write(syms.get_txt())


def main():
    VERSIONS = [
        "mt-u",
    ]

    parser = argparse.ArgumentParser(description="Find emulator's classes and guess the splits")
    parser.add_argument("-r", "--report", action="store_true", default=True, help="write a report file for the found classes (default: false)")
    parser.add_argument("-p", "--path", default=None, help="path to the workspace (default: the directory where this script is)")

    args = parser.parse_args()
    report_only: bool = args.report

    if args.path is None:
        base_path = Path("./").resolve()
    else:
        base_path = Path(args.path).resolve()

    if not base_path.exists():
        raise ValueError(f"ERROR: this path doesn't exist ({repr(base_path)})")

    if report_only:
        data = f"\n\n{SEPARATOR}\n\n".join(get_report(base_path, version) for version in VERSIONS) + "\n"
        with Path("class_report.txt").resolve().open("w") as file:
            file.write(data)
    else:
        for version in VERSIONS:
            update_config(base_path, version)


if __name__ == "__main__":
    main()
