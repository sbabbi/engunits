#
# GDB pretty printers for engunits::quantity
    
import gdb.printing
import re
from gdb.types import get_type_recognizers
from gdb.types import apply_type_recognizers
    
## For some reason Type.template_argument is broken with variadic templates
## Just do our-own parsing
def _parse_template_decl(string):
    "Parse a template declaration, return a (name, template-arg-list) tuple."

    match = _parse_template_decl.match_template.search(string)
    
    if not match:
        return string
    
    name = string[0:match.start()]
    string = string[match.start():].strip()
    
    args = []

    if string.startswith('<'):
        while string.startswith('<') or string.startswith(','):
            string = string[1:].strip()
            (x, string) = _parse_template_decl(string)
            args.append(x)

        string = string.strip()[1:]
    
    result = (name, args) if args else name

    return ( result, string ) if string else result

_parse_template_decl.match_template = re.compile('[<>,]')

def _get_unit_string(unit_name):
    "Compute a string to represent a unit, from a (unit-name, template-arg-list) tuple"

    def str2int(s):
        return int(_get_unit_string.match_int.match(s).group(0))
    
    # Remove namespaces and trailing underscore.
    basename = unit_name[0].split('::')[-1].strip().rstrip('_')
    exp = unit_name[1]
    
    num = str2int(exp[0]) if exp else 1
    den = str2int(exp[1]) if len(exp) > 1 else 1
    
    if num == den:
        return basename
    
    if den == 1:
        return basename + '^' + str(num)
    
    return basename + '^(' + str(num) + '/' + str(den) +')'

_get_unit_string.match_int = re.compile('\-?\d+')

class EngUnitsQuantityPrinter(object):
    "Print a quantity value"
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return self.val['value_']

class EngUnitsQuantityTypePrinter(object):
    "Print a quantity type"
    
    def __init__(self):
        self.name = 'quantity'
        self.enabled = True

    class _recognizer:        
        def recognize(self, type):
            if not type.tag:
                return None
            
            if not type.tag.startswith('engunits::quantity'):
                return None
            
            recognizers = get_type_recognizers()
            base_type = type.template_argument(0)
            
            ## Construct the result
            basename = apply_type_recognizers( recognizers, base_type ) or base_type.name
            
            return basename + " [" + ",".join( map( _get_unit_string, _parse_template_decl( type.tag )[1][1:]) ) + "]"

    def instantiate(self):
        return self._recognizer()

def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("engunits")

    pp.add_printer('quantity', '^engunits::quantity<.*>$', EngUnitsQuantityPrinter)
    return pp
