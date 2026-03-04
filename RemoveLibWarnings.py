Import("env")
from pathlib import Path

project_src_dir = Path(env.subst("$PROJECT_SRC_DIR")).resolve()
project_include_dir = Path(env.subst("$PROJECT_INCLUDE_DIR")).resolve()
pio_dir = Path(env.subst("$PROJECT_DIR")) / ".pio"


def filter_warnings(node):
    path = Path(node.abspath).resolve()

    # Keep warnings only for files in src/ or include/
    if path.startswith(project_src_dir) or path.startswith(project_include_dir):
        return node

    # Ignore everything else (all of .pio including libdeps, frameworks, cores)
    if path.startswith(pio_dir):
        node.env["CCFLAGS"] = [
            f for f in node.env.get("CCFLAGS", []) if not str(f).startswith("-W")
        ]

    return node


# Apply to C and C++
env.AddBuildMiddleware(filter_warnings, "*.c")
env.AddBuildMiddleware(filter_warnings, "*.c*")
env.AddBuildMiddleware(filter_warnings, "*.cpp")
