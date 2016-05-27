#
#
#
import os
import sys
import re
from os import listdir
from os.path import isfile, join

do_train = len(sys.argv[1:]) and sys.argv[1] is '1'

# TODO
# do_tess_finally = len(sys.argv[1:])>1 and sys.argv[2] is '1'

# if false, does only tess and open the result file for checking (at the end of file currently, 2016-05-23, Albert)
imgs_path = '../../img/'

if do_train:

    # arr_fonts_ext = sys.argv[1:]
    # ! DON'T forget to add fonts to font_properties
    arr_fonts_ext = [
        # 'heb.engCourierNew.exp0.tiff ',
        'heb.miriam.exp0.tiff', 'heb.courierNew.exp0.tiff', 'heb.arial.exp0.tiff', 'heb.david.exp0.tiff'
        ,'heb.kadosh.exp0.jpg', 'heb.kadosh.exp1.jpg', 'heb.kadosh.exp2.jpg'
        ,'heb.half_free.exp0.jpg', 'heb.supersal.exp0.jpg', 'heb.lahmanina.exp0.jpg'
    ]

    r_ext = re.compile('.(jpe?g|tiff?|png)', re.I)
    arr_fonts = [ re.sub(r_ext, '', x) for x in arr_fonts_ext ]

    for x in arr_fonts_ext:
        # os.system("tesseract -l heb -psm 4 "+x+" "+re.sub(r_ext, '', x)+" box.train")
        os.system("tesseract -l heb "+x+" "+re.sub(r_ext, '', x)+" box.train")

    print

    uni = 'unicharset_extractor '
    uni += ' '.join([str(x)+'.box' for x in arr_fonts ])
    os.system(uni)

    print

    os.system("cp unicharset.orig unicharset")
    os.system("set_unicharset_properties -U input_unicharset -O output_unicharset --script_dir=./")

    print

    mf = 'mftraining -F font_properties -U unicharset -O heb.unicharset '
    mf += ' '.join([str(x)+'.tr' for x in arr_fonts ])
    os.system(mf)

    print

    cn = 'cntraining '
    cn += ' '.join([str(x)+'.tr' for x in arr_fonts ])
    os.system(cn)

    print

    os.system("mv normproto heb.normproto; mv inttemp heb.inttemp; mv pffmtable heb.pffmtable; mv shapetable heb.shapetable")
    os.system("combine_tessdata heb. ; cp heb.traineddata /usr/local/Cellar/tesseract/3.04.01_1/share/tessdata")

    print

# finally do tess for each file in the img dir and open results...
imgs = [f for f in listdir(imgs_path) if isfile(join(imgs_path, f))]
# print imgs

for img in imgs:
    # print 'tesseract -l heb '+join(imgs_path, img)+' '+join(imgs_path, 'out/'+img) +'; open -e ' + join(imgs_path, 'out/'+img+'.txt')
    os.system ( 'tesseract -l heb+eng '+join(imgs_path, img)+' '+join(imgs_path, 'out/'+img) +'; open -e ' + join(imgs_path, 'out/'+img+'.txt') )

print
