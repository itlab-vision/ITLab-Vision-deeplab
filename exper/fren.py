import os
import sys
import re


if __name__ == '__main__':
    in_path = os.path.abspath(sys.argv[1])
    out_path = os.path.abspath(sys.argv[2])
    regex = sys.argv[3]
    repl = sys.argv[4]

    for entry in os.listdir(in_path):
        new_name = re.sub(regex, repl, entry)
       	os.rename(os.path.join(in_path, entry), os.path.join(out_path, new_name))