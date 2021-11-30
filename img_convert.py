IMAGE_CONVERTER_PATH='/home/andek/code/lv_img_conv/lv_img_conv.js'
ARG_COUNT=4

import sys
import os

# ./lv_img_conv.js ../komoot-navi-esp-idf/images/numbers/two.png -c CF_INDEXED_1_BIT -o ../two.c
def main():
    if len(sys.argv) != ARG_COUNT:
        print(f'Wrong argument count {len(sys.argv)} instead of {ARG_COUNT}');
        exit()

    root,dirs,filenames = list(os.walk(sys.argv[1]))[0]
    tmp_dir = os.path.join(root, 'tmp')
    if 'tmp' not in dirs:
        os.mkdir(tmp_dir)

    print(tmp_dir)


def convert_images_to_c_files(img_files,dest_dir):
    for img in img_files:
        cmd = f"{IMAGE_CONVERTER_PATH} {os.path.join(root, file)} -c CF_INDEXED_1_BIT -o {os.path.join(tmp_dir,os.path.splitext(file)[0])}.c"
        print(cmd)
        os.system(cmd)

    


if __name__ == "__main__":
    main()