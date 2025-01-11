#!/usr/bin/env python3

import subprocess
import zipfile
import os
import sys
import shutil

os.chdir(os.path.dirname(os.path.realpath(__file__)))

try:
    os.remove("test1-pyzip.zip")
except:
    pass

with zipfile.ZipFile('test1-pyzip.zip', 'w', compression=zipfile.ZIP_DEFLATED, allowZip64=True) as z:
    with z.open('one.txt', 'w', force_zip64=False) as f:
        f.write(b"this is test content1 - 1\n")
    with z.open('two.txt', 'w', force_zip64=False) as f:
        f.write(b"this is another test content2 - 1\n")
    with z.open('sub/three.txt', 'w', force_zip64=False) as f:
        f.write(b"this is yet a different test content3 - 1\n")

try:
    os.remove("test2-pyzip64.zip")
except:
    pass

with zipfile.ZipFile('test2-pyzip64.zip', 'w', compression=zipfile.ZIP_DEFLATED, allowZip64=True) as z:
    with z.open('one.txt', 'w', force_zip64=True) as f:
        f.write(b"this is test content1 - 1\n")
    with z.open('two.txt', 'w', force_zip64=True) as f:
        f.write(b"this is another test content2 - 1\n")
    with z.open('sub/three.txt', 'w', force_zip64=True) as f:
        f.write(b"this is yet a different test content3 - 1\n")


try:
    shutil.rmtree("zz")
except:
    pass

os.mkdir("zz")
os.mkdir("zz/sub")
with open("zz/one.txt", "wb") as f:
    f.write(b"this is test content1 - 1\n")
with open('zz/two.txt', 'wb') as f:
    f.write(b"this is another test content2 - 1\n")
with open('zz/sub/three.txt', 'wb') as f:
    f.write(b"this is yet a different test content3 - 1\n")

try:
    os.remove("test3-zipcmd.zip")
except:
    pass

subprocess.run(["zip", "test3-zipcmd.zip", "-r", "zz"])

try:
    os.remove("test3-zip64cmd.zip")
except:
    pass

subprocess.run([sys.executable, "thirdparty/zip64.py", "test4-zip64cmd.zip", "zz"])

try:
    shutil.rmtree("zz")
except:
    pass
