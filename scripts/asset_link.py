import sys
import os


def main(in_dir: str, out_dir: str):
    print("linking assets")

    if not os.path.exists(out_dir):
        os.symlink(in_dir, out_dir, target_is_directory=True)


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2]);
