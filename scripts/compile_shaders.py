import sys
import os
import json
import hashlib


class Lock:
    def __init__(self, lock_file: str):
        """ initialize Lock with path to .shader.lock.json """
        self.__lock_file = lock_file
        self.__data = {}

        try:
            with open(f"{lock_file}", mode="r") as lock:
                self.__data = json.load(lock)
        except:
            with open(f"{lock_file}", mode="w") as lock:
                print(f"Creating new {lock_file}")
                json.dump(self.__data, lock, indent=2)


    def is_invalid(self, shader: str) -> bool:
        """ query lock.json to see if shader needs to be updated """
        uuid = self.__uuid(shader)
        return not (shader in self.__data and self.__data[shader] == uuid)


    def update_hash(self, shader: str):
        """ update shader with approriate uuid """
        key = f"{shader}"
        self.__data[key] = self.__uuid(shader);
        with open(f"{self.__lock_file}", "w") as lock:
            json.dump(self.__data, lock, indent=2)


    def __uuid(self, shader: str) -> str:
        sha256 = hashlib.sha256(usedforsecurity=False)

        try:
            with open(f"{shader}", mode="r") as file:
                sha256.update(file.read().encode("utf-8"))
                return sha256.hexdigest()

        except IOError:
            print(f"could not open {shader}")
            exit(-1)



def main(glslc: str, in_dir: str, out_dir: str, force: bool):
    print("checking shader cache...")
    os.chdir(in_dir)
    shader_lock = ".shader.lock.json"

    lock = Lock(shader_lock)

    for shader in os.listdir():
        if shader == shader_lock:
            continue
        in_shader = os.path.join(in_dir, shader).replace("\\", "/")
        out_shader = os.path.join(out_dir, shader).replace("\\", "/")

        if lock.is_invalid(in_shader) or force:
            print(f"compiling {in_shader}...")
            lock.update_hash(in_shader)

            if os.name == "nt":
                os.system(f"{glslc} {in_shader} -o {out_shader}.spv")
            else:
                os.system(f"{glslc} {in_shader} -o {out_shader}.spv")


if __name__ == "__main__":
    force = "--force" in sys.argv or "-f" in sys.argv
    main(sys.argv[1], sys.argv[2], sys.argv[3], force);