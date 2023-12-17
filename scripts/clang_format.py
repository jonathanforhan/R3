import os
import threading
from subprocess import call


def format(root: str, fds: [str]):
    """ Recursively walk file tree and format files """
    for fd in fds:
        path = os.path.join(root, fd)
        if os.path.isfile(path) and path.endswith((".cxx", ".hxx", ".cpp", ".hpp")):
            call(["clang-format", "-i", path])
        elif os.path.isdir(path):
            format(path, os.listdir(path))


def main():
    paths = [
        "../engine/public",
        "../engine/private"
    ]

    for path in paths:
        t = threading.Thread(target=format, args=(path, os.listdir(path)))
        t.start()


if __name__ == "__main__":
    main()