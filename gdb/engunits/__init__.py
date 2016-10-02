

import gdb.printing

from .printers import build_pretty_printer

gdb.printing.register_pretty_printer( gdb.current_objfile(),
                                     printers.build_pretty_printer())

gdb.types.register_type_printer(gdb.current_objfile(),
                                printers.EngUnitsQuantityTypePrinter())
