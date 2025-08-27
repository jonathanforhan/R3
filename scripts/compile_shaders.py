import sys
import os
import json
import hashlib


class Lock:
    def __init__(self, lock_file: str):
        """ initialize Lock with path to .shader-lock.json """
        self.__lock_file = lock_file
        self.__data = {}

        try:
            with open(self.__lock_file, mode="r") as lock:
                self.__data = json.load(lock)
        except:
            with open(self.__lock_file, mode="w") as lock:
                print(f"creating new {lock_file}")
                json.dump(self.__data, lock, indent=2)

    def is_valid(self, shader_path: str) -> bool:
        """ query .shader-lock.json to see if shader needs to be updated """
        uuid = self.__uuid(shader_path)
        return shader_path in self.__data and self.__data[shader_path] == uuid

    def update_hash(self, shader_path: str):
        """ update shader with approriate uuid """
        self.__data[shader_path] = self.__uuid(shader_path)
        with open(self.__lock_file, mode="w") as lock:
            json.dump(self.__data, lock, indent=2)

    def __uuid(self, shader_path: str) -> str:
        sha256 = hashlib.sha256(usedforsecurity=False)

        try:
            with open(shader_path, mode="r") as file:
                sha256.update(file.read().encode("utf-8"))
                return sha256.hexdigest()
        except IOError:
            print(f"could not open {shader_path}")
            exit(-1)


def main(glslc: str, spirv_cross: str, in_dir: str, out_dir: str, force: bool):
    shader_dir = os.path.join(in_dir, "shaders")
    shader_lock = os.path.join(shader_dir, ".shader-lock.json")

    sub_dirs = ("_spirv", "_hlsl", "_reflection")

    print("checking shader cache...")

    for sub_dir in sub_dirs:
        path = os.path.join(out_dir, sub_dir)

        # if sub_dir does not exist, create it
        if not os.path.exists(path):
            os.makedirs(path, exist_ok=True)

        # if a sub_dir is empty directory, invalidate cache
        if not os.listdir(path):
            try: os.remove(shader_lock)
            except: ...

    # read in shader cache
    lock = Lock(shader_lock)

    for shader_filename in os.listdir(shader_dir):
        if shader_filename == ".shader-lock.json":
            continue

        in_shader = os.path.join(shader_dir, shader_filename).replace("\\", "/")

        if not lock.is_valid(in_shader) or force:
            print(f"compiling {in_shader}...")
            lock.update_hash(in_shader)

            out_spirv = os.path.join(out_dir, "_spirv", shader_filename).replace("\\", "/")
            out_hlsl = os.path.join(out_dir, "_hlsl", shader_filename).replace("\\", "/")
            out_reflection = os.path.join(out_dir, "_reflection", shader_filename).replace("\\", "/")

            # compile glsl to spirv
            os.system(f"{glslc} {in_shader} -o {out_spirv}.spv")
            # compile spirv to hlsl
            #os.system(f"{spirv_cross} --output {out_hlsl}.hlsl {out_spirv}.spv --hlsl")
            # compile spirv to json reflection file
            #os.system(f"{spirv_cross} --output {out_reflection}.json {out_spirv}.spv --reflect")


if __name__ == "__main__":
    force = "--force" in sys.argv or "-f" in sys.argv
    try:
        sys.argv.remove("--force")
        sys.argv.remove("-f")
    except:
        ...
    # python.exe compile_shaders.py glslc.exe spirv-cross.exe C:/in/dir C:/out/dir
    # python3 compile_shaders.py glslc spirv-cross /home/foo/in/dir /home/foo/in/dir
    main(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], force)