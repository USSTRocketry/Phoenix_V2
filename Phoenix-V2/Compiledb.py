import os
Import("env")

env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

def add_unique_paths(paths):
    current = env.get("CPPPATH", [])
    for p in paths:
        if p and p not in current:
            env.Prepend(CPPPATH=[p])

def add_recursive_includes(root_path):
    for dirpath, dirnames, filenames in os.walk(root_path):
        add_unique_paths([dirpath])

# Arduino core includes
framework_dir = env.get("FRAMEWORK_DIR")
board = env.get("BOARD")
if framework_dir and board:
    arduino_core = os.path.join(framework_dir, "cores", board)
    arduino_variant = os.path.join(framework_dir, "variants", board)
    add_unique_paths([arduino_core, arduino_variant])

# Add project include paths
project_includes = env.get("CPPPATH", [])
add_unique_paths(project_includes)

# Add all library source dirs and recursively all their subdirs
lib_source_dirs = env.get("LIBSOURCE_DIRS", [])
for lib_dir in lib_source_dirs:
    add_unique_paths([lib_dir])
    add_recursive_includes(lib_dir)

# Add other possible library paths
lib_extra = env.get("LIB_EXTRA_INCLUDES", [])
add_unique_paths(lib_extra)

lib_paths = env.get("LIBPATH", [])
add_unique_paths(lib_paths)

env.Replace(COMPILATIONDB_PATH=os.path.join(env["PROJECT_DIR"], "compile_commands.json"))
