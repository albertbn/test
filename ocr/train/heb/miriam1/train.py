#
# python train.py heb.miriam.exp0.tiff heb.engCourierNew.exp0.tiff
# python train.py heb.miriam.exp0.tiff heb.courierNew.exp0.tiff heb.kadosh.exp0.jpg heb.kadosh.exp1.jpg heb.kadosh.exp2.jpg
#
import os
import sys
import re

arr_fonts_ext = sys.argv[1:]
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
