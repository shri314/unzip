#!/usr/bin/env python3

import zipfile
import os

try:
    os.remove("test1.zip")
except:
    pass

with zipfile.ZipFile('test1.zip', 'w', compression=zipfile.ZIP_DEFLATED, allowZip64=True) as z:
    with z.open('one.txt', 'w', force_zip64=False) as f:
        f.write(b"this is test content1 - 1")
        f.write(b"this is test content1 - 2")
    with z.open('two.txt', 'w', force_zip64=False) as f:
        f.write(b"this is another test content2 - 1")
        f.write(b"this is another test content2 - 2")
    with z.open('sub/one.txt', 'w', force_zip64=False) as f:
        f.write(b"this is yet a different test content3 - 1")
        f.write(b"this is yet a different test content3 - 2")

try:
    os.remove("test2.zip")
except:
    pass

with zipfile.ZipFile('test2.zip', 'w', compression=zipfile.ZIP_DEFLATED, allowZip64=True) as z:
    with z.open('one.txt', 'w', force_zip64=True) as f:
        f.write(b"this is test content1 - 1")
        f.write(b"this is test content1 - 2")
    with z.open('two.txt', 'w', force_zip64=True) as f:
        f.write(b"this is another test content2 - 1")
        f.write(b"this is another test content2 - 2")
    with z.open('sub/one.txt', 'w', force_zip64=True) as f:
        f.write(b"this is yet a different test content3 - 1")
        f.write(b"this is yet a different test content3 - 2")
