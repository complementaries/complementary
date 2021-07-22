#!/usr/bin/env python3

import sys
import re
import os

file_type = sys.argv[1]
if not file_type:
    file_type = "class"
path = sys.argv[2]
if not path:
    raise "Expected path as the first argument"
if not path.startswith("src/"):
    path = "src/" + path
name = path.split('/')[-1]

def camel_to_snake(name):
  name = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', name)
  return re.sub('([a-z0-9])([A-Z])', r'\1_\2', name).upper()

with open(path + ".h", "w") as file:
    file_upper = camel_to_snake(name)

    if file_type == "class":
        file.writelines("""#ifndef {file_upper}_H
#define {file_upper}_H

class {file_camel} final {{
  public:
    {file_camel}();

  private:
}};

#endif
""".format(file_upper = file_upper, file_camel = name))
    elif file_type == "namespace":
        file.writelines("""#ifndef {file_upper}_H
#define {file_upper}_H

namespace {file_camel} {{

}};

#endif
""".format(file_upper = file_upper, file_camel = name))

with open(path + ".cpp", "w") as file:
    file_upper = camel_to_snake(name)

    if file_type == "class":
        file.writelines("""#include \"{file_camel}.h\"

{file_camel}::{file_camel}() {{

}}
""".format(file_upper = file_upper, file_camel = name))
    elif file_type == "namespace":
        file.writelines("""#include \"{file_camel}.h\"
""".format(file_upper = file_upper, file_camel = name))

os.system("meson rewrite target complementary add {}".format(
    path.replace("\\", "/") + ".cpp"))
