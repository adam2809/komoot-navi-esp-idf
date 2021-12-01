# Command line arguments:
# 1. Source of pdf images
# 2. Target .c file
# 3. Target .h file 

IMAGE_CONVERTER_PATH='/home/andek/code/lv_img_conv/lv_img_conv.js'
ARG_COUNT=4

import sys
import os
from shutil import rmtree


def convert_images_to_c_files(img_files,dest_dir,root):
    for img in img_files:
        cmd = f"{IMAGE_CONVERTER_PATH} {os.path.join(root, img)} -f -c CF_INDEXED_1_BIT -o {os.path.join(dest_dir,os.path.splitext(img)[0])}.c"
        print(f'Executing lv_img_conv')
        print(cmd)
        os.system(cmd)

def write_c_file(source_dir):
    cmd = f'> {sys.argv[2]}'
    print(cmd)
    os.system(cmd)

    cmd = f'echo "#include \\"{os.path.split(sys.argv[3])[1]}\\"" >> {sys.argv[2]}'
    print(cmd)
    os.system(cmd)

    cmd = f'cat {source_dir}/* >> {sys.argv[2]}'
    print(cmd)
    os.system(cmd)
# #ifndef _MISC_IMGS_H
# #define _MISC_IMGS_H
# #include <stdio.h>
# #include "lvgl_helpers.h"
# const lv_img_dsc_t greater_than;
# const lv_img_dsc_t km;
# const lv_img_dsc_t bt_icon;
# #endif
h_file_path = sys.argv[3]
def write_h_file(filenames):
    cmd = f'> {h_file_path}'
    print(cmd)
    os.system(cmd)

    include_guard_define_name = os.path.splitext(os.path.split(h_file_path)[1])[0].upper()

    cmd = f'echo "#ifndef _{include_guard_define_name}" >> {h_file_path}'
    print(cmd)
    os.system(cmd)

    cmd = f'echo "#define _{include_guard_define_name}" >> {h_file_path}'
    print(cmd)
    os.system(cmd)

    cmd = f'echo "#include \\"lvgl_helpers.h\\"" >> {h_file_path}'
    print(cmd)
    os.system(cmd)

    cmd = f'echo "#include <stdio.h>" >> {h_file_path}'
    print(cmd)
    os.system(cmd)

    for f in filenames:
        cmd = f'echo "const lv_img_dsc_t {os.path.splitext(f)[0]};" >> {h_file_path}'
        print(cmd)
        os.system(cmd)

    cmd = f'echo "#endif" >> {h_file_path}'
    print(cmd)
    os.system(cmd)

    



# ./lv_img_conv.js ../komoot-navi-esp-idf/images/numbers/two.png -c CF_INDEXED_1_BIT -o ../two.c
def main():
    if len(sys.argv) != ARG_COUNT:
        print(f'Wrong argument count {len(sys.argv)} instead of {ARG_COUNT}');
        exit()

    root,dirs,filenames = list(os.walk(sys.argv[1]))[0]
    tmp_dir = os.path.join(root, 'tmp')
    if 'tmp' not in dirs:
        os.mkdir(tmp_dir)

    convert_images_to_c_files(filenames,tmp_dir,root)
    write_c_file(tmp_dir)
    write_h_file(filenames)

    rmtree(tmp_dir);


    


if __name__ == "__main__":
    main()